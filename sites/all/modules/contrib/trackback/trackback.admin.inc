<?php
// $Id: trackback.admin.inc,v 1.1.2.2 2008/12/09 15:40:56 thepanz Exp $

function _trackback_help($path, $arg) {
  switch ($path) {
    case 'admin/help#trackback':
      $output = '<p>'. t('The trackback module allows users to give a blog post a contextual link to another. A context is made because the trackbacking poster is, in theory, writing about something mentioned on another blogger\'s trackbacked post.  Using the trackback URL accompanying each post, another website can send a ping to your website. Once received, the trackback will show up on the trackback page accompanying the post.  It also includes auto-discovery, spam moderation queues, and the ability to manually ping another site.') .'</p>';
      $output .= '<p>'. t('If trackback autodisovery is enabled on your website, someone need only visit your post via a link from another website post to have trackback <em>discover</em> the linking site and create the trackback. Trackback auto-discovery also works internally within a website, automatically creating connections between pages which link to each other. To manually send a ping to another site, edit your post and use the trackback URL field at the bottom of the edit page to submit the trackback URL for the post on the other site. Once you enter submit, your website will ping the other site for you. With trackback autodiscovery enabled, your site will attempt to do this automatically without your intervention.') .'</p>';
      $output .= '<p>'. t('To enable the moderation queue, go to the administer trackbacks page and select the configure tab. To view, approve, or delete trackbacks awaiting moderation, go to the administer trackbacks page and select the approval queue.  To administer the trackback settings for a particular content type go to that content types administration page.') .'</p>';
      $output .= '<p>'. t('For more information please read the configuration and customization handbook <a href="@trackback">Trackback page</a>.', array('@trackback' => 'http://www.drupal.org/handbook/modules/trackback')) .'</p>';
      return $output;
  }
}

/**
 * Menu callback; present an administrative trackback listing.
 */
function trackback_admin_overview(&$form_state, $type = 'new') {
  
  // Disable Approval Queue if moderation isn't enabled
  if(variable_get('trackback_moderation', 0) == 0 && $type == 'approval') {
    $form = array();
    $form['trackback_message'] = array(
      '#value' => t('Approval queue is disabled, enable in !url', array('!url' => l('Trackback settings', 'admin/content/trackback/settings'))),      
    );
    return $form;
  }
  
  $operations = array();
  if ($type == 'new') {
    $operations['unpublish'] = t('Unpublish the selected trackbacks');
  }
  else {
    $operations['publish'] = t('Publish the selected trackbacks');
  }

  if (TRACKBACK_WITH_SPAM) {
    $operations['_trackback_as_spam'] = t('Mark the selected trackbacks as spam');
    $operations['_trackback_as_notspam'] = t('Mark the selected trackbacks as not spam');
  }
  $operations['delete'] = t('Delete the selected trackbacks (no confirmation)');

  $form = array();
  $form['update'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    'operations' => array('#type' => 'value', '#value' => $operations)
  );
  $form['update']['operation'] = array(
    '#prefix' => '<div class="container-inline">',
    '#type' => 'select',
    '#options' => $operations
  );
  $form['update']['op'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
    '#suffix' => '</div>'
  );

  $form['trackbacks'] = array('#theme' => 'trackback_admin_table');
  $form['trackbacks']['header'] = array(
    '#type' => 'value',
    '#value' => trackback_admin_table_header($type)
  );

  $status = ($type == 'approval') ? 0 : 1;
  if (TRACKBACK_WITH_SPAM) {
    $spam_threshold = (int)variable_get('spam_threshold', 80);
    $sql = 'SELECT tr.*, s.probability FROM {trackback_received} tr LEFT JOIN {spam_tracker} s ON s.source = \'trackback\' AND tr.trid = s.id WHERE '. ($type == 'spam' ? 's.probability >= '. $spam_threshold : 'tr.status = '. $status);
  }
  else {
    $sql = 'SELECT tr.* FROM {trackback_received} tr WHERE tr.status = '. $status;
  }
  $sql .= tablesort_sql($form['trackbacks']['header']['#value']);
  $result = pager_query($sql, 50);

  $form['trackbacks']['status'] = array('#tree' => TRUE);
  while ($trackback = db_fetch_object($result)) {
    $form['trackbacks']['status'][$trackback->trid] = array('#type' => 'checkbox');
    $form['trackbacks'][$trackback->trid] = array();
    $form['trackbacks'][$trackback->trid][] = array('#value' => _trackback_path($trackback, $trackback->subject, array('attributes' => array('title' => truncate_utf8($trackback->excerpt, 128)))) .' '. theme('mark', node_mark($trackback->nid, $trackback->created)));
    $form['trackbacks'][$trackback->trid][] = array('#value' => truncate_utf8($trackback->name, 15, FALSE, TRUE));
    $form['trackbacks'][$trackback->trid][] = array('#value' => $trackback->site);
    if (TRACKBACK_WITH_SPAM) {
      if ($type == 'spam') {
        $form['trackbacks'][$trackback->trid][] = array('#value' => $trackback->status != 0 ? t('Published') : t('Not published'));
      }
      else {
        $form['trackbacks'][$trackback->trid][] = array('#value' => $trackback->probability >= $spam_threshold ? t('Spam') : t('Not Spam'));
      }
    }
    $form['trackbacks'][$trackback->trid][] = array('#value' => format_date($trackback->created, 'small'));
    $form['trackbacks'][$trackback->trid][] = array('#value' => l(t('edit'), 'trackback/edit/'. $trackback->trid));
    $form['trackbacks'][$trackback->trid][] = array('#value' => l(t('delete'), 'trackback/delete/'. $trackback->trid));
  }
  $form['pager'] = array('#value' => theme('pager', NULL, 50, 0));

  return $form;
}

