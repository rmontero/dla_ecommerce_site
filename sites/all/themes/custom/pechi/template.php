<?php
// $Id$
/**
 * Preprocessor for page.tpl.php template file.
 */
function pechi_preprocess_page(&$vars, $hook) {
  global $language, $user;

  // For easy printing of variables.
  $vars['logo_img']         = $vars['logo'] ? theme('image', substr($vars['logo'], strlen(base_path())), t('Home'), t('Home')) : '';
  $vars['linked_logo_img']  = $vars['logo_img'] ? l($vars['logo_img'], '<front>', array('attributes' => array('rel' => 'home'), 'title' => t('Home'), 'html' => TRUE)) : '';
  $vars['linked_site_name'] = $vars['site_name'] ? l($vars['site_name'], '<front>', array('attributes' => array('rel' => 'home'), 'title' => t('Home'))) : '';
  $vars['main_menu_links']      = theme('links', $vars['primary_links'], array('class' => 'links main-menu'));
  $vars['secondary_menu_links'] = theme('links', $vars['secondary_links'], array('class' => 'links secondary-menu'));

  // Make sure framework styles are placed above all others.
  $vars['css_alt'] = pechi_css_reorder($vars['css']);
  $vars['styles'] = drupal_get_css($vars['css_alt']);
  drupal_add_js(drupal_get_path('theme', 'pechi') . '/js/pechi.js');

  // Remove sidebars if disabled e.g., for Panels
  if (!$vars['show_blocks']) {
    $vars['sidebar_first'] = '';
    $vars['sidebar_last'] = '';
  }

  // Add to array of helpful body classes
  $body_classes = explode(' ', $vars['body_classes']);                                               // Default classes

  $body_classes[] = $language->prefix;

  if (isset($vars['node'])) {
    $body_classes[] = ($vars['node']) ? 'full-node' : '';                                            // Full node
    $body_classes[] = (($vars['node']->type == 'forum') || (arg(0) == 'forum')) ? 'forum' : '';      // Forum page
  }
  else {
    $body_classes[] = (arg(0) == 'forum') ? 'forum' : '';                                            // Forum page
  }
  if (module_exists('panels') && function_exists('panels_get_current_page_display')) {               // Panels page
    $body_classes[] = (panels_get_current_page_display()) ? 'panels' : '';
  }

  $body_classes = array_filter($body_classes);                                                       // Remove empty elements

  $vars['body_classes'] = implode(' ', $body_classes);                                               // Create class list separated by spaces
  $vars['body_id'] = 'pid-' . strtolower(preg_replace('/[^a-zA-Z0-9-]+/', '-', drupal_get_path_alias($_GET['q'])));            // Add a unique page id

  if (module_exists('uc_product') && uc_product_is_product((int)$vars['node']->nid)) {
    // Kill the page title to surface our own.
    $vars['title'] = NULL;
  }
}

/**
 * This rearranges how the style sheets are included so the framework styles
 * are included first.
 *
 * Sub-themes can override the framework styles when it contains css files with
 * the same name as a framework style. This can be removed once Drupal supports
 * weighted styles.
 */
function pechi_css_reorder($css) {
  global $theme_info, $base_theme_info;

  // Dig into the framework .info data.
  $framework = !empty($base_theme_info) ? $base_theme_info[0]->info : $theme_info->info;

  // Pull framework styles from the themes .info file and place them above all stylesheets.
  if (isset($framework['stylesheets'])) {
    foreach ($framework['stylesheets'] as $media => $styles_from_960) {
      // Setup framework group.
      if (isset($css[$media])) {
        $css[$media] = array_merge(array('framework' => array()), $css[$media]);
      }
      else {
        $css[$media]['framework'] = array();
      }
      foreach ($styles_from_960 as $style_from_960) {
        // Force framework styles to come first.
        if (strpos($style_from_960, 'framework') !== FALSE) {
          $framework_shift = $style_from_960;
          $remove_styles = array($style_from_960);
          // Handle styles that may be overridden from sub-themes.
          foreach ($css[$media]['theme'] as $style_from_var => $preprocess) {
            if ($style_from_960 != $style_from_var && basename($style_from_960) == basename($style_from_var)) {
              $framework_shift = $style_from_var;
              $remove_styles[] = $style_from_var;
              break;
            }
          }
          $css[$media]['framework'][$framework_shift] = TRUE;
          foreach ($remove_styles as $remove_style) {
            unset($css[$media]['theme'][$remove_style]);
          }
        }
      }
    }
  }

  return $css;
}

