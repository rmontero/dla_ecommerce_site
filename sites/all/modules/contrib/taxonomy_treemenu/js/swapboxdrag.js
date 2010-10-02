// $Id: swapboxdrag.js,v 1.1 2009/07/01 06:33:20 rcrowther Exp $

/**
 * Drag and drop table rows with field manipulation.
 *
 * Using the taxonomy_treemenu_enable_swapboxdrag() function, any div
 * containing further divs may be made into draggable set.
 *
 * Created swapboxDrag instances may be modified with custom behaviors by
 * overriding the .onDrag, .onDrop, .row.onSwap.
 */
Drupal.behaviors.swapboxDrag = function(context) {
  for (var base in Drupal.settings.swapboxDrag) {

    //alert(context);
    if (!$('#' + base + '.swapboxdrag-processed').size()) {
      var swapboxSettings = Drupal.settings.swapboxDrag[base];
          //alert(swapboxSettings.swapbox);
          
    //display(swapboxSettings.swapbox);
      $('#' + base).filter(':not(.swapboxdrag-processed)').each(function() {

        //alert(base);
        // Create the new swapboxDrag instance. Save in the Drupal variable
        // to allow other scripts access to the object.
        // 'this' comes from jQuery each() and is the DOM object (not an id).
        Drupal.swapboxDrag[base] = new Drupal.swapboxDrag(this, swapboxSettings);
      });

      $('#' + base).addClass('swapboxdrag-processed');
    }
  }
};

// Debug
display = function (o) {
    var out;
    if(typeof(o) == 'object') {
        for(prop in o) {
          out = out + prop + '\n';
         }
      }
      else {

    }
    alert('type:' + typeof(o) + '; '  + out);
  }




/**
 * Constructor for the swapboxDrag object.
 *
 * 'this' is a tag into Drupal settings for each bunch of
 *   swapboxDrag (container) data
 * @param container
 *   DOM object (DIV wrapper) containing the rows to be made draggable.
 * @param swapboxSettings
 *   Settings for the table added via drupal_add_dragtable().
 */
Drupal.swapboxDrag = function(container, swapboxSettings) {
  // i.e variable self is the entire contents of swapboxDrag.
  // Lets you pass stuff back into this object from calls, 
  // while allowing a jQuery each() to use 'this' for content identification.
  // The idea is used several times throughout this code.
  var self = this;
 //alert(swapbox);
    //display(swapboxSettings);
   // alert(swapboxSettings.swapbox);
  // Required object variables.
  this.container = container;
  this.swapboxSettings = swapboxSettings;
  this.dragObject = null; // Used to hold information about a current drag operation.
  this.rowObject = null; // Provides operations for row manipulation.
  this.oldRowElement = null; // Remember the previous element.
  this.oldY = 0; // Used to determine up or down direction from last mouse move.
  this.changed = false; // Whether anything in the entire table has changed.
  this.rtl = $(this.container).css('direction') == 'rtl' ? -1 : 1; // Direction of the table.

  // Configure the scroll settings.
  this.scrollSettings = { amount: 4, interval: 50, trigger: 70 };
  this.scrollInterval = null;
  this.scrollY = 0;
  this.windowHeight = 0;

  if(swapboxSettings.stripe) {
     self.preStripe();
  }
  // Make each applicable row draggable.
  //$('div.draggable', swapbox).each(function() { self.makeDraggable(this); });
 //alert(swapbox);
 // Not the same as the 'this' that comes in, comes fro jQuery.each();
  $(container).children('div.draggable').each(function() { self.makeDraggable(this); });
//alert(item.id);
  // Add mouse bindings to the document. The self variable is passed along
  // as event handlers do not have direct access to the tableDrag object.
  $(document).bind('mousemove', function(event) { return self.dragRow(event, self); });
  $(document).bind('mouseup', function(event) { return self.dropRow(event, self); });
};




/**
 * Take an item and add event handlers to make it become draggable.
 * 
 * item,
 *  a DOM item, from jQuery each()
 * this,
 *  an up to date swapboxDrag object
 */
