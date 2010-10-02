<?php
// $Id: theme.inc,v 1.1.2.1 2009/09/29 13:40:01 madsph Exp $

/**
 * @file
 * Patch by hanoii
 */

/**
 * Template helper for theme_views_view_row_node
 */
function template_preprocess_uc_views_view_row_invoice(&$vars) {
  require_once drupal_get_path('module', 'uc_order') . '/uc_order.admin.inc';
  $order_id = $vars['row']->{$vars['field_alias']};
  $order = uc_order_load($order_id);

  $invoice = uc_order_invoice($order);
  $vars['invoice'] = $invoice;
}