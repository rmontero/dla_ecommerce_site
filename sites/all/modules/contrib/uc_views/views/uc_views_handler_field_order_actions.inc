<?php
// $Id: uc_views_handler_field_order_actions.inc,v 1.1.2.1 2009/06/30 07:10:39 madsph Exp $

/**
 * @file
 * Views handler: Return actions for order id - by justindodge
 */

/**
 * Return actions for an order
 */
class uc_views_handler_field_order_actions extends views_handler_field {
  function render($values) {
    $order = uc_order_load($values->order_id);
    return uc_order_actions($order, true);
  }
  
  function query() {
    //do nada, this isn't a real db field, just one we're rendering
  }
}
