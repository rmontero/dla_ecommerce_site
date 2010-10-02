<?php
// $Id: taxonomy_treemenu.pages.inc,v 1.6 2009/08/26 17:55:54 rcrowther Exp $

/**
 * Render a taxonomy term page HTML output.
 *
 * @param $tids
 *   An array of term ids.
 * @param $result
 *   A pager_query() result, such as that performed by taxonomy_select_nodes().
 *
 * @ingroup themeable
 */
function theme_taxonomy_treemenu_term_page($term, $result) {
  drupal_add_css(drupal_get_path('module', 'taxonomy') .'/taxonomy.css');

  $output = '';

  $description = $term->description;
  if (!empty($description)) {
    $output .= '<div class="taxonomy-term-description">';
    $output .= filter_xss_admin($description);
    $output .= '</div>';
  }

  $output .= taxonomy_render_nodes($result);

  return $output;
}


/**
 * Output the children of a term (and optionally, extra tids) list as HTML,
 *  sorted and filtered using Treemenu options.
 * @param $ttm
 * @param $tid
 *   The primary tid for this page.
 * @param $terms
 *   Array of extra tids and the operator, returned from taxonomy_terms_parse_string().
 * @param $op
 *   Page (with feed) or ?
 * @return HTML output for paging, etc.
 */
// Unfortunately, I don't see any way round this. Using a hook on the taxonomy
// node selector means generalizing across the installation, which seems ugly
//  and inefficient.
// So this is a near straight copy of taxonomy.pages.inc,
// with ttm's node selector put in, and lots stripped out.
function taxonomy_treemenu_structured_term_output($ttm, $tid, $terms, $op = 'page') {
  //dpm('structured term output');
  //dpm($terms);
  if ($tid) {
    $result = db_query(db_rewrite_sql('SELECT t.tid, t.name, t.description FROM {term_data} t WHERE t.tid IN (%d)', 't', 'tid'), $tid);
    $term_data = db_fetch_object($result);
  
    if ($term_data) {
      $title = check_plain($term_data->name);
      drupal_set_title($title);
      switch ($op) {
        case 'page':
          if($ttm['term_as_links']) {
            $link_base = $ttm['menu_urls'] ? 'ttm/'. $ttm['menu_name'] .'/node/' : 'node/';
            $output = theme('taxonomy_treemenu_term_unformatted_page', $term_data, taxonomy_treemenu_select_descendant_nodes2($ttm, $terms['tids'], TRUE), $link_base);
          }
          else {
            $output = theme('taxonomy_treemenu_term_page', $term_data, taxonomy_treemenu_select_descendant_nodes2($ttm, $terms['tids'], TRUE));
          }
          drupal_add_feed(url('taxonomy_treemenu/term/'. implode('', $terms) .'/feed'), 'RSS - '. $title);
          return $output;
          break;
        // Ignoring feed until we run into it.
        default:
          drupal_not_found();
      }
    }
    else {
      drupal_not_found();
    }
  }
}


   /*
  * Treemenus built in 'list of nodes under a term' unformatted renderer option
  */
function theme_taxonomy_treemenu_term_unformatted_page($term, $result, $link_base = 'node/') {
  $output = '';

  $description = $term->description;
  if (!empty($description)) {
    $output .= '<div class="taxonomy-term-description">';
    $output .= filter_xss_admin($description);
    $output .= '</div>';
  }

  $output .= taxonomy_treemenu_render_nodes_unformatted($result, $link_base);

  return $output;
}


function theme_taxonomy_treemenu_term_row_data($data, $link_base) {
  $output = '<span class="ttm-term-row-title">';
  $output .= l(filter_xss_admin($data->title), $link_base . $data->nid);
  $output .= '</span>';
  return $output;
}

function theme_taxonomy_treemenu_term_row($row) {
   return '<div class="ttm-term-row">'. $row .'</div>';
}

function taxonomy_treemenu_render_nodes_unformatted($result, $link_base) {
  $output = '';
  $has_rows = FALSE;

  //dvm($settings);
  while ($data = db_fetch_object($result)) {
    //Bypassing the node view function
    // with a stripped version.
    $output .= theme('taxonomy_treemenu_term_row', theme('taxonomy_treemenu_term_row_data', $data, $link_base));
    $has_rows = TRUE;
  }

  if ($has_rows) {
    $output .= theme('pager', NULL, variable_get('default_nodes_main', 10), 0);
  }
  else {
    $message =  t('There are currently no nodes in this category.');
    $output .= '<p>'. $message .'</p>';
  }

  return $output;
}