/**
 * Node preprocessing
 */
function pechi_preprocess_node(&$vars) {
  // Build array of handy node classes
  $node_classes = array();
  $node_classes[] = $vars['zebra'];                                      // Node is odd or even
  $node_classes[] = (!$vars['node']->status) ? 'node-unpublished' : '';  // Node is unpublished
  $node_classes[] = ($vars['sticky']) ? 'sticky' : '';                   // Node is sticky
  $node_classes[] = (isset($vars['node']->teaser)) ? 'teaser' : 'full-node';    // Node is teaser or full-node
  $node_classes[] = 'node-type-'. $vars['node']->type;                   // Node is type-x, e.g., node-type-page
  $node_classes[] = (isset($vars['skinr'])) ? $vars['skinr'] : '';       // Add Skinr classes if present
  $node_classes = array_filter($node_classes);                           // Remove empty elements
  $vars['node_classes'] = implode(' ', $node_classes);                   // Implode class list with spaces

  // Add node_top and node_bottom region content
  $vars['node_top'] = theme('blocks', 'node_top');
  $vars['node_bottom'] = theme('blocks', 'node_bottom');

  // Render Ubercart fields into separate variables for node-product.tpl.php
  if (module_exists('uc_product') && uc_product_is_product($vars) && $vars['template_files'][0] == 'node-product') {
    $node = node_build_content(node_load($vars['nid']));
    $vars['pechi_uc_title'] = $node->title;
    // var_dump($node->content);
    // die();
    $vars['pechi_uc_image'] = drupal_render($node->content['image']);
    $vars['pechi_uc_body'] = drupal_render($node->content['body']);
    $vars['pechi_uc_display_price'] = drupal_render($node->content['display_price']);
    $vars['pechi_uc_add_to_cart'] = drupal_render($node->content['add_to_cart']);
    $vars['pechi_uc_sell_price'] = drupal_render($node->content['sell_price']);
    $vars['pechi_uc_cost'] = drupal_render($node->content['cost']);
    $vars['pechi_uc_weight'] = (!empty($node->weight)) ? drupal_render($node->content['weight']) : '';   // Hide weight if empty
    if ($vars['pechi_uc_weight'] == '') {
      unset($node->content['weight']);
    }
    $dimensions = !empty($node->height) && !empty($node->width) && !empty($node->length);                 // Hide dimensions if empty
    $vars['pechi_uc_dimensions'] = ($dimensions) ? drupal_render($node->content['dimensions']) : '';
    if ($vars['pechi_uc_dimensions'] == '') {
      unset($node->content['dimensions']);
    }
    $list_price = !empty($node->list_price) && $node->list_price > 0;                                     // Hide list price if empty or zero
    $vars['pechi_uc_list_price'] = ($list_price) ? drupal_render($node->content['list_price']) : '';
    if ($vars['pechi_uc_list_price'] == '') {
      unset($node->content['list_price']);
    }
    $vars['pechi_uc_additional'] = drupal_render($node->content);                                        // Render remaining fields
  }
  if (is_array($node->taxonomy) && !empty($node->taxonomy)) {
    $count = 0;
    foreach ($node->taxonomy as $tid => $taxo) {
      $vars['pechi_uc_taxonomy'][$count]['title'] = t($taxo->name);
      $vars['pechi_uc_taxonomy'][$count]['description'] = t($taxo->description);
      $vars['pechi_uc_taxonomy'][$count]['vid'] = t($taxo->tid);
      $vars['pechi_uc_taxonomy'][$count]['tid'] = t($taxo->vid);
      $count++;
    }
  }
}

