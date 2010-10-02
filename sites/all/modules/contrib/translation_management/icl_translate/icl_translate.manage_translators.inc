<?php 

module_load_include('inc', 'icl_translate', 'icl_translate.translator');

function icl_translator_manage_translators_form_cb(){
  return _icl_wrapper_drupal_get_form('icl_translator_manage_translators_form');
}

function theme_icl_translator_translators_form($form) {

  if (defined('ICL_D7')) {
    $form = $form['form'];
  }

  $header = array(t('Name'), t('Languages'), t('Edit'));
  $rows = array();
  foreach (element_children($form['name']) as $key) {
    $row = array();
    $row[] = drupal_render(_icl_wrapper_form_create_markup($form['name'][$key]));
    $row[] = drupal_render(_icl_wrapper_form_create_markup($form['langs'][$key]));
    $row[] = drupal_render(_icl_wrapper_form_create_markup($form['edit'][$key]));
    $rows[] = $row;
  }
    
  $output = _icl_wrapper_theme('table', array('header' => $header, 'rows' => $rows));

  return $output;
}

function icl_translator_manage_translators_form(){
  
  $form['add'] = _icl_wrapper_form_create_markup(array(
      '#type' => 'markup',
      '#value' => t('Translators are special Drupal users who can translate between language pairs that you choose.
                    <br />
                    <br />
                    You can give translator privileges to existing users by clicking on <a href="@url">add translator</a>.
                    If your translators aren\'t yet a Drupal user, you must first <a href="@create_url">create new Drupal users</a> for them.',
                    array('@url' => _icl_wrapper_url(_icl_wrapper_get_root_menu('translation-management/add-translator')),
                          '@create_url' => _icl_wrapper_url(_icl_wrapper_get_drupal_menu('admin/user/user/create')))),
  ));
  
  $form['users'] = array(
    'name' => array(),
    'langs' => array(),
    'edit' => array(),
    '#theme' => 'icl_translator_translators_form',
  );

  if (isset($_GET['from']) && isset($_GET['to'])) {
    $args = array('from_lang' => $_GET['from'], 'to_lang' => $_GET['to']);
  } else {
    $args = NULL;
  }

  $translators = _icl_translate_get_translators($args);
  $languages_list = language_list('enabled');
  $languages_list = $languages_list[1];
  
  foreach ($translators as $trans) {
    $form['users']['name'][] = array('#value' => $trans['name']);
    
    $pairs = _icl_translate_get_translator_pairs($trans['uid']);
    $langs = '';
    foreach ($pairs as $from => $to_list) {
      foreach ($to_list as $to => $temp) {
        if ($langs != '') {
          $langs .= '<br />';
        }
        $langs .= $languages_list[$from]->name . t(' to ') . $languages_list[$to]->name;
      }
    }
    
    $form['users']['langs'][] = _icl_wrapper_form_create_markup(array('#value' => $langs));
    $form['users']['edit'][] = _icl_wrapper_form_create_markup(array('#value' =>  l(t('edit languages'), _icl_wrapper_get_root_menu('translation-management/translator/' . $trans['uid'] . '/edit'))));
  }
  

  return $form;
    
}