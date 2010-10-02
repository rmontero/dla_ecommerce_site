$id$

Installing
----------
Like any other Drupal module.

If you have Views on board, Taxonomy treemenu will use that module for rendering lists and nodes.

If you do not have Views, Taxonomy Treemenu loads it's own page rendering functions.

Why use Views or the internal functions?

If you have Views enabled, you have no choice - Taxonomy Treemenu will use it.

If you don't have Views,
- Views can add information to the lists from data other than the basic node information. So if you want to display a node author name, for example, you can do this relatively easily through the Views GUI.
- The internal functions just can't to this easily. You can add information from the menu data or node data itself, such as descriptions, or a simple 'created' date, using the templates nd the variables they carry. But that's it.

- Views requires some power to run, we find we need 500M of RAM.
- The internal functions use few resources. if Drupal and the module runs, the site will work.

And one minor item.
Views integration is not very advanced, and lists generated from Views may not reflect configuration options in treemenu admin.
The internal functions render lists just as the module renders them in the treemenus.


Permissions
-----------
There are no special permissions associated with this module.

Menu viewing is permitted by 'access content' (like Taxonomy).

Node viewing through 'taxmenu/%taxonomy_treemenu_ttid/node/%taxmenu_tid_trail2/%node' constructions are permitted with 'node_access', 'access arguments' => array('view', 4)' (like node/%).

Administration is permitted by 'administer menu'.


Note that, if rendering lists using the Views module, the default view permissions are set to 'none'. This is because the views are embeded, and not acessible to a user except through Treemenu, which does it's own checks.


Taxonomy treemenu
----------------
This module generates a menu tree built from any root in your taxonomy.

The menu is saved as a custom menu (you can get at it through conventional admin), but renders as a system menu (auto expanding).

The generated treemenus auto update to reflect changes in the Taxonomy.

Node links are auto-generated, and not customizable, not for weight, title, or linkage (this is done for efficiency).

There are overall options, for showing menus without node links (they link to 'term' node lists), node sorting, and others.

No RSS, as of now.

 
Taxonomy Treemenu and Views
---------------------------
If you have the Drupal module Views installed and enabled, Taxonomy Treemenu will use it for term lists and node display.

Taxonomy Treemenu loads two starter displays as default, and you should find them in the Views list.

These two default displays MUST be enabled, or Taxonomy Treemenu will not show term lists of nodes.
(Views overrides other modules routing. Even if you do not promote the default displays, the internal renderer will not work?).

Note that, on the default views, the permissions have not been set. Taxonomy Treemenu does its own URL checking, so there is no need for further checks.



Theming
-------
Stock Drupal menus, so you can do a lot here.

- Override the template 'treemenu.tpl.php' to alter the look of all menus.

**Currently depreciated!** - Override the template 'page-treemenus' to alter a multiple treemenu page.

- Override theme_taxonomy_treemenu_item_link() to change the structure of the links.

If you want to change the look of a link, you don't need to look at the theme. The links use stock Drupal CSS, so override it. For example,

li.leaf.a:hover {
 background-color:lightSalmon;
}

placed in your theme's 'style.css' file should have an effect.

There is a special link which joins menus together. By default it appears as a tag to the title saying ' (submenu)'. To change the structure or the words, override theme_taxonomy_treemenu_root_link(). To change the look, add CSS to alter <span class="treemenu-root-link">.

If you enable link node counts, they are enclosed in a <span></span>, so you can style them differently using descendant CSS.


Theming within menus
---------------------
Wait, there's more.

When Drupal themes a page, it constructs template suggestions based on the URL.
 Let's say a user clicks on a node link in a treemenu, and arrives at node number (nid) 91,

 /?q=taxmenu/menu-redtree/node/27/91

Drupal constructs these template suggestions
  [template_files] => Array
        (
            [0] => page-taxmenu
            [1] => page-taxmenu-menu-redtree
            [2] => page-taxmenu-menu-redtree-node
            [3] => page-taxmenu-menu-redtree-node-27
            [4] => page-taxmenu-menu-redtree-node-91
        )

Here are the programming notes, copied from theme.inc

 // Build a list of suggested template files in order of specificity. One
  // suggestion is made for every element of the current path, though
  // numeric elements are not carried to subsequent suggestions. For example,
  // http://www.example.com/node/1/edit would result in the following
  // suggestions:
  //
  // page-node-edit.tpl.php
  // page-node-1.tpl.php
  // page-node.tpl.php
  // page.tpl.php

Anyway, we add a twist. We explode the tid trail in the suggestions, and call it
 '...-descendant-[tid]'.

For a taxmenu node, the suggestions should be,
(
    [0] => page-taxmenu
    [1] => page-taxmenu-menu-redtree
    [2] => page-taxmenu-menu-redtree-node
    [3] => page-taxmenu-menu-redtree-node-23-8-12
    [4] => page-taxmenu-menu-redtree-node-23-8-12-83
)