Drupal.swapboxDrag.prototype.makeDraggable = function(item) {
  var self = this;
//alert(item.id);
 $(item).prepend('<span class="warning-container">&nbsp</span>');
  // Create the handle.
  var handle = $('<a href="#" class="tabledrag-handle"><div class="handle">&nbsp;</div></a>').attr('title', Drupal.t('Drag to re-order'));
  // Stick it in.
  $(item).prepend(handle);


  // Add hover action for the handle.
  handle.hover(function() {
    self.dragObject == null ? $(this).addClass('tabledrag-handle-hover') : null;
  }, function() {
    self.dragObject == null ? $(this).removeClass('tabledrag-handle-hover') : null;
  });

  // Add the mousedown action for the handle.
  handle.mousedown(function(event) {
    // Create a new dragObject recording the event information.
    self.dragObject = new Object();
    self.dragObject.initMouseOffset = self.getMouseOffset(item, event);
    self.dragObject.initMouseCoords = self.mouseCoords(event);


    // If there's a lingering row object from the keyboard, remove its focus.
    if (self.rowObject) {
      $('a.tabledrag-handle', self.rowObject.element).blur();
    }

    // Create a new rowObject for manipulation of this row.
    self.rowObject = new self.row(item, 'mouse');

    // Save the position of the table.
    self.container.topY = self.getPosition(self.container).y;
    self.container.leftX = self.getPosition(self.container).x;
    self.container.bottomY = self.container.topY + self.container.offsetHeight;
    self.container.rightX = self.container.leftX + self.container.offsetWidth;
//display(self.rowObject);
//alert(self.container);
    // Add classes to the handle and row.
    $(this).addClass('tabledrag-handle-hover');
    $(item).addClass('drag');

    // Set the document to use the move cursor during drag.
    $('body').addClass('drag');
    if (self.oldRowElement) {
      $(self.oldRowElement).removeClass('drag-previous');
    }

    // Hack for IE6 that flickers uncontrollably if select lists are moved.
    if (navigator.userAgent.indexOf('MSIE 6.') != -1) {
      $('select', this.container).css('display', 'none');
    }

    // Hack for Konqueror, prevent the blur handler from firing.
    // Konqueror always gives links focus, even after returning false on mousedown.
    self.safeBlur = false;

    // Call optional placeholder function.
    self.onDrag();
    return false;
  });

  // Prevent the anchor tag from jumping us to the top of the page.
  handle.click(function() {
    return false;
  });

  // Similar to the hover event, add a class when the handle is focused.
  handle.focus(function() {
    $(this).addClass('tabledrag-handle-hover');
    self.safeBlur = true;
  });

  // Remove the handle class on blur and fire the same function as a mouseup.
  handle.blur(function(event) {
    $(this).removeClass('tabledrag-handle-hover');
    if (self.rowObject && self.safeBlur) {
      self.dropRow(event, self);
    }
  });

  // Add arrow-key support to the handle.
  //TODO: Not fully checked.
  handle.keydown(function(event) {
    //show('keypress');
    // If a rowObject doesn't yet exist and this isn't the tab key.
    if (event.keyCode != 9 && !self.rowObject) {
      self.rowObject = new self.row(item, 'keyboard');
    }

    var keyChange = false;
    switch (event.keyCode) {
      //case 37: // Left arrow.
      //case 63234: // Safari left arrow.
        //keyChange = true;
        //self.rowObject.indent(-1 * self.rtl);
        //break;
      case 38: // Up arrow.
      case 63232: // Safari up arrow.
        var previousRow = $(self.rowObject.element).prev('div').get(0);

        if (previousRow) {
          self.safeBlur = false; // Do not allow the onBlur cleanup.
          self.rowObject.direction = 'up';
          keyChange = true;

          if ( $(previousRow).is('.draggable')) {
            // Swap with the previous row (unless previous row is the first one
            // and undraggable).
            self.rowObject.swap('before', previousRow);
            self.rowObject.interval = null;
            window.scrollBy(0, -parseInt(item.offsetHeight));
          }
          handle.get(0).focus(); // Regain focus after the DOM manipulation.
        }
        break;
      //case 39: // Right arrow.
      //case 63235: // Safari right arrow.
       // keyChange = true;
        //self.rowObject.indent(1 * self.rtl);
       // break;
      case 40: // Down arrow.
      case 63233: // Safari down arrow.
        var nextRow = $(self.rowObject.element).next('div').get(0);

        if (nextRow) {
          self.safeBlur = false; // Do not allow the onBlur cleanup.
          self.rowObject.direction = 'down';
          keyChange = true;
          
          if ( $(nextRow).is('.draggable')) {
            // Swap with the next row.
            self.rowObject.swap('after', nextRow);
            self.rowObject.interval = null;
            window.scrollBy(0, parseInt(item.offsetHeight));
          }
          handle.get(0).focus(); // Regain focus after the DOM manipulation.
        }
        break;
    }

    if (self.rowObject && self.rowObject.changed == true) {
      $(item).addClass('drag');
      if (self.oldRowElement) {
        $(self.oldRowElement).removeClass('drag-previous');
      }
      self.oldRowElement = item;
      if (self.swapboxSettings.stripe) {self.restripeTable()};
      self.onDrag();
    }

    // Returning false if we have an arrow key to prevent scrolling.
    if (keyChange) {
      return false;
    }
  });

  // Compatibility addition, return false on keypress to prevent unwanted scrolling.
  // IE and Safari will suppress scrolling on keydown, but all other browsers
  // need to return false on keypress. http://www.quirksmode.org/js/keys.html
  handle.keypress(function(event) {
    switch (event.keyCode) {
      case 37: // Left arrow.
      case 38: // Up arrow.
      case 39: // Right arrow.
      case 40: // Down arrow.
        return false;
    }
  });
};

