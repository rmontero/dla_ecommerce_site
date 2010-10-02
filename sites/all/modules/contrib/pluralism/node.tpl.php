<?php
// $Id: node.tpl.php,v 1.1.2.2 2008/03/08 03:23:53 jswaby Exp $
  drupal_add_js('misc/collapse.js');
?>
  <div class="node<?php if ($sticky) { print " sticky"; } ?><?php if (!$status) { print " node-unpublished"; } ?><?php if($page) { print " node-page-view"; } ?>">
    <?php if ($page == 0) { ?><h2 class="title"><a href="<?php print $node_url?>"><?php print $title?></a></h2><?php }; ?>
    <?php if (!((count($taxonomy) == 1) && ($node->type == 'forum'))) { if ($terms) { ?>
    <fieldset class="collapsible collapsed">
    <legend><a href="#">Categories</a></legend>
	<div class="fieldset-wrapper"><?php print $terms?></div>
    </fieldset>
    <?php }} ?>
    <?php if ($picture) { print $picture; }?>
    <div class="content"><?php print $content?></div>
    <div class="meta">
    <?php if ($submitted) { ?><div class="submitted"><?php print $submitted?></div><?php }; ?>
    <?php if ($links) { ?><div class="links"><?php print $links?></div><?php }; ?>
    <div style="clear: both;"> </div>
    </div>
  </div>
 