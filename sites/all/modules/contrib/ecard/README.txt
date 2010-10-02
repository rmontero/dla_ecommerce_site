// $Id: README.txt,v 1.1.4.3 2009/02/17 21:09:55 karst Exp $
*************************************************
           Ecard Module
*************************************************

This module allows users to create ecards and send a link to a node with
that contains a message they wrote to their friends by email.

Author: msarath
co-maintainers: Karsten Frohwein
This is the Drupal 6 version.

There is one module postcard available , but its tied up with image module
and this ecard module is a general purpose one.

You can hook it to any content type whether its image, blog , story, or any
custom CCK content type.

You can customize the letter to send through admin interface at
admin/settings/ecard also set which content type you want to hook to. You
can also customize the notification letter and the number of days ecards
should kept in the database and also the number max number of recipient
which default to 100.

You can also hook to blogs/story I feel then its better to rename the
submit button as 'share this post' :)

Example
-------------------------
A working example can be found at http://www.mahlove.com - where it es
hooked to flash content.

-------------------------
Please read INSTALL.txt to know about installation.
-------------------------

Feel free to post suggestionsand bugs at
http://drupal.org/project/issues/ecard