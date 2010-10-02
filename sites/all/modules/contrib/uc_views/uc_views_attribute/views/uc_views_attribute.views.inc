<?php
// $Id: uc_views_attribute.views.inc,v 1.1.2.3 2009/12/03 12:48:31 madsph Exp $
/**
 * @file
 * Views 2 hooks and callback registries.
 */

/**
 * Implementation of hook_views_data().
 */
function uc_views_attribute_views_data() {
  $uc_product_attributes = drupal_get_schema('uc_product_attributes');
  
  //Create a filter for each product attribute
  $result = db_query("SELECT aid, name, description FROM {uc_attributes}");
  while($row = db_fetch_object($result)) {
    $data['uc_order_products']['attr_'.$row->aid] = array(
      'title' => 'Attribute: '.$row->name,
      'help' => 'Attribute desc: '.$row->description,
      'filter' => array(
        'handler' => 'uc_views_attribute_handler_filter_attr',
      ),
      'aid' => $row->aid,
    );
  }

  // Patch by hanoii
  $data['uc_product_adjustments']['table']['group'] = t('Product attributes');

  $uc_product_adjustments = drupal_get_schema('uc_product_adjustments');

  $data['uc_product_adjustments']['table']['join']['node'] = array(
      'left_field' => 'nid',
      'field' => 'nid',
  );

  $data['uc_product_adjustments']['model'] = array(
    'title' => t('Model'),
    'help' => $uc_product_adjustments['fields']['model']['description'],
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'relationship' => array(
      'base' => 'uc_product_stock',
      'base field' => 'sku',
      'relationship field' => 'model',
      'handler' => 'views_handler_relationship',
      'label' => t('Model'),
    )
  );

  $data['uc_product_adjustments']['combination'] = array(
    'title' => t('Attributes'),
    'help' => t('Product combination of attributes.'),
    'field' => array(
      'handler' => 'uc_views_attribute_handler_field_combination',
    ),
  );

  // Add stock relationship to the adjustments table
  $data['uc_product_stock']['sku']['relationship'] = array(
    'base' => 'uc_product_adjustments',
    'base field' => 'model',
    'relationship field' => 'sku',
    'handler' => 'views_handler_relationship',
    'label' => t('SKU'),
  );

  // Add viewhandler for uc_order_product attributes
  $data['uc_order_products']['attributes'] = array(
    'title' => t('Product attributes'),
    'help' => t('List of attribute selection for the ordered product.'),
    'group' => t('Order product'),
    'field' => array(
      'table' => 'uc_order_products',
      'field' => 'data',
      'handler' => 'uc_views_attribute_handler_field_order_product_attribute',
    ),
  );

  return $data;
}

/**
 * Implementation of hook_views_handlers().
 */
function uc_views_attribute_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'uc_views_attribute') . '/views',
    ),
    'handlers' => array(
      // fields
      'uc_views_attribute_handler_field_combination' => array('parent' => 'views_handler_field'),
      'uc_views_attribute_handler_field_order_product_attribute' => array('parent' => 'views_handler_field'),
      //fiters
      'uc_views_attribute_handler_filter_attr' => array('parent' => 'views_handler_filter_in_operator',),
    ),
  );
}

/**
 * Conditionally add editablefields support.
 */
function uc_views_attribute_views_tables_alter(&$tables) {
}

/**
 * Load all attributes.
 */
function uc_get_attributes() {
  $result = db_query("SELECT aid FROM {uc_attributes} ORDER BY ordering");
  $chosen_attr = array();
  while ($attr = db_fetch_object($result)){
    $chosen_attr[$attr->aid] = uc_attribute_load($attr->aid);
  }
  return $chosen_attr;
}
