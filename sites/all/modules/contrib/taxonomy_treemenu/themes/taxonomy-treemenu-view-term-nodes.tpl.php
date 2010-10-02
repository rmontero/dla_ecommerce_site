<?php
// $Id: taxonomy-treemenu-view-term-nodes.tpl.php,v 1.1 2009/06/11 16:18:03 rcrowther Exp $

/**
 * @file taxonomy_treemenu_view_term-nodes.tpl.php
 * Default template to display a list of rows.
 *
 * @ingroup taxonomy_treemenu_templates
 */
?>
<?php if (!empty($title)): ?>
  <h3><?php print $title; ?></h3>
<?php endif; ?>
<div class="taxonomy-treemenu-view-term-nodes">
      <?php print $body; ?>
</div>
