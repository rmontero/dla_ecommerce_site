<?php
// $Id: node.tpl.php,v 1.1.1.7 2009/02/04 19:23:30 gibbozer Exp $
?><div id="node-<?php print $node->nid; ?>" class="node<?php if ($sticky) { print ' sticky'; } ?><?php if (!$status) { print ' node-unpublished'; } ?> clear-block">

  <?php if (!$page): ?>
    <h2 class="title">
      <a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a>
    </h2>
  <?php endif; ?>

  <div class="post-info">

    <?php print $picture ?>

    <?php if ($submitted): ?>
      <span class="submitted"><?php print $submitted ?></span>
    <?php endif; ?>

    <?php if ($terms): ?>
      <div class="terms"> <?php print t('Related Terms') ?> : <?php print $terms ?></div>
    <?php endif; ?>

  </div>

  <div class="content">
    <?php print $content ?>
  </div>

  <?php if ($links): ?>
    <div class="node-links">
      <?php print $links; ?>
    </div>
  <?php endif; ?>

</div>