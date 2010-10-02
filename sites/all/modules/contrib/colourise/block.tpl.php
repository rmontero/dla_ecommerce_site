<?php
// $Id: block.tpl.php,v 1.1.1.7 2009/02/04 19:23:30 gibbozer Exp $
?>
<div id="block-<?php print $block->module .'-'. $block->delta; ?>" class="block block-<?php print $block->module ?>">
<?php if ($block->subject): ?>
  <h2 class="block-title"><?php print $block->subject ?></h2>
<?php endif;?>

  <div class="content">
    <?php print $block->content ?>
  </div>
</div>