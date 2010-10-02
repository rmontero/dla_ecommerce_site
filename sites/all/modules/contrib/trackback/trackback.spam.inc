<?php
// $Id: trackback.spam.inc,v 1.1 2007/12/06 17:53:49 zorac Exp $

/**
 * Implementation of hook_trackback()
 */
function trackback_trackback(&$trackback, $op) {
  switch ($op) {
    case 'receive':
      if (variable_get('trackback_spam_filter', 1)) {
        // drop silently if this is from a known spammer IP address
        module_invoke('spam', 'ip_filter', 'trackback', 0);
      }
      break;

    case 'insert':
      if (variable_get('trackback_spam_filter', 1)) {
        // invoke spam.module's spam filter
        module_invoke('spam', 'content_filter', 'trackback', $trackback->trid, $trackback->subject .' '. $trackback->url, $trackback->excerpt);
      }
      break;

    case 'delete':
      db_query("DELETE FROM {spam_tracker} WHERE source='trackback' AND id=%d", $trackback->trid);
      spam_log(SPAM_LOG, t('spam_delete_trackback: deleted trackback "%subject".', array('%subject' => $trackback->subject)), 'trackback', $trackback->trid);
      break;
  }
}

function trackback_spam($op, $a2, $a3, $a4) {
  $return = array();
  switch ($op) {
    case 'filter_settings':
      $return['group']['trackback_spam_filter'] = array(
        '#type' => 'checkbox',
        '#title' => t('Filter trackbacks'),
        '#default_value' => variable_get('trackback_spam_filter', 1),
        '#description' => t('Enable this option to filter new trackbacks as they are posted, determining whether or not they are spam.'));
      break;

    case 'page':
      if (arg(1) == 'trackback') {
        $trackback = trackback_load($a2);
        $return = array(
          'old' => spam_load('trackback', $a2),
          'header' => $trackback->subject .' '. $trackback->url,
          'body' => $trackback->excerpt,
          'goto' => _trackback_path($trackback),
          'goto_fragment' => 'trackbacks'
        );
      }
      break;
  }
  return $return;
}

function spam_publish_trackback($trid) {
  if ($trackback = _trackback_operation('publish', $trid)) {
    spam_log(SPAM_LOG, t('spam_publish_trackback: published trackback "%subject".', array('%subject' => $trackback->subject)), 'trackback', $trid);
  }
}

function spam_unpublish_trackback($trid) {
  if ($trackback = _trackback_operation('unpublish', $trid)) {
    spam_log(SPAM_LOG, t('spam_unpublish_trackback: unpublished trackback "%subject".', array('%subject' => $trackback->subject)), 'trackback', $trid);
  }
}

function spam_delete_trackback($trid) {
  if ($trackback = _trackback_operation('delete', $trid)) {
    spam_log(SPAM_LOG, t('spam_delete_trackback: deleted trackback "%subject".', array('%subject' => $trackback->subject)), 'trackback', $trid);
  }
}

function spam_spam_trackback($trid) {
  _trackback_operation('_trackback_as_spam', $trid);
}

function spam_notspam_trackback($trid) {
  _trackback_operation('_trackback_as_notspam', $trid);
}

function _trackback_as_spam($trackback) {
  _trackback_spam_modify($trackback, 1);
  watchdog('spam', 'Spam: marked trackback %subject as spam.', array('%subject' => $trackback->subject));
  spam_log(SPAM_LOG, t('trackback manually marked as spam'), 'trackback', $trackback->trid);
}

function _trackback_as_notspam($trackback) {
  _trackback_spam_modify($trackback, 0);
  watchdog('spam', 'Spam: marked trackback %subject as not spam.', array('%subject' => $trackback->subject));
  spam_log(SPAM_LOG, t('trackback manually marked as not spam'), 'trackback', $trackback->trid);
}

function _trackback_spam_modify($tb, $spam) {
  $tokens = spam_tokenize($tb->subject .' '. $tb->url, 'header*');
  $tokens = array_merge($tokens, spam_tokenize($tb->excerpt));
  spam_tokens_unsave($tokens, $spam);
  spam_tokens_save($tokens, $spam);
  $prob = $spam ? 99 : 1;
  db_query("UPDATE {spam_tracker} SET probability=%d, timestamp=%d WHERE source='trackback' AND id=%d", $prob, time(), $tb->trid);
  spam_default_actions('trackback', $tb->trid, $tb->subject, $tb->excerpt, $prob, NULL, FALSE);
}

function _trackback_spam_link($trackback) {
  $output = array();
  if (!variable_get('trackback_spam_filter', 1)) {
    return $output;
  }

  $p = db_fetch_object(db_query("SELECT probability FROM {spam_tracker} WHERE id = %d AND source = 'trackback'", $trackback->trid));
  $spam = array(
    'href' => 'spam/trackback/'. $trackback->trid .'/spam',
    'title' => t('mark as spam')
  );
  $notspam = array(
    'href' => 'spam/trackback/'. $trackback->trid .'/notspam',
    'title' => t('mark as not spam')
  );
  $access = user_access('access spam');
  $admin = user_access('administer spam');
  if (variable_get('spam_display_probability', 0)) {
    if (variable_get('spam_log_level', SPAM_LOG)) {
      $prob = l($p->probability, 'admin/content/spam/logs/trackback/'. $trackback->trid);
    }
    else {
      $prob = $p->probability;
    }
    $prob = ' ('. $prob .')';
  }

  if (!$p->probability && $admin) {
    $output['spam-spam'] = $spam;
    $output['spam-notspam'] = $notspam;
  }
  else if ($p->probability < variable_get('spam_threshold', 80)) {
    if ($access) {
      $output['spam-probability'] = array('title' => t('not spam') . $prob, 'html' => TRUE);
    }
    if ($admin) {
      $output['spam-spam'] = $spam;
    }
  }
  else {
    if ($access) {
      $output['spam-probability'] = array('title' => t('spam') . $prob, 'html' => TRUE);
    }
    if ($admin) {
      $output['spam-notspam'] = $notspam;
    }
  }
  return $output;
}