function trackback_admin_overview_validate($form_id, $form_state) {
  $status = array();
  if (isset($form_state['values']['status'])) {
    $status = array_diff((array)$form_state['values']['status'], array(0));
  }
  if (count($status) == 0) {
    form_set_error('', t('Please select one or more trackbacks to perform the update on.'));
    drupal_goto($_GET['q']);
  }
}

function trackback_admin_overview_submit($form_id, $form_state) {
  if (isset($form_state['values']['operations'][$form_state['values']['operation']])) {
    foreach ($form_state['values']['status'] as $trid => $value) {
      if ($value) {
        _trackback_operation($form_state['values']['operation'], $trid, FALSE);
      }
    }
    cache_clear_all();
    drupal_set_message(t('The update has been performed.'));
  }
}

function trackback_admin_table_header($type) {
  $header = array();
  $header[] = theme('table_select_header_cell');
  $header[] = array('data' => t('Subject'), 'field' => 'tr.subject');
  $header[] = array('data' => t('Author'), 'field' => 'tr.name');
  $header[] = array('data' => t('Host'), 'field' => 'tr.site');
  if (TRACKBACK_WITH_SPAM) {
    if ($type == 'spam') {
      $header[] = array('data' => t('Status'), 'field' => 'tr.status');
    }
    else {
      $header[] = array('data' => t('Spam'), 'field' => 's.probability');
    }
  }
  $header[] = array('data' => t('Time'), 'field' => 'created', 'sort' => 'desc');
  $header[] = array('data' => t('Operations') , 'colspan' => '2');
  return $header;
}

function theme_trackback_admin_table($form) {
  $header = $form['header']['#value'];
  $rows = array();
  foreach (element_children($form['status']) as $key) {
    $row = array(drupal_render($form['status'][$key]));
    foreach (element_children($form[$key]) as $column_key) {
      $row[] = drupal_render($form[$key][$column_key]);
    }
    $rows[] = $row;
  }
  if (count($rows) == 0) {
    $rows[] = array(array('data' => t('No trackbacks available.'), 'colspan' => count($header)));
  }
  return theme('table', $header, $rows);
}

