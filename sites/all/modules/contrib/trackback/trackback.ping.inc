<?php
// $Id: trackback.ping.inc,v 1.1.2.3 2009/05/08 16:32:31 thepanz Exp $

function trackback_page($node) {
  $output = array();
  $output[] = '<?xml version="1.0" encoding="utf-8"?>';
  $output[] = '<response>';

  $trackback = trackback_receive($node);
  
  if (empty($trackback->error)) {
    db_query("INSERT INTO {trackback_received} (nid, created, site, name, subject, url, excerpt, status) VALUES (%d, %d, '%s', '%s', '%s', '%s', '%s', %d)", $trackback->nid, $trackback->created, $trackback->site, $trackback->name, $trackback->subject, $trackback->url, $trackback->excerpt, $trackback->status);
    $trackback->trid = db_last_insert_id('trackback_received', 'trid');
    trackback_invoke_trackback($trackback, 'insert');
    watchdog('trackback', 'Added trackback %subject.', array('%subject' => $trackback->subject), WATCHDOG_NOTICE, _trackback_path($trackback, t('view')));
    $output[] = '<error>0</error>';
  }
  else {
    $output[] = '<error>1</error>';
    $output[] = '<message>'. $trackback->error .'</message>';
  }
  $output[] = '</response>';

  header('Content-Type: text/xml');
  print implode("\n", $output) ."\n";
}

function trackback_receive($node) {
  $trackback = new stdClass();
  if (empty($_REQUEST['url']) || !_trackback_valid_url($_REQUEST['url'])) {
    $trackback->error = t('Missing TrackBack url.');
  }
  elseif (variable_get('trackback_reject_oneway', 0)) {
    $reply = drupal_http_request($_REQUEST['url']);
    if (!empty($reply->error)) {
      $trackback->error = t('Could not retrieve the sender page.');
    }
    elseif (stristr($reply->data, $GLOBALS['base_url'] .'/') === FALSE) {
      $trackback->error = t('The sender page does not refer to recipient site.');
    }
  }

  if (empty($trackback->error)) {
    $trackback->nid = $node->nid;
    $trackback->created = time();
    $trackback->site = ip_address();

    list($name, $subject, $excerpt) = _trackback_optional_params('blog_name', 'title', 'excerpt');
    $trackback->name = strip_tags($name ? $name : $_REQUEST['url']);
    $trackback->subject = $subject ? $subject : $_REQUEST['url'];
    $trackback->url = $_REQUEST['url'];
    $trackback->excerpt = strlen($excerpt) > 255 ? truncate_utf8($excerpt, 252) .'...' : $excerpt;
    $trackback->status = (variable_get('trackback_moderation', 0) == 0) ? 1 : 0;

    trackback_invoke_trackback($trackback, 'receive');
  }
  return $trackback;
}

function _trackback_optional_params() {
  $args = func_get_args();
  foreach ($args as $i) {
    $params[] = isset($_REQUEST[$i]) ? $_REQUEST[$i] : '';
  }
  if (preg_match('/;\s*charset=([^\s;]+)/i', $_SERVER['CONTENT_TYPE'], $match)) {
    $charset = $match[1];
  }
  else {
    $utf8 = '/^(?:[\s\x21-\x7F]|[\xC2-\xDF][\x80-\xBF]|[\xE0-\xEF][\x80-\xBF]{2}|[\xF0-\xF7][\x80-\xBF]{3}|[\xF8-\xFB][\x80-\xBF]{4}|[\xFC-\xFD][\x80-\xBF]{5})*$/';
    $sample = implode(' ', $params);
    if (!preg_match($utf8, $sample)) {
      global $locale;
      $defaults = array(
        'be' => 5, 'cs' => 2, 'el' => 7, 'hr' => 2, 'hu' => 2, 'pl' => 2,
        'ro' => 2, 'ru' => 5, 'sk' => 2, 'sl' => 2, 'tr' => 9, 'uk' => 5,
        'ja' => array('ISO-2022-JP', 'EUC-JP', 'SJIS'),
        'ko' => array('ISO-2022-KR', 'EUC-KR'),
        'zh-hans' => array('HZ', 'EUC-CN'),
        'zh-hant' => array('BIG-5', 'EUC-TW')
      );
      if ($charset = $defaults[$locale] and is_array($charset)) {
        if (function_exists('mb_detect_encoding')) {
          $charset = @mb_detect_encoding($sample, $charset);
        }
        else {
          foreach ($charset as $charset) {
            if (drupal_convert_to_utf8($sample, $charset) != '') break;
            $charset = NULL;
          }
        }
      }
      if (!$charset) {
        $charset = 'ISO-8859-1'; // or 'ISO-8859-15'
      }
      elseif (!is_string($charset)) {
        $charset = 'ISO-8859-'. $charset;
      }
    }
  }
  if ($charset && strcasecmp($charset, 'UTF-8') != 0) {
    foreach ($params as $i => $t) {
      if ($t != '') {
        $t = drupal_convert_to_utf8($t, $charset);
        if ($t != '') {
          $params[$i] = $t;
        }
      }
    }
  }
  return $params;
}

