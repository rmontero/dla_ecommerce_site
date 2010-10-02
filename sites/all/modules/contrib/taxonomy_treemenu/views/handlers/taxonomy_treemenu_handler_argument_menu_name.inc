<?php
// $Id: taxonomy_treemenu_handler_argument_menu_name.inc,v 1.1 2009/08/03 18:57:48 rcrowther Exp $
/**
 * @defgroup views_sort_handlers Views' sort handlers
 * @{
 * Handlers to tell Views how to sort queries
 */


/**
 * handler for Taxonomy Treemenu menu_name.
 */
class taxonomy_treemenu_handler_argument_menu_name extends views_handler_argument_string {

  /*
  function validate_argument_basic($arg) {
    dpm('validate');
    dvm(TTMData::validateMenuName($arg));
    return TTMData::validateMenuName($arg);
  }
  */
}
