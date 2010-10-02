<?php
// $Id: ecard.views.inc,v 1.1.2.1 2009/10/30 18:20:17 karst Exp $

/**
 * Implementation of hook_views_data().
 */
function ecard_views_data() {
  $data = array();

  $data['ecard']['table']['group'] = t('Ecard');
  $data['ecard']['table']['base'] = array(
    'field' => 'random',
    'title' => t('Ecard'),
    'help' => t('E-cards sent using the ecard module'),
  );

  $data['ecard']['random'] = array(
    'title' => t('Ecard id'),
    'help' => t('The id of the ecard'),
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

  $data['ecard']['nid'] = array(
    'title' => t('Nid'),
    'help' => t('The node id of the node'),
    'field' => array(
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_node_nid',
      'parent' => 'views_handler_argument_numeric',
    ),
  );

  $data['ecard']['sender_name'] = array(
    'title' => t("Sender's name"),
    'help' => t('The name of the sender'),
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

  $data['ecard']['sender_email'] = array(
    'title' => t("Sender's email"),
    'help' => t('The email address of the sender'),
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

  $data['ecard']['recp_mail'] = array(
    'title' => t("Recipient's email"),
    'help' => t('The email address of the recipient'),
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

  $data['ecard']['message'] = array(
    'title' => t('Message'),
    'help' => t('The ecard personalized message'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
  );

  $data['ecard']['send_time'] = array(
    'title' => t('Sent time'),
    'help' => t('The time the card was sent'),
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

  return $data;
}
