<?php
// $Id$

/**
 * @file
 *   Provides an edit link to Views.
 */

/**
 * Field handler to add a field with an edit link.
 */
class facebook_status_views_handler_field_delete extends views_handler_field {
  function render($values) {
    $status = facebook_status_load($values->facebook_status_sid);
    if (_facebook_status_can_edit($status, TRUE)) {
      drupal_add_css(drupal_get_path('module', 'facebook_status') .'/facebook_status.css');
      $q = $_GET['q'];
      if ($q == 'facebook_status/js') {
        $q = 'user';
      }
      return '<span class="facebook_status_edit_delete">'. l(t('Delete'), 'statuses/'. $status->sid .'/delete', array('query' => array('destination' => $q))) .'</span>';
    }
  }
}