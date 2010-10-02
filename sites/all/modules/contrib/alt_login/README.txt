$Id: README.txt,v 1.3 2007/05/03 14:11:41 thehunmonkgroup Exp $

****************************************************

Alternate Login Module -- README

written by Chad Phillips: thehunmonkgroup at yahoo dot com
****************************************************

This module provides an interface that allows registered users to use a login
name which is different than their username.

To use, simply enable the module, set permissions for who can create an alt
login, then visit the user edit page. Enter the alternate login name in the
'Alternate Login' textfield, and save.

Note that users can still login with their normal username--this just adds the
option of another login name. Also note that an alternate login name may not
be equivalent to any other current alternate login name, nor any current
username.

INSTALLATION:

1. Put the entire 'alt_login' folder in either:
      a.  Your main 'modules' folder.
      b.  The modules folder of a multisite installation.
      c.  The 'sites/all/modules' folder.

2. Enable the module at Administer -> Modules.

3. At Administer -> Access control, set the 'create alterate login'
   permission for those roles that should be able to create/edit
   an alt login. If a user has and alt login created for them, but
   they do not have create permissions, then the alt login will be
   displayed as a non-editable form item.

4. At Administer -> User Settings, set your preference for displaying the
   alternate login option upon user registration.  Note that permissioned
   users can still add/edit their alternate login at their account edit
   page regardless of this setting.
