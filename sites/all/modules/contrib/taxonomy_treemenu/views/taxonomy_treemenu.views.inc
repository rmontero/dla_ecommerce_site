<?php
// $Id: taxonomy_treemenu.views.inc,v 1.2 2009/08/03 18:57:48 rcrowther Exp $

function taxonomy_treemenu_views_data() {

  // Note: the join on this table delivers every node in the vocab, and need
  // further tid filtering to deliver a treemenu.
  $data['taxonomy_treemenu']['table']['group'] = t('Taxonomy Treemenu');
  $data['taxonomy_treemenu']['table']['base'] = array(
     'field' => 'menu_name',
     'title' => t('Taxonomy Treemenu'),
     'help' => t("Everything about Taxonomy Treemenus."),
     'weight' => -10,
  );
  
  $data['taxonomy_treemenu']['table']['join']['node'] = array(
    'left_table' => 'term_data',
    'left_field' => 'vid',
    'field' => 'vid',
  );
  
  $data['taxonomy_treemenu']['table']['join']['menu_custom'] = array(
    'left_field' => 'menu_name',
    'field' => 'menu_name',
  );

  $data['taxonomy_treemenu']['menu_name'] = array(
    'title' => t('Menu Name'),
    'help' => t('The machine name of a Taxonomy Treemenu, as provided in URLs (prefixed with "menu-").'),
/*
     'field' => array(
        'handler' => 'taxonomy_treemenu_handler_field_taxonomy_treemenu',
        'click sortable' => TRUE,
        ),
     'sort' => array(
        'help' => t('By the name of Taxonomy Treemenu (where this node has links).'),
        'handler' => 'views_handler_sort',
        ),
     'filter' => array(
        'help' => t('Wether or not a node has a link within a named treemenu.'),
        'handler' => 'taxonomy_treemenu_handler_filter_menu_name',
        ),

 */
     'argument' => array(
       'help' => t('Delivers every node in a TTM vocabulary.'),
       'handler' => 'taxonomy_treemenu_handler_argument_menu_name',
        ),
   );

/*
  $data['taxonomy_treemenu']['vid_name'] = array(
    'title' => t('Menu Name'),
    'help' => t('The vid of Taxonomy Treemenu.Will not affect the query.'),
  );
*/
  //TOCONSIDER: This aliased table may better be aliased via real table to a tid collector?
  $data['taxonomy_treemenu']['menu_name_options'] = array(
    'real field' => 'menu_name',
    'title' => t('Menu Name (node) Options'),
    'help' => t('Use Treemenu settings for filtering and sorting nodes.'),
     'argument' => array(
       'help' =>t('Delivers every node in the TTM vocabulary. Overrides all filter and sorts with Taxonomy Treemenu settings.') ,
       'handler' => 'taxonomy_treemenu_handler_argument_menu_name_options',
      ),
   );
  return $data;
}

function taxonomy_treemenu_views_plugins() {
  return array(
    
    'argument validator' => array(
      // TO CONSIDER: see views handler argument, options_form(&$form, &$form_state)
      // It will appear on a handler of vauguely similar nameing, but can't be
      //  summoned elsewhere.
      // Fuck all that I can get it to work. Or find whats wrong.
      'taxonomy_treemenu' => array(
        'path' => drupal_get_path('module', 'taxonomy_treemenu') . '/views/plugins',
        'file' => 'taxonomy_treemenu_validate_menu_name.inc',
        'title' => t('Taxonomy Treemenu menu name'),
        'help' => t('checks is plain text, and menu exists.'),
        'handler' => 'taxonomy_treemenu_plugin_argument_validate_menu_name',
      ),
       
    ),
  );
}

function taxonomy_treemenu_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'taxonomy_treemenu') . '/views/handlers',
    ),
    'handlers' => array(
      // argument handlers
      'taxonomy_treemenu_handler_field_taxonomy_treemenu' => array(
        'parent' => 'views_handler_field',
        ),
      'taxonomy_treemenu_handler_filter_menu_name' => array(
        'parent' => 'views_handler_filter_in_operator',
        ),
      'taxonomy_treemenu_handler_argument_menu_name_options' => array(
        'parent' => 'views_handler_argument_string',
        ),
      'taxonomy_treemenu_handler_argument_menu_name' => array(
        'parent' => 'views_handler_argument_string',
        ),
    ),
  );
}

