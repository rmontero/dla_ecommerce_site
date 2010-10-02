<?php
// $Id: uc_views.views.inc,v 1.13.2.5.2.11 2009/12/16 08:30:45 madsph Exp $
/**
 * @file
 * Views 2 hooks and callback registries.
 */

define('UC_PAYER_PREFIX', "Payer's");
define('UC_RECIPIENT_PREFIX', "Recipient's");

/**
 * Implementation of hook_views_data().
 */
function uc_views_views_data() {
  //Make the uc_order_products table visible for Views module
  $order_products = drupal_get_schema('uc_order_products');
  $data['uc_order_products']['table']['group'] = t('Order product');
  $data['uc_order_products']['table']['base'] = array(
    'field' => 'order_product_id',
    'title' => t('Order Products'),
    'help' => t('Product orders placed in your Ubercart store.'),
    'weight' => -10,
  );
  $data['uc_order_products']['table']['join']['node'] = array(
      'left_field' => 'nid',
      'field' => 'nid',
  );

  $data['uc_products']['table']['join']['uc_order_products'] = array(
      'left_field' => 'nid',
      'field' => 'nid',
  );

  $data['uc_order_products']['model'] = array(
    'title' => t('SKU'),
    'help' => $order_products['fields']['model']['description'],
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['uc_order_products']['qty'] = array(
    'title' => t('Qty'),
    'help' => $order_products['fields']['qty']['description'],
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );
  $data['uc_order_products']['price'] = array(
    'title' => t('Price'),
    'help' => $order_products['fields']['price']['description'],
    'field' => array(
      'handler' => 'uc_views_handler_field_money_amount',
      'click sortable' => TRUE,
      'float' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_float',
    ),
  );
  $data['uc_order_products']['cost'] = array(
    'title' => t('Cost'),
    'help' => $order_products['fields']['cost']['description'],
    'field' => array(
      'handler' => 'uc_views_handler_field_money_amount',
      'click sortable' => TRUE,
      'float' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_float',
    ),
  );
  $data['uc_order_products']['weight'] = array(
    'title' => t('Weight'),
    'help' => $order_products['fields']['weight']['description'],
    'field' => array(
      'additional fields' => array(
      'weight_units' => array(
        'table' => 'uc_products',
        'field' => 'weight_units',
      ),      ),
      'handler' => 'uc_product_handler_field_weight',
      'click sortable' => TRUE,
      'float' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_float',
    ),
  );
  $data['uc_order_products']['nid'] = array(
    'title' => t('Product NID'),
    'help' => t('The Node ID of the ordered product'),
    'relationship' => array(
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'title' => t('Product NID'),
    ),
  );
  //////////Added by jd
  $data['uc_order_products']['title'] = array(
    'title' => t('Product Title'),
    'help' => $order_products['fields']['title']['description'],
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['uc_order_products']['data'] = array(
      'title' => 'Product Attributes',
      'help' => 'Filter by the value of a purchased products selected attributes.',
      'filter' => array(
        'handler' => 'views_handler_filter_in_operator',
      ),
    );
  /////////////////////

  $data['uc_order_products']['ordering'] = array(
    'title' => t('List position'),
    'help' => t('The default sort criteria in the catalog.'),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  //Make the uc_orders table visible for Views module
  //Thank you river_t for original patch (order_id, order_status and uid)!
  $data['uc_orders']['table']['group'] = t('Ubercart order');

  // Base and use of schema for descriptions added by sammys
  $order_schema = drupal_get_schema('uc_orders');
  $data['uc_orders']['table']['base'] = array(
    'field' => 'order_id',
    'title' => t('Ubercart orders'),
    'help' => t('Orders placed in your Ubercart store.'),
    'weight' => -10,
  );
  $data['users']['table']['join']['uc_orders'] = array(
    'left_field' => 'uid',
    'field' => 'uid',
  );
  $data['users_roles']['table']['join'] = array(
    'uc_orders' => array(
      'left_table' => 'users',
      'left_field' => 'uid',
      'field' => 'uid',
    ),
  );
  $data['uc_order_products']['table']['join']['uc_orders'] = array(
    'left_field' => 'order_id',
    'field' => 'order_id',
  );
  $data['node']['table']['join']['uc_orders'] = array(
    'left_table' => 'uc_order_products',
    'left_field' => 'nid',
    'field' => 'nid',
  );

  $data['term_node']['table']['join'] = array(
    'uc_orders' => array(
      'left_table' => 'uc_order_products',
      'left_field' => 'nid',
      'field' => 'nid',
    ),
  );

  $data['node']['table']['join']['uc_order_products'] = array(
    'left_field' => 'nid',
    'field' => 'nid',
  );
  $data['uc_orders']['table']['join']['node'] = array(
    'left_table' => 'uc_order_products',
    'left_field' => 'order_id',
    'field' => 'order_id',
  );
  $data['uc_orders']['table']['join']['uc_order_products'] = array(
    'left_field' => 'order_id',
    'field' => 'order_id',
  );

  //sammys: optionally link to order and allow order_id as argument
  $data['uc_orders']['order_id'] = array(
    'title' => t('Order ID'),
    'help' => $order_schema['fields']['order_id']['description'],
    'field' => array(
      'handler' => 'uc_views_handler_field_order_id',
      'click sortable' => TRUE,
    ),
     'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    // sammys: Information for accepting an order id as an argument
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'name field' => 'title',
      'numeric' => TRUE,
      'validate type' => 'order_id',
    ),
  );

//  $data['uc_orders']['distinct'] = array(
//    'title' => t('Distinct Order ID'),
//    'help' => t('Forces the query to distinctly select order id\'s, reducing duplicates.'),
//    'filter' => array(
//      'handler' => 'uc_views_handler_filter_distinct',
//    ),
//  );
//
  $data['uc_orders']['order_status'] = array(
    'title' => t('Order Status'),
    'help' => $order_schema['fields']['order_status']['description'],
    'field' => array(
      'handler' => 'uc_views_handler_field_order_status',
      'click sortable' => TRUE,
    ),
     'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_order_status',
    ),
  );
  $data['uc_orders']['uid'] = array(
    'title' => t('Order User ID'),
    'help' => $order_schema['fields']['uid']['description'],
    'field' => array(
      'handler' => 'views_handler_field_user',
      'click sortable' => TRUE,
    ),
     'argument' => array(
      'handler' => 'views_handler_argument_user_uid',
      'name field' => 'name', // the field to display in the summary.
      'numeric' => TRUE,
      'validate type' => 'uid',
    ),
     'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'relationship' => array(
      'base' => 'users',
      'field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('Order User'),
    ),
  );
  //sammys: Changed field handler to display as a price
  $data['uc_orders']['order_total'] = array(
    'title' => t('Order total'),
    'help' => $order_schema['fields']['order_total']['description'],
    'field' => array(
      'handler' => 'uc_views_handler_field_money_amount',
      'click sortable' => TRUE,
      'float' => TRUE,
    ),
     'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_float',
    ),
  );
  $data['uc_orders']['payment_method'] = array(
    'title' => t('Order Payment Method'),
    'help' => $order_schema['fields']['payment_method']['description'],
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
     'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'uc_views_handler_filter_payment_method',
    ),
  );
  $data['uc_orders']['created'] = array(
    'title' => t('Order Creation Date'),
    'help' => $order_schema['fields']['created']['description'],
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
     'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );
  $data['uc_orders']['product_count'] = array(
    'title' => t('Order product count'),
    'help' => $order_schema['fields']['product_count']['description']." (currently only available for dev versions of UC)",
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  //Use the sleeker date_api views handler if the module is available
  if(module_exists('date_api')) {
    $data['uc_orders']['created']['filter']['handler'] = 'date_api_filter_handler';
  }

  $data['uc_orders']['modified'] = array(
    'title' => t('Order Modification Date'),
    'help' => $order_schema['fields']['modified']['description'],
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
     'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  //Use the sleeker date_api views handler if the module is available
  if(module_exists('date_api')) {
    $data['uc_orders']['modified']['filter']['handler'] = 'date_api_filter_handler';
  }

  $data['uc_orders']['actions'] = array(
    'title' => t('Order Actions'),
    'help' => 'Returns the actions a user may perform on an order.',
    'field' => array(
      'handler' => 'uc_views_handler_field_order_actions',
      'click sortable' => FALSE,
    ),
  );

  $data['uc_orders']['primary_email'] = array(
    'title' => t('Email address'),
    'help' => t($order_schema['fields']['primary_email']['description']),
    'field' => array(
      'handler' => 'views_handler_field_user_mail',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  $string_fields = array(
    'first_name' => 'first name',
    'last_name' => 'last name',
    'phone' => 'phone number',
    'company' => 'company',
    'street1' => 'address line 1',
    'street2' => 'address line 2',
    'city' => 'city',
    'postal_code' => 'zip/postal code',
  );

  foreach (array('delivery', 'billing') as $prefix) {
    $args = array(
      '!titleprefix' => ($prefix == 'delivery' ? UC_RECIPIENT_PREFIX : UC_PAYER_PREFIX),
    );
    foreach ($string_fields as $field => $suffix) {
      $data['uc_orders'][$prefix.'_'.$field] = array(
        'title' => t('!titleprefix !titlesuffix', $args + array('!titlesuffix' => $suffix)),
        'help' => $order_schema['fields'][$prefix.'_'.$field]['description'],
        'field' => array(
          'handler' => 'views_handler_field',
          'click sortable' => TRUE,
        ),
        'sort' => array(
          'handler' => 'views_handler_sort',
        ),
        'filter' => array(
          'handler' => 'views_handler_filter_string',
        ),
      );
    }
  }

  //Added by openbook
  $comments_schema = drupal_get_schema('uc_order_comments');
  $data['uc_order_comments']['table']['join'] = array(
    'uc_orders' => array(
      'left_field' => 'order_id',
      'field' => 'order_id',
    ),
  );
  $data['uc_order_comments']['table']['join'] = array(
    'uc_order_products' => array(
      'left_field' => 'order_id',
      'field' => 'order_id',
    ),
  );
  $data['uc_order_comments']['table']['group'] = t('Ubercart order comments');
  $data['uc_order_comments']['message'] = array(
    'title' => t('Order Comments'),
    'help' => t($comments_schema['fields']['message']['description']),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
  );

  /**
   * Countries: added by sammys
   */

  $country_schema = drupal_get_schema('uc_countries');

  $country_views = array(
    'billing_countries' => array(
      'field' => 'billing_country',
      'prefix' => UC_PAYER_PREFIX,
    ),
    'delivery_countries' => array(
      'field' => 'delivery_country',
      'prefix' => UC_RECIPIENT_PREFIX,
    ),
  );

  foreach ($country_views as $key => $value) {
    $data['uc_orders'][$value['field']] = array(
      'title' => t($value['prefix'].' country'),
      'help' => t('Relate orders to '.str_replace('_', ' ', $value['field'])),
      'filter' => array(
        'handler' => 'uc_views_handler_filter_country',
      ),
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'base' => 'uc_countries',
        'relationship table' => $key,
        'field' => 'country_id',
        'label' => t($value['prefix'].' country'),
      ),
    );

    $data[$key]['table']['group'] = t('Ubercart order');

    $data[$key]['table']['join']['uc_orders'] = array(
      'table' => 'uc_countries',
      'left_field' => $value['field'],
      'field' => 'country_id',
    );
    $data[$key]['country_id'] = array(
      'title' => t($value['prefix'].' country id'),
      'help' => t($order_schema['fields'][$value['field']]['description']),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      // Information for accepting an order id as an argument
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
        'name field' => 'country_iso_code_2',
        'numeric' => TRUE,
        'validate type' => 'country_id',
      ),
      // Information for accepting an order id as a filter
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      // Information for sorting on an order id.
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );
    $data[$key]['country_name'] = array(
      'title' => t($value['prefix'].' country name'),
      'help' => t(str_replace(' ID ', ' ', $order_schema['fields'][$value['field']]['description'])),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
    );
    $data[$key]['country_iso_code_2'] = array(
      'title' => t($value['prefix'].' country ISO code (2 characters)'),
      'help' => t($country_schema['fields']['country_iso_code_2']['description']),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
    );
    $data[$key]['country_iso_code_3'] = array(
      'title' => t($value['prefix'].' country ISO code (3 characters)'),
      'help' => t($country_schema['fields']['country_iso_code_3']['description']),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
    );
  }

  /**
   * Zones: added by sammys
   */
  $zone_schema = drupal_get_schema('uc_zones');

  $zone_views = array(
    'billing_zones' => array(
      'field' => 'billing_zone',
      'prefix' => UC_PAYER_PREFIX,
    ),
    'delivery_zones' => array(
      'field' => 'delivery_zone',
      'prefix' => UC_RECIPIENT_PREFIX,
    ),
  );

  foreach ($zone_views as $key => $value) {
    $data['uc_orders'][$value['field']] = array(
      'title' => t($value['prefix'].' zone'),
      'help' => t('Relate orders to '.str_replace('_', ' ', $value['field'])),
      'filter' => array(
        'handler' => 'uc_views_handler_filter_zone',
      ),
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'base' => 'uc_zones',
        'relationship table' => $key,
        'field' => 'zone_id',
        'label' => t($value['prefix'].' zone'),
      ),
    );

    $data[$key]['table']['group'] = t('Ubercart order');

    $data[$key]['table']['join']['uc_orders'] = array(
      'table' => 'uc_zones',
      'left_field' => $value['field'],
      'field' => 'zone_id',
    );
    $data[$key]['zone_id'] = array(
      'title' => t($value['prefix'].' zone id'),
      'help' => t($order_schema['fields'][$value['field']]['description']),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      // Information for accepting an order id as an argument
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
        'name field' => 'zone_code',
        'numeric' => TRUE,
        'validate type' => 'zone_id',
      ),
      // Information for accepting an order id as a filter
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      // Information for sorting on an order id.
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );
    $data[$key]['zone_name'] = array(
      'title' => t($value['prefix'].' zone name'),
      'help' => t(ereg_replace(' (id|ID) ', ' ', $order_schema['fields'][$value['field']]['description'])),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
    );

    $data[$key]['zone_code'] = array(
      'title' => t($value['prefix'].' zone code'),
      'help' => t(str_replace('of the ', 'of the '.strtolower($value['prefix']).' ', $zone_schema['fields']['zone_code']['description'])),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
    );
  }

  // Support for any module's line item, if new modules defines other line items
  // the views cache will have to be rebuilt
  // Although new line items views support should be defined on each module,
  // I don't think this wider apporach would harm. At most, it will duplicate
  // line items
  $line_items = array();
  foreach (_line_item_list() as $line_item) {
    if (!in_array($line_item['id'], array('subtotal', 'tax_subtotal', 'total', 'generic'))) {
      $line_items[$line_item['id']] = $line_item['title'];
    }
  }
  foreach ($line_items as $line_item_id => $line_item_desc) {
    $data['uc_order_line_items_' . $line_item_id]['table']['join']['uc_orders'] = array(
    'table' => 'uc_order_line_items',
    'left_field' => 'order_id',
    'field' => 'order_id',
    'extra' => array(
      array(
        'field' => 'type',
          'value' => $line_item_id,
      ),
    ),
  );
    $data['uc_order_line_items_' . $line_item_id]['table']['group'] = t('Ubercart order line item');
    $data['uc_order_line_items_' . $line_item_id]['title'] = array(
      'title' => t('!line_item_desc title', array('!line_item_desc' => $line_item_desc)),
      'help' => t('!line_item_desc order line item', array('!line_item_desc' => $line_item_desc)),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
  );
    $data['uc_order_line_items_' . $line_item_id]['amount'] = array(
      'title' => t('!line_item_desc amount', array('!line_item_desc' => $line_item_desc)),
      'help' => t('!line_item_desc order line item', array('!line_item_desc' => $line_item_desc)),
    'field' => array(
      'handler' => 'uc_views_handler_field_money_amount',
      'click sortable' => TRUE,
        'float' => TRUE,
    ),
  );
  }
  
  //Make the uc_cart_products table visible for Views module
  $data['uc_cart_products']['table']['group'] = t('Cart product');
  $data['uc_cart_products']['table']['join']['node'] = array(
      'left_field' => 'nid',
      'field' => 'nid',
  );
  $data['uc_cart_products']['table']['join']['users'] = array(
      'left_field' => 'uid',
      'field' => 'cart_id',
  );
  $data['uc_cart_products']['nid'] = array(
    'title' => t('Cart product id'),
    'help' => t('The nid of the product for in a cart.'),
    'field' => array(
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
    ),
     'argument' => array(
      'handler' => 'views_handler_argument_node_nid',
      'name field' => 'title', // the field to display in the summary.
      'numeric' => TRUE,
      'validate type' => 'nid',
    ),
     'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );
  $data['uc_cart_products']['cart_id'] = array(
    'title' => t('Cart id'),
    'help' => t('The id of the cart (either user or session id).'),
    'field' => array(
      'handler' => 'uc_views_handler_field_cart_user',
      'click sortable' => TRUE,
    ),
     'argument' => array(
      'handler' => 'views_handler_argument_user_uid',
      'name field' => 'name', // the field to display in the summary.
      'numeric' => TRUE,
      'validate type' => 'cart_id',
    ),
     'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );
  $data['uc_cart_products']['qty'] = array(
    'title' => t('qty'),
    'help' => t('The amount ordered.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );
  $data['uc_cart_products']['changed'] = array(
    'title' => t('Cart Product Line Changed Date'),
    'help' => t('The date the cart line was changed'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
     'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  $data['uc_cart_products']['ordering'] = array(
    'title' => t('List position'),
    'help' => t('The default sort criteria in the catalog.'),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  //Extra handlers for uc_products
  $data['uc_products']['conditionalbuyitnowbutton'] = array(
    'title' => t('Conditional buy it now button'),
    'help' => t('A button to add a product to the cart without quantity or attribute fields, if certain conditions are met.'),
    'group' => t('Product'),
    'field' => array(
      'table' => 'node',
      'additional fields' => array(
        'nid' => array(
          'table' => 'node',
          'field' => 'nid',
        ),      ),
      'handler' => 'uc_views_handler_field_conditional_buyitnow',
    ),
  );
  $data['uc_products']['conditionaladdtocart'] = array(
    'title' => t('Conditional add to cart form'),
    'help' => t("Form to put the product in the customer's cart, if certain conditions are met."),
    'group' => t('Product'),
    'field' => array(
      'additional fields' => array(
        'nid' => array(
          'table' => 'node',
          'field' => 'nid',
        ),
        'type' => array(
          'table' => 'node',
          'field' => 'type',
        ),
      ),
      'handler' => 'uc_views_handler_field_conditional_addtocart',
    ),
  );
  //Thank you hanoii for this addition
  $data['uc_products']['ordering'] = array(
  'field' => array(
    'help' => t("The product list position."),
    'handler' => 'views_handler_field_numeric',
  ),
);


  return $data;
}

/**
 * Implementation of hook_views_handlers().
 */
function uc_views_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'uc_views') . '/views',
    ),
    'handlers' => array(
      // fields
      'uc_views_handler_field_order_id' => array( 'parent' => 'views_handler_field',),
      'uc_views_handler_field_cart_user' => array('parent' => 'views_handler_field',),
      'uc_views_handler_field_order_actions' => array( 'parent' => 'views_handler_field',),
      'uc_views_handler_field_country' => array( 'parent' => 'views_handler_field',),
      'uc_views_handler_field_zone' => array( 'parent' => 'views_handler_field',),
      'uc_views_handler_field_order_status' => array( 'parent' => 'views_handler_field',),
      'uc_views_handler_field_conditional_buyitnow' => array('parent' => 'views_handler_field',),
      'uc_views_handler_field_conditional_addtocart' => array('parent' => 'views_handler_field',),
      'uc_views_handler_field_money_amount' => array('parent' => 'uc_product_handler_field_price',),
      //fiters
      'views_handler_filter_order_status' => array('parent' => 'views_handler_filter_in_operator',),
      'uc_views_handler_filter_country' => array('parent' => 'views_handler_filter_in_operator',),
      'uc_views_handler_filter_zone' => array('parent' => 'views_handler_filter_in_operator',),
      'uc_views_handler_filter_payment_method' => array('parent' => 'views_handler_filter_in_operator',),
//      'uc_views_handler_filter_distinct' => array('parent' => 'views_handler_filter',),
    ),
  );
}

/**
 * Conditionally add editablefields support.
 */
function uc_views_views_tables_alter(&$tables) {
}

function uc_views_views_plugins() {
  return array(
    'module' => 'uc_views', // This just tells our themes are elsewhere.
    'row' => array(
      'uc_views_invoice' => array(
        'title' => t('Invoice'),
        'help' => t('Display the order with standard invoice view.'),
        'handler' => 'uc_views_plugin_row_invoice_view',
        'path' => drupal_get_path('module', 'uc_views') . '/views',
        'theme' => 'uc_views_view_row_invoice',
        'theme path' => drupal_get_path('module', 'uc_views') . '/theme',
        'theme file' => 'theme.inc',
        'base' => array('uc_orders'), // only works with 'uc_orders' as base.
        //'uses options' => TRUE,
        'type' => 'normal',
      ),
    ),
  );
}