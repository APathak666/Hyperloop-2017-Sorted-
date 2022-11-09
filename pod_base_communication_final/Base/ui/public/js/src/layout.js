$('#left-sidebar-control').sideNav({
      //menuWidth: $(window).width()*(3/12), // Default is 300
      edge: 'left', // Choose the horizontal origin
      closeOnClick: false, // Closes side-nav on <a> clicks, useful for Angular/Meteor
      draggable: false, // Choose whether you can drag to open on touch screens,
      onOpen: function(el) { /* Do Stuff */ }, // A function to be called when sideNav is opened
      onClose: function(el) { /* Do Stuff */ }, // A function to be called when sideNav is closed
    }
  );
$('#right-sidebar-control').sideNav({
      //menuWidth: $(window).width()*(3/12), // Default is 300
      edge: 'right', // Choose the horizontal origin
      closeOnClick: false, // Closes side-nav on <a> clicks, useful for Angular/Meteor
      draggable: false, // Choose whether you can drag to open on touch screens,
      onOpen: function(el) { /* Do Stuff */ }, // A function to be called when sideNav is opened
      onClose: function(el) { /* Do Stuff */ }, // A function to be called when sideNav is closed
    }
  );
console.log($(window).width());
/**
 * **************************
 * loadSVG
 * **************************
 * loads the given svg and appends it to the html element with the given id
 * 
 * usage: loadSVG('svg-filename.svg', 'id-of-html-element')
 */

loadSVG('levitation.svg', 'levitation-distance');
loadSVG('brake-pad.svg', 'brake-pad-distance');

function loadSVG(file,id) {
  var xhr = new XMLHttpRequest;
  xhr.open('get','img/'+file,true);
  xhr.onreadystatechange = function(){
    if (xhr.readyState != 4) return;
    var svg = xhr.responseXML.documentElement;
    svg = document.importNode(svg,true); // surprisingly optional in these browsers
    $('#'+id).append(svg);
  };
  xhr.send();
}

