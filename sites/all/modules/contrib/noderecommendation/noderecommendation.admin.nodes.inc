<?php
// $Id: noderecommendation.admin.nodes.inc,v 1.1.2.1 2009/06/01 16:23:08 hanoii Exp $

function noderecommendation_admin_node_edit_submit($form, &$form_state) {
  db_query("DELETE FROM {noderecommendation_node} WHERE nid = %d", $form_state['values']['nid']);
  unset($form_state['values']['op']);
  unset($form_state['values']['buttons']);
  unset($form_state['values']['form_token']);
  unset($form_state['values']['form_id']);
  db_query("INSERT INTO {noderecommendation_node} (nid, data) VALUES (%d, '%s')", $form_state['values']['nid'], serialize($form_state['values']));

  drupal_set_message(t('The recommendation settings have been saved.'));
}

function noderecommendation_admin_node_delete_submit($form, &$form_state) {
  db_query("DELETE FROM {noderecommendation_node} WHERE nid = %d", $form_state['values']['nid']);
  drupal_set_message(t('The recommendation settings have been deleted for this node, defaults will be used.'));

  // set the redirect
  $form_state['redirect'] = 'node/' . $form_state['values']['nid'] . '/noderecommendation';
}

function noderecommendation_admin_node_delete(&$form_state, $node) {
  $form['nid'] = array('#type' => 'value', '#value' => $node->nid);
  $cancel_path = 'node/' . $node->nid . '/noderecommendation';
  if (isset($_REQUEST['destination'])) {
    $cancel_path = $_REQUEST['destination'];
  }
  return confirm_form($form, t('Are you sure you want to delete the recommendation settings for %title?', array('%title' => $node->title)), $cancel_path, null, t('Delete'), t('Cancel'));
}

function noderecommendation_admin_node_edit(&$form_state, $node) {
  drupal_set_title($node->title);
  $recommendation = _noderecommendation_get($node->nid);

  $form = array('#tree' => TRUE);
  $form['nid'] = array('#type' => 'value', '#value' => $node->nid);

  $form += noderecommendation_admin_form($recommendation, $node);

  $form['buttons']['#weight'] = 5;
  $form['buttons']['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  $form['buttons']['reset'] = array('#value' => l(t('Reset to defaults'), 'node/'.$node->nid.'/noderecommendation/delete'));

  /*
  $form['preview'] = array(
    '#type' => 'fieldset',
    '#title' => t('Preview'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('This a preview of what the recommendation block will contain when shown.'),
    '#weight' => 10,
  );
  $form['preview']['block'] = array(
    '#type' => 'markup',
    '#value' => noderecommendation_block_recommendations($nid),
  );
  */

  return $form;
}