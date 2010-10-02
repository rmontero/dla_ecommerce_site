; $Id: README.txt,v 1.2.2.1 2008/02/14 00:09:10 yelvington Exp $

OVERVIEW

The fbstatus module provides a way to retrieve your Facebook "status" setting and publish it
on your Drupal website automatically. The information is synchronized automatically based on
a configurable interval. A Drupal block is created consisting of the status field, nothing more.


This module is intended for someone who uses Drupal as a single-user, personal blogging platform.

INSTALLATION

Untar the package and place it in the usual place (generally sites/all/modules). Activate it,
then be sure to set permissions for administration and viewing of the status.

This module REQUIRES PHP5. The installation process will abort and complain if you are
running an outdated version.

CONFIGURATION

You'll need the URL of Facebook's RSS feed for your status information. This URL encodes your
identity and some security information, and should not be shared (unless you don't care about
making your status info public).

The URL is somewhat difficult to find on Facebook, but it's there. To find the URL:

-- Log into Facebook.
-- Click on "Profile" to go to your profile page.
-- On the "Mini-Feed" box, click "See All."
-- Find one of your own status updates in the feed. Click the icon attached to it. This will take
you to a feed that is filtered to include only your own status updates.
-- Look for the RSS feed icon under the "Subscribe to these Stories" title on the right
side of the page. It links to your unique RSS feed URL.

Be sure you have an RSS2.0 URL. This module will not process an Atom feed. The URL should look
something like this:

http://www.facebook.com/feeds/status.php?id=12345&viewer=67890&key=123aa12e3&format=rss20

Copy and paste that URL into your Drupal fbstatus module configuration. Set the refresh interval
and you're good to go.

The module creates a block that is available in the blocks configuration page. You'll need to
assign it to a region.

By default, the block has no title (you can change this in admin/build/block). On the Garland
theme, the block fits nicely at the top of the left sidebar, right under your site logo.

The standard Drupal permissions model applies to the retrieved content, so you can control
the audience for this information.

AUTHOR

Steve Yelvington <steve@yelvington.com>
