<?php
// $Id: taxonomy_treemenu_handler_filter_menu_name.inc,v 1.1 2009/08/03 18:57:48 rcrowther Exp $
/**
 * Filter by menu name
 */
class taxonomy_treemenu_handler_filter_menu_name extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Menu Name');
      $types = TTMData::allNamesDisplay();
      $this->value_options = $types;
    }
  }
}