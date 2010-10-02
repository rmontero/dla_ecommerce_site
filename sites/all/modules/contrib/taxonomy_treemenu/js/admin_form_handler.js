// $id$

Drupal.behaviors.TTMOps = function() {
    //do some fancy stuff

//test
//$('#edit-link-roots-to-menutrees').change(function () {
//      getWeights('sort');
//})

//function show(v){
//  $('#tab1_display').html(v);
//}


function getWeights(group) {
  //TODO: pretty jQuery or not, this is a general search on an ATTRIBUTE,
  // of all things, so fairly horrible in my book. We've got swapbox div info..
  var target = $("div[title='" + group +"']");

  if(target.length != 1) {
    return false;
  }

  // TODO: can this object access be done without an eval()? eval()s are evil.
  $(target).children('div.draggable').each( function(i) {
    exp = "Drupal.settings.ttm." + group + '.'+ $(this).attr('title') + '.weight = ' + i;
    eval(exp);
  });

  return true;
}

// eval() is generally a bit evil, though there is nothing user defined here,
// so it is naturally validated. Still if anyone knows another way, or would we
// have to change our options object to a straight array?
function setOpt(group, opt, prprty, value) {
  var mult = opt.split('-');
  if(mult.length > 1) {
    // Note that setOptMult discards the properrty parameter.
    // It always sets value.
    setOptMult(group, mult[0], mult[1], value);
    return;
  }

  var exp = 'Drupal.settings.ttm.'+ group +'.'+ opt +'.'+ prprty +' = '+ value;
  eval(exp);
}


function setOptMult(group, p0, p1, value) {
  var exp= 'Drupal.settings.ttm.'+ group +'.'+ p0 +'.value.'+ p1 +' = '+ value;
  var a = eval( 'Drupal.settings.ttm.'+ group +'.'+ p0 +'.value.'+ p1 +' = '+ value);
}

function enableOpts(grp) {
    var o = $("div[title='"+ grp +"']");

    // Toggles
    o.find(".ttm-toggle-left").click(function () {
      // We won't waste jQuery on this!
      var node = $(this)[0];
      node.className = "ttm-toggle-left ttm-toggle-on";
      parent = node.parentNode;
      parent.lastChild.className = "ttm-toggle-right ttm-toggle-off";
      setOpt(grp,  $(parent).attr('title'), 'value', 1);
    });

    o.find(".ttm-toggle-right").click(function () {
      // We won't waste jQuery on this!
      node = $(this)[0];
      node.className = "ttm-toggle-right ttm-toggle-on";
      parent = node.parentNode;
      parent.firstChild.className = "ttm-toggle-left ttm-toggle-off";
      setOpt(grp, $(parent).attr('title'), 'value', 0);
    });

    // Checkboxes
    o.find("input").change(function () {
       setOpt(grp, $(this).attr('title'), 'active', $(this).attr('checked'));
    });
}

// Main. Setup functions.

    //  Just before we submit, we update the weight fields in weighed option
    //  groups.  This is a bit of a pain to javascript on the fly, what with
    //  js events not really liking parameters and so forth, so we do it here.
    $("form").submit(function() {
      getWeights('sort');
      getWeights('fields');
      $('#edit-ops').val(JSON.stringify(Drupal.settings.ttm));
    });

    // Enable toggles
    enableOpts('sort');
    enableOpts('filter');
    enableOpts('fields');
}
