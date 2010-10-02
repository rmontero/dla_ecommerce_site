<?php
// $Id: page.tpl.php,v 1.1.1.8 2009/02/05 18:09:25 gibbozer Exp $
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">

  <head>

    <title><?php print $head_title ?></title>
    <?php print $head ?>
    <?php print $styles ?>
    <?php print $scripts ?>

    <!--[if lte IE 6]>
      <link type="text/css" rel="stylesheet" media="all" href="<?php print $base_path . $directory ?>/css/ie6.css" />
    <![endif]-->

    <?php if (theme_get_setting('colourise_iepngfix')) : ?>
    <!--[if lte IE 6]>
      <script type="text/javascript"> 
        $(document).ready(function(){ 
          $(document).pngFix(); 
        }); 
      </script> 
    <![endif]-->
    <?php endif; ?>

  </head>

  <body class="<?php print $body_classes ?>">

    <div id="page" class="<?php print $page_class ?>">

      <?php print $nav_access ?>

      <div id="header" class="clear-block">

      <?php if ($site_name): ?>
        <h1 id="site-name">
          <a href="<?php print check_url($front_page) ?>" title="<?php print t('Home') ?>">
            <?php print $site_name ?>
          </a>
        </h1>
      <?php endif; ?>

      <?php if ($site_slogan): ?>
        <p id="slogan"><?php print $site_slogan ?></p>
      <?php endif; ?>

      <?php if (!empty($primary_menu)) : ?>
        <div id="primary-menu">
          <h3 class="hidden">Primary Menu</h3>
            <?php print $primary_menu; ?>
        </div>
      <?php endif; ?>

      <?php if ($search_box): ?>
        <?php print $search_box ?>
      <?php endif; ?>

      <?php if ($breadcrumb): ?>
        <?php print $breadcrumb; ?>
      <?php endif; ?>

      </div> <!-- /header -->

      <div id="content-wrapper" class="clear-block">

        <div id="main-content" class="column center">
        <div id="content-inner" class="inner">

        <?php if ($mission): ?>
          <div id="mission"><?php print $mission ?></div>
        <?php endif; ?>

        <?php if ($content_top): ?>
          <div id="top-content-block" class="content-block">
            <?php print $content_top ?>
          </div>
        <?php endif; ?>

        <?php if ($title): ?>
          <h1 class="title"><?php print $title ?></h1>
        <?php endif; ?>

        <?php if (!empty($messages)): ?><?php print $messages ?><?php endif; ?>

        <?php if (!empty($help)): ?><?php print $help ?><?php endif; ?>

        <?php if (!empty($tabs)): ?><?php print $tabs ?><?php endif; ?>

        <?php print $content ?>

        <?php if ($content_bottom): ?>
          <div id="bottom-content-block" class="content-block">
            <?php print $content_bottom ?>
          </div>
        <?php endif; ?>

        <?php if ($breadcrumb): ?>
          <?php print $breadcrumb; ?>
        <?php endif; ?>

        <?php print $feed_icons ?>

        </div> <!-- /content-inner -->
        </div> <!-- /main-content -->

        <?php if ($left): ?>
          <div id="sidebar-left" class="column sidebar">
          <div id="sidebar-left-inner" class="inner">
            <?php print $left ?>
          </div>
          </div> <!-- /sidebar-left -->
        <?php endif; ?>

        <?php if ($right): ?>
          <div id="sidebar-right" class="column sidebar">
          <div id="sidebar-right-inner" class="inner">
            <?php print $right ?>
          </div>
          </div> <!-- /sidebar-right -->
        <?php endif; ?>

      </div> <!-- /content-wrapper -->

      <?php if ($footer_1 || $footer_2 || $footer_3): ?>
        <div id="footer-column-wrap" class="clear-block">

          <?php if ($footer_1): ?>
            <div class="footer-column">
              <?php print $footer_1 ?>
            </div>
          <?php endif; ?>

          <?php if ($footer_2): ?>
            <div class="footer-column">
              <?php print $footer_2 ?>
            </div>
          <?php endif; ?>

          <?php if ($footer_3): ?>
            <div id="last" class="footer-column">
              <?php print $footer_3 ?>
            </div>
          <?php endif; ?>

        </div> <!-- /footer-column-wrap -->
      <?php endif; ?>

      <div id="footer" class="clear-block">

        <?php print $to_top ?>

        <?php if ($footer_message): ?>
          <p id="site-info"><?php print $footer_message ?></p>
        <?php endif; ?>

        <?php print $closure ?>
      </div> <!-- /footer -->

    </div> <!-- /page -->
  </body>
</html>