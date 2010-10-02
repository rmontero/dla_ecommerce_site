<?php
/**
 * Get related items from the database and show them at the cart screen.
 * - If there aren't enough related products assigned in the admin, show a random selection.
 * - set's msg to the string to in the uc_upsell variable
 *
 * @param $msg
 *   A message to override the default with
 * @return
 *   Fully loaded array of related nodes
 */
function uc_upsell_resolve_associates(&$msg = '') {
  $config = uc_upsell_get_config();
  $dpath = $_GET['q'];

  // If this is the cart...
  if ($dpath == 'cart') {
    $context = 'pane';
    $is_block = FALSE;
  }
  else {
    $is_block = TRUE;
    $node = node_load(arg(1));
    // If this is a product node...
    if (uc_product_is_product($node) == TRUE) {
      $product_nid = $node->nid;
      $context = 'product';
    }
    else {
      $context = 'node';
    }
  }

  //drupal_set_message("context: '$context' dpath: $dpath nid: {$product_nid}");

  // If can't be enabled for this context, exit function
  if ($context != 'pane' && (($config['block']['enable'] == 1 && $context == 'product' && intval(arg(1)) == 0)
    || ($config['block']['enable'] == 0 && $context == 'node')  // Can't enable for node
    || ($context == 'product' && arg(0) == 'catalog') // Don't want to show on Catalog pages
    || ($context == 'product' && arg(2) == 'edit') // Don't enable it on edit pages
    )) {
    return array();
  }

  // set vars to use when getting related items, based on the type of page being shown
  $vars = array(
    'max' => ($is_block) ? $config['block']['max'] : $config['pane']['max'],
    'use_random' => ($is_block) ? $config['block'][$context][(($context == 'node') ? 'source' : 'use_random')] : $config['pane']['use_random'],
  );

  $related = array(); // List of related nodes to return

  $primary = 0; // Flag whether primary source is in result
  $sources = 0; // Flag

  // If source is a product node, begin with nodes related to this product that are not in cart or itself
  if (isset($product_nid)) {
    // Start list with nids related to the product node
    $related = uc_upsell_get_associate_nids(array(arg(1)), intval($vars['max']), array(arg(1)), $vars);

    // Get/add nids related to the cart
    if ($config['block']['product']['use_cart'] == TRUE) {
      $related = array_unique(array_merge($related, uc_upsell_get_associate_nids(uc_upsell_get_cart_nids(), $vars['max'] - count($related), arg(1), $vars)));
    }

    // If related is not empty...
    if (!empty($related)) {
      $primary = 1;
      $sources += 1;
    }
  }
  else {
    // We are on the cart, so we need to use $cart_nids as the source, as well as filter.
    $cartnids = uc_upsell_get_cart_nids();

    $related = uc_upsell_get_associate_nids($cartnids, intval($vars['max']), $cartnids, $vars);
    if (!empty($related)) {
      $primary = 1;
      $sources += 1;
    }
  }

  // If nothing is related, exit function
  if (empty($related)) return array();

  //dsm("primary: $primary, sources: $sources, strict msg? ". (($config['global']['loose_msg_req']) ? 'yes' : 'no'));

  // Set msg based on strict flag and number of sources...
  $msg = ($primary && ($sources == 1 || $config['global']['loose_msg_req'])) ? (($context == 'pane') ? $config['pane']['msg_related'] : $config['block'][$context]['msg_related']) : (($context == 'pane') ? $config['pane']['msg_mixed'] : $config['block'][$context]['msg_mixed']);

  // If this is a form submission, the contents of the upsell products
  // must stay fixed; otherwise the form we are posting to may not exist.
  if (isset($_POST['form_id'])) {
    if (strpos($_POST['form_id'], 'uc_product_add_to_cart_form_') === 0 ||
      strpos($_POST['form_id'], 'uc_catalog_buy_it_now_form_') === 0) {
      if (isset($_SESSION['uc_upsell_related'][$dpath])) {
        $products = array_map('node_load', $_SESSION['uc_upsell_related'][$dpath]);
        return $products;
      }
    }
  }

  // Shuffle the products
  if ($config['global']['shuffle']) shuffle($related);
  $related = array_slice($related, 0, $vars['max']);

  // Store the product list in the session, it's needed later if one of the forms gets submitted.
  if (!isset($_SESSION['uc_upsell_related'])) {
    $_SESSION['uc_upsell_related'] = array();
  }
  $_SESSION['uc_upsell_related'][$dpath] = $related;

  $products = array_map('node_load', $related);
  return $products;
}

