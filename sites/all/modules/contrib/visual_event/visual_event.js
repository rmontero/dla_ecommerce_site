// $Id: visual_event.js,v 1.1 2009/10/30 17:33:51 enzo Exp $ 
var visualEventEnabled = 0;

var visualEventESC = function(e){
	  if (e.keyCode == 27) {
		visualEventEnabled = 0;
		$('#visual_event-toggle :checkbox').attr('checked','');
		$(document).unbind('keyup',visualEventESC);
	  }	
};	

var visualEventToggle = function() {
	visualEventEnabled = !visualEventEnabled;
	$('#visual_event-toggle :checkbox').attr('checked',visualEventEnabled ? 'checked' : '');

	if (visualEventEnabled) {
		QVisualEvent.Init();
		$(document).keyup(visualEventESC);
	}
};

if (Drupal.jsEnabled) {
	$(document)
			.ready(
					function() {
						var strs = Drupal.settings.veStrings;

						$(
								'<div id="visual_event-toggle"><input type="checkbox" />' + strs.visual_events + '</div>')
								.appendTo($('body')).click(visualEventToggle);

					});
}