/**
 *self is container info?
 * Mousemove event handler, bound to document.
 */
Drupal.swapboxDrag.prototype.dragRow = function(event, self) {
  if (self.dragObject) {
    self.currentMouseCoords = self.mouseCoords(event);

    var y = self.currentMouseCoords.y - self.dragObject.initMouseOffset.y;
    var x = self.currentMouseCoords.x - self.dragObject.initMouseOffset.x;

    // Check for row swapping and vertical scrolling.
    if (y != self.oldY) {
      self.rowObject.direction = y > self.oldY ? 'down' : 'up';
      self.oldY = y; // Update the old value.

      // Check if the window should be scrolled (and how fast).
      var scrollAmount = self.checkScroll(self.currentMouseCoords.y);
      // Stop any current scrolling.
      clearInterval(self.scrollInterval);
      // Continue scrolling if the mouse has moved in the scroll direction.
      if (scrollAmount > 0 && self.rowObject.direction == 'down' || scrollAmount < 0 && self.rowObject.direction == 'up') {
        self.setScroll(scrollAmount);
      }

      // If we have a valid target, perform the swap and restripe the table.
      var currentRow = self.findDropTargetRow(x, y);

      if (currentRow) {
        if (self.rowObject.direction == 'down') {
          self.rowObject.swap('after', currentRow);
        }
        else {
          self.rowObject.swap('before', currentRow);
        }
        if (self.swapboxSettings.stripe) {self.restripeTable()};
      }
    }

    return false;
  }
};

/**
 * Mouseup event handler, bound to document.
 * Blur event handler, bound to drag handle for keyboard support.
 */
Drupal.swapboxDrag.prototype.dropRow = function(event, self) {
  // Drop row functionality shared between mouseup and blur events.
  if (self.rowObject != null) {
    var droppedRow = self.rowObject.element;
    // The row is already in the right place so we just release it.
    if (self.rowObject.changed == true) {
      self.rowObject.markChanged();
      if (self.changed == false && self.swapboxSettings.changed_warning) {
        $(Drupal.theme('swapboxDragChangedWarning')).insertAfter(self.container).hide().fadeIn('slow');
        self.changed = true;
      }
    }


    if (self.oldRowElement) {
      $(self.oldRowElement).removeClass('drag-previous');
    }
    $(droppedRow).removeClass('drag').addClass('drag-previous');
    self.oldRowElement = droppedRow;
    self.onDrop();
    self.rowObject = null;
  }

  // Functionality specific only to mouseup event.
  if (self.dragObject != null) {
    $('.tabledrag-handle', droppedRow).removeClass('tabledrag-handle-hover');

    self.dragObject = null;
    $('body').removeClass('drag');
    clearInterval(self.scrollInterval);

    // Hack for IE6 that flickers uncontrollably if select lists are moved.
    if (navigator.userAgent.indexOf('MSIE 6.') != -1) {
      $('select', this.container).css('display', 'block');
    }
  }
};

