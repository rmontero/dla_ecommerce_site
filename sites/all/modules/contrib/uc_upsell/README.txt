UC_Upsell

Coded by Erik Peterson (torgosPizza)
Sponsored by Mike Nelson's RiffTrax: http://www.rifftrax.com

---

This module provides Ubercart store owners with a configurable block, which is displayable at 
checkout as well as on site pages. The block is made by selecting "related products" during the
node edit process, at which point the associations are stored in the database. 

Users can also choose to show random product nodes mixed in with the related products.

New admin pages are created:

1) admin/store/settings/upsell - Upsell Global configuration settings (This is the most important!)
2) admin/store/products/upsell - Shows all current product associations in a paged table format.

Likewise, a new block is created, so its configuration is available under admin/build/block

NOTE: When you first enable the module, make sure you go to admin/store/settings/upsell and check
  a few things:
  
  1) Node types: If you are using multiple product classes besides the default Ubercart "product"
     type, then you will need to select which product classes you can allow in the Upsell block.
     Note that this also affects which products show up in the Node Edit form!
  2) Image handling: If you're using a non-default Imagecache Preset for your product images, or
     you want to change the Preset that's used for the Upsell block, you'll need to configure that here.
  3) Block settings vs. Global settings: You can choose in the admin/build/block/uc_upsell configuration
     what products to include, such as Random products, as well as the message to show to the customer.
     Keep in mind these are handled differently between Product pages and the Cart pane.
  
  Most issues that arise in the module are due to a strange configuration of these options. Check
  them first before submitting an Issue! If, however, you can't get something to work, feel free
  to create a new Issue at the Upsell project page (http://drupal.org/project/uc_upsell)

This module is maintained solely by me. If you'd like to help contribute directly to further development,
you can donate via PayPal at the below link:

https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=3939947

As always, patches, code snippets, use cases and overall feedback are hugely appreciated.

Thanks!