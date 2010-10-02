$(document).ready(function() {
  var parents = $('a.active').parent('li').get(0);
  if (parents) {
    $('a.active').parent('li').get(0).setAttribute('class', 'active');
  }
});