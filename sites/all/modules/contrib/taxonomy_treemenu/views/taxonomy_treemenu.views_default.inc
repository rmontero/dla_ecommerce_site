<?php
// $Id: taxonomy_treemenu.views_default.inc,v 1.2 2009/09/26 21:53:11 rcrowther Exp $

/**
 * Implementation of hook_views_default_views().
 */
function taxonomy_treemenu_views_default_views() {
// This is the main provider for TTM Views. It delivers all the nodes from a
// treemenu when the menu name is placed in the URL.
$view = new view;
$view->name = 'taxonomy_treemenu_extended_term';
$view->description = 'A view to page Taxonomy Treemenu extended URL terms; it also emulates Views 1\'s handling by having two possible feeds.';
$view->tag = 'default';
$view->view_php = '';
$view->base_table = 'node';
$view->is_cacheable = FALSE;
$view->api_version = 2;
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
$handler = $view->new_display('default', 'Defaults', 'default');
$handler->override_option('fields', array(
  'title' => array(
    'label' => 'Title',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
    ),
    'link_to_node' => 0,
    'exclude' => 0,
    'id' => 'title',
    'table' => 'node',
    'field' => 'title',
    'relationship' => 'none',
  ),
  'name' => array(
    'label' => 'Term',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
    ),
    'link_to_taxonomy' => 0,
    'exclude' => 0,
    'id' => 'name',
    'table' => 'term_data',
    'field' => 'name',
    'relationship' => 'none',
  ),
));
$handler->override_option('arguments', array(
  'menu_name_options' => array(
    'default_action' => 'ignore',
    'style_plugin' => 'default_summary',
    'style_options' => array(),
    'wildcard' => 'all',
    'wildcard_substitution' => 'All',
    'title' => '',
    'default_argument_type' => 'fixed',
    'default_argument' => '',
    'validate_type' => 'none',
    'validate_fail' => 'not found',
    'glossary' => FALSE,
    'limit' => 0,
    'case' => 'none',
    'path_case' => 'none',
    'transform_dash' => FALSE,
    'validate type' => 't',
    'id' => 'menu_name_options',
    'table' => 'taxonomy_treemenu',
    'field' => 'menu_name_options',
    'relationship' => 'none',
    'default_options_div_prefix' => '',
    'default_argument_user' => 0,
    'default_argument_fixed' => '',
    'default_argument_php' => '',
  ),
  'tid' => array(
    'id' => 'tid',
    'table' => 'term_node',
    'field' => 'tid',
  ),
));
$handler->override_option('access', array(
  'type' => 'none',
  'role' => array(),
  'perm' => '',
));
$handler->override_option('use_pager', '1');
$handler->override_option('row_plugin', 'node');
$handler->override_option('row_options', array(
  'teaser' => TRUE,
  'links' => TRUE,
));
$handler = $view->new_display('page', 'Page', 'page');
$handler->override_option('path', 'ttm/%/term/%');
$handler->override_option('menu', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
  'name' => 'navigation',
));
$handler->override_option('tab_options', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
));
$handler = $view->new_display('feed', 'Core feed', 'feed');
$handler->override_option('items_per_page', 15);
$handler->override_option('style_plugin', 'rss');
$handler->override_option('style_options', array(
  'mission_description' => FALSE,
  'description' => '',
));
$handler->override_option('row_plugin', 'node_rss');
$handler->override_option('row_options', array(
  'item_length' => 'default',
));
$handler->override_option('path', 'taxonomy/term/%/%/feed');
$handler->override_option('menu', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
  'name' => 'navigation',
));
$handler->override_option('tab_options', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
));
$handler->override_option('displays', array(
  'page' => 'page',
  'default' => 0,
));
$handler->override_option('sitename_title', FALSE);
$handler = $view->new_display('feed', 'Views 1 feed', 'feed_1');
$handler->override_option('items_per_page', 15);
$handler->override_option('style_plugin', 'rss');
$handler->override_option('style_options', array(
  'mission_description' => FALSE,
  'description' => '',
));
$handler->override_option('row_plugin', 'node_rss');
$handler->override_option('row_options', array(
  'item_length' => 'default',
));
$handler->override_option('path', 'taxonomy/term/%/feed');
$handler->override_option('menu', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
  'name' => 'navigation',
));
$handler->override_option('tab_options', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
));
$handler->override_option('displays', array());
$handler->override_option('sitename_title', FALSE);


  $views[$view->name] = $view;

  // This Bayou Bill's multiple title fix. Untested.
  $view = new view;
  $view->name = 'taxonomy_term_ttm_default_path';
  $view->description = 'Taxonomy Treemenu version of taxonomy_term view, useful when using default path';
  $view->tag = 'ttm_taxonomy';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = TRUE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('sorts', array(
    'sticky' => array(
      'id' => 'sticky',
      'table' => 'node',
      'field' => 'sticky',
      'order' => 'DESC',
      'relationship' => 'none',
    ),
    'created' => array(
      'id' => 'created',
      'table' => 'node',
      'field' => 'created',
      'order' => 'DESC',
      'granularity' => 'second',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('arguments', array(
    'tid' => array(
      'default_action' => 'default',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '',
      'breadcrumb' => '',
      'default_argument_type' => 'php',
      'default_argument' => '',
      'validate_type' => 'php',
      'validate_fail' => 'not found',
      'break_phrase' => 1,
      'add_table' => 0,
      'require_value' => 0,
      'reduce_duplicates' => 0,
      'set_breadcrumb' => 0,
      'id' => 'tid',
      'table' => 'term_node',
      'field' => 'tid',
      'validate_user_argument_type' => 'uid',
      'validate_user_roles' => array(
        '2' => 0,
        '3' => 0,
      ),
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_argument_user' => 0,
      'default_argument_fixed' => '',
      'default_argument_php' => 'return true;',
      'validate_argument_node_type' => array(
        'webform' => 0,
        'forum' => 0,
        'book' => 0,
        'notice' => 0,
        'page' => 0,
        'story' => 0,
      ),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(
        '1' => 0,
        '2' => 0,
        '8' => 0,
        '3' => 0,
      ),
      'validate_argument_type' => 'tid',
      'validate_argument_transform' => 0,
      'validate_user_restrict_roles' => 0,
      'validate_argument_php' => 'if( $argument) {
        $terms = explode("+", arg(2));
        $names = taxonomy_get_term($terms[0]);
        $handler->options[\'title\'] = $names->name;
  }
  return true;',
      'override' => array(
        'button' => 'Override',
      ),
    ),
  ));
  $handler->override_option('filters', array(
    'status_extra' => array(
      'id' => 'status_extra',
      'table' => 'node',
      'field' => 'status_extra',
      'operator' => '=',
      'value' => '',
      'group' => 0,
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'none',
    'role' => array(),
    'perm' => '',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('use_pager', '1');
  $handler->override_option('row_plugin', 'node');
  $handler->override_option('row_options', array(
    'teaser' => TRUE,
    'links' => TRUE,
  ));
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->override_option('path', 'taxonomy/term/%');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
  ));

  $views[$view->name] = $view;

  return $views;
}


