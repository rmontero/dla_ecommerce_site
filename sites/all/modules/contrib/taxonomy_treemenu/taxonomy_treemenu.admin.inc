<?php
// $Id: taxonomy_treemenu.admin.inc,v 1.31 2009/10/12 19:58:01 rcrowther Exp $

// WARNING: This is a large file with lots of code for handling options,
//  which need to be bulked up with admin display info, then re-converted
//  back into an internal format.
// Don't remove this line thoughtlessly!
require_once(drupal_get_path('module', 'taxonomy_treemenu') . '/includes/taxonomy_treemenu.options.forms.inc');


// Here for temporary work
//require_once(drupal_get_path('module', 'taxonomy_treemenu') . '/jsmin-1.1.1.php');

// Output a minified version of example.js.
//dpm( JSMin::minify(file_get_contents(drupal_get_path('module', 'taxonomy_treemenu') . '/js/json.js')));

/******************************
 * Menu rebuild hook functions
 *****************************/


function taxonomy_treemenu_menu_update_submit($form, &$form_state) {
  $menu = $form_state['values'];
  $action_list = array();
  $done = array();
  $rebuild_menu = 0;

  // I dunno what Drupal does with these arrays, but I don't trust them
  // to preserve ordering, or anything usable.
  foreach ($menu['targets'] as $key => $target ) {
    if (!$target) continue;
    // Menu checkbox, not default options.
    $index = strpos($key, '_default');
    if ($index === FALSE) {
      $action_list[$key]['update_menu'] = TRUE;
    }
  }

  foreach ($action_list as $menu_name => $info) {
    $action_list[$menu_name]['default_ops'] = ($menu['targets'][$menu_name . '_default'] == TRUE);
  }

  foreach ($action_list as $menu_name => $info) {
      $ok = taxonomy_treemenu_rebuild($menu_name, $info['default_ops']);
      if(!$ok) {
         $failed[$menu_name] = $info;
         unset($action_list[$menu_name]);
      }
  }

  if(empty($action_list)) {
    drupal_set_message(t('No treemenus were rebuilt.'));
  }
  else {
    drupal_set_message(format_plural(count($action_list), '1 treemenu rebuilt;', '@count treemenus rebuilt:'));
    foreach ($action_list as $menu_name => $info ) {
      $display_str = $menu_name;
      $display_str .= ($info['default_ops']) ? ' (link display options also reset) ' : '';
      drupal_set_message(t('%menu', array('%menu' => $display_str)) );
    }
  }

  if(!empty($failed)) {
    drupal_set_message(format_plural(count($failed), '1 treemenu failed to rebuild;', '@count treemenus failed to rebuild:'));
    foreach ($failed as $menu_name => $info ) {
      $display_str = $menu_name;
      $display_str .= ($info['default_ops']) ? ' (also attempted to reset link display options) ' : '';
      drupal_set_message(t('%menu', array('%menu' => $display_str)) );
    }
  }

  $form_state['redirect'] = $menu['destination'];  
}


function taxonomy_treemenu_menu_update_validate($form, &$form_state) {
  $checked = FALSE;

  foreach ($form_state['values']['targets'] as $key => $target) {
    if(strpos($key, '_default') === FALSE) {
      if (!empty($target)) {$checked = TRUE;}
    }
  }
  if (!$checked) {
    form_set_error('targets', t("Please choose at least one menu!"));    
  }
}


function taxonomy_treemenu_menu_update($form_state) {
  $form = array();
  $options = array();

  $tms = TTMData::allNames();
  foreach ($tms as $menu_name) {
    $options[$menu_name] = substr($menu_name , strpos($menu_name, '-') + 1);
    $options[$menu_name . '_default'] = ' (link options to default)';
  }
  
  // We don't want the persistancy of a drupal warning message,
  // but we do want it's immediacy, so we steal the css class.
  // TODO: Would a no-repeat work?
  $form['message'] = array(
    '#value' => "<div class='messages warning'>" . t("Treemenus should update automatically. This form provides some disaster recovery. If subnitted, this form rebuilds parentage of the checked menu(s), checking against the taxonomy. In the worst case, these functions will delete and rebuild all links. This may not be the problem - try emptying Drupal and browser caches, or a menu rebuild, first.") . "</div>",
  );
    $form['message2'] = array(
    '#value' => "<div>" . t("The (link options to default) checkbox additionally removes 'hidden' 'expanded' and HTML customization from links, even if the links exist and are otherwise corectly parented.") . "</div>",
  );

  $form['targets'] = array(
    '#type' => 'checkboxes',
    '#options' => $options,
    '#default value' => '',
    '#description' => t('Perform a full update on any treemenu individually.'),
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Rebuild'),
  );
  $form['back'] = array(
  '#type' => 'value',
  '#value' => $_GET['destination'],
  );

  return $form;
}



/**
 * Function for generating a set of options for selecting a branch from the taxonomy.
 * Not the same options or keys as taxonomy_form_all(). And we allow free tagged
 * vocabularies.
 */
function taxonomy_treemenu_form_all() {
  $vocabularies = taxonomy_get_vocabularies();
  $options = array();
  $options['-:-'] = t('choose a branch here');
  foreach ($vocabularies as $vid => $vocabulary) {
    $vocab_path = $vid . ':';
    $options[$vocab_path . '0'] = $vocabulary->name . ' (vocab)';
    $tree = taxonomy_get_tree($vid);
    if ($tree && (count($tree) > 0)) {
    
      //$options[$vocabulary->name] = array();
      foreach ($tree as $term) {
        $options[$vocab_path . $term->tid] = str_repeat('-', $term->depth + 1) . $term->name;
        //$options[$vocabulary->name][$term->tid] = str_repeat('-', $term->depth) . $term->name;
      }
    }
  }
  return $options;
}

function taxonomy_treemenu_languages_options() {
  $languages = language_list('language');
  array_walk($languages, create_function('&$data, $k', '$data = $data->language;'));
  return $languages;
}

/***************************
 * Themes and preprocessors
 **************************/

/**
 * Use the stock form-element theme.
 */
function theme_branch_selector_by_option($element) {
  return theme('form_element', $element, $element['#children']);
}

function theme_branch_selector_by_gui($element) {
  drupal_add_js(drupal_get_path('module', 'taxonomy_treemenu') .'/js/gui_selector.js');
  return theme('form_element', $element, $element['#children']);
}


function theme_taxonomy_admin_js_link($title, $id) {
//edit-tree-root-data
  return "<a href='#' onclick=\"updateTargetBranch('" . $id . "')\">" . check_plain(trim($title)) . "</a>";
}

function taxonomy_treemenu_taxonomy_tree_output() {
  $output = '';
  $items = taxonomy_treemenu_form_all();
  foreach($items as $id => $title) {
    $link = theme('taxonomy_admin_js_link', $title, $id);
    //dpm($link);
    $output .= theme('menu_item', $link, FALSE);
  }
  return $output ? theme('menu_tree', $output) : '';
}

function taxonomy_treemenu_taxonomy_full_admin_js_tree() {
  //static $taxonomy_treemenu_js_tree_output = array();
  //if (!isset($taxonomy_treemenu_js_tree_output)) {
    $taxonomy_treemenu_js_tree_output = taxonomy_treemenu_taxonomy_tree_output();
  //}
  return $taxonomy_treemenu_js_tree_output;
}


/********************************
 * Our element dragger
 *******************************/
//TODO:(should be in a subfile)

/**
 * Turn any set of divs in a div container into a draggable rorderable display.
 *
 * What it will not do:
 * Handle headers and sortable columns. You need drupal_add_tabledrag(), or
 * some kind of supermooth custom module?
 * What it will do:
 * Turn a stack of divs within a container div into a sortable drag and drop
 * interface, using Drupal stock theming. In addition, you can have several
 * swapboxes on one page.
 * 
 * The divs must have the class 'draggable' aded. As there is only one column,
 * unlike drupal_add_dragtable(), there is no need to specify column classes.
 *
 * Drupal tables come ready striped, if you use a Drupal theme. A swapbox is '
 * unthemed divs, soswapboxdrag() will stripe box contents for you. See also the
 * parameter switch to turn striping off. Uses stock Drupal classes of .odd/.even.
 *
 * For row move warnings (the asterix), drupal_add_tabledrag() inserts a html span
 * into the first td. This is not a good idea for swapbox rows, as they could 
 * contain complex content. So swapboxDrag() prepends its own span to the row, 
 * just after the draggable handle. The span has a class; "warning-container".
 *
 * The stock 'changed' warning, as in the original code, is inserted directly AFTER the
 * swapbox div. If this results in crazy theming, the way to go is to put your
 * swapbox into a liner/wrapper div. Then the warning appears in a tidy formatted 
 * way, at the bottom of the swapbox, between the container div and the wrapper.
 * See also the parameter switch to turn the warning off.
 *
 * @staticvar <type> $swapboxdrag_js_added
 * @param <type> $container_id
 * @param <type> $target_class
 * @param <type> $stripe
 *   Switch striping on or off. Stripe themeing of tables is stock Drupal, and
 *   good for acessibility, but may confuse the user in small swapboxes within
 *   a larger theme.
 * @param bool   $changed_warning
 *   Switch the stock warning off. With small swapboxes, in the context of forms,
 * the form itself implies a submit is nececesary.
 */

