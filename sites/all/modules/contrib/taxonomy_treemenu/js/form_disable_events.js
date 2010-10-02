// $id$

Drupal.behaviors.ttm_disable_events = function() {
 jQuery.each(Drupal.settings.ttm_disable_events, function(k, v) {
   $("#"+k).change(function () {
     var flag = ($("#"+k).attr('checked'));
     jQuery.each(v, function(k, v) {
       if(flag) {
         $("#"+k).removeAttr("disabled");
       }
       else {
         $("#"+k).attr("disabled","disabled"); 
       }
     });
   });
 });
}
