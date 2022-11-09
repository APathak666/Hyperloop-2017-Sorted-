$('#left-sidebar-control').sideNav({
      menuWidth: 300, // Default is 300
      edge: 'left', // Choose the horizontal origin
      closeOnClick: false, // Closes side-nav on <a> clicks, useful for Angular/Meteor
      draggable: false, // Choose whether you can drag to open on touch screens,
      onOpen: function(el) { /* Do Stuff */ }, // A function to be called when sideNav is opened
      onClose: function(el) { /* Do Stuff */ }, // A function to be called when sideNav is closed
    }
  );
$('#right-sidebar-control').sideNav({
      menuWidth: 300, // Default is 300
      edge: 'right', // Choose the horizontal origin
      closeOnClick: false, // Closes side-nav on <a> clicks, useful for Angular/Meteor
      draggable: false, // Choose whether you can drag to open on touch screens,
      onOpen: function(el) { /* Do Stuff */ }, // A function to be called when sideNav is opened
      onClose: function(el) { /* Do Stuff */ }, // A function to be called when sideNav is closed
    }
  );

var xhr = new XMLHttpRequest;
xhr.open('get','img/levitation.svg',true);
xhr.onreadystatechange = function(){
  if (xhr.readyState != 4) return;
  var svg = xhr.responseXML.documentElement;
  svg = document.importNode(svg,true); // surprisingly optional in these browsers
  $('.levitation').append(svg);
};
xhr.send();

var xhr1 = new XMLHttpRequest;
xhr1.open('get','img/brake-pad.svg',true);
xhr1.onreadystatechange = function(){
  if (xhr1.readyState != 4) return;
  var svg = xhr1.responseXML.documentElement;
  svg = document.importNode(svg,true); // surprisingly optional in these browsers
  $('.brakes').append(svg);
};
xhr1.send();

$('.controls .switch input#controls-toggle').change(function() {
   $('.controls .collapsible').collapsible('open', 0);
});