function trackback_exit() {
  global $_trackback_ping_node;
  if ($_trackback_ping_node) {
    _trackback_send($_trackback_ping_node);
  }
}

function _trackback_send($node) {
  $urls = array();
  if (!empty($node->trackback_urls)) {
    foreach (explode("\n", $node->trackback_urls) as $url) {
      if ($url = trim($url)) {
        $urls[$url] = TRUE;
      }
    }
  }
  if ($node->status && variable_get('trackback_auto_detection_enabled', 0) == 1) {
    $urls += trackback_urls_via_nodebody($node);
  }
  $retry = array();
  if (isset($node->trackback_urls_to_retry)) {
    $retry = array_diff($node->trackback_urls_to_retry, array(0));
  }
  _trackback_ping($node, $urls, $retry);
}

function trackback_urls_via_nodebody($node) {
  $trackback_urls = array();

  // First, grab anything that looks like a url from the body of the node.
  $node = _trackback_build_content(drupal_clone($node));
  $content = drupal_render($node->content);
  $pattern = '((?:http|https)://[a-z0-9;/?:@&=+#$,_.!~*()%-]+)';

  if (variable_get('trackback_link_only', 0)) {
    $content = strip_tags($content, '<a>'); // remove comment.
    $pattern = '<a\s+[^>]*href\s*=\s*(?:"|\')'. $pattern;
  }

  if (preg_match_all('`'. $pattern .'`i', $content, $parsed_urls)) {
    $parsed_urls = array_unique($parsed_urls[1]);
    foreach ($parsed_urls as $url) {

      // Now, send http HEAD requests so we can see if the content type is something that *might* contain autodetection text.
      // In other words, check if Content-Type of each URL is text based rather than digital.
      $url = html_entity_decode($url, ENT_QUOTES);
      if (_trackback_url_parsable_content($url)) {

        //Finally, download each page, scan each, and compile a list of all the trackback URLs listed in the first RDF of each scanned page.
        $reply = drupal_http_request($url);
        if (empty($reply->error)) {
          $url = preg_replace('/.*<rdf:RDF.*trackback:ping="([^"]+)".*<\/rdf:RDF>.*/s', '\1', $reply->data);
          if (_trackback_valid_url($url)) {
            $trackback_urls[$url] = FALSE;
          }
        }
      }
    }
  }
  return $trackback_urls;
}

// Since autodetection might encounter a link to a media file, we first want to make a
// simple 'HEAD' HTTP request instead of an actual GET.  This results in having to make
// an extra drupal_http_request() later for an actual GET, but it is worth it considering
// the strong likelihood that auto-detection may encounter a URL that links to a media file.
function _trackback_url_parsable_content($url) {
  global $base_url;

  if (!strstr($url, $base_url)) {
    $http_reply = drupal_http_request($url, array(), 'HEAD');
    $content_type = $http_reply->headers['Content-Type'];

    return (substr_count($content_type, 'text/html') || substr_count($content_type, 'application/xhtml+xml') || substr_count($content_type, 'application/xml') || substr_count($content_type, 'text/xml'));
  }
}

function _trackback_ping($node, $urls, $force = array()) {
  if ($urls) {
    $result = db_query('SELECT url FROM {trackback_sent} WHERE nid=%d', $node->nid);
    while ($sent = db_fetch_object($result)) {
      unset($urls[$sent->url]);
    }
  }
  $urls += $force;
  if ($urls) {
    $node = _trackback_build_content($node, TRUE);
    $excerpt = drupal_render($node->content);
    $excerpt = preg_replace(array('/<p\b/i', '/<div\b/i'), array("\n\$0", ' $0'), $excerpt);
    $params = array(
      'title' => $node->title,
      'excerpt' => truncate_utf8(trim(strip_tags($excerpt)), 255),
      'blog_name' => variable_get('site_name', ''),
      'url' => url('node/'. $node->nid, array('absolute' => TRUE))
    );
    $query = array();
    foreach ($params as $key => $value) {
      $query[] = $key .'='. urlencode($value);
    }
    $query = implode('&', $query);
    foreach ($urls as $url => $type) {
      $reply = drupal_http_request($url, array('Content-Type' => 'application/x-www-form-urlencoded; charset=utf-8'), 'POST', $query);
      $succ = 0;
      if (empty($reply->error) && preg_match('|<error>([0-9]+)</error>|', $reply->data, $match)) {
        $succ = $match[1] ? 0 : 1;
      }
      db_query("UPDATE {trackback_sent} SET successful=%d WHERE nid=%d AND url='%s'", $succ, $node->nid, $url);
      if (!db_affected_rows()) {
        db_query("INSERT INTO {trackback_sent} (nid, url, successful) VALUES (%d, '%s', %d)", $node->nid, $url, $succ);
      }
    }
  }
}
