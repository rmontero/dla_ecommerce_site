<?php
// $Id: page-treemenus.tpl.php,v 1.1 2008/10/30 15:47:17 rcrowther Exp $

/**
 * @file page-treemenus.tpl.php
 *
 * Theme implementation to display a page of treemenus.
 *
 *
 * Available variables:
 * - $treemenu_sane: Array of treemenu HTML output, keyed as (short)$name, containing,
 *
 * $treemenus_sane][menu-name]: the (sanitized) full machine name of the menu. Useless for the user,
 * good for making a CSS id.
 * $treemenus_sane][body]: the (sanitized) HTML of each treemenu.
 *
 * NOTE: You can set/unset/CSS/whatever, the titles/descriptions and body of the
 *  treemenus themselves in treemenu.tpl.php
 *
 *
 *
 * Other variables:
 * - $treemenus: Full term object AS ARRAY. Contains data that may not be safe.
 *  Also, really hairy and contains all the configuration data (nodes enabled, and so forth).
 * - $zebra: Outputs either "even" or "odd". Useful for zebra striping in listings.
 *
 *
 * NOTE: The unusual template. You for_each() and pull out the completed menus from $treemenus_sane.
 *  This contrasts to the usual method of just printing a variable.
 *
 * If you use the menu-name, take care you do not clash with the treemenu template,
 *  which also passes this variable, and uses it by default. You could append numbers, for example,
 *  or a defining string e.g. id = "menu-<?php print $treemenu_sane ?>-wrapper".
 *
 */
?>

  <?php foreach ($treemenus_sane as $treemenu_sane): ?>
    <div>
      <?php print $treemenu_sane['body'] ?>
    </div>
  <?php endforeach; ?>
