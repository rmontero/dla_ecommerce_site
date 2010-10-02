<?php
// $Id: page.tpl.php,v 1.1.2.4 2008/03/08 16:09:23 jswaby Exp $
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $language->language ?>" xml:lang="<?php print $language->language ?>">

<head>
  <title><?php print $head_title ?></title>
  <?php print $head ?>
  <?php print $styles ?>
  <!--[if IE]>
  <link type="text/css" rel="stylesheet" media="all" href="<?php print base_path().$directory; ?>/iefix.css" />
  <![endif]-->
  <!--[if gte IE 7]>
  <link type="text/css" rel="stylesheet" media="all" href="<?php print base_path().$directory; ?>/ie7fix.css" />
  <![endif]-->
  <?php print $scripts ?>
  <script type="text/javascript"><?php /* Needed to avoid Flash of Unstyle Content in IE */ ?> </script>
</head>

<body>

<div id="wrapper">
  <div id="wrapper2">
    <div id="header">
      <div id="logo">
        <?php if ($site_name) { ?><h1 class='site-name'><a href="<?php print $front_page ?>" title="<?php print t('Home') ?>"><?php print $site_name ?></a></h1><?php } ?>
        <?php if ($site_slogan) { ?><div class='site-slogan'><?php print $site_slogan ?></div><?php } ?>
      </div>
      <div id="menu">
        <?php if (isset($primary_links)) { ?><?php print theme('links', $primary_links, array('class' => 'links', 'id' => 'navlist')) ?><?php } ?>
      </div>
    </div>
    <div id="container">
      <?php if ($header) { ?><div id="precontent"><?php print $header ?></div><?php } ?>
      <div id="main">
        <?php if ($mission) { ?><div id="mission"><?php print $mission ?></div><?php } ?>
        <?php if (!$is_front) print $breadcrumb ?>
        <?php if ($show_messages) { print $messages; } ?>
        <?php if ($tabs) { ?><div class="tabs"><?php print $tabs ?></div><?php } ?>
        <?php if ($title) { ?><h1 class="title"><?php print $title ?></h1><?php } ?>
        <?php print $help ?>
        <?php print $content; ?>
        <?php print $feed_icons; ?>
      </div>
      <div id="sidebar-right">
        <?php print $right ?>
      </div>
      <div style="clear: both;"> </div>
      <div id="postcontent"><?php print $footer ?><div style="clear: both;"> </div></div>
    </div>
  </div>
  <div id="footer">
    <?php print $footer_message ?>
    <div id="footer_copy">Copyright &copy; <?php print date("Y"); ?> <?php print $site_name ?>. <a href="http://drupal.org/project/pluralism">Pluralism</a> is designed by <a href="http://www.nodethirtythree.com/">NodeThirtyThree</a> + <a href="http://www.freecsstemplates.org/">Free CSS Templates</a>. Ported by <a href="http://drupal.org/user/39343">Jason Swaby</a>.</div>
  </div>
</div>

<?php print $closure ?>
</body>
</html>
