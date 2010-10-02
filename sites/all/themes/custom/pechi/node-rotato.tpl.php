<?php
// $Id$
var_dump($vars);
?>

<div id="node-<?php print $node->nid; ?>" class="node <?php print $node_classes; ?>">
  <div class="inner">
    <?php print $picture ?>
    <div class="content clearfix">
      <?php print $content ?>
    </div>
  </div><!-- /inner -->
</div><!-- /node-<?php print $node->nid; ?> -->
