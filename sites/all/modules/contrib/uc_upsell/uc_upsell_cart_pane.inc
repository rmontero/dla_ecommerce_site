<?php
/**
 * Implementation of hook_cart_pane().
 */
function uc_upsell_cart_pane($items) {

  $panes[] = array(
    'id' => 'upsell_cart_form',
    'title' => t('Upsell Items &nbsp;'. l('(pane options)', 'admin/store/settings/upsell', array('attributes' => array('title' => 'configure the upsell pane')))),
    'enabled' => TRUE,
    'weight' => 1,
    'body' => uc_upsell_cart_pane_content($items),
  );
  return $panes;
}


/**
 * Get parts for cart pane
 */
function uc_upsell_cart_pane_content($data = '') {
  $related = uc_upsell_resolve_associates($data); // get related cart items (sets $msg too)
  return theme('pane_upsell', $related, $data);
}



/**
 * Theme function to render pane on cart page.
 */
function theme_pane_upsell($products, $msg = 'Related Products') {
  global $conf;
  if (empty($products)) return '';

  drupal_add_css(drupal_get_path('module', 'uc_upsell') .'/uc_upsell.css');
  $output = '<div id="cart-related-products"><p>'. t($msg) .'</p>';

  $output .= '<table id="related" width="100%"><tr><td align="center">';

  // Render a link and thumbnail for each product
  foreach ($products as $related) {
    $output .= '<div class="cart-related-item">';
    $output .= theme('upsell_item', $related);
    $output .= '</div>';
  }

  $output .= '</td></tr></table>';
  $output .= '</div>';

  return $output;
}
