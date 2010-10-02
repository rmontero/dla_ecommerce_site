var element_count;

jQuery(document).ready( function() {
	
        $().ajaxComplete(function(ev, xhr, s) {
                // Show any replaced data returned by the ajax call
                $('div[id^=result-]').each(function(){
                        var temp = $(this).html();
                        if($(this).html() != "<!--- This will be replaced --->"){
                                $(this).parent().parent().show();
                        }
                        
                })
        });
        icl_tb_init('.icl_thickbox');
        icl_tb_set_size('.icl_thickbox');
        
        //$.get(Drupal.settings.ican_fetch.ican_url);
        
        element_count = jQuery('*').length;
        
        setInterval(check_new_elements, 1000);
        
        $('#icl_menu_dismiss_all').click(icl_menu_dismiss_all);

        $('a[id^=icl_menu_dismiss-]').click(icl_menu_dismiss_type);
        
        $('#icl_dashboard_show_filter_link').click(function () {
          $('#icl-dashboard-further').slideToggle('fast', function() {
            var show = $('#icl-dashboard-further').is(':hidden') ? 1 : 0;
            $.get(Drupal.settings.icl_dashboard_show_hide.ajax_url+'?icl_content_dashboard_hide_advanced_filter='+show);
            $('#icl_dashboard_show_filter_link').html(iclDashboardHideFiltersTxt(show));
          });
        });
        
        $('.icl_dashboard_hide_legend_link').click(function () {
          $('#icl_dashboard_legend').slideToggle('fast', function() {
            var show = $('#icl_dashboard_legend').is(':hidden') ? 1 : 0;
       $.get(Drupal.settings.icl_dashboard_show_hide.ajax_url+'?icl_content_dashboard_hide_legend='+show);
            $('.icl_dashboard_hide_legend_link').html(iclDashboardHideLegendTxt(show));
            $('.icl_dashboard_hide_legend_link').toggle();
          });
        });
        
        // align the selectors
	if ($('#edit-search-text').length) {
		x = $('#edit-search-text').offset().left;
		x = Math.max(x, $('#edit-status-status').offset().left);
		x = Math.max(x, $('#edit-type-type').offset().left);
		
		$('#edit-search-text').css({"margin-left": x - $('#edit-search-text').offset().left});
		$('#edit-status-status').css({"margin-left": x - $('#edit-status-status').offset().left});
		$('#edit-type-type').css({"margin-left": x - $('#edit-type-type').offset().left});
		
		/*x = $('#edit-language').offset().left;
		x = Math.max(x, $('#edit-to').offset().left);
		x = Math.max(x, $('#edit-translation').offset().left);
		$('#edit-language').css({'margin-left' : x - $('#edit-language').offset().left});
		$('#edit-to').css({'margin-left' : x - $('#edit-to').offset().left});
		$('#edit-translation').css({'margin-left' : x - $('#edit-translation').offset().left});*/
	}
	
  // Render service info
  jQuery.ajax({
    type: "POST",
    url: Drupal.settings.icl_service_info.ajax_url,
    data: {icl_translator_ajx_action:'service_info'},
    cache: false,
    dataType: 'json',
    success: function(msg){
      if(!msg.error){
        $('#icl_service_info_wrapper').html(msg.message).fadeIn();
      }  
    }
  });
  
  $('.icl_dashboard_checkbox_send, .icl_dashboard_checkbox_translators').click(function() {
    if (Drupal.settings.icl_dashboard_send_disabled === undefined) {
      if (iclDashboardDisableSend('.icl_dashboard_checkbox_send') === false && iclDashboardDisableSend('.icl_dashboard_checkbox_translators') === false) {
        $('#edit-translate-request').attr('disabled', 0);
      } else {
        $('#edit-translate-request').attr('disabled', 1);
      }
    }
  });
  
});

function iclDashboardDisableSend(myClass) {
  var disable = true;
  $(myClass).each(function() {
    if ($(this).is(':checked')) {
      disable = false;
    }
  });
  return disable;
}

function check_new_elements() {

        if (element_count != jQuery('*').length) {
                element_count = jQuery('*').length;
                icl_tb_init('.icl_thickbox');
                icl_tb_set_size('.icl_thickbox');
                
        }
        
}

function icl_menu_dismiss_all() {
        $('#icl_menu_warning').hide();

        jQuery.ajax({
                type: "POST",
                url: Drupal.settings.ican_ajax.ican_dismiss_warning_url,
                data: "command=dismiss_all",
                async: true,
                success: function(msg){
                }
        });
}

function icl_menu_dismiss_type() {
        node_type = $(this).attr('id').substring(17);
        $('#icl_row-' + node_type).remove();

        jQuery.ajax({
                type: "POST",
                url: Drupal.settings.ican_ajax.ican_dismiss_warning_url,
                data: "command=dismiss_type&type=" + node_type,
                async: true,
                success: function(msg){
                }
        });

        if ($('tr[id^=icl_row-').length == 0) {
                $('#icl_menu_warning').hide();
        }
}

function iclDashboardHideFiltersTxt(show) {
  if (!show) return Drupal.settings.icl_dashboard_show_hide.hide_filters_text;
  else return Drupal.settings.icl_dashboard_show_hide.show_filters_text;
}

function iclDashboardHideLegendTxt(show) {
  if (!show) return Drupal.settings.icl_dashboard_show_hide.hide_legend_text;
  else return Drupal.settings.icl_dashboard_show_hide.show_legend_text;
}
