<?php
// $Id: uc_views_attribute_handler_field_order_product_attribute.inc,v 1.1.2.1 2009/10/21 10:00:48 madsph Exp $

/**
 * Field handler to provide a human-readable version of the selected combination of attributes
 */
class uc_views_attribute_handler_field_order_product_attribute extends views_handler_field {
 
  /**
   * Defines a few default options for the combination field
   */
  function option_definition() {
    $options = parent::option_definition();

    return $options;
  }

  function render($values) {
    $data = unserialize($values->{$this->field_alias});
    $result = "";
    if (is_array($data['attributes'])) {
      $result = '';
      $rows = array();
      foreach ($data['attributes'] as $key => $value) {
        $result .= "$key => $value[0] <br/>";
        $rows[] = t('@attribute: @option', array('@attribute' => $key, '@option' => $value[0]));
        if (count($rows)) {
          $result = theme('item_list', $rows, NULL, 'ul', array('class' => 'product-description'));
        }
      }
    }
    return $result;
  }
}
