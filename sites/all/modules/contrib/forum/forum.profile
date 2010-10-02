<?php

/**
 * Basic base profile that I use when starting a new Drupal project
 * This is a modified version of the default.profile which is well
 * commented - notes left here for your info - thanks Drupal!
 *
 * I copied this from the_base install profile.
 */

/**
 * Return an array of the modules to be enabled when this profile is installed.
 *
 * @return
 *   An array of modules to enable.
 */
function forum_profile_modules() {
  return array(
  // CORE OPTIONAL
  'comment', 'help', 'menu', 'dblog', 'path', 'php', 'update',
  
  // cck
  'content', 'fieldgroup', 'nodereference', 'number', 'optionwidgets', 'text', 'content_copy',
  
  // views
  'views', 'views_ui',
  
  );
}

/**
 * Return a description of the profile for the initial installation screen.
 *
 * @return
 *   An array with keys 'name' and 'description' describing this profile,
 *   and optional 'language' to override the language selection for
 *   language-specific profiles.
 */
function forum_profile_details() {
  return array(
    'name' => 'Forum',
    'description' => 'Basic forum installation profile.'
  );
}

/**
 * Return a list of tasks that this profile supports.
 *
 * @return
 *   A keyed array of tasks the profile will perform during
 *   the final stage. The keys of the array will be used internally,
 *   while the values will be displayed to the user in the installer
 *   task list.
 */
function forum_profile_task_list() {
/**
  return array(
    'some_task_id' => st('Description'),
  );
*/
}

/**
 * Perform any final installation tasks for this profile.
 *
 * For notes, see the default install profile
 */
function forum_profile_tasks(&$task, $url) {
  // Insert default user-defined node types into the database. For a complete
  // list of available node type attributes, refer to the node type API
  // documentation at: http://api.drupal.org/api/HEAD/function/hook_node_info.
  $types = array(
    array(
      'type' => 'page',
      'name' => st('Page'),
      'module' => 'node',
      'description' => st("A <em>page</em> is for creating and displaying information that rarely changes, such as an \"About us\" page of a website. A <em>page</em> does not allow visitor comments and is not featured on the site's initial home page."),
      'custom' => TRUE,
      'modified' => TRUE,
      'locked' => FALSE,
      'help' => '',
      'min_word_count' => '',
    ),
  );

  foreach ($types as $type) {
    $type = (object) _node_type_set_defaults($type);
    node_type_save($type);
  }  
  
  // Default page to not be promoted and have comments disabled.
  variable_set('node_options_page', array('status'));
  variable_set('comment_page', COMMENT_NODE_DISABLED);

  // Don't display date and author information for page nodes by default.
  $theme_settings = variable_get('theme_settings', array());
  $theme_settings['toggle_node_info_page'] = FALSE;
  variable_set('theme_settings', $theme_settings);

  // add custom content types from dump dir
  custom_content_types();
  
  // build views from dump dir
  views_setup();

  // pathauto
  variable_set('pathauto_node_applytofeeds', '');

  // Update the menu router information.
  menu_rebuild();
}

/**
 * Implementation of hook_form_alter().
 *
 * Allows the profile to alter the site-configuration form. This is
 * called through custom invocation, so $form_state is not populated.
 */
function forum_form_alter(&$form, $form_state, $form_id) {
  if ($form_id == 'install_configure') {
    // Set defaults
    $form['site_information']['site_name']['#default_value'] = $_SERVER['SERVER_NAME'];
  }
}

/** 
 * Set up additional content types
 * add each foo.cck.inc file found in the install profiles cck dir 
 * where foo.cck.inc is an export dump from content_copy
*/
function custom_content_types(){  
  foreach (glob(drupal_get_path('profile', 'forum') . '/cck/*.cck.inc') as $importfile) {  
    $content = array();

    ob_start();
    include $importfile;
    ob_end_clean();

    $form_state = array();
    $form = content_copy_import_form($form_state, $type_name);

    $form_state['values']['type_name'] = '<create>';
    $form_state['values']['macro'] = '$content = '. var_export($content, 1) .';';
    $form_state['values']['op'] = t('Import');

    content_copy_import_form_submit($form, $form_state);  
  }  
}

/**
 * Set up views
 * for each view, paste the body of:function foo_views_default_views() {
  * ....
  * ...
  * }
  * in a php file (including "<?php") in a text file and place in the views directory named foo.views.inc 
  *
  * remove the return $views; line, and also any $views['name'] = $view if you feel up to it.
 */
 function views_setup(){
  foreach (glob(drupal_get_path('profile', 'forum') . '/views/*.views.inc') as $importfile) {  
    include $importfile;
    $view->save();
  }
 }
 