/**
 * Get the position of an element by adding up parent offsets in the DOM tree.
 */
//Could have trouble here
Drupal.swapboxDrag.prototype.getPosition = function(element){
  var left = 0;
  var top  = 0;
  // Because Safari doesn't report offsetHeight on table rows, but does on table
  // cells, grab the firstChild of the row and use that instead.
  // http://jacob.peargrove.com/blog/2006/technical/table-row-offsettop-bug-in-safari
  if (element.offsetHeight == 0) {
    element = element.firstChild; // a table cell
  }

  while (element.offsetParent){
    left   += element.offsetLeft;
    top    += element.offsetTop;
    element = element.offsetParent;
  }

  left += element.offsetLeft;
  top  += element.offsetTop;

  return {x:left, y:top};
};

/**
 * Get the mouse coordinates from the event (allowing for browser differences).
 */
Drupal.swapboxDrag.prototype.mouseCoords = function(event){
  if (event.pageX || event.pageY) {
    return {x:event.pageX, y:event.pageY};
  }
  return {
    x:event.clientX + document.body.scrollLeft - document.body.clientLeft,
    y:event.clientY + document.body.scrollTop  - document.body.clientTop
  };
};

/**
 * Given a target element and a mouse event, get the mouse offset from that
 * element. To do this we need the element's position and the mouse position.
 */
Drupal.swapboxDrag.prototype.getMouseOffset = function(target, event) {
  var docPos   = this.getPosition(target);
  var mousePos = this.mouseCoords(event);
  return {x:mousePos.x - docPos.x, y:mousePos.y - docPos.y};
};

/**
 * Find the row the mouse is currently over. This row is then taken and swapped
 * with the one being dragged.
 * 'this' a swapboxDrag
 * @param x
 *   The x coordinate of the mouse on the page (not the screen).
 * @param y
 *   The y coordinate of the mouse on the page (not the screen).
 */
Drupal.swapboxDrag.prototype.findDropTargetRow = function(x, y) {
  var self = this;
  var row = null;

  $(self.container).children('div.draggable').each( function() {

    // Safari fix see Drupal.swapboxDrag.prototype.getPosition()
    if (this.offsetHeight == 0) {
      var rowY = self.getPosition(this.firstChild).y;
      var rowX = self.getPosition(this.firstChild).x;
      var rowHeight = parseInt(this.firstChild.offsetHeight)/2;
    }
    // Other browsers.
    else {
      var rowY = self.getPosition(this).y;
      var rowX = self.getPosition(this).x;
      var rowHeight = parseInt(this.offsetHeight)/2;
    }

    var swapboxWidth = parseInt(this.offsetWidth);
//show('x:' +x + 'y:' + y + 'rowX:' +rowX +  'swapboxWidth:' + swapboxWidth);
    // Because we always insert before, we need to offset the height a bit.
    // We check the row width, and should be ok to check if the user exceeds table left/right edges.
    if ((y > (rowY - rowHeight)) && (y < (rowY + rowHeight)) && (x > rowX ) && (x < (rowX + swapboxWidth))) {
      //this.weight = i;
      row = this;
    }
  });


  return row;
};

Drupal.swapboxDrag.prototype.checkScroll = function(cursorY) {
  var de  = document.documentElement;
  var b  = document.body;

  var windowHeight = this.windowHeight = window.innerHeight || (de.clientHeight && de.clientWidth != 0 ? de.clientHeight : b.offsetHeight);
  var scrollY = this.scrollY = (document.all ? (!de.scrollTop ? b.scrollTop : de.scrollTop) : (window.pageYOffset ? window.pageYOffset : window.scrollY));
  var trigger = this.scrollSettings.trigger;
  var delta = 0;

  // Return a scroll speed relative to the edge of the screen.
  if (cursorY - scrollY > windowHeight - trigger) {
    delta = trigger / (windowHeight + scrollY - cursorY);
    delta = (delta > 0 && delta < trigger) ? delta : trigger;
    return delta * this.scrollSettings.amount;
  }
  else if (cursorY - scrollY < trigger) {
    delta = trigger / (cursorY - scrollY);
    delta = (delta > 0 && delta < trigger) ? delta : trigger;
    return -delta * this.scrollSettings.amount;
  }
};

