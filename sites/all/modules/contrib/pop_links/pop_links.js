// $Id: pop_links.js,v 1.1.4.2 2008/05/10 10:28:34 sicjoy Exp $

var PopLinksTrackClick = function() {
  // an external link has been clicked
  var args = {};
  args.operation = "track_click";
  // get the URL of the site the user is going to
  args.url = $(this).attr("href");
  // get the id of the node that contains the clicked link
  args.nid = $(this).parents("div.node").attr("id").substring(5);
  // call the popular links AJAX handler
  var rsp = $.ajax({
    data    : args,
    url     : Drupal.settings.pop_links.base_path + "pop_links/handle",
    type    : "get",
    datatype: "html",
    async   : false // make sure vote is cast before sending user on her merry way
  }).responseText;
  return true;
}

if (Drupal.jsEnabled) {
  $(document).ready(function(){
    // Strip the host name down, removing subdomains or www.
    var host = window.location.host.replace(/^(([^\/]+?\.)*)([^\.]{4,})((\.[a-z]{1,4})*)$/, '$3$4');

    // Build regular expressions that define an internal link.
    var internal_link = new RegExp("^https?://([^/]*)?" + host, "i");

    // Find all links which are NOT internal and begin with http (as opposed
    // to ftp://, javascript:, etc. other kinds of links.
    // When operating on the 'this' variable, the host has been appended to
    // all links by the browser, even local ones.
    // In jQuery 1.1 and higher, we'd us a filter method here, but it is not
    // available in jQuery 1.0 (Drupal 5 default).
    $("div.node a").each(function(el) {
      try {
        var url = this.href.toLowerCase();
        if (url.indexOf('http') == 0 && !url.match(internal_link)) {
          $(this).click(PopLinksTrackClick);
        }
      }
      // IE7 throws errors often when dealing with irregular links, such as:
      // <a href="node/10"></a> Empty tags.
      // <a href="http://user:pass@example.com">example</a> User:pass syntax.
      catch(error) {
        return false;
      }
    });
  });
}