/**
 * Find the associate nids!
 *
 * @param $source
 *   An array of source nodes, which will be the basis for our search
 * @param $max
 *   The maximum number of nids to return (required)
 * @param $filter
 *   An array (if given) to use as a filter against the returned array (optional)
 * @param $vars
 *   An array of extra variables (optional)
 * @return
 *   An array of nids to be resolved by uc_upsell_resolve_associates()
 */
function uc_upsell_get_associate_nids($source, $max = 0, $filter = array(), $vars = array()) {
  // Process args (a bit more robust here, since developers will likely use it)
  $config = uc_upsell_get_config();
  // Ensure source an array
  if (!is_array($source) && (is_numeric($source) || (is_object($source) && $source->nid))) $source = array((is_numeric($source)) ? $source : $source->nid);
  // Ensure filter an array
  if (!is_array($filter) && (is_numeric($filter) || (is_object($filter) && $filter->nid))) $filter = array((is_numeric($filter)) ? $filter : $filter->nid);
  // Ensure max is an integer
  if (!is_int($max) && is_numeric($max)) $max = abs(round($max));
  // If source or filter is not an array, or max is not an integer, exit function
  if (!is_array($source) || !is_array($filter) || !is_int($max)) {
    return array();
  }

  $ary = array(); // Array to return
  $sought = array(); // Capture found nid
  $related = array(); // Final related array
  $related_by_term = array(); // Any nodes related by taxonomy we happen to find.
  
  foreach ($source as $nid) {
    if ((is_object($nid) && $nid->nid) || (is_array($nid) && $nid['nid'])) $nid = (is_object($nid)) ? $nid->nid : $nid['nid'];

    if (is_numeric($nid)) {
      $sought[] = $nid;
      $related = db_result(db_query("SELECT `related_products` FROM {uc_upsell_products} WHERE nid = %d", $nid));

      // If we found an array of related products specified in the node form, let's initialize them.
      if ($related && is_array(unserialize($related))) {
        $related = unserialize($related);
      }
      else {
        $related = array();
      }

      // Depending on our taxonomy settings, manipulate the array
      switch($config['global']['taxonomy']) {
        case 'ignore':
          break;
        case 'override':
          $node = node_load($nid);
          $related = uc_upsell_get_tids_for_intersect($node);
          break;
        case 'add':
          $node = node_load($nid);
          $related_by_term = uc_upsell_get_tids_for_intersect($node);
          $related_by_term = array_unique(array_merge((array)$related, (array)$related_by_term));
          break;
      }
    }
  }

  // First, if we are including related-by-taxonomy nodes, and an array exists, only include as many as needed.
  if (count($related_by_term) > 0 && count($related) < $max) {
  
    $needed = $max - count($related); // How many do we need?

    if ($needed > 0) {
      // Filter out the nodes already in our list
      $related_by_term = array_diff(array_values($related_by_term), array_values($related));
      $related = array_unique(array_merge((array)$related, (array)$related_by_term));
    }
  }

  // Remove found nids with filter and already sought nids, make them unique
  $ary = uc_upsell_get_types_array($related);
  $ary = uc_upsell_filter_types($ary);
  $ary = array_diff(array_keys($ary), $filter, $sought);
  $ary = array_unique($ary);

  // If count of related nodes doesn't meet threshold and can include random nodes, add them.
  
  /*** Notes: This code should be moved into its own method, since it's "context agnostic." 
     - Should find a way to honor the product types settings (it currently does not) ****/
  
  if (count($ary) < $max && $vars['use_random']) {

    $random_nids = array();
    $filter_query = implode(",", array_merge($ary, uc_upsell_get_cart_nids()));

    $randoms = db_query("SELECT `nid`
                FROM {node}
                WHERE `status` = 1
                AND `type` IN ('". implode("','", uc_product_product_types()) ."')
                AND NOT nid IN (". $filter_query .")
                ORDER BY RAND()");

    while ($random = db_fetch_array($randoms)) {
      $random_nids[] = $random['nid'];
    }

    if (!empty($random_nids)) {
      $sources += 1;
    }

    $ary = array_unique(array_merge($ary, array_slice($random_nids, 0, $max - count($ary))));
  }
  
  // Allow other modules to alter associate array
  drupal_alter('uc_upsell_related', $ary);

  // Shuffle related nodes
  shuffle($ary);

  // If max is valid, slice array to only show our configured maximum
  if ($max > 0) $ary = array_slice($ary, 0, $max);
  return $ary;
}


/**
 * Compute the taxonomy terms and store them as an array (calculated at first run, and during cron)
 */
function uc_upsell_compute_terms() {
  $terms = db_query("SELECT a.name, b.tid, count( b.tid ) AS count
    FROM {term_data} a
    INNER JOIN {term_node} b ON a.tid = b.tid
    INNER JOIN {node} c ON b.nid = c.nid
    WHERE c.type IN ('". implode("','", uc_product_product_types()) ."')
    AND c.status = 1
    GROUP BY b.tid
    ORDER BY count DESC");

  $store = array();

  while ($term = db_fetch_object($terms)) {
    if ($term->count > 1) $store[] = intval($term->tid);
  }

  cache_set('uc_upsell_terms', serialize($store), 'cache', CACHE_TEMPORARY);
  //drupal_set_message('Upsell term array was updated.');
}

/**
 * Get related nids based on term ids (calculated at first run, and during cron)
 */
function uc_upsell_nids_by_term($intersect, $exclude_nid) {
  $related = db_query("SELECT n.nid FROM {node} n
  INNER JOIN {term_node} t ON n.nid = t.nid
  WHERE t.tid IN ('". implode("','", $intersect) ."')
  AND n.type IN ('". implode("','", uc_product_product_types()) ."')
  AND n.nid <> %d
  AND n.status = 1
  GROUP BY n.title
  ORDER BY n.title", $exclude_nid);
  // Should exclude_nid be filtered out after the array is built? In case we need to exclude multiple nids?

  $nids = array();
  while ($result = db_fetch_object($related)) {
    $nids[] = $result->nid;
  }
  return $nids;
}

/**
 * Get intersecting term ids
 *
 * @param $node
 *   The node object to examine
 * @return
 *   An array of nodes related by taxonomy term
 */
function uc_upsell_get_tids_for_intersect($node) {
  // Load the terms for the node in question
  $terms = taxonomy_node_get_terms($node);
  $config = uc_upsell_get_config();
  $node_tids = array();

  foreach ($terms AS $term) {
    // If we are restricting by term id, only add the node to the array if the term exists in config.
    if ($config['global']['restrict_terms'] && $config['global']['restrict_terms'][0] != 0) {
      if (in_array($term->tid, $config['global']['restrict_terms'])) {
        $node_tids[] = $term->tid;
      }
    }
    else {
      $node_tids[] = $term->tid;
    }
  }

  $allterms = cache_get('uc_upsell_terms');
  if ($allterms) {
    $intersect = array_intersect($node_tids, unserialize($allterms->data));
    $related_by_term = uc_upsell_nids_by_term($intersect, $node->nid);
    return $related_by_term;
  }
}


/**
 * Helper function go get cart nids
 *
 * @return
 *    An array of cart nids (just the 'nid' element)
 */
function uc_upsell_get_cart_nids() {
  // Get cart nids, as a base filter
  $cart_nids = array();
  $user_cart = uc_cart_get_contents();

  foreach ($user_cart as $cart) {
    $cart_nids[$cart->nid] = $cart->nid;
  }
  return $cart_nids;
}
