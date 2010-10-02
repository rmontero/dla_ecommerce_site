<?php
// $Id: fb_social_like.admin.inc,v 1.3.2.2 2010/05/07 02:49:42 ferdi Exp $


/**
 * Settings form
 */
function fb_social_like_settings_form(){
  $form = array();

  $form['node_types'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content types'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['node_types']['fb_social_like_node_types'] = array(
    '#type' => 'checkboxes',
    '#description' => t('Select types that will use the facebook like plugin'),
    '#default_value' => variable_get('fb_social_like_node_types', array()),
    '#options' => node_get_types('names'),
  );
  
  $form['widget_location'] = array(
    '#type' => 'fieldset',
    '#title' => t('Widget location and display'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['widget_location']['fb_social_like_location'] = array(
    '#type' => 'radios',
    '#title' => t('widget location'),
    '#default_value' => variable_get('fb_social_like_location', 0),
    '#options' => array(t('Node links'), t('Node content')),
    '#description' => t('The widget can be printed in the "links" are of the node or as part of the node content'),
  );
  
  $form['widget_location']['fb_social_like_display_teasers'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show on teasers'),
    '#default_value' => variable_get('fb_social_like_display_teasers', 1),
    '#description' => t('Should the widget be displayed on teasers?'),
  );
  
  $form['widget_setings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Widget settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  
  $form['widget_setings']['fb_social_like_layout_style'] = array(
    '#type' => 'select',
    '#title' => t('Layout style'),
    '#description' => t('Determines the size and the amount of the social context next to the button'),
    '#default_value' => variable_get('fb_social_like_layout_style', 'button_count'),
    '#options' => array('standard' => t('standard'), 'button_count' => t('button_count') )
  );
  
  $form['widget_setings']['fb_social_like_show_faces'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show faces'),
    '#description' => t('Show profiles pictures below the button'),
    '#default_value' => variable_get('fb_social_like_show_faces', 0),
  );
  
  $form['widget_setings']['fb_social_like_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#description' => t('The width of the plugin in pixel'),
    '#default_value' => variable_get('fb_social_like_width', 350),
  );
  
  $form['widget_setings']['fb_social_like_verb'] = array(
    '#type' => 'select',
    '#title' => t('Verb'),
    '#description' => t('The verb to display in the button'),
    '#default_value' => variable_get('fb_social_like_verb', 'like'),
    '#options' => array('like' => t('like'), 'recommend' => t('recommend') )
  );
  
  $form['widget_setings']['fb_social_like_font'] = array(
    '#type' => 'select',
    '#title' => t('Font'),
    '#description' => t('The font of the plugin'),
    '#default_value' => variable_get('fb_social_like_font', 'verdana'),
    '#options' => array('arial' => t('arial'), 'verdana' => t('verdana') )
  );
  
  $form['widget_setings']['fb_social_like_color'] = array(
    '#type' => 'select',
    '#title' => t('Color'),
    '#description' => t('The color scheme of the plugin'),
    '#default_value' => variable_get('fb_social_like_color', 'dark'),
    '#options' => array('dark' => t('dark'), 'light' => t('light'), 'evil' => t('evil') )
  );
  
  $form = system_settings_form($form);
  return $form;
}