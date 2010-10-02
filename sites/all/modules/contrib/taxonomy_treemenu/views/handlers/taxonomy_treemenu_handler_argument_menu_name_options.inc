<?php
// $Id: taxonomy_treemenu_handler_argument_menu_name_options.inc,v 1.1 2009/08/03 18:57:48 rcrowther Exp $
/**
 * @defgroup views_sort_handlers Views' sort handlers
 * @{
 * Handlers to tell Views how to sort queries
 */


/**
 * handler to gather Taxonomy Treemenu options and forcibly apply them to the view.
 */
// TO CONSIDER: Using init the handlers can be made to appear in the gui,
//  which is nice. But there is no arguement, so the handlers can't be set.
// I failed at initing the handlers from pre_query(). maybe there could be
// options could go in afterwards? This is one for the adventurous coder.
class taxonomy_treemenu_handler_argument_menu_name_options extends views_handler_argument_string {

  function destroy_option(&$target,$option) {
    if ($target->is_defaulted($option)) {
      $this->destroy_option($target->default_display, $option);
      return;
    }
    $target->display->display_options[$option]= array();
    $target->options[$option] = array();
  }

  // Destroy all sort and filter options in this view.
  function init(&$view, $options) {
    parent::init($view, $options);
    $types = views_object_types();
    $display_id = $view->current_display;
    // Destroy sorts.
    $this->destroy_option($view->display[$display_id]->handler, $types['sort']['plural']);
    // Destroy filters.
    $this->destroy_option($view->display[$display_id]->handler, $types['filter']['plural']);
  }


  // TO CONSIDER: Words cann't express how bad this is, but they can crapping try.
  // Having failed to summon a validator, we just build half the form.
  // See views_handler_argument().
  function options_form(&$form, &$form_state) {
    $defaults = $this->default_actions();

    $form['title'] = array(
      '#prefix' => '<div class="clear-block">',
      '#suffix' => '</div>',
      '#type' => 'textfield',
      '#title' => t('Title'),
      '#default_value' => $this->options['title'],
      '#description' => t('The title to use when this argument is present. It will override the title of the view and titles from previous arguments. You can use percent substitution here to replace with argument titles. Use "%1" for the first argument, "%2" for the second, etc.'),
    );

    $form['clear_start'] = array(
      '#value' => '<div class="clear-block">',
    );

    $form['defaults_start'] = array(
      '#value' => '<div class="views-left-50">',
    );

    $form['default_action'] = array(
      '#type' => 'radios',
      '#title' => t('Action to take if argument is not present'),
      '#default_value' => $this->options['default_action'],
    );

    $form['defaults_stop'] = array(
      '#value' => '</div>',
    );

    $form['wildcard'] = array(
      '#prefix' => '<div class="views-right-50">',
      // prefix and no suffix means these two items will be grouped together.
      '#type' => 'textfield',
      '#title' => t('Wildcard'),
      '#size' => 20,
      '#default_value' => $this->options['wildcard'],
      '#description' => t('If this value is received as an argument, the argument will be ignored; i.e, "all values"'),
    );

    $form['wildcard_substitution'] = array(
      '#suffix' => '</div>',
      '#type' => 'textfield',
      '#title' => t('Wildcard title'),
      '#size' => 20,
      '#default_value' => $this->options['wildcard_substitution'],
      '#description' => t('The title to use for the wildcard in substitutions elsewhere.'),
    );

    $form['clear_stop'] = array(
      '#value' => '</div>',
    );

    $options = array();
    $validate_options = array();
    foreach ($defaults as $id => $info) {
      $options[$id] = $info['title'];
      if (empty($info['default only'])) {
        $validate_options[$id] = $info['title'];
      }
      if (!empty($info['form method'])) {
        $this->{$info['form method']}($form, $form_state);
      }
    }

    $form['default_action']['#options'] = $options;
  }


  // The only pissing way I have found of doing this.
  // Preferably, we would adjust the validator list.
  function validate_argument_basic($arg) {
    //dpm('validate');
    //dvm(TTMData::validateMenuName($arg));
    return TTMData::validateMenuName($arg);

  }

  // We must work on the base table and it's alias, not the table 'taxonomy_treemenu'.
  // I believe the base table goes in by default, but we assert existance anyhow.
  function query() {
    $this->query->ensure_table($this->view->base_table);
    $menu_name = $this->get_value();
    $opts = TTMData::opts($menu_name);
    $opts = TTMOptsQuery::viewsArgs( TTMData::opts($menu_name) );

    // sorts
    foreach ($opts['sort'] as $oid => $opt) {
      $this->query->add_orderby($this->query->base_table, $opt['field'], $opt['value']);
    }

    // filters
    $i = 1;
    foreach ($opts['filter'] as $opt) {
      if (is_array($opt['value'])) {
        $this->query->set_where_group('OR', $i,'where');
        foreach ($opt['value'] as $val => $on) {
           $this->query->add_where($i, $this->query->base_table.'.'.$opt['field'] .' = "%s"', $val);
        }
        $i++;
      }
      else {
        $q = $this->query->base_table.'.'.$opt['field']." = %d";
        $this->query->add_where($i++, $q , $opt['value']);
      }
    }

  }
  
}
