<?php
// $Id: views-view-grid.tpl.php,v 1.3.4.1 2010/03/12 01:05:46 merlinofchaos Exp $
/**
 * @file views-view-grid.tpl.php
 * Default simple view template to display a rows in a grid.
 *
 * - $rows contains a nested array of rows. Each row contains an array of
 *   columns.
 *
 * @ingroup views_templates
 */
?>
<?php if (!empty($title)) : ?>
  <h3><?php print $title; ?></h3>
<?php endif; ?>
<div class="views-view-grid">
  <div class="views-view-grid-inner">
    <?php foreach ($rows as $row_number => $columns): ?>
      <?php
        $row_class = 'row-' . ($row_number + 1);
        if ($row_number == 0) {
          $row_class .= ' row-first';
        }
        if (count($rows) == ($row_number + 1)) {
          $row_class .= ' row-last';
        }
      ?>
      <div class="row <?php print $row_class; ?> clear-block">
        <? $ncols = count($columns); ?>
        <?php foreach ($columns as $column_number => $item): ?>
          <div class="grid-4 <?php print 'col-'. ($column_number + 1); ?><?php print ($column_number == 0? ' alpha':''); ?><?php print ($column_number+1 == $ncols ? ' omega':''); ?>">
            <?php print $item; ?>
          </div>
        <?php endforeach; ?>
      </div>
    <?php endforeach; ?>
  </div>
</div>