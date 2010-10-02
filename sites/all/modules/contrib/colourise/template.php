<?php
// $Id: template.php,v 1.1.1.7 2009/02/05 10:34:25 gibbozer Exp $

/**
 * Force refresh of theme registry.
 * DEVELOPMENT USE ONLY - COMMENT OUT FOR PRODUCTION
 */
// drupal_rebuild_theme_registry();

/**
 * Initialize theme settings
 */
if (is_null(theme_get_setting('colourise_page_class'))) {
  global $theme_key;
  // Save default theme settings
  $defaults = array(
    'colourise_page_class'     => 'wide',
    'colourise_iepngfix'       => 0,
    'colourise_custom'         => 0,
    'colourise_breadcrumb'     => 0,
    'colourise_totop'          => 0,
  );

  variable_set(
    str_replace('/', '_', 'theme_'. $theme_key .'_settings'),
    array_merge(theme_get_settings($theme_key), $defaults)
  );
  // Force refresh of Drupal internals
  theme_get_setting('', TRUE);
}

function phptemplate_preprocess(&$vars, $hook) {
  global $theme;

  // Set Page Class
  $vars['page_class'] = theme_get_setting('colourise_page_class');

  // Hide breadcrumb on all pages
  if (theme_get_setting('colourise_breadcrumb') == 0) {
    $vars['breadcrumb'] = '';  
  }

  $vars['closure'] .= '
  <p id="theme-credit">'. t('Original Design is ') . '<a href="http://www.styleshout.com/templates/preview/Colourise1-0/index.html">StyleShout\'s  Colourise</a>. | ' . t('Ported to Drupal by ') . '<a href="http://webzer.net/">Webzer.net</a>.</p>
  ';

  // Theme primary and secondary links.
  $vars['primary_menu'] = theme('links', $vars['primary_links'], array('class' => 'links primary-menu'));
  $vars['secondary_menu'] = theme('links', $vars['secondary_links'], array('class' => 'links secondary-menu'));

  // Set Accessibility nav bar
  if ($vars['primary_menu'] != '') {
  $vars['nav_access'] = '
    <ul id="nav-access" class="hidden">
      <li><a href="#primary-menu" accesskey="N" title="'.t('Skip to Primary Menu').'">'. t('Skip to Primary Menu') .'</a></li>
      <li><a href="#main-content" accesskey="M" title="'.t('Skip to Main Content').'">'.t('Skip to Main Content').'</a></li>
    </ul>
  ';
  }
  else {
  $vars['nav_access'] = '
    <ul id="nav-access" class="hidden">
      <li><a href="#main-content" accesskey="M" title="'.t('Skip to Main Content').'">'.t('Skip to Main Content').'</a></li>
    </ul>
  ';
  }

  // Set Back to Top link toggle
  $vars['to_top'] = theme_get_setting('colourise_totop');
  if (theme_get_setting('colourise_totop') == 0) {
    $vars['to_top'] = '';
  }
  else {
    $vars['to_top'] = '<p id="to-top"><a href="#page">'. t('Back To Top') .'</a></p>';
  }

}

/**
 *  Create some custom classes for comments
 */
function comment_classes($comment) {
  $node = node_load($comment->nid);
  global $user;
 
  $output .= ($comment->new) ? ' comment-new' : ''; 
  $output .=  ' '. $status .' '; 
  if ($node->name == $comment->name) {	
    $output .= 'node-author';
  }
  if ($user->name == $comment->name) {	
    $output .=  ' mine';
  }
  return $output;
}

/**
 *  Set Custom Stylesheet
 */
if (theme_get_setting('colourise_custom')) {
  drupal_add_css(drupal_get_path('theme', 'colourise') .'/css/'.'custom.css', 'theme');
}

/**
 *  Add IE PNG Transparent fix
 */
if (theme_get_setting('colourise_iepngfix')) {
  drupal_add_js(drupal_get_path('theme', 'colourise') .'/js/jquery.pngFix.js', 'theme');
}