function taxonomy_treemenu_enable_swapboxdrag($container_id, $stripe = TRUE, $warning = TRUE) {
  static $swapboxdrag_js_added = FALSE;
  if (!$swapboxdrag_js_added) {
    drupal_add_js(drupal_get_path('module', 'taxonomy_treemenu') .'/js/swapboxdrag.js');
    $swapboxdrag_js_added = TRUE;
  }

  $settings['swapboxDrag'][$container_id] = array(
    'swapbox' => $container_id,
    'stripe' => $stripe,
    'changed_warning' => $warning,
  );
  drupal_add_js($settings, 'setting');
}


/***************************************************
 * Primary processes and themes for special elements
 ***************************************************/

/**
 * Generate a gui for selecting a branch, returning the path.
 */
 //now, the way we've done this, we do not have a full menu to call,
 //and Durpal doesn't supply one (which is why we are here in the first place)
 //Making a menu is kind of hard as a one off,
 //but a list should be possible by Drupal methods...
function expand_branch_selector_by_gui($element) {
  $element['tree_root_data'] = array(
    '#type' => 'hidden',
    '#default_value' => '-:-',
  );

  //Includes stolen CSS to make a 'required' asterix.
  // (we can't do that properly, as this selector uses a hidden field)
  //The result is real enough, as we do validate for value existance.
  $element['selector_display'] = array(
    '#prefix' => "<div><b>Select Branch:&nbsp</b>&nbsp<b>vocabulary:&nbsp</b><span id='branch-selector-display-vid'>no choice made</span>&nbsp<b>term:&nbsp</b><span id='branch-selector-display-tid'>no choice made</span><span class=\"form-required\">&nbsp*</span></div>",
    '#value' => taxonomy_treemenu_taxonomy_full_admin_js_tree(),
  );

  $element['#tree'] = TRUE;
  return $element;
}

/**
 * Generate a set of options for selecting a branch, returning the path.
 */
function expand_branch_selector_by_option($elements, $form_values) {
  $options = array();
  $paths = array();

  //get titles and info
  //$options['choose a branch here'] = taxonomy_treemenu_form_all();
  $options = taxonomy_treemenu_form_all();

  $elements['#type'] = 'select';
  $elements['#title'] = t('Select Branch');
  $elements['#default_value'] = '-:-';
  $elements['#options'] = $options;
  $elements['#tree'] = TRUE;
  //$elements['#description'] = t("On submiting, the chosen branch (and sub-branches, depending on the 'depth' setting) are turned into a custom menu.");
  return $elements;
}


function theme_depth($element) {
  return theme('form_element', $element, $element['#children']);
}

function expand_depth_selector($elements, $form_values) {
  $options = array('full depth');
  for ($i = 1; $i < 9; $i++) {
    $options[] = $i;
  }
  $elements['#type'] = 'select';
  $elements['#options'] = $options;  
  return $elements;
}

/**
 * Make a sole, empty div on a form.
 *
 * If you need a fieldset, use a fieldset. This element is intended for building
 *  your own widgets. It has a supplemenatry use in that it can invisibly group
 * form data, both for presentational and data retrieval purposes.
 * Things it can't do:
 * - collapse
 * - be a proper fieldset (which is HTML tag <fieldset>)
 * Things it can do:
 * - take children
 * - grow a unique id, which a fieldset will rightly not. If you wish to cut
 * down on HTML/css bloat, there is a special property '#no_id' which will do
 * as it says.
 *  - Takes #attributes.
 * - Unusually, it has no class of its own, and will take class attributes
 *   (Drupal form items usually have their own generic classes, or reject class).
 * So it is not a reduction of a fieldset. It allows group organisation and
 * presentation of data, which is more or less the definition of a div?
 *
 * Properties used: #no_id, #attributes.
 *
 * @param <type> $element
 * @return <type>
 */
function theme_ttm_div($element) {
  $id = !empty($element['#no_id']) && ($element['#no_id']) ? '' : 'id="' . $element['#id'] . '"' ;
  $output  = '<div '. $id .' '. drupal_attributes($element['#attributes']) .">\n";
  $output .= $element['#children'];
  $output .= "</div>\n";
  return $output;
}

/**
 * Empty box for creating your own widgets.
 * 
 * Is a full element, which takes children and #attributes. And provides titles
 * and descriptions and so forth. Be careful here, this theme takes the #id
 * property straight, so you don't want to place the same id on contents.
 * 
 * This theme can also be used as an item wrapper. If you neeed a Drupal-like 
 * box, use theme_item() or theme_form_element(). But this will work as
 * a outside-Drupal-styleing wrapper. You should unset the element [#id] if that
 *  will duplicate ids with primary content (the widget contains single active
 * element, say). And, with this usage, you'll want to preset the class to
 * something like .-wrapper'.
 *  Otherwise, quite like a form item, or a checkbox wrapper.
 * Takes #title, #required, #description, #attributes.
 *
 * @param <type> $element
 * @return <type>
 */

function theme_ttm_widget_container($element) {
  $output = '<div';
  if (!empty($element['#id'])) {
    $output .= ' id="'. $element['#id'] .'" ';
  }
  $output .= drupal_attributes($element['#attributes']);

  $output .= " >\n";
  $required = !empty($element['#required']) ? '<span class="form-required" title="'. t('This field is required.') .'">*</span>' : '';

  if (!empty($element['#title'])) {
    $title = $element['#title'];

    $element['#help_topic'] = !empty($element['#help_topic']) ? $element['#help_topic'] : '';
    $help = ttm_help_link($element['#help_topic'], 'icon', 'small', 'right');
    if (!empty($element['#id'])) {
      $output .= ' <label for="'. $element['#id'] .'">'. $help . t('!title: !required', array('!title' => filter_xss_admin($title), '!required' => $required)) ."</label>\n";
    }
    else {
      $output .= ' <label>'. $help . t('!title: !required', array('!title' => filter_xss_admin($title), '!required' => $required)) ."</label>\n";
    }
  }

  $output .= $element['#children'];

  if (!empty($element['#description'])) {
    $output .= ' <div class="description">'. $element['#description'] ."</div>\n";
  }

  $output .= "</div>\n";
  //dpm($output);
  return $output;
}


/**
 * A container for ttm tabs.
 *
 * @param <type> $element
 * @return <type>
 */
// Here's a novelty - Drupal's theme_checkboxes() is almost perfect for this. It
// could be a generalized function for all widget containers? Here's our
// version for tabs.
function theme_ttm_tab_container($element) {
  $class = 'ttm-tab-container';
  if (isset($element['#attributes']['class'])) {
    $class .= ' '. $element['#attributes']['class'];
  }
  
  // Trailing div below clears the container.
  // TODO: Druapl defaults.css container markup free clearing rules,
  // - when you figure how Druapl implements them.
  $element['#children'] = '<div class="'. $class .'">'. (!empty($element['#children']) ? $element['#children'] : '') .'</div>'.'<div style="clear:both"><div>';
  if ($element['#title'] || $element['#description']) {
    unset($element['#id']);
    // An un-Drupal widget, so use a container without form-item styling.
    //but if we make it a form item, it will get a bold label.
    $element['#attributes']['class'] = 'form-item';
    return theme('ttm_widget_container', $element);
  }
  else {
    return $element['#children'];
  }
}



/**
 * A tab to go in the tab container.
 * This item is slightly odd. Mostly it acts like an empty ttm-form item.
 * Unfortunately, if we use tt-form, it will give us a <label>, which is no good
 * for a title bar.
 * We take the opportunity to add a custom wrapper, class = 'ttm-tab-wrapper'.
 * We also add a class 'ttm-tab-title'. Kind of handy for css.
 * Takes #title, #help-topic
 *
 * @param <type> $element
 * @return <type>
 */

function theme_ttm_tab($element) {
  $output  = '<div class="ttm-tab-wrapper"';
  $output .= ">\n";

  $output .= '  <div class="ttm-tab">';
  $title = (!empty($element['#title'])) ? $element['#title'] : 'No title given!';
  $output .= '<div class="ttm-tab-title">';
  // We want this first in the div, for floating.
  $element['#help_topic'] = !empty($element['#help_topic']) ? $element['#help_topic'] : '';
  $output .= ttm_help_link($element['#help_topic'], 'icon', 'small', 'right');
  $output .= filter_xss_admin($title) ."</div>\n";

  $output .= $element['#children'];
 
  $output .= "  </div>\n</div>\n";
  return $output;
}


function theme_ttm_tab_display($element) {
  $output  = '<div id="'. $element['#id'] .'-wrapper" class="ttm-display"';
  $output .= ">\n";
  $output .= '<div id = "' . $element['#id'] . '-display" class="ttm-display-display">' . $element['#display_value'].'</div>';
  $output .= $element['#children'];
  $output .= "</div>\n";
  return $output;
}



