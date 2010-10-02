<?php
// $Id$
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">

  <head>
    <title><?php print $head_title; ?></title>
    <?php print $head; ?>
    <?php print $styles; ?>
    <?php print $scripts; ?>
    <?php print $suy; ?>
  </head>

  <body class="<?php print $body_classes; ?> <?php print ($show_grid ? 'show-grid' : ''); ?>">
    <div id="page" class="container-16 clear-block">

      <div id="site-header" class="grid-16 clear-block">
        <div id="branding" class="grid-6 alpha omega clear-block">
          <?php if ($linked_site_name): ?>
          <h1 id="site-name" class="grid-6 alpha omega"><?php print $linked_site_name; ?></h1>
          <?php endif; ?>
          <!--
          <?php if ($linked_logo_img): ?>
            <span id="logo" class="grid-1 omega"><?php print $linked_logo_img; ?></span>
          <?php endif; ?>
          -->
        </div>
        <div id="header-top" class="grid-10 omega">
            <?php print $header_top; ?>
        </div>

        <div id="header-region" class="region <?php print ns('grid-12', $linked_site_name, 6); ?>">
            <?php print $header; ?>
        </div>

        <?php if ($search_box): ?>
        <div id="search-box" class="grid-4"><?php print $search_box; ?></div>
        <?php endif; ?>


        <?php if ($primary_nav): ?>
        <div id="site-menu-wrapper" class="grid-16 clear-block">
          <div id="site-menu" >
            <?php print $primary_nav; ?>
          </div>
        </div>
        <?php endif; ?>

        <?php if ($secondary_links): ?>
        <div id="secondary-menu" class="secondary-nav region grid-16 clear-block alpha omega">
          <?php print $secondary_menu_links; ?>
        </div>
        <?php elseif ($site_slogan): ?>
          <div id="slogan" class="region grid-16 clear-block alpha omega">
            <div class="slogan">
              <?php print $site_slogan; ?>
            </div>
          </div>
        <?php endif; ?>

        <?php if ($mission): ?>
        <div id="site-subheader" class="clear-block">
          <div id="mission" class="<?php print ns('grid-16', $header, 7); ?>">
              <?php print $mission; ?>
          </div>
        </div>
        <?php endif; ?>

        <?php if ($preface_top): ?>
        <div id="preface_top" class="grid-16 clear-block">
            <?php print $preface_top; ?>
        </div>
        <?php endif; ?>
      </div>
       <?php if ($tabs): ?>
        <div class="tabs grid-12 push-4 clear-block"><?php print $tabs; ?></div>
      <?php endif; ?>
      <div id="main" class="column <?php print ns('grid-16', $left, 4, $right, 4) . ' ' . ns('push-4', !$left, 4); ?>">
        <div id="breadcrumb" class="clear-block">
          <?php print $breadcrumb; ?>
        </div>

        <?php if ($title): ?>
          <h1 class="title" id="page-title"><?php print $title; ?></h1>
        <?php endif; ?>
        <div id="main-content" class="region clear-block">
          <?php if ($messages): ?>
            <div id="messages" class="clear-block">
              <?php print $messages; ?>
              <?php print $help; ?>
            </div>
          <?php endif; ?>
          <?php print $content_top; ?>
          <?php print $content; ?>
          <?php print $content_bottom; ?>
        </div>
      </div>

      <?php if ($left): ?>
      <div id="sidebar-left" class="column sidebar region grid-4 <?php print ns('pull-12', $right, 4); ?>">
          <?php print $left; ?>
      </div>
      <?php endif; ?>

      <?php if ($right): ?>
      <div id="sidebar-right" class="column alpha sidebar region grid-4">
          <?php print $right; ?>
      </div>
      <?php endif; ?>
      <?php if ($postscript_bottom): ?>
      <div id="postscript-bottom" class="region grid-16 clear-block">
        <?php print $postscript_bottom; ?>
      </div>
      <?php endif; ?>
      <div id="footer" class="region grid-16 clear-block">
        <?php if ($footer_message || $primary_links): ?>
          <div id="footer-nav" class="grid-12 alpha omega prefix-2 suffix-2 clear-block">
            <?php print $main_menu_links; ?>
          </div>
          <div class="clear"></div>
          <div id="footer-message" class="grid-16 alpha omega clear-block">
              <?php print $footer_message; ?>
          </div>
          <div class="clear"></div> 
           <?php if ($footer || $feed_icons): ?>
          <div id="footer-region" class="alpha omega grid-16 clear-block">
              <?php print $footer; ?>
              <?php print $feed_icons; ?>
          </div>
          <?php endif; ?>
        <?php endif; ?>
        <div class="clear"></div>
      </div>

    </div>
    <?php print $closure; ?>
  </body>
</html>