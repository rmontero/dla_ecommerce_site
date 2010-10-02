$Id 

By enzo - Eduardo Garcia | enzo at anexusit dot com && drupalcr dot org

Copyright 2009 http://anexusit.com and http://sprymedia.co.uk/

Description
-----------
This module provides a tool to get information about javascripts events 

Events in Javascript are often seen as a bit of an enigma. 

This is odd given that Javascript is very much an event driven language, 
but it is typically down to their complex nature and difficulty to debug. 

To this end I've created Visual Event to help track events which are subscribed to DOM nodes.

    * Introduction
    * Usage
    * Technical information
    * Future work

Introduction

When working with events in Javascript, it is often easy to loose track of what events are subscribed where. 

This is particularly true if you are using a large number of events, which is typical in a modern interface employing progressive enhancement. Javascript libraries also add another degree of complexity to listeners from a technical point of view, while from a developers point of view they of course can make life much easier! But when things go wrong it can be difficult to trace down why this might be.

It is due to this I've put together a Javascript bookmarklet called Visual Event which visually shows the elements on a page that have events 
subscribed to them, what those events are and the function that the event would run when triggered. This is primarily intended to assist 
debugging, but it can also be very interesting and informative to see the subscribed events on other pages.

More Information at: http://sprymedia.co.uk/article/Visual+Event