but are processed to become,
Array
(
    [0] => page-taxmenu
    [1] => page-taxmenu-menu-redtree
    [2] => page-taxmenu-menu-redtree-term
    [3] => page-taxmenu-menu-redtree-term-23-8
    [4] => page-taxmenu-menu-redtree-term-23-8-12
    [5] => page-taxmenu-menu-redtree-descendant-23
    [6] => page-taxmenu-menu-redtree-descendant-8
)


For terms and paths, we also add the term itself as a '-descendant-[tid]' in the
suggestion list,
For a taxmenu term, should be,
Array
(
    [0] => page-taxmenu
    [1] => page-taxmenu-menu-redtree
    [2] => page-taxmenu-menu-redtree-term
    [3] => page-taxmenu-menu-redtree-term-23-8
    [4] => page-taxmenu-menu-redtree-term-23-8-12
)

but is,
Array
(
    [0] => page-taxmenu
    [1] => page-taxmenu-menu-redtree
    [2] => page-taxmenu-menu-redtree-term
    [3] => page-taxmenu-menu-redtree-term-23-8
    [4] => page-taxmenu-menu-redtree-term-23-8-12
    [5] => page-taxmenu-menu-redtree-descendant-23
    [6] => page-taxmenu-menu-redtree-descendant-8
    [7] => page-taxmenu-menu-redtree-descendant-12
)

What does all that mean? Make a template, and it affects this,

page-taxmenu
 - any taxmenu page (bit wierd).

page-taxmenu-menu-redtree
 - Any redtree page. Could be useful for instant submenu identification. Blatantly,
I could colour this menu red.

page-taxmenu-menu-redtree-term
 - Any term page in redtree. Make your 'used car' term list have little cars on it.

page-taxmenu-menu-redtree-term-23-8
page-taxmenu-menu-redtree-term-23-8-12
- Nutty, these. Redtree term list expanded to trail 23-8 and node 23-8-12. Bit
specialized, and ambiguous (is that a node or a term?).

page-taxmenu-menu-redtree-descendant-23
page-taxmenu-menu-redtree-descendant-8
page-taxmenu-menu-redtree-descendant-12
Any redtree page descended from (and including) these terms.

The joy of the last options is this. Let's say you have a site, and
all of the data is based in one taxonomy (because its congruous stuff). For the sake of
an example, it's your shot at EBay glory.

You might construct little submenus, one, say, for 'used cars'. In which case you could
style the 'used car' menu.

But if the data is congruous, all you really want is to put a picture of a used
car next to the 'used car' sections. Well, locate the tid which is the root of your
'used car' section, make a template with a used car picture, name it,

page-taxmenu-menu-[whatever your big menu is]-descendant-['used car'tid'].tpl.php

and bingo, every paged menu, term list, or node (which is visited via taxmenu), has a
picture of a used car on it.

Pretty neat, I'd say.


Things taxonomy-treemenu can do
-------------------------------
Well integrated into the Drupal custom menu system. Create treemenus using the special button on the menu page 'Add taxonomy treemenu'. Read, Update, Delete, as you would with any other custom menu.

Generate a treemenu from any part of the taxonomy.

Auto-expanding menus, just like 'navigation'

Auto-updates itself to reflect changes in the taxonomy (Most of the time! Sometimes needs cache clearance to encourage the displays to update).

Works with multiple heirachies. Or seems to - not fully tested.

Show nodes, or not.

Alter depth or not. At last, a real use for depth! (Unless you're a fan of Menu Block)

Looks intelligently for link reuse.

Has a pager.

**Currently Depreciated**Try '?q=treemenu/' and the name of your menu. You can also use ',' or '+' as a separator and get multiple menus on one page. e.g. 'treemenu/cheapcars+stuffedanimals'.

Works in blocks, of course. Treemenus are proper custom menus.

Works with the DHTML Menu module, kind of.

Has interesting render options, such as 'link to treemenus' (links treemenus together) and 'display node count'.


Things it can't do
------------------
Update is erratic. Unlike core menu displays, much treemenu updates usually happen instantly - whereas the core modules must call menu refreshes. So treemenu updating and creation are faster than the main menus. Don't get used to this, because we use core menu garbage collection for deletes and cacheing, so **sometimes** you will have to rebuild menus or empty cache.

The page function will not page menus. Which is to say, if your menu is very big, the module will not clip it, but push the display off the screen.

No RSS, or integration with 'pathauto' type modules.

If you tamper with taxonomy terms, taxonomy treemenu looses track of treemenu customizations. 'Expand' and 'hidden' are lost, but 'nodes' and 'depth' are not.

Has abandoned seperate links for nodes. This seems more efficent, but you don't have any control over the order or title of node links.


Extra stuff
-----------

There is a 'treemenu rebuild' link which appears in the 'admin' menus or, if you have that module, the 'devel' module menu. This is for desperate situations and development work. You shouldn't have to use it, but it may help if things go wrong.
