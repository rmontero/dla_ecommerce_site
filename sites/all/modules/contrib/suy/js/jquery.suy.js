(function($) {

/**
 * Simple IE6 notification based on sevenup (and the sevenup_black theme)
 * from http://code.google.com/p/sevenup/
 *
 * Usage:
 *    $.suy({options});
 *
 * Options:
 *    delay: 1000,
 *        delay after $(window).load() before popup
 *    dialogOptions: {
 *    	modal: true,
 *    	width: 550,
 *    	height: 500,
 *    	resizable: false          	
 *    },
 *        jQuery UI Dialog Options (http://jqueryui.com/demos/dialog)
 *    enableClosing: true,
 *        Set to false to lock website from IE6-
 *    enableCookie: true,
 *        Set to false to show every time
 *    onLoad: true,
 *        Set to false to fire popup directly
 *    downloadLink: osSupportsUpgrade ?
 *                  "http://www.microsoft.com/windows/internet-explorer" :
 *                  "http://getfirefox.com",
 *    onClose: function(){}
 *        Callback function that gets called when dialog is closed
 *
 * Examples and documentation at: http://www.caktux.ca/
 * Version: 1.0 (3/05/2009)
 * Requires: jQuery v1.2.6+
 * Copyright (c) 2009 Vincent Gariepy
 * Dual licensed under the MIT (http://en.wikipedia.org/wiki/MIT_License)
 * and GPL (http://en.wikipedia.org/wiki/GNU_General_Public_License) licenses
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.

 */

  $.fn.suy = function(options) {

    var osSupportsUpgrade = /(Windows NT 5.1|Windows NT 6.0|Windows NT 6.1|)/i.test(navigator.userAgent); // XP, Vista, Win7
    var settings = {
      delay: 1000,
      dialogOptions: {
      	modal: true,
      	width: 550,
      	height: 500,
      	resizable: false
      },
      enableClosing: true,
      enableCookie: true,
      onLoad: true,
      downloadLink: osSupportsUpgrade ? 
                    "http://www.microsoft.com/windows/internet-explorer" :
                    "http://getfirefox.com",
      onClose: function(){}
    };
    if (options) $.extend(settings, options);

    if (document.cookie.length > 0 && document.cookie.indexOf("suy=") != -1) return false;

    $dialogContent = $(' \
      <div class="suy"> \
        <div class="suy_content"> \
          <h1 class="suy_title">Your web browser is outdated and unsupported</h1> \
          <p class="suy_subtitle">You can easily upgrade to a better browser</p> \
          <div class="suy_chrome"> \
            <a href="http://www.google.com/chrome" rel="nofollow">&nbsp;</a> \
          </div> \
          <p class="suy_eightup_label"><a href="http://www.google.com/chrome" rel="nofollow">Google Chrome</a></p> \
          <div class="suy_upgrade"> \
            <h3>Why should I upgrade?</h3> \
            <dl> \
              <dt>Websites load faster</dt> \
              <dd>often double the speed of this older version.</dd> \
              <dt>Websites look better</dt> \
              <dd>you see sites they way they were intended.</dd> \
              <dt>Websites render correctly</dt> \
              <dd>with more web standards compliance.</dd> \
              <dt>Tabs Interface</dt> \
              <dd>lets you view multiple sites in one window.</dd> \
              <dt>Safer browsing</dt> \
              <dd>with better security and phishing protection.</dd> \
              <dt>Convenient Printing</dt> \
              <dd>with fit-to-page capability.</dd> \
            </dl> \
          </div> \
          <div class="suy_browsers"> \
            <h3>Explore other browsers</h3> \
            <ul> \
              <li class="suy_firefox"><a href="http://getfirefox.com/" rel="nofollow">Mozilla Firefox</a></li> \
              <li class="suy_safari"><a href="http://www.apple.com/safari/download" rel="nofollow">Apple Safari</a></li> \
              <li class="suy_eightup"><a href="http://www.microsoft.com/windows/internet-explorer" rel="nofollow">Internet Explorer 8</a></li> \
              <li class="suy_opera"><a href="http://www.opera.com/download" rel="nofollow">Opera</a></li> \
            </ul> \
          </div> \
          <div class="clear"></div> \
        </div> \
      </div>');

    if (settings.enableClosing) {
      $closeLinks = {};
      if (settings.enableCookie) {
        $.extend($closeLinks, {
          'Do not notify me again': function() {
            var exp = new Date();
            exp.setTime(exp.getTime() + 604800000);
            document.cookie = "suy=true; expires=" + exp.toUTCString();
            $(this).dialog('close');
            if (typeof(settings.onClose) == 'function') $(settings.onClose($(this)));
          }
        });
      }
      $.extend($closeLinks, {
        'Close': function() {
          $(this).dialog('close');
          if (typeof(settings.onClose) == 'function') $(settings.onClose($(this)));
        }
      });
      $.extend(settings.dialogOptions, {
        'buttons': $closeLinks
      });
    }

    if (settings.onLoad) {
      $(window).load( function() {
        setTimeout(( function() {
          $dialogContent.dialog(settings.dialogOptions);
        }), settings.delay);
      });
    }
    else {
      $dialogContent.dialog(settings.dialogOptions);  
    }
  };
  
  Drupal.behaviors.suy = function () {
    $.fn.suy();
  };

})(jQuery);