Drupal.swapboxDrag.prototype.setScroll = function(scrollAmount) {
  var self = this;

  this.scrollInterval = setInterval(function() {
    // Update the scroll values stored in the object.
    self.checkScroll(self.currentMouseCoords.y);
    var aboveTable = self.scrollY > self.container.topY;
    var belowTable = self.scrollY + self.windowHeight < self.container.bottomY;
    if (scrollAmount > 0 && belowTable || scrollAmount < 0 && aboveTable) {
      window.scrollBy(0, scrollAmount);
    }
  }, this.scrollSettings.interval);
};


Drupal.swapboxDrag.prototype.restripeTable = function() {
  // :even and :odd are reversed because jquery counts from 0 and
  // we count from 1, so we're out of sync.
  show('stripe called');
  $('div.draggable', this.swapbox)
    .filter(':odd').filter('.odd')
      .removeClass('odd').addClass('even')
    .end().end()
    .filter(':even').filter('.even')
      .removeClass('even').addClass('odd');
};


/**
 * Stub function. Allows a custom handler when a row begins dragging.
 */
Drupal.swapboxDrag.prototype.onDrag = function() {
  return null;
};

/**
 * Stub function. Allows a custom handler when a row is dropped.
 */
Drupal.swapboxDrag.prototype.onDrop = function() {
   //show(this.oldRowElement.weight);
  return null;
};

/**
 * Constructor to make a new object to manipulate a table row.
 *
 * @param swapboxRow
 *   The DOM element for the table row we will be manipulating.
 * @param method
 *   The method in which this row is being moved. Either 'keyboard' or 'mouse'.
 */
Drupal.swapboxDrag.prototype.row = function(swapboxRow, method) {
  this.element = swapboxRow;
  this.method = method;
  this.changed = false;
  this.containerDiv = $(swapboxRow).parent('div').get(0);
  this.direction = ''; // Direction the row is being moved.
};

/**
 * Perform the swap between two rows.
 *
 *'this' is a rowObject.
 *
 * @param position
 *   Whether the swap will occur 'before' or 'after' the given row.
 * @param row
 *   DOM element what will be swapped with the row group.
 */
Drupal.swapboxDrag.prototype.row.prototype.swap = function(position, row) {
  if(position == 'after') {
    $(this.element).before(row);
  }
  else {
    $(this.element).after(row);
  }
  this.changed = true;


  // For custom handlers.
  this.onSwap(row);
};


/**
 * Add an asterisk or other marker to the changed row.
 */
Drupal.swapboxDrag.prototype.row.prototype.markChanged = function() {
  var marker = Drupal.theme('swapboxDragChangedMarker');
  var span = $('.warning-container', this.element);
  if ($('span.swapboxdrag-changed', span).length == 0) {
    span.append(marker);
  }
};


/**
 * Stub function. Allows a custom handler when a row is swapped.
 */
Drupal.swapboxDrag.prototype.row.prototype.onSwap = function(swappedRow) {
  return null;
};


/*
 * Stripe draggable divs within a container
 */
Drupal.swapboxDrag.prototype.preStripe = function() {
   $(this.container).children('div.draggable').each(function(i) {
    if(i & 1) {
      $(this).addClass('even');
    }
    else {
      $(this).addClass('odd');
    }
   });
};

Drupal.theme.prototype.swapboxDragChangedMarker = function () {
  return '<span class="warning swapboxdrag-changed">*</span>';
};


Drupal.theme.prototype.swapboxDragChangedWarning = function () {
  return '<div class="warning">' + Drupal.theme('swapboxDragChangedMarker') + ' ' + Drupal.t("Changes made in this table will not be saved until the form is submitted.") + '</div>';
};
