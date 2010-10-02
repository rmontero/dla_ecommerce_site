<?php
// $Id: template.php,v 1.1.2.2 2008/03/08 14:59:54 jswaby Exp $

/**
 * Change the format for the "Submitted by username on date/time" text for each node,
 * to "Posted by username on date/time".
 */
function pluralism_node_submitted($node) {
  return t('Posted by !username on @datetime',
    array(
      '!username' => theme('username', $node),
      '@datetime' => format_date($node->created),
    ));
}

/**
 * Change the format for the "Submitted by username on date/time" text for each comment,
 * to "Posted by username on date/time".
 */
function pluralism_comment_submitted($comment) {
  return t('Posted by !username on @datetime.',
    array(
      '!username' => theme('username', $comment),
      '@datetime' => format_date($comment->timestamp)
    ));
}

/**
 * Change the icon used for the RSS feed.
 */
function pluralism_feed_icon($url, $title) {
	//print base_path().path_to_theme().'/images/rss_icon.jpg';
  if ($image = theme('image', path_to_theme().'/images/rss_icon.jpg', t('Syndicate content'), $title)) {
    return '<a href="'. check_url($url) .'" class="feed-icon">'. $image .'</a>';
  }
}

/**
 * Change how the links for comments are printed.
 */
function pluralism_links($links, $attributes = array('class' => 'links')) {
  if(isset($links['comment_comments'])){
    $pieces = explode(" ", $links['comment_comments']['title']);
    $links['comment_comments']['title'] = t('Comments')." (".$pieces[0].")";
  }

  if(isset($links['comment_new_comments'])){
    $pieces = explode(" ", $links['comment_new_comments']['title']);
    $links['comment_new_comments']['title'] = t('New comments')." (".$pieces[0].")";
  }
  
  $output = '';

  if (count($links) > 0) {
    $output = '<ul'. drupal_attributes($attributes) .'>';

    $num_links = count($links);
    $i = 1;

    foreach ($links as $key => $link) {
      $class = $key;

      // Add first, last and active classes to the list of links to help out themers.
      if ($i == 1) {
        $class .= ' first';
      }
      if ($i == $num_links) {
        $class .= ' last';
      }
      if (isset($link['href']) && $link['href'] == $_GET['q']) {
        $class .= ' active';
      }
      $output .= '<li class="'. $class .'">';

      if (isset($link['href'])) {
        // Pass in $link as $options, they share the same keys.
        $output .= l($link['title'], $link['href'], $link);
      }
      else if (!empty($link['title'])) {
        // Some links are actually not links, but we wrap these in <span> for adding title and class attributes
        if (empty($link['html'])) {
          $link['title'] = check_plain($link['title']);
        }
        $span_attributes = '';
        if (isset($link['attributes'])) {
          $span_attributes = drupal_attributes($link['attributes']);
        }
        $output .= '<span'. $span_attributes .'>'. $link['title'] .'</span>';
      }

      $i++;
      $output .= "</li>\n";
    }

    $output .= '</ul>';
  }

  return $output;
}

/**
 * Change the new and updated markers for content with images/icons.
 */
function pluralism_mark($type = MARK_NEW) {
  global $user;
  if ($user->uid) {
    if ($type == MARK_NEW) {
      return ' <span class="marker">'. theme('image', path_to_theme().'/images/new.png', t('New content'), t('New')) .'</span>';
    }
    else if ($type == MARK_UPDATED) {
      return ' <span class="marker">'. theme('image', path_to_theme().'/images/wand.png', t('Updated content'), t('Update')) .'</span>';
    }
  }
}

/**
 * Change how the more link is displayed on the page.
 *
 * @param unknown_type $url
 * @param unknown_type $title
 * @return a string of the rendered more link
 */
function pluralism_more_link($url, $title) {
  return '<div class="more-link">['. t('<a href="@link" title="@title">more</a>', array('@link' => check_url($url), '@title' => $title)) .']</div>';
}

/**
 * Change how the breadcrumbs are rendered
 *
 * @param array $breadcrumb 
 * @return rendered breadcrumb
 */
function pluralism_breadcrumb($breadcrumb) {
  if (!empty($breadcrumb)) {
    return '<div class="breadcrumb">'. implode(' &#8250; ', $breadcrumb) .'</div>';
  }
}