<?php
/**
 * Implementation of hook_form_alter()
 *
 * Adds the list of Related Products to a node_product edit form.
 */
function uc_upsell_form_alter(&$form, $form_state, $form_id) {
  global $user;

  // Add form associations field to base - but only for users with given permissions.
  if (user_access('administer upsell settings')) {

    // reduce to form's node type
    $node_type = preg_replace('/_node_form$/i', '', $form_id);
    // get list of product types
    $product_types = array_keys(uc_product_node_info());
    $product_types[] = 'product_kit';

    // Get all currently associated products - besides this one...
    if (in_array($node_type, $product_types)) {
      $current_prods_array = db_result(db_query("SELECT related_products FROM {uc_upsell_products} WHERE nid = %d", $form['nid']['#value']));
      $current_prods_array = unserialize($current_prods_array);
      //dsm('current '.print_r($current_prods_array, true));
      $current_nids = array();

      if (is_array($current_prods_array)) {
        foreach ($current_prods_array AS $current_product) {
          $current_nids[] = $current_product;
        }
      }

      // Alphabetize the array (since there's no telling by nid)
      (is_array($current_nids)) ? $current_nids = asort($current_nids) : '' ;

      // Get product types
      $config = uc_upsell_get_config();
      $product_types = array_values($config['global']['types']);
      $products_query = "('". implode("', '", $product_types) ."')";

      // Get all products
      $avail_products = array();
      $available_models = db_query("SELECT n.title, n.nid
        FROM {node} AS n
        WHERE n.type IN $products_query
        AND n.status = 1 AND n.nid != %d
        ORDER BY n.title", intval(arg(1)));

      if ($available_models) {
        while ($file_products = db_fetch_object($available_models)) {
          $avail_products[$file_products->nid] = $file_products->title;
        }
      }

      $assoc_form = array(
        '#type' => 'select',
        '#multiple' => TRUE,
        '#title' => t('Related Products for Upsell'),
        '#weight' => 30,
        '#description' => t('Select related products to offer at checkout.'),
        '#options' => $avail_products,
        '#default_value' => $current_prods_array,
        '#id' => 'upsell',
        );

      $form['base']['relatedProducts'] = $assoc_form;
    }
  }
}


/**
 * Insert or update the db with related products for a particular node
 * @param $nid
 *   The current node id of the product we are updating
 * @param $products
 *   An array of product nids to associate with $nid
 * @return array $reciprocals
 *   Return an array of the reciprocal nids associated with this product, so we can go through and add their relationships

 // 1. Find new vs. Old products in form
 // 2. Go to 'removed' products and remove current product association
 // 3. Update the current db
 // 4. Go to 'set' products and add current product association
 */
function uc_upsell_relateds_add($node) {
  global $user;
  $config = uc_upsell_get_config();

  // Init the array of related products from our form for final insertion into dabase
  $products = array();
  // Get list of product types
  $product_types = array_keys(uc_product_node_info());
  // Include product kits also
  $product_types[] = 'product_kit';

  // Create an array of selected related products and add the current product to the related products lists for each of them
  if (in_array($node->type, $product_types) && is_array($node->relatedProducts)) {

    //dsm('config: '.print_r($config['global']['reciprocal_behavior'], true));

    /** Update reciprocal nodes first *******************/
    if ($config['global']['reciprocal_behavior'] != 'none') {
      foreach ($node->relatedProducts as $product) {
        $ary = array();
        $ary[] = $node->nid;
        $products[] = $product;
  
        $related = _uc_upsell_get_related($product);

        if (!$related || $related == FALSE) {
          // Add a new row for this product
          uc_upsell_insert_nids($product, $ary);
        }
        else {
          if (is_array($ary)) {                  
            $ary = array_merge($ary, $related);
            $ary = array_unique($ary);          
          }
          // Update this product's related nodes (a row already exists)
          uc_upsell_update_nids($product, $ary);
        }

        switch ($config['global']['reciprocal_behavior']) {
          case 'many':
            // Loop through the array of associate nodes and update them with the whole array, if we're configured to do so.
            if (count($products > 0)) {
              foreach ($products AS $key => $nid) {
                $currentnids = _uc_upsell_get_related($nid);
                
                if ($currentnids) {
                  $merge = array_unique(array_merge($currentnids, $products));
                  $merge = array_diff($merge, (array)$nid); // Get rid of the current nid
                  uc_upsell_update_nids($nid, $merge);               
                }
              }
            }
            break;
          
          // If we're doing one-to-one, only update reciprocal nodes with main node.
          case 'one':
            if (count($products > 0)) {
              foreach ($products AS $key => $nid) {
                $currentnids = _uc_upsell_get_related($nid);
                
                // If we already have nids for this product, grab them and update with this master node.
                if ($currentnids) {

                  $merge = array_unique(array_merge($currentnids, (array)$nid));
                  $merge = array_diff($merge, (array)$nid); // Get rid of the current nid
                  uc_upsell_update_nids($nid, $merge); // What if there are not current nids already?                 
                }
                else {
                  uc_upsell_insert_nids($nid, (array)$node->nid);
                }
              }
            }
            break;
        }

        // Get the list of related products for the current product
        $currentnids = _uc_upsell_get_related($node->nid);

        if ($currentnids) {       
          // Delete current product from the related products list for any products that are being removed
          foreach ($currentnids as $currentnid) {
            if (!in_array($currentnid, $node->relatedProducts)) {
              $nidarray = array();
              $nidarray[] = $node->nid;
              $updatenids = _uc_upsell_get_related($currentnid);
              
              if (is_array($updatenids) && count($updatenids) > 0) {
                $updatenids = array_unique(array_diff($updatenids, $nidarray));
              }
             
              uc_upsell_update_nids($currentnid, $updatenids);
              //dsm('REMOVAL: updated xxx => '.$currentnid.' with '.print_r($updatenids, true));
            }
          }
        }
      }
    }

    /** Update or Insert an array of related products for the main node *************/
    $products = array_unique($products);

    // Decide whether we are updating the related products entry or inserting a new one.
    if (!db_result(db_query("SELECT related_products FROM {uc_upsell_products} WHERE nid = %d", $node->nid))) {
      uc_upsell_insert_nids($node->nid, $node->relatedProducts);
    }
    else {
      uc_upsell_update_nids($node->nid, $node->relatedProducts);
    }
    drupal_set_message(t('Related products were updated.'));
  }
}


/**
 * Get a product's related nodes
 * @param $nid
 *   The nid of the node to retrieve associates from
 * @return $currentnids
 *   An array of nodes (or an empty array if nothing is found)
 */
function _uc_upsell_get_related($nid) {
  $currentnids = db_result(db_query("SELECT related_products FROM {uc_upsell_products} WHERE nid = %d", $nid));

  // Only return an actual value if there are related products in the database.
  if (is_array(unserialize($currentnids)) && count(unserialize($currentnids)) > 0) {
    return unserialize($currentnids);
  }
  elseif (count(unserialize($currentnids)) == 0) {
    // If we got an empty array back, delete it.
    db_query("DELETE FROM {uc_upsell_products} WHERE nid = %d", $nid);
    return FALSE;
  }
  else {
    return FALSE;
  }
}

/**
 * @param $removed
 *   The nid of the node whose db table we are updating
 * @param $nid
 *   The nid of the product to remove from the array
 * @return Nothing
 */
function _uc_upsell_remove_related($nid, $removed) {
 // dsm ('removing '.$nid.' from '.$removed);

  $currentnids = _uc_upsell_get_related($nid);

  if ($currentnids) {
    // Get the array minus the nid to be removed and update the db
    $updatenids = array_diff((array)$currentnids, (array)$removed);
    db_query("UPDATE {uc_upsell_products} SET related_products = '%s' WHERE nid = %d", serialize($updatenids), $nid);
  }
}


/**
 * Administer the related products area (associations browser)
 */
function upsell_products_admin() {
  $config = uc_upsell_get_config();
  
  // Let the administrator know that Override settings for Taxonomy handling will make the Associations Browser meaningless.
  if ($config['global']['taxonomy'] && $config['global']['taxonomy'] == 'override') {
    drupal_set_message(t('Your current !settings_link will override the assocations listed below. Currently set to: <strong>%current_setting</strong>',
      array('!settings_link' => l('Upsell taxonomy handling settings', 'admin/store/settings/upsell'), '%current_setting' => $config['global']['taxonomy'])), 'status'
    );
  }
  
  $header = array(
    array('data' => t('Product')),
    array('data' => t('Related nodes')),
    'data' => t('Action'),
  );

  $query = "SELECT n.title, p.nid, up.related_products FROM {uc_products} as p
      LEFT JOIN {node} AS n
      ON n.nid = p.nid
      LEFT JOIN {uc_upsell_products} AS up
      ON up.nid = p.nid
      WHERE n.status = 1
      AND n.type IN ('". implode("','", uc_product_product_types()) ."')
      ORDER BY n.title";

  // Build a paged query for easy viewing
  $pagedquery = pager_query($query . tablesort_sql($header), $config['browser']['pager_size'], 0, NULL);

  while ($product = db_fetch_object($pagedquery)) {
    $related_products = unserialize($product->related_products);
    $titles = '';

    if (is_array($related_products)) {
      foreach ($related_products as $related) {
        if (!empty($titles)) $titles .= ', ';
        $title = db_result(db_query("SELECT title FROM {node} WHERE nid = %d", $related));
        $titles .= l($title, 'node/'. $related .'/edit', array('query' => 'destination='. $_GET['q'] .'?page='. $_GET['page'], 'fragment' => 'upsell'));
      }
    }

    $rows[] = array(
      array('data' => $product->title, 'class' => 'description-row'),
      array('data' => $titles, 'class' => 'description-row'),
      array('data' => l('Edit', 'node/'. $product->nid .'/edit', array('attributes' => array('class' => 'download-row'), 'query' => 'destination='. $_GET['q'] .'?page='. $_GET['page'], 'upsell'))),
    );
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No associations'), 'colspan' => 4));
  }

  // Show admin link if allowed...
  if (user_access('administer upsell settings')) {
    $output .= '<div class="help"><p>'. l(t('Edit upsell module settings'), 'admin/store/settings/upsell') .'</p></div>';
  }

  $output .= theme('table', $header, $rows) . theme('pager', NULL, $config['browser']['pager_size'], 0);

  return $output;
}


/**
 * Settings for the Upsell products
 */
function upsell_products_settings() {
  return drupal_get_form('uc_upsell_settings_form');
}


/**
 * Settings form
 */
function uc_upsell_settings_form() {
  
  // Check to see if we have a cached array of related-by-taxonomy nids.
  $allterms = cache_get('uc_upsell_terms');
  if (!$allterms->data) {
    drupal_set_message(t('You need to !runcron to build a list of products related by taxonomy.', array('!runcron' => l('run cron', 'admin/reports/status/run-cron'))), 'notice');
  }
  
  $config = uc_upsell_get_config();

  $form['upsell_global'] = array(
    '#type' => 'fieldset',
    '#title' => t('Global Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('These settings affect both the "Upsell Items" block and Ubercart check-out pane. <b>Developers:</b> These settings also impact the behavior and output of the <i>get_associate_nids()</i> function.'),
  );
  $form['upsell_global']['upsell_loose_msg_req'] = array(
    '#type' => 'checkbox',
    '#multiple' => FALSE,
    '#title' => t('Use default message when at least one upsell item is associated.'),
    '#default_value' => $config['global']['loose_msg_req'],
    '#description' => t('Check this option and the default message will be used, if at least one found product is associated with the target node (for product pages) or cart items (on the checkout page). <i>The "Upsell Items" block always uses the alternate message, when displayed with non-product pages.</i>'),
    '#required' => FALSE,
  );
  $form['upsell_global']['upsell_shuffle'] = array(
    '#type' => 'checkbox',
    '#multiple' => FALSE,
    '#title' => t('Shuffle related and supplemental nodes.'),
    '#default_value' => $config['global']['shuffle'],
    '#description' => t('Check this option and related nodes will be shuffled with any supplemental nodes, if any. <i>Note: Related and supplemental node groups are always randomized.</i>'),
    '#required' => FALSE,
  );
  $form['upsell_global']['buy_now'] = array(
    '#type' => 'checkbox',
    '#multiple' => FALSE,
    '#title' => t('Use a "Buy Now" button instead of Add to Cart form'),
    '#default_value' => $config['global']['buy_now'],
    '#description' => t('Checking this option will show a Buy Now button in the Upsell block instead of the normal themed Add to Cart button, which will not display any attributes for related products.'),
    '#required' => FALSE,
  );
  $form['upsell_global']['reciprocal_behavior'] = array(
      '#type' => 'select',
      '#multiple' => FALSE,
      '#title' => t('Reciprocal assocations behavior'),
      '#default_value' => isset($config['global']['reciprocal_behavior']) ? $config['global']['reciprocal_behavior'] : 'one',
      '#options' => uc_upsell_get_reciprocal_presets(),
      '#description' => t('Choose the reciprocal assocation action to perform when saving nodes:
        <br/><strong>Do not associate reciprocals:</strong>: <em>Will associate the selected products with the current node only.</em>
        <br/><strong>One to one</strong>: <em>Node A -> Node B, Node B -> Node A</em>
        <br/><strong>One to many</strong>: <em>Node A -> Node B, Node C; Node B -> Node C, Node A (default)</em>'),
      '#required' => FALSE,
  );

  $form['upsell_global']['upsell_images'] = array(
    '#type' => 'fieldset',
    '#title' => t('Image handling options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => '<p>Decide how to handle product image presets as well as which product image to use for Upselling.</p>',
  );
  // Imagecache requires presets; only show them if the module is enabled
  if (module_exists('imagecache')) {
    $imagecache_presets = array();

    foreach (imagecache_presets() AS $preset) {
      $imagecache_presets[$preset['presetname']] = $preset['presetname'];
    }

    $form['upsell_global']['upsell_images']['thumbnail_preset'] = array(
      '#type' => 'select',
      '#multiple' => FALSE,
      '#title' => t('Imagecache preset to use'),
      '#default_value' => uc_upsell_get_imagecache_preset(),
      '#options' => array_filter($imagecache_presets),
      '#description' => t('Choose the imagecache preset to apply to all blocks'),
      '#required' => FALSE,
    );
  }
  else {
    drupal_set_message(t('Install ImageCache module to get preset configurations.'));
  }

  if (module_exists('content')) {
    $fields = content_fields();
    $imagefields = array();
    foreach ($fields as $field) {
      if ($field['widget']['module'] == 'imagefield') {
        $imagefields[$field['field_name']] = t($field['widget']['label'] . ' (' .$field['field_name']. ')');
      }
    }
  }
  $form['upsell_global']['upsell_images']['upsell_image'] = array(
    '#type' => 'select',
    '#multiple' => FALSE,
    '#title' => t('Product imagefield to use'),
    '#default_value' => uc_upsell_get_upsell_image(),
    '#options' => array_filter($imagefields),
    '#description' => t('Choose the image field to apply to all blocks'),
    '#required' => FALSE,
  );
  $form['upsell_global']['types'] = array(
    '#type' => 'fieldset',
    '#title' => t('Node Types'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['upsell_global']['types']['upsell_types'] = array(
    '#type' => 'checkboxes',
    '#multiple' => TRUE,
    '#title' => t('Product types to allow associations with'),
    '#default_value' => $config['global']['types'],
    '#options' => uc_upsell_get_product_names(uc_product_node_info()),
    '#description' => t('Choose which product types can be selected in the Node Edit form.'),
    '#required' => FALSE,
  );
  $form['upsell_global']['taxonomy'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxonomy Handling'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['upsell_global']['taxonomy']['upsell_taxonomy'] = array(
    '#type' => 'select',
    '#title' => t('Handle Related Products based on Taxonomy'),
    '#default_value' => $config['global']['taxonomy'] ? $config['global']['taxonomy'] : 'ignore',
    '#description' => t('How should we deal with Products that are related by taxonomy terms? Use this feature for smarter Upselling.
      <br />- <strong>Ignore: </strong> <em>Default</em>. Displays products specified in the Node Edit form, plus any configured randoms.
      <br />- <strong>Override: </strong> Displays products related by taxonomy first, ignoring products specified in Node Edit Form.
      <br />- <strong>Add: </strong> Displays products specified by Node Edit Form, plus nodes related by taxonomy, then randoms if needed.'),
    '#options' => array(
      'ignore' => t('Ignore them (do nothing)'),
      'override' => t('Override them'),
      'add' => t('Add them'),
    ),
  );
  
  $cache = cache_get('uc_upsell_terms');  
  if ($cache) {
    $termlist = array();
    $termlist[0] = 'Do not restrict by terms';
    $terms = unserialize($cache->data);
    foreach($terms AS $tid) {
      $termlist[$tid] = db_result(db_query("SELECT `name` FROM {term_data} WHERE `tid` = %d", $tid));
    }
   $form['upsell_global']['taxonomy']['restrict_terms'] = array(
      '#type' => 'select',
      '#title' => t('Restrict related-by-taxonomy products by specific terms'),
      '#default_value' => $config['global']['restrict_terms'] ? $config['global']['restrict_terms'] : 0,
      '#description' => t('If you wish to restrict related-by-taxonomy nodes to a specific terms, select them here.'),
      '#options' => $termlist,
      '#multiple' => TRUE,
    );
  }
  $form['upsell_pane'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cart Pane'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('The "Upsell Items" pane displays products related to items in a cart.
      You can edit the order and presence of cart panes via the !panelink.',
      array('!panelink' => l("cart settings pane", 'admin/store/settings/cart/edit/panes'))),
  );
  $form['upsell_pane']['upsell_msg_related'] = array(
    '#type' => 'textfield',
    '#title' => 'Default checkout message',
    '#description' => t("Message displayed when all items are associated to the user's cart."),
    '#default_value' => check_plain($config['pane']['msg_related']),
    '#length' => 40,
    '#required' => FALSE,
  );
  $form['upsell_pane']['upsell_msg_mixed'] = array(
    '#type' => 'textfield',
    '#title' => 'Alternate checkout message',
    '#description' => t("Message displayed when one or more items have no association to the user's cart.'"),
    '#default_value' => check_plain($config['pane']['msg_mixed']),
    '#length' => 40,
    '#required' => FALSE,
  );
  $form['upsell_pane']['upsell_max'] = array(
    '#type' => 'textfield',
    '#title' => 'Number of products',
    '#description' => t('How many products to suggest at checkout'),
    '#default_value' => check_plain($config['pane']['max']),
    '#length' => 2,
    '#size' => 3,
    '#required' => TRUE,
  );
  $form['upsell_pane']['upsell_use_random'] = array(
    '#type' => 'checkbox',
    '#multiple' => FALSE,
    '#title' => t('Supplement with random products'),
    '#default_value' => $config['pane']['use_random'],
    '#description' => t('Check this option and random products will be listed when there are not enough associated items. <strong>NOTE</strong> This will override your Upsell Product Type settings, and will use any Ubercart product types available!'),
    '#required' => FALSE,
  );
  $form['upsell_browser'] = array(
    '#type' => 'fieldset',
    '#title' => t('Associations Browser'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Configure the behavior and appearance of the !browser',
      array('!browser' => l('product associations browser', 'admin/store/products/upsell'))),
  );
  $form['upsell_browser']['upsell_pager_size'] = array(
    '#type' => 'select',
    '#title' => t('Number of products to list per page'),
    '#description' => t('Change this to suit your paging needs'),
    '#default_value' => $config['browser']['pager_size'],
    '#options' => drupal_map_assoc(uc_range(10, 100, 10)),
    '#required' => TRUE,
  );
  $form['submit'] = array('#type' => 'submit',
    '#value' => t('Save Settings'),
    '#weight' => 10,
  );
  $form['upsell_block_link'] = array(
    '#prefix' => '<div><p>',
    '#suffix' => '</p></div>',
    '#value' => t('You may also !configure.', array('!configure' => l('configure the "Upsell Items" block', 'admin/build/block/configure/uc_upsell/'))),
    '#weight' => 11
  );

  return $form;
}

/**
 * Submit the settings form
 */
function uc_upsell_settings_form_submit($form, &$form_state) {
  $config = uc_upsell_get_config();
  $config['global']['loose_msg_req'] = $form_state['values']['upsell_loose_msg_req'];
  $config['global']['shuffle'] = $form_state['values']['upsell_shuffle'];
  $config['global']['buy_now'] = $form_state['values']['buy_now'];
  $config['global']['reciprocal_behavior'] = $form_state['values']['reciprocal_behavior'];
  $config['global']['types'] = $form_state['values']['upsell_types'];
  $config['global']['thumbnail_preset'] = array($form_state['values']['thumbnail_preset']);
  $config['global']['upsell_image'] = array($form_state['values']['upsell_image']);
  $config['global']['restrict_terms'] = $form_state['values']['restrict_terms'];
  $config['global']['taxonomy'] = $form_state['values']['upsell_taxonomy'];
  $config['pane']['msg_related'] = $form_state['values']['upsell_msg_related'];
  $config['pane']['msg_mixed'] = $form_state['values']['upsell_msg_mixed'];
  $config['pane']['max'] = $form_state['values']['upsell_max'];
  $config['pane']['use_random'] = $form_state['values']['upsell_use_random'];
  $config['browser']['pager_size'] = $form_state['values']['upsell_pager_size'];
  uc_upsell_set_config($config);
  drupal_set_message(t('Upsell configuration was updated.'));
}


/**
 * Get variables
 */
function uc_upsell_get_config() {
  $config = variable_get('uc_upsell_config', uc_upsell_get_config_defaults());
  return $config;
}

/**
 * Return the config defaults
 */
function uc_upsell_get_config_defaults() {
  $uc_upsell_config_defaults = array(
    'global' => array(
      'loose_msg_req' => FALSE,
      'shuffle' => TRUE,
      'reciprocal_behavior' => 'many',
      'buy_now' => FALSE,
      'types' => array('product' => 'Product'),
      'thumbnail_preset' => array('uc_thumbnail'),
      'taxonomy' => FALSE,
    ),
    'pane' => array(
      'max' => 5,
      'use_random' => TRUE,
      'msg_related' => 'Users who bought items in your cart also purchased:',
      'msg_mixed' => 'Users who bought items in your cart also purchased:',
    ),
    'block' => array(
      'use_default' => FALSE,
      'max' => 5,
      'enable' => 0,
      'product' => array(
        'use_cart' => TRUE,
        'use_random' => TRUE,
        'msg_related' => 'Related products',
        'msg_mixed' => 'Also available',
      ),
      'node' => array(
        'req_cart' => FALSE,
        'source' => 1,
        'msg_related' => 'Compliment your order with...',
        'msg_mixed' => 'Must see products',
      ),
    ),
    'browser' => array(
      'pager_size' => 10,
      'truncate' => TRUE,
    ),
  );
  return $uc_upsell_config_defaults;
}


/**
 * Return an array of reciprocal behaviors for config
 */
function uc_upsell_get_reciprocal_presets() {
  return array('none' => 'Do not associate reciprocals', 'one' => 'One to one', 'many' => 'One to many');
}

/**
 * Save uc_upsell config to drupal variable
 */
function uc_upsell_set_config($config) {
  variable_set('uc_upsell_config', $config);
}


/**
 * Revert parts of upsell variable's root keys
 */
function uc_upsell_reset_config($delkeys = array()) {

  if (!is_array($delkeys)) {
    $delkeys = array($delkeys);
  }

  $config = uc_upsell_get_config();
  $original = uc_upsell_get_config_defaults();

  // If no specific keys to delete...
  if (empty($delkeys)) {
    $config = $original;
  }
  else { // Otherwise, only replace targeted keys...
    foreach ($delkeys as $key) {
      if (array_key_exists($key, $original)) {
        $config[$key] = $original[$key];
      }
    }
  }
  uc_upsell_set_config($config);
  drupal_set_message(t('Upsell configuration was reset to defaults.'));
}

/**
 * Implementation of hook_product_class().
 *
 * When a new product class is created, remind the user they need to visit UC Upsell config again.
 */
function uc_upsell_product_class($pcid, $op) {
  switch ($op) {
    case 'insert':
      drupal_set_message(t('You need to !config if you wish to include the %class product class in Upsell blocks!',
        array('%class' => $pcid, '!config' => l('configure Upsell', 'admin/store/settings/upsell'))));  
      break;
  }
}

/********************************
 * Helper functions
 ********************************/
 
/**
 * Insert nids into the db
 */
function uc_upsell_insert_nids($nid, $nids) {
  db_query("INSERT INTO {uc_upsell_products} (nid, related_products) VALUES (%d, '%s')", $nid, serialize($nids));
}

/**
 * Update nids in the db
 */
function uc_upsell_update_nids($nid, $nids) {
  db_query("UPDATE {uc_upsell_products} SET `related_products` = '%s' WHERE `nid` = %d", serialize($nids), $nid);
}

/**
 * Get an upsell imagecache preset
 * TODO: Make this context-sensitive, i.e. different presets for cart vs. node page, etc.
 */
function uc_upsell_get_imagecache_preset($context = NULL) {
  $config = uc_upsell_get_config();
  $preset = isset($config['global']['thumbnail_preset']) ? $config['global']['thumbnail_preset'][0] : 'uc_thumbnail';
  return $preset;
}

/**
 * Get an upsell imagefield
 */
function uc_upsell_get_upsell_image($context = NULL) {
  $config = uc_upsell_get_config();
  $preset = isset($config['global']['upsell_image']) ? $config['global']['upsell_image'][0] : 'field_image_cache';
  return $preset;
}