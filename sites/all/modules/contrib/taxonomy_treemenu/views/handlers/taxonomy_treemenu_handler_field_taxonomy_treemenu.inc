<?php
// $Id: taxonomy_treemenu_handler_field_taxonomy_treemenu.inc,v 1.1 2009/08/03 18:57:48 rcrowther Exp $
/**
 * @file
 * Contains the basic 'taxonomy treemenu' field handler.
 */

/**
 * Field handler to provide simple renderer that allows linking to a node.
 */
class taxonomy_treemenu_handler_field_taxonomy_treemenu extends views_handler_field {
  /**
   * Constructor to provide additional field to add.
   */
  function construct() {
    parent::construct();
    $this->additional_fields['menu_name'] = 'menu_name';
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_treemenu'] = array('default' => FALSE);
    return $options;
  }

  /**
   * Provide link to node option
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['link_to_treemenu'] = array(
      '#title' => t('Link this field to the treemenu'),
      '#description' => t('This will override any other link you have set.'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['link_to_treemenu']),
    );
  }

  /**
   * Render whatever the data is as a link to the node.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    if (!empty($this->options['link_to_treemenu']) && $data !== NULL && $data !== '') {
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = "ttm/" . $values->{$this->aliases['menu_name']};
    }
    return $data;
  }

  function render($values) {
    return $this->render_link(check_plain($values->{$this->field_alias}), $values);
  }
}