/**
 * Format a checkbox, with twists.
 * It doesn't use Drupal general formatting (hence raw). So it never calls
 * theme_form_element() or theme_item(). It just wraps in a div. This cuts down
 *  on theming and HTML, but you have to theme by yourself. Also, it won't take
 * #required, though it will do #title/#description.
 * It can take attributes, through ['#attribute'].
 * It can take children. These are placed within the wrapper, and ABOVE the
 *   input tag. Non-intuitively, children are more useful there,
 *   especially if the span is floated right. Any children ARE drupal rendered
 *   but without the tree, so rendering will only go one item deep.
 * Like the Drupal checkbox, the input is wrapped INSIDE the label.
 *Properties used: #value, #default_value, #descriptin, #title, #attributes, #input_attributes.
 * @param $element
 *   An associative array containing the properties of the element.
 *   Properties used:  #title, #value, #default_value, #description, #attributes, #input_attributes
 * @return
 *   A themed HTML string representing the checkbox.
 *
 * @ingroup themeable
 */
function theme_ttm_checkbox_raw($element) {
  //dpm('tt theme checkbox raw:');
  //dpm($element['#id']);
  //dpm($element['#sub_type_class']);

  _form_set_class($element, array('ttm-checkbox-raw-input'));

  // The input itself
  $checkbox = '<input ';
  $checkbox .= 'type="checkbox" ';
  $checkbox .= 'name="'. $element['#name'] .'" ';
  $checkbox .= 'id="'. $element['#id'] .'" ' ;
  $checkbox .= 'value="'. $element['#default_value'] .'" ';

  $checkbox .= $element['#default_value'] ? ' checked="checked" ' : ' ';
  $checkbox .= drupal_attributes($element['#attributes']) .' />';

  // Wrap in a label, if there is a #title.
  if (!is_null($element['#title'])) {
    $checkbox = '<label class="option" for="'. $element['#id'] .'">'. $checkbox .' '. $element['#title'] .'</label>';
  }
  
  // Our stuff...
  // Prefix with children.
  $checkbox = !empty($element['#children']) ? $element['#children'] . $checkbox : $checkbox;


  // Back to the theme.
  //unset($element['#title']);
  //return theme('form_element', $element, $checkbox);//$checkbox;

  // Lacking  theme_form_element() call, we add this.
  if (!empty($element['#description'])) {
    $checkbox .= ' <div class="description">'. $element['#description'] ."</div>\n";
  }

  $wrap  = '<div id="'. $element['#id'] .'-wrapper" class="ttm-checkbox-raw"';
  //$wrap .= drupal_attributes($element['#attributes']);
  $wrap .= ">\n";
  $checkbox = $wrap . $checkbox;
  $checkbox .= "</div>\n";
  return $checkbox;
}


/**
 *  A little widget to toggle two values. Doesn't have a HTML input, for javascript.
 * The #default_value should match one of the two #titles.
 * Europe is dexterous, so 'left' is on, 'right' is off. For now.
 * Takes:
 * #titles (not #title! an array of two labels), #attributes, #default_value
 * @param <type> $element
 * @return <type>
 */
function theme_ttm_toggle($element) {
    // Attributes handled as a drupal button.
  if (isset($element['#attributes']['class'])) {
    $element['#attributes']['class'] = "ttm-toggle" .' '. $element['#attributes']['class'];
  }
  else {
    $element['#attributes']['class'] = "ttm-toggle";
  }

  if ($element['#default_value']) {
    $class_l = "ttm-toggle-on";
    $class_r = "ttm-toggle-off";
  }
  else {
    $class_l = "ttm-toggle-off";
    $class_r = "ttm-toggle-on";
  }

  if (isset($element['#titles'])) {
    $title_l = $element['#titles'][0];
    $title_r = $element['#titles'][1];
  }
  else {
    $title_l = 'ON';
    $title_r = 'OFF';
  }

  $html_l = '<a class="ttm-toggle-left ' . $class_l . '">'. $title_l .'</a> &nbsp';
  $html_r = '<a class="ttm-toggle-right ' . $class_r . '">'. $title_r .'</a>';

  return '<div '. drupal_attributes($element['#attributes']) . '>'. $html_l . $html_r ."</div>\n";
}


/** #?
 * Druapla checkbox with an advanced help button floated right.
 * Add the topic using the property #help_topic.
 * Will not render anything if the 'advanced_help' module is not on board.
 *
 * @param <type> $element
 * @return <type>
 */
/*
function theme_ttm_checkbox_help($element) {
  _form_set_class($element, array('form-checkbox'));

  $checkbox = '<input ';
  $checkbox .= 'type="checkbox" ';
  $checkbox .= 'name="'. $element['#name'] .'" ';
  $checkbox .= 'id="'. $element['#id'] .'" ' ;
  $checkbox .= 'value="'. $element['#return_value'] .'" ';
  $checkbox .= $element['#value'] ? ' checked="checked" ' : ' ';
  $checkbox .= drupal_attributes($element['#attributes']) .' />';

  if (!is_null($element['#title'])) {
    $checkbox = '<label class="option" for="'. $element['#id'] .'">'. $checkbox .' '. $element['#title'] .'</label>';
  }

  // We want this first in the div, for floating.
  if (module_exists('advanced_help')) {
    $checkbox = ttm_help_link($element['#help_topic'], 'icon', 'large', 'right') . $checkbox;
  }
  unset($element['#title']);
  return theme('form_element', $element, $checkbox);
}
*/

function taxonomy_treemenu_form_element($element, $value) {
  $t = get_t();

  $output = '<div class="form-item"';
  if (!empty($element['#id'])) {
    $output .= ' id="'. $element['#id'] .'-wrapper"';
  }
  $output .= ">\n";
  if (module_exists('advanced_help')) {
    //dpm('yea');
    $output .= ttm_help_link('node-sort', 'icon', 'large', 'right') . $checkbox;
  }
  $required = !empty($element['#required']) ? '<span class="form-required" title="'. $t('This field is required.') .'">*</span>' : '';

  if (!empty($element['#title'])) {
    $title = $element['#title'];
    if (!empty($element['#id'])) {
      $output .= ' <label for="'. $element['#id'] .'">'. $t('!title: !required', array('!title' => filter_xss_admin($title), '!required' => $required)) ."</label>\n";
    }
    else {
      $output .= ' <label>'. $t('!title: !required', array('!title' => filter_xss_admin($title), '!required' => $required)) ."</label>\n";
    }
  }

  $output .= " $value\n";

  if (!empty($element['#description'])) {
    $output .= ' <div class="description">'. $element['#description'] ."</div>\n";
  }

  $output .= "</div>\n";

  return $output;
}


/**
 *  A help link.
 * Needs advanced help (most of us do). Not an element, append it as content to something.
 *
 * Needs javascript for the class 'ttm-help-link'. Without css for the class
 * 'ttm-help-link' it looks like a link.
 * 'href' = TTM_URLROOT .'/ajax/help/'. $title
 *
 * @param <type> $topic
 * @param <type> $type
 * @param <type> $size
 * @param <type> $float
 * @return <type>
 */
// Mainly lifted from advanced_help.module. But bigger buttons.
function ttm_help_link($topic, $type='icon', $size = 'small', $float = '') {
  if (!module_exists('advanced_help')) {
    return '';
  }
//TODO: larger/smaller for text titles?
// Have to look in a different css file for that...
  $info = advanced_help_get_topic('taxonomy_treemenu', $topic);
  if (!$info) {
    return;
  }

  if($type == 'icon') {
      $text = '<span>' . t('Help') . '</span>';
      $class = 'ttm-help-link' . (($size == 'large') ? '-large' : '');
   }
   else {
      $text = $info['title'] ? $info['title'] : $topic;
      $class = 'advanced-help-title' . (($size == 'large') ? '-large' : '');
  }

  // Float?
  if ($float == 'right') {
      $class .= ' ttm-fright';
  }
  elseif ($float == 'left') {
      $class .= ' ttm-fleft';
  }
//dvm($text);
//dvm($type);
//dvm($topic);
  if (user_access('view advanced help popup')) {
    drupal_add_css(drupal_get_path('module', 'taxonomy_treemenu') . '/css/ttm-help-icon.css');
    $output = l($text, "help/taxonomy_treemenu/$topic", array(
      'attributes' => array(
        'class' => $class,
        'onclick' => "var w=window.open(this.href, 'advanced_help_window', 'width=". $info['popup width'] .", height=". $info['popup height'] .",scrollbars,resizable'); w.focus(); return false;",
        'title' => $info['title']
      ),
      'query' => array('popup' => TRUE),
      'html' => TRUE)
    );
  }
  else {
    $output = l($text, "help/taxonomy_treemenu/$topic", array(
      'attributes' => array(
        'class' => $class,
        'title' => $info['title']
      ),
      'html' => TRUE)
    );
  }
  return $output;
}


