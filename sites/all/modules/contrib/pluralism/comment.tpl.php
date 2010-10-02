<?php
// $Id: comment.tpl.php,v 1.1.2.1 2008/03/08 01:28:28 jswaby Exp $
?>
  <div class="comment<?php print ' '. $status; ?>">
<h3 class="title"><?php print $title; ?></h3><?php if ($new != '') { ?><span class="new"><?php print pluralism_mark(MARK_NEW); ?></span><?php } ?>
    <?php if ($picture) {
    print $picture;
  } ?>
    <div class="content">
     <?php print $content; ?>
     <?php if ($signature): ?>
      <div class="clear-block">
       <div>—</div>
       <?php print $signature ?>
      </div>
     <?php endif; ?>
    </div>

    <div class="meta">
    <?php if ($submitted) { ?><div class="submitted"><?php print $submitted?></div><?php }; ?>
    <?php if ($links) { ?><div class="links"><?php print $links?></div><?php }; ?>
    <div style="clear: both;"> </div>
    </div>

  </div>