function trackback_configure() {
  $form['trackback_auto_detection'] = array(
    '#type' => 'fieldset',
    '#title' => t('Auto-detection'),    
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Auto-detection for pings to other sites.'),
  );
  
  $form['trackback_auto_detection']['trackback_auto_detection_enabled'] = array(
    '#type' => 'radios',
    '#default_value' => variable_get('trackback_auto_detection_enabled', 0),
    '#options' => array(
      t('Disabled'),
      t('Enabled'),
      t('Run auto-detection on cron')
    ),
    '#description' => t('If auto-detection is enabled, each URL in any posted content (whether in textile, link, or plain-text form) will be checked for a trackback URL upon submission. For each URL in the body of the posted content, trackback will check to see if that URL accepts trackbacks from other sites. If a URL accepts trackbacks, trackback will ping the trackback URL found on that page if one has been posted at that URL.<br>*note: This has the potential to take a very long time depending on the amount of links you have in your posts. Using the \'Run auto-detection on cron\' option delays the most time consuming part of the process to when cron is run on the site. This speeds perfomance when editing and creating content, but delays trackbacks until cron is run.'),    
  );
  
  $form['trackback_auto_detection']['trackback_link_only'] = array(
    '#type' => 'checkbox',
    '#title' => t('Link only'),
    '#default_value' => variable_get('trackback_link_only', 0),
    '#description' => t('If checked, auto-detection will check link only.')
  );
  
  $form['trackback_retention_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Trackback retention settings'),
    //'#description' => t(''),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  $form['trackback_retention_settings']['trackback_moderation'] = array(
    '#type' => 'radios',
    '#title' => t('Trackback moderation'),
    '#default_value' => variable_get('trackback_moderation', 0),
    '#options' => array(t('Disabled'), t('Enabled')),
    '#description' => t('Enabling moderation forces every received trackback to be approved before it will appear on your site. The moderation queue can then be viewed on the !linked_page.', array('!linked_page' => l(t('trackback administration page'), 'admin/content/trackback/list/approval')))
  );
  
  $form['trackback_retention_settings']['trackback_reject_oneway'] = array(
    '#type' => 'radios',
    '#title' => t('Reject one-way trackbacks'),
    '#default_value' => variable_get('trackback_reject_oneway', 0),
    '#options' => array(t('Disabled'), t('Enabled')),
    '#description' => t('If enabled, trackbacks that the sender page does not refer to your site will be rejected.')
  );

  // Setting for Trackback display (trackbacks and Trackback-URL)
  $form['trackback_display'] = array(
    '#type' => 'fieldset',
    '#title' => t('Trackback display'),
    '#description' => t('Settings to configure Trackback displaing modes'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  // Setting to hide/show trackback counts in links
  $form['trackback_display']['trackback_links_count_display'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display Trackback count in links'),
    '#default_value' => variable_get('trackback_links_count_display', 1),
  );

  // Location to display Trackbacks
  $form['trackback_display']['trackback_view'] = array(
    '#type' => 'radios',
    '#title' => t('Location of received trackbacks'),
    '#default_value' => variable_get('trackback_view', 0),
    '#options' => array(
      t('Display below post'),
      t('Display on separate page'),
      t('Display in block')
    ),
  );

  // Setting to place Trackback URL
  $form['trackback_display']['trackback_url_display'] = array(
    '#type' => 'radios',
    '#title' => t('Trackback URL display position'),
    '#description' => t('Setting for displayed trackback URL in node view. RDF tags will always be added in HTML code.'),    
    '#default_value' => variable_get('trackback_url_display', 'default'),
    '#options' => array(
      'default' => t('Below post'),
      'hide' => t('Hide from post'),
      'links' => t('Display in links'),
      'links-teaser' => t ('Display in links (only in teaser view)'),
      'links-full' => t ('Display in links (only in full-node view)')
    ),
  );
  
  
  /**
   * Mass edit for node-type setting
   */
  $form['trackback_nodetypes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Node types'),
    '#description' => t('Mass-Edit of node type trackback settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $node_types = node_get_types();
  foreach($node_types as $key => $type) {
    $form['trackback_nodetypes']['trackback_' . $key] = array(
      '#type' => 'checkbox',      
      '#default_value' => _trackback_node_type($key),
      '#title' => $type->name,
    );
  }  
  
  $form['#tree'] = FALSE;
  return system_settings_form($form);
}

function trackback_delete(&$form_state, $tb) {
  return confirm_form(
    array('trackback' => array('#type' => 'value', '#value' => $tb)),
    t('Are you sure you want to delete the trackback %title?', array('%title' => $tb->subject)),
    _trackback_path($tb), t('This action cannot be undone.'), t('Delete'), t('Cancel')
  );
}

function trackback_delete_submit($form_id, &$form_state) {
  $tb = $form_state['values']['trackback'];
  _trackback_operation('delete', $tb);
  $form_state['redirect'] = array(_trackback_path($tb), NULL, 'trackbacks');
}

function trackback_edit(&$form_state, $tb) {
  $node = _trackback_build_content(node_load($tb->nid), TRUE);
  $node->teaser = drupal_render($node->content);
  $form = array('trackback' => array('#type' => 'value', '#value' => $tb));
  $form['preview'] = array(
    '#prefix' => '<div class="preview">',
    '#value' => theme('trackback', $tb, module_invoke_all('link', 'trackback', $tb, TRUE)),
    '#suffix' => '</div>'
  );
  $form['status'] = array(
    '#type' => 'radios',
    '#title' => t('Status'),
    '#default_value' => $tb->status,
    '#options' => array(1 => t('Published'), 0 => t('Not published'))
  );
  $form['op'] = array('#type' => 'submit', '#value' => t('Save'));
  $form['node'] = array(
    '#value' => theme('box', t('This trackback is in response to: '), theme('node', $node, TRUE, FALSE))
  );
  return $form;
}

function trackback_edit_submit($form_id, &$form_state) {
  $status = $form_state['values']['status'];
  $tb = $form_state['values']['trackback'];
  if ($status != $tb->status) {
    if ($status) {
      _trackback_operation('publish', $tb);
      drupal_set_message(t('The trackback is now published.'));
    }
    else {
      _trackback_operation('unpublish', $tb);
      drupal_set_message(t('The trackback was unpublished.'));
    }
  }
  $form_state['redirect'] = array(_trackback_path($tb), NULL, 'trackbacks');
}