// Elements need explicit declaration of a theme.
// Documentation says they don't, but they bail out without one
// and an element gets no default theme
/**
 *
 * By the by, we've settled on a naming scheme.
 * The #id of an element goes on the primary HTML element, eg. the input in a checkbox.
 * If further id's are needed they go with tags such as '-wrapper'.
 * Most ttm elements get individual classes, not generic form_item theming, so
 * they can be picked out in multiple widgets.
 * Classes are named from the outside, with tags such as '-input' for identifying
 * inner widgets.
 *
 * Some documentation of ttm's form elements types:
 * ['ttm_div']
 * Takes #title, #required, #description. Could also take other elements such
 * as pref_values, but you'd have to be handling these yourself, with further
 * wraps, javascript, etc.
 * Outputs a form item, containing a div with an id of #id'-content',
 *   mimicing the wrapper of #id'-wrapper'.
 * Dammed useful for making up your own displays and widgets without using the
 *  visuals of a frameset. Doesn't assert $element['#tree'] = TRUE, but should
 *  work in circumstances where you wandt active content or not?
 *
 * ['ttm_tab']
 * Not an active tab at all, just a display box with a liner, title,
 * and body divs. Only the outer div has a unique id.
 * Any children appear in the 'body' div, genericly classed as "tt-edit-tab-body".
 * Not a proper form item, so should be used inside other items e.g. ['ttm_div'].
 * The extra HTML for the liner, and the simplicity, make it nice for styling.
 *
 * #tree => TRUE puts an item into the tree, so it gets a unique id and is
 * processed for output.
 * #input => TRUE is a bit over the top for us, as it kicks out processes,
 * names and values. Which we then ignore in favour of javascript and JSON.
 * But that's Drupal, so we may as well build it right, heh?
 * @return <type>
 */
function taxonomy_treemenu_elements() {
  $type = array();
  $type['branch_selector_by_gui'] = array('#input' => TRUE, '#process' => array('expand_branch_selector_by_gui') );
  $type['branch_selector_by_option'] = array('#input' => TRUE, '#process' => array('expand_branch_selector_by_option') );
  $type['ttm_div'] = array('#input' => TRUE);
  $type['ttm_widget_container'] = array('#input' => TRUE);
  $type['ttm_tab_container'] = array();
  $type['ttm_tab'] = array();
  $type['ttm_tab_display'] = array();
  $type['ttm_checkbox_raw'] = array('#input' => TRUE);
  $type['ttm_toggle'] = array('#input' => FALSE);
  // Form.inc _form_builder_handle_input_element(). Scene of the crime.
  // One of my few deep objections to Drupal is the impossible nature of building custom form elemnts.
  // It took me two hours to find why this override of checkboxes wouldn't work.
  // Return value needs to be 1 or zero, anything so it is not NULL.
  // And value callback needs to be defined (here, hijacking the checkbox version).
  //$type['ttm_checkbox_help'] = array('#input' => TRUE, '#return_value' => 1, '#value_callback' => 'form_type_checkbox_value');
  $type['depth'] = array('#input' => TRUE, '#process' => array('expand_depth_selector'));
  return $type;
}


function taxonomy_treemenu_form_branch_selector() {    
  $branch_selector_is_gui = variable_get('taxonomy_treemenu_selector_is_gui', FALSE);
  //dvm($branch_selector_is_gui);
  $selector['branch_selector'] = array(
    '#type' => 'fieldset',
    '#title' => t('Branch selector'),
    '#collapsible' => FALSE,
    '#description' => t("On submitting, the sub-tree of the chosen branch item (a taxonomy term or vocabulary, modified by the 'depth' setting) is turned into a custom menu. Please note that the chosen branch itself, the root of the menu, does not appear, as this would produce a lone base link, which is a waste of space and defies user expectation. So select the parent item of the terms you wish to show at the base of the visible menu."),
    '#required' => TRUE,
  );

  // Add the 'change selector type' button
  $sector_submit_label = 'toggle branch selector to ';
  $sector_submit_label .= ($branch_selector_is_gui) ? 'option list' : 'gui';
  $selector['branch_selector']['change_selector'] = array(
    '#type' => 'submit',
    '#value' => $sector_submit_label,
    '#validate' => array('taxonomy_treemenu_edit_menu_change_selector'),
  );
  //dvm(variable_get('taxonomy_treemenu_selector_is_gui', FALSE));

  // Get the selector itself.
  if ($branch_selector_is_gui) {
    $selector['branch_selector']['tree_root_display'] = array('#type' => 'branch_selector_by_gui', '#required' => TRUE);
  }
  else {
 	  $selector['branch_selector']['tree_root_data'] = array('#type' => 'branch_selector_by_option', '#required' => TRUE);
  }
 
  return $selector;
}


/**
 * Build form elements for the node view options.
 */
// TODO: some of this could be superceeded by a 'ttm_checkbox_row' element.
class TTMOptsFormBuild
{

  static function opts(&$f, $gid, $group) {

    $class = !empty($group['#type']) && ($group['#type'] == 'weighed') ? 'ttm-draggable-row draggable' : '';

    // Load in the options
    foreach($group as $oid => $opt) {
      if ($oid[0] == '#') continue;

      // First, a box to put the rows in.
      // Note that we place descriptions in here, not the checkbox.
      $f[$gid][$oid] = array(
       '#type'           => 'ttm_widget_container',
       '#description'    => !empty($opt['#description']) ? t($opt['#description']) : '',
       '#attributes'     => array('title' => $oid, 'class' => $class),
      );

        // Toggles in first...
        if ( (!empty($opt['#children']['ttm_toggle'])) && ($opt['#children']['ttm_toggle']) ) {
          $f[$gid][$oid]['tgl'] = array(
             '#type' => 'ttm_toggle',
             '#titles' =>$opt['#children']['ttm_toggle']['#titles'],
             '#default_value' =>$opt['#children']['ttm_toggle']['#value'],
             '#attributes' => array('title' => $oid),
            );
        }
        
        // and the main checkbox,
        $f[$gid][$oid]['cb'] = array(
          '#type' => 'ttm_checkbox_raw',
          '#title' => t($opt['#title']),
          '#default_value' => $opt['#value'],
          '#attributes' => array('title' => $oid),
        );

        // And any other children.
        if (count($opt['#children']) < 2) continue;
        // A div sub-container.
        $f[$gid][$oid]['sb'] = array(
          '#type' => 'ttm_div',
          '#no_id' => TRUE,
          '#attributes' => array('class' => 'ttm-opts-sub-container'),
        );

        foreach($opt['#children'] as $SOid => $SOpt) {
             $f[$gid][$oid]['sb'][$SOid]   = array (
                '#type' => $SOpt['#type'],
                '#title' => $SOpt['#title'],
                '#default_value' => $SOpt['#value'],
                '#attributes' => array('title' => $oid .'-'. $SOid),
             );
        }
    }
  }


  public static function get(&$f, $gid, $group) {
    // Put a new id and a box round whatever it is.
    $gid .= '-d';
    $f[$gid] = array(
       '#type'           => 'ttm_div',
       '#has_id'         => FALSE,
       '#attributes'     => array('title' => $group['#title'], 'class' => 'ttm-tab-content'),
    );

    self::opts($f, $gid, $group);

  }
  //EOC
}

function taxonomy_treemenu_help_append_icon($element) {
  //dpm($element['#id']);
  $data = array();
  $ht = $element['#help_topic'];
  // I suppose this should be a theme (but it isn't).
  // Maybe the theme should be passed into Drupal.themes, but that seems
  // ornate for something so fixed in form.
  $link = ttm_help_link($ht['topic'], $ht['style'], $ht['size'], $ht['align']);
  // drupal_add_js() can concatenate repeated calls (while regarding the namespace).
  $data['advanced_help_inject'][$element['#id']] = $link;
  drupal_add_js($data, 'setting');
  return $element;
}

function taxonomy_treemenu_form_disable_events_js($element) {
  //dpm($element);
  $data['ttm_disable_events'][$element['#id']] = $element['#disable'];
  drupal_add_js($data, 'setting');
  return $element;
}

/**
 *
 */
//TODO: Currently unused - selector should be converted to this, though.
function taxonomy_treemenu_advanced_options_ahah() {
  //dpm('well yes');
  $form = TTMAdvancedOptions::reprocess_form();
  $tal_form = $form['term_as_links_wrapper']['term_as_links'];
  unset($tal_form['#prefix'], $tal_form['#suffix']);
  //  Do something...
  $tal_form['#description'] = "wow";
  //dpm($tal_form);
  $output = drupal_render($tal_form);
  //dpm($output);
  // Final rendering callback.
  drupal_json(array('status' => TRUE, 'data' => $output));
}

/**
 * Handle advanced options such as TTM Paging. Includes a form builder and processor.
 *
 * The form offers options which are user-friendly,
 * but these have complex interactions with the data held in the database.
 */
