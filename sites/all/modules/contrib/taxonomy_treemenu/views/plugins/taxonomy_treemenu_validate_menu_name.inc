<?php
// $Id: taxonomy_treemenu_validate_menu_name.inc,v 1.1 2009/08/03 18:57:48 rcrowther Exp $
/**
 * @file
 * Contains the Taxonomy Treemenu argument validator plugin.
 */

/**
 * Validate whether an argument is a Taxonomy Treemenu machine name id or not.
 *
 * @ingroup views_argument_validate_plugins
 */
class taxonomy_treemenu_plugin_argument_validate_menu_name extends views_plugin_argument_validate {
  //var $option_name = 'validate_argument_taxonomy_treemenu';

  function validate_argument($arg) {
    //dpm('validation called');
    return TTMData::validateMenuName($arg);
  }
}