/**
 * Theme Product Images
 */
function pechi_uc_product_image($images, $teaser = 0, $page = 0) {
  static $rel_count = 0;

  // Get the current product image widget.
  $image_widget = uc_product_get_image_widget();
  $image_widget_func = $image_widget['callback'];

  $first = array_shift($images);

  $output = '<div class="product-image"><div class="main-product-image">';
  $output .= '<a href="'. imagecache_create_url('product_full', $first['filepath']) .'" title="'. $first['data']['title'] .'"';
  if ($image_widget) {
    $output .= $image_widget_func($rel_count);
  }
  $output .= '>';
  $output .= theme('imagecache', 'product', $first['filepath'], $first['data']['alt'], $first['data']['title']);
  $output .= '</a></div><div class="more-product-images">';

  foreach ($images as $thumbnail) {
    // Node preview adds extra values to $images that aren't files.
    if (!is_array($thumbnail) || empty($thumbnail['filepath'])) {
      continue;
    }

    if ($rel_count >= 4) {
      break;
    }

    $output .= '<a href="'. imagecache_create_url('product_full', $thumbnail['filepath']) .'" title="'. $thumbnail['data']['title'] .'"';
    if ($image_widget) {
      $output .= $image_widget_func($rel_count);
    }
    $output .= '>';
    $output .= theme('imagecache', 'uc_thumbnail', $thumbnail['filepath'], $thumbnail['data']['alt'], $thumbnail['data']['title']);
    $output .= '</a>';
  }
  $output .= '</div></div>';
  $rel_count++;
  return $output;
}

function xx_pechi_preprocess_views_view_grid(&$vars) {
  $view     = $vars['view'];
  $result   = $view->result;
  $options  = $view->style_plugin->options;
  $handler  = $view->style_plugin;
  /*
  if ((arg(0) == 'store') && (arg(1) != NULL)) {
    $taxonomy_name = arg(1);
    $result = taxonomy_get_term_by_name($taxonomy_name);
    if (!empty($result)) {
      $taxonomy = $result[0];
    }
    // var_dump($taxonomy);
    // $taxonomy->description;
    //$vars['header'] = t('I got this!');
    $vars['header'] = $taxonomy->description;
  }
  */
  $columns  = $options['columns'];

  $rows = array();

  if ($options['alignment'] == 'horizontal') {
    $row = array();
    $row_count = 0;
    foreach ($vars['rows'] as $count => $item) {
      $row[] = $item;
      $row_count++;
      if (($count + 1) % $columns == 0) {
        $rows[] = $row;
        $row = array();
        $row_count = 0;
      }
    }
    if ($row) {
      // Fill up the last line only if it's configured, but this is default.
      if (!empty($handler->options['fill_single_line']) || count($rows)) {
        for ($i = 0; $i < ($columns - $row_count); $i++) {
          $row[] = '';
        }
      }
      $rows[] = $row;
    }
  }
  else {
    $num_rows = floor(count($vars['rows']) / $columns);
    // The remainders are the 'odd' columns that are slightly longer.
    $remainders = count($vars['rows']) % $columns;
    $row = 0;
    $col = 0;
    foreach ($vars['rows'] as $count => $item) {
      $rows[$row][$col] = $item;
      $row++;

      if (!$remainders && $row == $num_rows) {
        $row = 0;
        $col++;
      }
      else if ($remainders && $row == $num_rows + 1) {
        $row = 0;
        $col++;
        $remainders--;
      }
    }
    for ($i = 0; $i < count($rows[0]); $i++) {
      // This should be string so that's okay :)
      if (!isset($rows[count($rows) - 1][$i])) {
        $rows[count($rows) - 1][$i] = '';
      }
    }
  }
  $vars['rows'] = $rows;
}
?>