class TTMAdvancedOptions
{
  static function reprocess_form() {
    //$form_state = array('storage' => NULL,  'rebuild' => TRUE);
    $form_state = array('storage' => NULL, 'submitted' =>FALSE);
    $form_build_id = $_POST['form_build_id'];
    $form = form_get_cache($form_build_id, $form_state);
    $args = $form['#parameters'];
    $form_id = array_shift($args);
    $form_state['post'] = $form['#post'] = $_POST;
    $form['#programmed'] = $form['#redirect'] = FALSE;
    drupal_process_form($form_id, $form, $form_state);
    return drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
  }
  
  // Set everything on, initially (also a listing of whats availible!).
  static private function defaults() {
    return $fState = array(
      //'ttm_paging' => array('disabled' => FALSE, 'value' => 0),
      //'hooks'      => array('disabled' => FALSE, 'value' => 0),
    );
  }


  /**
   *  We work the interactions out here.
   */
  static private function buildFormState($tm) {
    $fState = self::defaults();

    //ttm paging: Disable if other url options are on (unnecessary).
    // Otherwise, leave in the state of the prefix.
    if(!( $tm['menu_urls']) && !($tm['prefix_urls']) ) {
      $fState['multiple_breadcrumbs']['#disabled'] = TRUE;
      $fState['term_as_links']['#disabled'] = TRUE;
    }
    $fState['menu_urls']['#disable'] = array('edit-multiple-breadcrumbs'=> 'on','edit-term-as-links' => 'on');
    $fState['menu_urls']['#after_build'] = array('taxonomy_treemenu_form_disable_events_js');

    // hooks: Don't offer if any url options are enabled.
    //if($tm['prefix_url'] || $tm['menu_url'] || $tm['path_url']) {
     // $fState['hooks']['disabled'] = TRUE;
    //}
    // Disabled or not, hooks retains the db value.
    //$fState['hooks']['value'] = $tm['hooks'];
    //if($fState['hooks']['disabled']) {
//$fState['hooks']['message'] =t("This option is disabled, as some form of URL modification has been chosen.");
    //}
    //else {
//$fState['hooks']['message'] =t('Only offered when there is no URL modification. Will slow site links. Will not require duplicate aliases.', array('!menu_name' => $menu_name));
   // }

    
    return $fState;
  }


  static function process(&$tm, $db) {
    //dpm('form process');
    //dvm($tm['menu_urls']);
    // DHTML menu
    // If 'dhtml_pages' is on then we switch 'dhtml_blocks'
    // off, or the DHTML gets confused.
    $tm['dhtml_blocks'] = ($tm['dhtml_options'] == 'dhtml_blocks');
    $tm['dhtml_pages'] = ($tm['dhtml_options'] == 'dhtml_pages');


    // Menu Prefixes
    // If any of the prefixes are on, also switch on prefix_url
    if ($tm['menu_urls']) {
      $tm['prefix_urls'] = 1;
    }

    // TEMPORARY! If no advanced prefixes are on, switch prefix off.
     if (!$tm['menu_urls']) {
      $tm['prefix_urls'] = 0;
    }
    // Avoid some NULL's on new menus without modules existing?
    $menu['pathauto'] = isset($menu['pathauto']) * $menu['pathauto'];


   // ttm paging: if enabled, but other urls are not, use either hooks or a
   // prefix to trigger them.
    // if($tm['ttm_paging'] && ( !($tm['menu_url'] || $tm['path_url']))) {
     //  $tm['url_prefix'] = !($tm['hooks']);
    // }
     //hooks: left as they are. Unused if paging not on.

     
  }
  // Note: it is a pain in the butt to test for and restore values after a form
  // is submitted. They go NULL and all sorts of behaviour, which is one reason
  // why the Drupal Form API exists - to smooth all this over.
  // The internal opts are handled by our own code, go into a database blob,
  // and are all handled on one HTML input. So we're not worried about overloading
  // the form.
  // Therefore, we handle the form by throughputting everything. Unavailable
  // options are turned into hidden? Form API will simply skip unused
  // properties.
  // TODO: Turn unused options into hidden, and enable AHA?
  //static function get(&$f, $tm) {
  static function get(&$f, $tm) {
    //$f = array();
    $menu_name = !empty($tm['menu_name']) ? $tm['menu_name'] : ' the new menu ';
    // $AO = self::buildFState($tm);


    // $type = (module_exists('dhtml_menu')) ? 'radios': 'hidden';
    // While DHTML menu goes through it's development spasm?
    // (6.x-2.2 -> 6.x-3.5)
    $type = 'hidden';
    $v = 'none';
    if ($tm['dhtml_pages']) {
      $v = 'dhtml_pages';
    }
    if ($tm['dhtml_blocks']) {
      $v = 'dhtml_blocks';
    }
    $f['dhtml_options'] = array(
      '#type'           => $type,
      '#title'          => t('Use the DHTML effect for !menu_name', array('!menu_name' => $menu_name)),
      '#description'    => t('These options are exclusive - the DHTML gets confused if similar menus share the same page. The block setting can be changed in the block configuration pages, also.'),
      '#default_value'  => $v,
      '#options'        => array('dhtml_pages' => t('in pages'), 'dhtml_blocks' => t('in blocks'), 'none' => t('off')),
     );

/*
    $f['prefixl_url'] = array(
      '#type'           => 'checkbox',
      '#help_topic'     => 'custom-paging',
      '#title'          => t('Process terms and nodes for !menu_name through Taxonomy Treemenu', array('!menu_name' => $menu_name)),
      '#description'    => t('Enables a breadcrumb for paged menus, and term lists to reflect !menu_name settings. May require duplicate aliases if hooks are undesirable or unavailible.', array('!menu_name' => $menu_name)),
      //'#disabled'       => $AO['ttm_paging']['disabled'],
      '#default_value'  => 1, //$AO['ttm_paging']['value'],
      );
*/
 
     $f['prefix_urls'] = array(
      '#type'           => 'hidden',
      //'#type'           => 'checkbox',
      //'#help_topic'     => array('topic' => 'menu-url', 'style' => 'icon', 'size' => 'large', 'align' => 'right'),
      //'#after_build'    => array('taxonomy_treemenu_help_append_icon'),
      '#title'          => t('Use prefixes on urls'),
      '#default_value'  =>  !empty($tm['prefix_urls']) ? $tm['prefix_urls'] : '',
      );

    $f['page_urls_to_mlid'] = array(
      '#type'           => 'checkbox',
      //'#help_topic'     => '',
      '#title'          => t('Use mlids in page URLS'),
      '#description'    => t('Add the menu item id to page urls. Menu pages can not be aliased, but will be able to expand. No effect unless expand option also selected.'),
      //'#disabled'       => $AO['ttm_paging']['disabled'],
      '#default_value'  =>  $tm['page_urls_to_mlid'],
    );
      
     $f['menu_urls'] = array(
      '#type'           => 'hidden',
      '#help_topic'     => array('topic' => 'menu-url', 'style' => 'icon', 'size' => 'large', 'align' => 'right'),
      //'#after_build'    => array('taxonomy_treemenu_help_append_icon'),
      '#title'          => t('Prefix term and node link urls with the menu name'),
      '#description'    => t('Style a menu with two or three templates only, and other uses. May require duplicate aliases if hooks are undesirable or unavailable.', array('!menu_name' => $menu_name)),
      //'#disabled'       => $AO['ttm_paging']['disabled'],
      '#default_value'  =>  !empty($tm['menu_urls']) ? $tm['menu_urls'] : '',

      );


    $f['multiple_breadcrumbs'] = array(
      '#type'           => 'checkbox',
      '#help_topic'     => array('topic' => 'multiple-breadcrumbs', 'style' => 'icon', 'size' => 'large', 'align' => 'right'),
      '#after_build'    => array('taxonomy_treemenu_help_append_icon'),
      '#title'          => t('Enable multiple breadcrumbs.'),
      '#description'    => t('...if !menu_name becomes a multiple hierarchy. Only active if menu prefixes are enabled.', array('!menu_name' => $menu_name)),
      //'#disabled'       => $AO['ttm_paging']['disabled'],
      '#default_value'  =>  !empty($tm['multiple_breadcrumbs']) ? $tm['multiple_breadcrumbs'] : '',

      );

    $type = (module_exists('views')) ?  'hidden' : 'checkbox';


    $f['term_as_links'] = array(
      '#type'           => $type,
      '#disabled'      => FALSE,
      '#help_topic'     => array('topic' => 'term-theme', 'style' => 'icon', 'size' => 'large', 'align' => 'right'),
      '#after_build'    => array('taxonomy_treemenu_help_append_icon'),
      '#title'          => t('Use internal theme for term lists.'),
      '#description'    => t('The list becomes a page of links without Drupal term styling. Fast, compact, and templated via functions. Only works on extended URLs.'),
      //'#disabled'       => $AO['ttm_paging']['disabled'],
      '#default_value'  =>  !empty($tm['term_as_links']) ? $tm['term_as_links'] : '',
      );





     $f['use_menu_breadcrumb'] = array(
      '#type'           => 'hidden',
      //'#help_topic'     => 'treemenu-url-prefixes',
      //'#after_build'    => array('taxonomy_treemenu_help_append_icon'),
      '#title'          => t(''),
      '#description'    => t(''),
      '#default_value' =>  !empty($tm['use_menu_breadcrumb']) ? $tm['use_menu_breadcrumb'] : '',
      );

     $f['url_append_title'] = array(
      '#type'           => 'hidden',
      //'#help_topic'     => 'treemenu-url-prefixes',
      //'#after_build'    => array('taxonomy_treemenu_help_append_icon'),
      '#title'          => t(''),
      '#description'    => t(''),
      '#default_value' =>  !empty($tm['url_append_title']) ? $tm['url_append_title'] : '',
      );
      
      // Merge in logic for interconnecting form elements - #disable, etc.
      //dvm(self::buildFormState($tm));
     $f = array_merge_recursive($f, self::buildFormState($tm));
  }

//EOC
}


