<?php
/**
 * Revert upsell's block configuration
 */
function uc_upsell_block_config_revert() {
  uc_upsell_reset_config('block');
  drupal_set_message('Upsell Block configuration reverted to defaults');
}


/**
 * Renders upsell block
 */
function uc_upsell_block_contents($id = 0) {

  // Get related cart items (sets $msg too)
  if ($nodes = uc_upsell_resolve_associates($msg)) {
    drupal_add_css(drupal_get_path('module', 'uc_upsell') .'/uc_upsell.css');

    // Loop through the nodes and generate a link with thumbnail.
    foreach ($nodes as $node) {
      $content[] = theme('upsell_item', $node);
    }
    return array('subject' => $msg, 'content' => '<div id="upsell-block-'.$id.'"><div class="upsell-block-product">'. implode('</div><div class="upsell-block-product">', $content) .'</div></div>', 'related' => $nodes);
  }
  else {
    return array();
  }
}

/**
 * Implementation of hook_block
 */
function uc_upsell_block($op = 'list', $delta = 0, $edit = array()) {
  $config = uc_upsell_get_config();

  switch ($op) {
    case 'list':
      $blocks[0] = array(
        'info' => t('Upsell Items'),
        'cache' => BLOCK_CACHE_PER_PAGE,
        'pages' => 'node/*',
        'visibility' => 1,
      );
      return $blocks;

    case 'configure':
      $form = array();
      if ($delta == 0) {

        $form['upsell'] = array(
          '#type' => 'fieldset',
          '#title' => t('Upsell specific settings'),
          '#collapsible' => FALSE,
          '#collapsed' => FALSE,
          '#description' => '<p>Configure how this block lists related products, for product and non-product pages.',
        );

        // Add admin link if appropriate
        if (user_access('administer upsell settings')) {
          $form['upsell']['#description'] .= ' Other options can be configured on the '. l('Upsell Settings', 'admin/store/settings/upsell') .' page.';
        }

        $form['upsell']['#description'] .= '</p>'; // Close p tag in description
        $form['upsell']['upsell_enable'] = array(
          '#type' => 'radios',
          '#title' => 'Enable for',
          '#description' => 'Select whether this block will appear on product nodes only and/or all other drupal pages. This setting does not override your block visibility settings (below), but determines - when visible - what the block displays.',
          '#options' => array(
            '0' => 'Product pages',
            '1' => 'Non-product pages',
            '2' => 'Both product and non-product pages',
          ),
          '#default_value' => $config['block']['enable'],
          '#required' => TRUE,
        );
        $form['upsell']['use_default'] = array(
          '#type' => 'checkbox',
          '#multiple' => FALSE,
          '#title' => t('If no product image exists, use the default'),
          '#default_value' => $config['block']['use_default'],
          '#description' => t('Check this option to use the Default Product Image; otherwise, no Image will be shown if none exists for the product.'),
          '#required' => FALSE,
        );
        $form['upsell']['upsell_max'] = array(
          '#type' => 'textfield',
          '#title' => 'Number to display',
          '#description' => 'How many products to list in this block',
          '#default_value' => $config['block']['max'],
          '#length' => 2,
          '#size' => 3,
          '#required' => TRUE,
        );
        $form['upsell']['upsell_block_weight'] = array(
          '#type' => 'textfield',
          '#title' => 'Weight',
          '#description' => 'How heavy to make the Upsell block',
          '#default_value' => variable_get('uc_upsell_block_weight', 20),
          '#length' => 2,
          '#size' => 3,
          '#required' => FALSE,
        );
        $form['upsell']['product'] = array(
          '#type' => 'fieldset',
          '#title' => t('Product Page configuration'),
          '#collapsible' => TRUE,
          '#collapsed' => $config['block']['enable'] == 1,
          '#description' => '<p>Configure this block when displayed on product pages. The currently view product node is the source for resolving related nodes. Below you may specify the secondary sources, and message to show the user when products are found.</p>',
        );
        $form['upsell']['product']['upsell_product_msg_related'] = array(
          '#type' => 'textfield',
          '#title' => 'Default product message',
          '#description' => 'The message displayed when all found items are related to the current product.',
          '#default_value' => $config['block']['product']['msg_related'],
          '#length' => 40,
          '#required' => FALSE,
        );
        $form['upsell']['product']['upsell_product_msg_mixed'] = array(
          '#type' => 'textfield',
          '#title' => 'Alternate product message',
          '#description' => 'The message displayed when one or more found items are not related to the current product.',
          '#default_value' => $config['block']['product']['msg_mixed'],
          '#length' => 40,
          '#required' => FALSE,
        );
        $form['upsell']['product']['upsell_product_use_cart'] = array(
          '#type' => 'checkbox',
          '#multiple' => FALSE,
          '#title' => t('Supplement with products related to the user\'s cart'),
          '#default_value' => $config['block']['product']['use_cart'],
          '#description' => t('Check this option and items related to the user\'s cart will be listed when there are not enough items associated with the current product.'),
          '#required' => FALSE,
        );
        $form['upsell']['product']['upsell_product_use_random'] = array(
          '#type' => 'checkbox',
          '#multiple' => FALSE,
          '#title' => t('Supplement with random products'),
          '#default_value' => $config['block']['product']['use_random'],
          '#description' => t('Check this option and random products will be listed when there are not enough associated items (or cart items, if that option is selected).'),
          '#required' => FALSE,
        );
        $form['upsell']['node'] = array(
          '#type' => 'fieldset',
          '#title' => t('Non-Product Page configuration'),
          '#collapsible' => TRUE,
          '#collapsed' => $config['block']['enable'] == 0,
          '#description' => '<p>Configure this block when shown on non-product pages.</p>',
        );
        $form['upsell']['node']['upsell_node_req_cart'] = array(
          '#type' => 'checkbox',
          '#multiple' => FALSE,
          '#title' => t('Disable when cart is empty'),
          '#default_value' => $config['block']['node']['req_cart'],
          '#description' => t('Check this option and this block will only appear when the user has items in their cart.'),
          '#required' => FALSE,
        );
        $form['upsell']['node']['upsell_node_msg_related'] = array(
          '#type' => 'textfield',
          '#title' => 'Default message',
          '#description' => 'The message displayed when all found items are related to the user\'s cart.',
          '#default_value' => $config['block']['node']['msg_related'],
          '#length' => 40,
          '#required' => FALSE,
        );
        $form['upsell']['node']['upsell_node_msg_mixed'] = array(
          '#type' => 'textfield',
          '#title' => 'Alternate message',
          '#description' => 'The message displayed when one or more found items are not related to the user\'s cart.',
          '#default_value' => $config['block']['node']['msg_mixed'],
          '#length' => 40,
          '#required' => FALSE,
        );
        $form['upsell']['node']['upsell_node_source'] = array(
          '#type' => 'radios',
          '#title' => 'List associate nodes from',
          '#description' => 'Select whether this block will list products related to the user\'s cart, a random selection or both. <b>Note:</b> Only the alternate message is displayed if choosing "Random selection".',
          '#options' => array(
            '0' => 'User cart',
            '1' => 'Random selection',
            '2' => 'User cart, supplemented by random selection',
          ),
          '#default_value' => $config['block']['node']['source'],
          '#required' => FALSE,
        );
      }

      return $form;

    case 'save':
      switch ($delta) {
        case 0:
          $config['block']['enable'] = $edit['upsell_enable'];
          $config['block']['max'] = $edit['upsell_max'];
          $config['block']['use_default'] = $edit['use_default'];
          $config['block']['product']['msg_related'] = $edit['upsell_product_msg_related'];
          $config['block']['product']['msg_mixed'] = $edit['upsell_product_msg_mixed'];
          $config['block']['product']['use_cart'] = $edit['upsell_product_use_cart'];
          $config['block']['product']['use_random'] = $edit['upsell_product_use_random'];
          $config['block']['node']['req_cart'] = $edit['upsell_node_req_cart'];
          $config['block']['node']['msg_related'] = $edit['upsell_node_msg_related'];
          $config['block']['node']['msg_mixed'] = $edit['upsell_node_msg_mixed'];
          $config['block']['node']['source'] = $edit['upsell_node_source'];
          variable_set('uc_upsell_block_weight', $edit['upsell_block_weight']);
          uc_upsell_set_config($config);
        break;
      }
    break;

    case 'view':
      default:
        $block = array();
        $matches = array();

        // Only show block if not on cart pages
        if (!preg_match('/^cart\/?/', $_GET['q'], $matches)) {
          switch ($delta) {
            case 0:
              $block = uc_upsell_block_contents($delta);
            break;
          }
        }
      return $block;
    break;
  }
}


