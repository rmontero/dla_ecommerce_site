// $id$

Drupal.behaviors.advanced_help_inject = function() {
 jQuery.each(Drupal.settings.advanced_help_inject, function(k, v) {
   $("#"+k).parent().before(v);
 });
}
