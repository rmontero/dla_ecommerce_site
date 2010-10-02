<?php
// $Id$
?>
<div id="node-<?php print $node->nid; ?>" class="node clear-block <?php print $node_classes; ?>">
  <div class="inner">
    <?php if (isset($pechi_uc_taxonomy) && !empty($pechi_uc_taxonomy)): ?>
      <h1 class="page-title title taxonomy-title"><?php print $pechi_uc_taxonomy[0]['title']; ?></h1>
      <?php if (!empty($pechi_uc_taxonomy[0]['description'])): ?>
      <div class="taxonomy-description"><?php print $pechi_uc_taxonomy[0]['description']; ?></div>
      <?php endif; ?>
    <?php elseif ($page == 0): ?>
      <h2 class="title"><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
    <?php endif; ?>

    <?php if ($submitted): ?>
    <div class="meta">
      <span class="submitted"><?php print $submitted ?></span>
    </div>
    <?php endif; ?>

    <div id="product-group" class="product-group grid-12 alpha omega">
      <?php if (!empty($pechi_uc_image)): ?>
      <div class="images grid-8 alpha">
        <?php print $pechi_uc_image; ?>
      </div>
      <?php else: ?>
      <div class="images noimage grid-8 alpha">
        <?php print t('Image coming soon'); ?>
      </div>
      <?php endif; ?>

      <div class="content grid-4 omega">
        <div id="content-body">
          <?php print $pechi_uc_body; ?>
        </div>

        <div id="product-details" class="clear">
          <div id="field-group">
            <h2 id="product-title">
            <?php print $pechi_uc_title; ?>
            </h2>
            <?php // print $pechi_uc_list_price; ?>
            <?php // print $pechi_uc_sell_price; ?>
            <?php // print $pechi_uc_cost; ?>
          </div>
          <div id="price-group">
            <?php print $pechi_uc_display_price; ?>
            <?php print $pechi_uc_add_to_cart; ?>
          </div>
        </div><!-- /product-details -->

        <?php if ($pechi_uc_additional && !$teaser): ?>
        <div id="product-additional" class="product-additional">
          <?php print $pechi_uc_model; ?>
          <?php print $pechi_uc_weight; ?>
          <?php print $pechi_uc_dimensions; ?>
          <?php print $pechi_uc_additional; ?>
        </div>
        <?php endif; ?>

        <?php if ($terms): ?>
          <div class="terms">
            <?php print $terms; ?>
          </div>
        <?php endif;?>

      </div><!-- /content -->

    <?php if ($links && !$teaser): ?>
      <div class="links clear">
        <?php print $links; ?>
      </div>
    <?php endif; ?>

    </div><!-- /product-group -->
  </div><!-- /inner -->

  <?php if ($node_bottom && !$teaser): ?>
  <div id="node-bottom" class="node-bottom row nested">
    <div id="node-bottom-inner" class="node-bottom-inner inner">
      <?php print $node_bottom; ?>
    </div><!-- /node-bottom-inner -->
  </div><!-- /node-bottom -->
  <?php endif; ?>
</div>