/**
 * Build the multibuy form
 */
function uc_upsell_multibuy_form($form_id, $nodes) {
  $config = uc_upsell_get_config();
  $cartnids = uc_upsell_get_cart_nids();
  
  $form = array();
  $parent_node = menu_get_object();
  $parent_nid = $parent_node->nid;
  $form['upsell_parent_nid'] = array(
    '#type' => 'hidden',
    '#value' => $parent_nid,
  );
  foreach ($nodes as $node) {
    $form['product_upsell']['node_display--'. $node->nid] = array(
      '#type' => 'markup',
      '#value' => theme('upsell_item', $node),
      '#prefix' => '<div class="upsell-block-product">',
    );
    if (!in_array($node->nid, $cartnids)) {
      $form['product_upsell']['upsell_add_to_cart_nid--'. $node->nid] = array(
        '#type' => 'checkbox',
        '#title' => t('Include this !type', array('!type' => $node->type)),
        '#prefix' => '<div class="upsell_add_to_cart_checkbox">',
        '#suffix' => '</div></div>',
      );
    }
    else {
      $form['product_upsell']['upsell_in_cart--'. $node->nid] = array(
        '#type' => 'markup',
        '#value' => t('In your cart'),
        '#prefix' => '<div class="upsell_add_to_cart_checkbox in_cart">',
        '#suffix' => '</div></div>',
      );
    }
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Purchase these along with !title', array('!title' => '"'. $parent_node->title .'"')),
    '#weight' => 50,
    '#prefix' => '<div class="upsell_submit">',
    '#suffix' => '</div>'
  );
  return $form;  
}

/**
 * Submit handler
 */
function uc_upsell_multibuy_form_submit($form_id, &$form_state) {
  $add_nids = array();
  $add_nids[] = $form_state['values']['upsell_parent_nid'];
  
  foreach ($form_state['values'] AS $key => $value) {
    $parts = explode('--', $key);
    if($parts[0] == 'upsell_add_to_cart_nid' && $value == 1) {
      // Build the array of nids to add
      $add_nids[] = $parts[1];
    }
  }
    
  // Add these products to the cart
  foreach($add_nids as $nid) {
    uc_cart_add_item($nid);
  }
  
  drupal_goto('cart');
}