function taxonomy_treemenu_vocabulary_translate_status_message($vid) {
  global $language;
  //dpm($language);
  if (module_exists('i18ntaxonomy')) {
     switch (i18ntaxonomy_vocabulary($vid)) {
      case I18N_TAXONOMY_NONE:
        $trm = '<i>'. t('no translation options defined') . '</i><br><span class="description">'.
         t('Treemenus derived from this vocabulary will use the default language.') .'</span>';
      case I18N_TAXONOMY_LOCALIZE:
        $trm = '<i>'. t('localizable vocabulary') . '</i><br><span class="description">'.
         t('(translates using strings).
         Treemenus derived from this vocabulary will use the default language.') .'</span>';
        break;
      case I18N_TAXONOMY_LANGUAGE:
        $trm = '<i>'. t('fixed language') . '</i><br><span class="description">'.
         t('Treemenus derived from this vocabulary will use the fixed language.') .'</span>';
        break;
      case I18N_TAXONOMY_TRANSLATE:
        $trm = '<i>'. t('multilingual vocabulary') . '</i><br><span class="description">'.
         t('(terms can be defined in other languages, and presented as alternatives).
         Treemenus derived from this vocabulary can be set to the default language, 
         a fixed language, or be multilingual (use the switch block).') .'</span>';
        break;
      default:
        $trm = 'errm';
     }
  }
  else {
    $trm = '<i>'. t('%ulang', array('%ulang' => language_default('name'))) . '</i><span class="description"> ' .
    t('(User default language)') .'</span><br>';
  }
  return $trm;
}


//should really use double submit, as i'm not sure this would run validate functions, etc.
//pulled in the edit form, rather than altering it,
// allows us to put the function in a separate file
// but does it run proper validate funtions and so forth?
// and not really compatible with 'edit' function in hook_alter().
// Yeah, but it works and its callable from a seperate file.
// Can't be called with an invalid menu_name, as we set these ourselves in the hook_menu.
//but we use out hook_load anyway. Will protect future code.
function taxonomy_treemenu_edit_menu($form_state, $type, $menu_name = '') {
  //dpm($menu_name);
  $form = array();
  $treemenu = array();

  //TTMOptsDefaults::admin();
  //TTMOptsDefaults::stripped();
  //TTMOptsDefaults::display();
  //TTMOptsDefaults::display('yuch');

  $opts = new TTMOptsAdmin;
  //$opts->displayStatic();


  //$list = module_list();
  //dpm($list);

    if ( module_exists('views')) {
      $message = '<p>' . t('Views is installed. Taxonomy Treemenu can use default templates to render lists.') . '</p>';
    }
    else {
       $message = '<p>' . t('Taxonomy Treemenu can use internal functions to render lists. The templates are roughly similar to a Views raw output.') . '</p>';
    }
    if (!module_exists('advanced_help')) {
       $message = '<p>' . t('To get help for this form, please see the files in the folder "/taxonomy_treemenu/help". Or install the Drupal module "Advanced Help".') . '</p>';
       drupal_set_message($message);
    }

  /*
  switch ($type) {
    case 'add':
      break;
   case 'edit':
      break;
    case 'ahah_rebuild':
      dpm('ahah rebuild');
      dpm($form_state);
      $ttm = $form_state['ttm'];
      break;
  }
  // Selector wrapper
      $f['term_as_links_wrapper'] = array(
      '#tree' => FALSE,
      '#prefix' => '<div class="clear-block" id="term-as-links-wrapper">',
      '#suffix' => '</div>',
    );
   * // selector ahah
         '#ahah' => array(
          'path' => 'taxonomy_treemenu/ahah',
       // 'event' => 'change',
          'wrapper' => 'term-as-links-wrapper',
          'method' => 'replace',
          'effect' => 'fade',
          'progress' => array('type' => 'bar', 'message' => t('Please wait...')),
        ),
  */
  if ($type == 'add') {
    // Set general defaults for new menus.
    // Missing values are picked up in hook_submit()
    $treemenu = array(
      'vid'    => 0,
      'tid'    => 0,
      'depth'  => 0,
      'page_urls_to_mlid' => 0,
      'multiple_breadcrumbs' => 0,
      'terms_as_links' => 0,
      'prefix_urls'     => 0,
      'menu_urls'     => 0,
      'nodes'      => 0,
      'node_count'  => 0,
      'show_term_descendants' => 0,
      'hide_empty_links' => 0,
      'translation_mode' => 0,
      'dhtml_blocks' => 0,
      'dhtml_pages'  => 0,
      'pathauto'     => module_exists('pathauto'),
      'link_roots'   => 1,
      //structural opts arive ready defaulted, and are just passed through
      'title' => 'New menu', 
    );

    $form = taxonomy_treemenu_form_branch_selector();
    //will be done automatically by the require, I think, but lets assert it.
    $form['#insert'] = TRUE;
  }
  else {
    //get current data, amd descriptive display data.
    if (!$treemenu = taxonomy_treemenu_load($menu_name, TRUE)) {
      drupal_not_found();
      return;
    };

    // Load options, if there are any, else default.
    if (!empty($treemenu['options'])) {
       $opts->injectPrefs(unserialize($treemenu['options']));
       //$opts->displayStatic();
    }

    // Menu info display
    $form['data_display'] = array(
      '#tree' => FALSE,
      '#prefix' => '<div id="ttm-data-display"><div id="ttm-data-display-liner"><div id="ttm-data-display-liner-2">',
      '#suffix' => '</div></div></div>'
    );
    
    $root_data = TTMData::rootDataDisplay($menu_name);
 
     $form['data_display']['display1'] = array(
       '#value' => '<b>' . t('Grown from:') . '</b> ' . 'Vocabulary: <i>'. $root_data[0]['name'] . '</i> (vid = '. $treemenu['vid'] .')    ' . ', Term: <i>'. $root_data[1]['name'] .'</i> (tid = '. $treemenu['tid'] . ")<br/>",
     );

     $form['data_display']['display2'] = array(
       '#value'  =>  '<b>' . t('Machine name:') . '</b> <span class="description">(used for addressing in urls)</span>: ' . $menu_name . "<br/>",
     );

     $form['data_display']['url'] = array(
       '#value'  => '<b>' . t('View the menu on page:') . '</b> ' . TTMData::pageUrl($menu_name) . '<br/>',
     );

     $ms = TTMData::isMultipleStatusData($menu_name);
     $form['data_display']['display3'] = array(
       '#prefix' => '<b>' . t('Vocabulary Connection Status:') . '</b> <span class="description">(For the vocabulary this menu is grown from)</span>: ',
       '#value'  => 'Terms:<i>'. $ms['terms'] .'</i>    Nodes: <i>'. $ms['nodes'] .'</i>'. "<br/>",
     );
     
     $vs = TTMData::vocabularyStatusData($menu_name);
     $form['data_display']['display4'] = array(
       '#prefix' => '<b>' . t('Vocabulary Details:') . '</b> <span class="description">(For the vocabulary this menu is grown from)</span>: ',
       '#value'  => 'Tags:<i>'. $vs['tags'] .'</i>    Relations: <i>'. $vs['relations'] .'</i>'. "<br/>",
     );

    $e = taxonomy_treemenu_is_expandable_data($treemenu, TTMData::isMultiple($menu_name));
     $form['data_display']['expand'] = array(
       '#value' => '<b>' . t('Expandable:') . '</b> <span class="description">(with options current at time of form creation)</span> <i>' . $e['status'] .'</i><br/>',
     );
     if ($e['because']) {
       $form['data_display']['expand']['#suffix'] = '  <span class="description">because</span> ' . implode('/', $e['because']) . '<br/>';
     }

     $trvs = taxonomy_treemenu_vocabulary_translate_status_message($treemenu['vid']);
      $form['data_display']['display5'] = array(
       '#prefix' => '<b>' . t('Vocabulary Translation Status:') . '</b> <span class="description">(For the vocabulary this menu is grown from)</span>: ',
       '#value'  => $trvs,
     );
  }


  
  if (module_exists('pathauto')) {
      $form['pathauto'] = array(
        '#type'           => 'hidden',
        '#title'          => t('Create Pathauto aliases for terms and nodes'),
        '#description'    => t('The aliases are set to the patterns you define for nodes and terms in the Pathauto admin page, prefixed by /tt/.'),
        '#default_value'  => $treemenu['pathauto'],
      );
  }


  // One little call. Does so much.
  // (all the configuration dependant and URL sections of the form)
  $form['advanced_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Treemenu URL prefix options'),
    '#collapsible' => FALSE,
    //'#collapsed' => FALSE,
    '#description' => t('Complex interactions, but allow useful extra functions. If your site uses URL aliases, you should consider the side effects before using these options. See the Help files for details.'),
  );
  TTMAdvancedOptions::get($form['advanced_options'], $treemenu);


  $form['expand'] = array(
    '#type'           => 'checkbox',
    //'#help_topic'     => '',
    '#title'          => t('Expand'),
    '#description'    => t('Ask this menu to expand. Treemenu will revert to full menus if the context is unsuitable. NOTE: Will not work if DHTML Menu is installed.'),
    //'#disabled'       => $AO['ttm_paging']['disabled'],
    '#default_value'  => $treemenu['expand'],
  );


  $form['ttm_renderer_options'] = array(
    '#type' => 'fieldset',
    '#title' =>  t('Treemeunu renderer options'),
    '#collapsible' => FALSE,
    '#description' => t('Treemenu has a special renderer. Other menu rendering modules can use the menus, but not make use of the following options.'),
  );

  $form['ttm_renderer_options']['nodes'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show nodes'),
    '#default_value' => $treemenu['nodes'],
    '#description' => t('Treemenu can extend the treebuilding to include nodes. This is a little slower than core, but useful.'),
  );

  $form['ttm_renderer_options']['node_count'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show node counts'),
    '#default_value' => $treemenu['node_count'],
    '#description' => t('Add node counts to terms in the menu.'),
  );
  
    $form['ttm_renderer_options']['show_term_descendants'] = array(
      '#type'           => 'checkbox',
      //'#help_topic'     => 'treemenu-url-prefixes',
      //'#after_build'    => array('taxonomy_treemenu_help_append_icon'),
      '#title'          => t('Term pages include the nodes from descendant terms', array('!menu_name' => $menu_name)),
      '#description'    => t('Useful for menus where you need term pages to reflect the action of drilling through a menu. Works for translations, also. NOTE: If enabled, node counts will become a full descendant count. However, this setting will not affect "show nodes", which creates links for child nodes only.'),
      '#default_value' => $treemenu['show_term_descendants'],
      );

    $form['ttm_renderer_options']['hide_empty_links'] = array(
      '#type'           => 'checkbox',
      //'#help_topic'     => 'treemenu-url-prefixes',
      //'#after_build'    => array('taxonomy_treemenu_help_append_icon'),
      '#title'          => t('Hide term links with no nodes attached'),
      '#description'    => t('Remove links with no nodes attached. Somewhat intelligent, this option "erodes" or "prunes" the tree recursively from empty leaves, down towards the root.'),
      '#default_value' => $treemenu['hide_empty_links'],
      );

 $form['ttm_renderer_options']['link_roots'] = array(
    '#title' => t('Link treemenus'),
    '#type' => 'hidden',
    '#default_value' => $treemenu['link_roots'],
    '#description' => t('If a link in this menu points to another treemenu root, then the link will page the treemenu directly (not page a list of terms or nodes). Only works if the menu is shown in a page.'),
  );



  // Opts selector builders. Much custom code.
  // Note we strike the tree out - this will be javascripted.
  // TODO: Don't add this section if not javascript?
  //if (!isJsEnabled()) {
  //}
  //else {
    $form['ttm_renderer_options']['ttm_opts'] = array(
      '#type' => 'ttm_tab_container',
      '#title' => t('Options for nodes'),
      '#tree' => FALSE,
    );

      $form['ttm_renderer_options']['ttm_opts']['sort'] = array(
         '#type'           => 'ttm_tab',
        //  '#title'          => t('sort nodes by:' . theme( 'taxonomy_treemenu','sort-order', 'icon')),
         '#title'          => t('sort nodes by'),
         '#help_topic'     => 'sortable-opts',
      );
      TTMOptsFormBuild::get($form['ttm_renderer_options']['ttm_opts']['sort'], 'sort', $opts->get('sort'));
      taxonomy_treemenu_enable_swapboxdrag("edit-sort-d", FALSE, FALSE);

  //dpm($form['ttm_options2']['sort']);
      $form['ttm_renderer_options']['ttm_opts']['filter'] = array(
         '#type'           => 'ttm_tab',
         '#title'          => t('filter nodes by'),
      );
      TTMOptsFormBuild::get($form['ttm_renderer_options']['ttm_opts']['filter'], 'filter', $opts->get('filter'));

    /*
      $form['ttm_opts']['fields'] = array(
         '#type'           => 'ttm_tab',
         '#title'          => t('place extra info in the link'),
         '#help_topic'     => 'sortable-opts',
      );
      TTMOptsFormBuild::get($form['ttm_opts']['fields'], 'fields', $opts->get('fields'));
      taxonomy_treemenu_enable_swapboxdrag("edit-fields-d", FALSE, FALSE);
  */
    drupal_add_js(drupal_get_path('module', 'taxonomy_treemenu') .'/js/json_minified.js');
    drupal_add_js(drupal_get_path('module', 'taxonomy_treemenu') .'/js/admin_form_handler.js');
    drupal_add_js(array('ttm' => $opts->strippedAll()), "setting");
  //}
  
  //For V5?, the hidden options return.
  // As usual, it exists, if used or not.
  $form['ops'] = array(
    '#type' => 'hidden',
    '#default_value' => json_encode($opts->strippedAll()),
  );



/*===============================
   Translations
 ================================*/
  $l = language_default();
  if (module_exists('i18ntaxonomy') && taxonomy_treemenu_is_multilingual($treemenu['vid'])) {
    $form['language'] = array(
      '#title' => t('Language (if fixed)'),
      '#type' => 'select',
      '#help_topic'     => array('topic' => 'iph-translation', 'style' => 'icon', 'size' => 'large', 'align' => 'right'),
      '#after_build'    => array('taxonomy_treemenu_help_append_icon'),
      '#default_value' => $treemenu['language'] ? $treemenu['language'] : $l->langauge,
      '#options' => taxonomy_treemenu_languages_options(),
      '#description' => t('If the translation mode fixes the menu, this language will be used (overiding other Drupal settings).'),
     );
        $tropts    = array(
          t('No translation. The menu will use the default language.'),
          t('Translate the menu to a language chosen above (Not switchable/multilingual)'),
        );
        //if (i18ntaxonomy_vocabulary($treemenu['vid']) == I18N_TAXONOMY_TRANSLATE) {
         $tropts[] = t('By translated term or node (switchable/multilingual)');
        //}
    $form['translation_mode'] = array(
      '#type' => 'radios',
      '#title' => t('Translation mode'),
      '#default_value' =>  $treemenu['translation_mode'],
      '#options' => $tropts,
      '#description' => t('The modules "String translation" and "Taxonomy translation" are installed. Choose how to translate this treemenu here.'),
     );
  }
  else {
    $form['language'] = array(
      '#type' => 'hidden',
      //eitehr treemenu language, or default langauage.
      '#default_value' => $treemenu['language'] ? $treemenu['language'] : $l->language,
     );
    $form['translation_mode'] = array(
      '#type' => 'hidden',
      '#default_value' => 0,
     );
  }



  $form['depth'] = array(
    '#type' => 'depth',
    '#title' => t('Depth'),
    '#description' => t('...of menu. The tree root will never show, just descendant terms (and maybe nodes).'),
    '#default_value' => $treemenu['depth'], 
  );
 
  //pull in menu_edit_menu_submit()
  require_once(drupal_get_path('module', 'menu') .'/menu.admin.inc');
  $form = array_merge($form, menu_edit_menu($form_state, $type, $treemenu));

  /* Yes we do require these, but that requirement will validate
  before the branch selector, tripping errors, and annoying the user.
  So we'll validate these fields in the function, not automatically.
  */
  $form['title']['#required'] = FALSE;
  $form['menu_name']['#required'] = FALSE;
  
  /* ...but some warning. */
  $form['title']['#title'] .= "<span class=\"form-required\">*</span>";
  $form['menu_name']['#title'] .= "<span class=\"form-required\">*</span>";
  //dpm($treemenu);
  //dpm($form);
  drupal_add_js(drupal_get_path('module', 'taxonomy_treemenu') . '/js/form_disable_events.js', 'module');
  drupal_add_js(drupal_get_path('module', 'taxonomy_treemenu') . '/js/advanced_help_form_element_inject.js', 'module');
  drupal_add_css(drupal_get_path('module', 'taxonomy_treemenu') . '/css/taxonomy-treemenu-admin.css');


  return $form;  
}

function taxonomy_treemenu_edit_menu_change_selector($form, &$form_state) {
    $form_state['rebuild'] = TRUE;
    //dpm($form_state['values']);
    variable_set('taxonomy_treemenu_selector_is_gui', !(variable_get('taxonomy_treemenu_selector_is_gui', FALSE)));
}

/**
 * Validates the human and machine-readable names when adding or editing a menu.
 */
 //aw, stuff it. A near copy of menu.admin.incs validation.
 //some of this may be depreciated now?
function taxonomy_treemenu_edit_menu_validate($form, &$form_state) {
  $item = $form_state['values'];
  //check there are values at all
  //in title/menuname/hidden variables
  //dpm($item);
  if (strlen(trim($item['menu_name'])) == 0) {
    form_set_error('menu_name', t('The menu name is required.'));
  }
  if (strlen(trim($item['title'])) == 0) {
    form_set_error('title', t('The title name is required.'));
  }
  if ($item['tree_root_display']['tree_root_data'] == '-:-') {
    form_set_error('tree_root_display', t('A branch selection is required.'));
  }
  if ($item['tree_root_data'] == '-:-') {
    form_set_error('tree_root_data', t('A branch selection is required.'));
  } 
  if (preg_match('/[^a-z0-9-]/', $item['menu_name'])) {
    form_set_error('menu_name', t('The menu name may only consist of lowercase letters, numbers, and hyphens.'));
  }
  if (strlen($item['menu_name']) > MENU_MAX_MENU_NAME_LENGTH_UI) {
    form_set_error('menu_name', format_plural(MENU_MAX_MENU_NAME_LENGTH_UI, "The menu name can't be longer than 1 character.", "The menu name can't be longer than @count characters."));
  }
  if ($form['#insert']) {
    // We will add 'menu-' to the menu name to help avoid name-space conflicts.
    $item['menu_name'] = 'menu-'. $item['menu_name'];
    if (db_result(db_query("SELECT menu_name FROM {menu_custom} WHERE menu_name = '%s'", $item['menu_name'])) ||
      db_result(db_query_range("SELECT menu_name FROM {menu_links} WHERE menu_name = '%s'", $item['menu_name'], 0, 1))) {
      form_set_error('menu_name', t('The menu already exists.'));
    }
  }
}


/**
 * Special submit handler calls menu_edit_menu_submit() first,
 * thus creating initial framework,
 * then calls this, which adds submenu data and submenu items.
 */
 // I tried. I tried using hook_form_alter, but its too lightweight. Either it fails to address stuff in other files,
 //loosing callbacks and form elements, or you stick your entire admin in the main file, and have to jiggle like crazy.
 //I tried redirection (drupal_goto) from hook_alter, but once the form handlers have the form, they're off, 
 //and you can't cancel the request, not fully - and you've built a form only to throw it away.
 //I considered a seperate structure, but that would be awful for the user - 'use this form, not this one'. 
  //The current idea is to hijack the menu-edit button by using specific paths, which are built in hook_menu. 
  //This is not elegant, but it effective, and Drupalish.
  //The add 'Add' version gets it's own link, to appear in the menu pages.
  //The end result callbacks here, where we require the menu edit forms.
  //Anyone got any tidier ideas? (We could use file attributes to pull in, and drupal submits, I suppose). 
function taxonomy_treemenu_edit_menu_submit($form, &$form_state) {
  //dpm('ttm edit submit:');

  //pull in menu_edit_menu_submit()
  require_once(drupal_get_path('module', 'menu') .'/menu.admin.inc');
  menu_edit_menu_submit($form, $form_state);
  
  // Recover the form data and, for new menus, set the title.
  $menu = $form_state['values'];

  if ($form['#insert']) {
    $menu_title = $menu['menu_name'];
    $menu['menu_name'] = 'menu-'. $menu['menu_name'];
  }

  // Retrieve current data, if any.
  $crnt = taxonomy_treemenu_load($menu['menu_name'], TRUE);

  // Failsafe for settings data load.
  if (!$form['#insert'] && empty($crnt)) {
    //TODO: With message...
    return FALSE;
  }

  // Is a rebuild necessary on existing menus?
  // Change to or from language mode 2?
  $rebuild = ((($crnt['language_mode'] == 2) || ($menu['language_mode'] == 2)) && (($crnt['language_mode'] != $menu['language_mode'])));
  // This covers new menus also, as $crnt = FALSE
  $rebuild = ($rebuild || ($crnt['depth'] != $menu['depth']));


  // Some option interlocking work, then an array of data.
  TTMAdvancedOptions::process($menu, $crnt);


  //V5
  // Process the opts.
  $opts = json_decode($menu['ops'], TRUE);
  TTMOptsAdmin::formProcess($opts);
  $menu['options'] = serialize($opts);

  // Test without damage.
  //dpm($menu);
  //dpm($form_state);
  //dpm($opts);
  //return;



  if ($form['#insert']) {

    // Retrieve tree root data
    $tree_root_data = variable_get('taxonomy_treemenu_selector_is_gui', FALSE) ? $menu['tree_root_display']['tree_root_data'] : $menu['tree_root_data'] ;
    $root_ids = explode(':', $tree_root_data);

    $menu['vid'] = $root_ids[0];
    $menu['tid'] = $root_ids[1];

    //ttm_paging, prefix pathauto
    $placeholders = "'%s',". implode(',', array_fill(0, 19, '%d')) .",'%s','%s'";
    //$placeholders = "'%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s'";
    db_query("INSERT INTO {taxonomy_treemenu} (
      menu_name,
      depth, vid, tid, page_urls_to_mlid, expand,
      nodes, node_count, translation_mode, dhtml_blocks, dhtml_pages,
      prefix_urls, menu_urls, multiple_breadcrumbs, 
      term_as_links,
      show_term_descendants, hide_empty_links, use_menu_breadcrumb, url_append_title,
      link_roots, language,
      options) VALUES (" . $placeholders .")",
      $menu['menu_name'],
        $menu['depth'],
        $menu['vid'],
        $menu['tid'],
        $menu['page_urls_to_mlid'],
        $menu['expand'],
      $menu['nodes'],
      $menu['node_count'],
      $menu['translation_mode'],
      $menu['dhtml_blocks'],
      $menu['dhtml_pages'],
        $menu['menu_urls'],
        $menu['prefix_urls'],
        $menu['multiple_breadcrumbs'],
        $menu['term_as_links'],
           $menu['show_term_descendants'],
           $menu['hide_empty_links'],
           $menu['use_menu_breadcrumb'],
           $menu['url_append_title'],
     $menu['link_roots'], $menu['language'], // Language moved to end so can still autogenerate %d
        $menu['options']);

    //...build the new tree of links
    //taxonomy_treemenu_linktree_all_createupdate2($menu['menu_name']);
  }
  else {
    // Out with the old, in with the new.
    // Should inherit treeroots.
    $menu = array_merge($crnt, $menu);
  
    
    // Sadly, lots of junk in $menu, can't do this without detail.
    db_query("UPDATE {taxonomy_treemenu} SET
    depth = %d, vid = %d, tid = %d, page_urls_to_mlid = %d, expand= %d,
    nodes = %d, node_count = %d, translation_mode = %d, language = '%s',
    dhtml_blocks = %d, dhtml_pages = %d,
    prefix_urls = %d, menu_urls = %d, multiple_breadcrumbs = %d, 
    term_as_links = %d,
    show_term_descendants = %d, hide_empty_links = %d, use_menu_breadcrumb = %d, url_append_title = %d,
    link_roots = %d,
    options = '%s', options_changed = %d
    WHERE menu_name ='%s'",
      $menu['depth'],
      $menu['vid'],
      $menu['tid'],
      $menu['page_urls_to_mlid'],
      $menu['expand'],
    $menu['nodes'],
    $menu['node_count'],
    $menu['translation_mode'],
    $menu['language'],
        $menu['dhtml_blocks'],
        $menu['dhtml_pages'],
      $menu['menu_urls'],
      $menu['prefix_urls'],
      $menu['multiple_breadcrumbs'],
      $menu['term_as_links'],
           $menu['show_term_descendants'],
           $menu['hide_empty_links'],
           $menu['use_menu_breadcrumb'],
           $menu['url_append_title'],
    $menu['link_roots'],
    $menu['options'],
      $menu['options_changed'],
      $menu['menu_name']
    );
     //dpm($menu);
     // TODO: And what if depth has changed, or other direct tree link items?
  }

  if ($rebuild) {
    //dpm('rebuild');
    taxonomy_treemenu_linktree_all_createupdate($menu['menu_name']);
    // The function above empties cache, but new menus need their admin links
    // to appear too.
    menu_rebuild();
  }
  else {
    //dpm('no rebuild');
    taxonomy_treemenu_term_links_modify($menu);
    menu_cache_clear($menu['menu_name']);
    _menu_clear_page_cache();
  }

  
  if ($form['#insert']) {
    $t_args = array('%menu_title' => $menu_title,
                '!url' => TTMData::pageUrl($menu['menu_name']),
                '%block_name' => ( $menu_title . '-treemenu'));
    drupal_set_message(t('If successfully created, the menu %menu_title can be viewed on a page at !url. Or use the menu in a block by promoting the block %block_name', $t_args));
  }
}
