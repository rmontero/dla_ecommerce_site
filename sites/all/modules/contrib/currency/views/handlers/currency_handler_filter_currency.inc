<?php
// $Id: currency_handler_filter_currency.inc,v 1.1.2.1 2010/01/03 04:00:51 kbahey Exp $

/**
 * @file
 * Views filter handler.
 */

class currency_handler_filter_currency extends views_handler_filter_in_operator {     
  function get_value_options() {
    $this->value_options = currency_api_get_list();
  }       
  
  function query() {                  
    currency_views_preload($this->view); 
    return parent::query();
  }  

}

