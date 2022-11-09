/**
 ****************************
 * Slider code 
 ****************************
 * We define a new class 
 * Each slider is an object declared using this class
 * The sliders used are defined at the end of the file 
 */

/**
 * Constructor for @class Slider which initializes the Slider and attaches event handlers
 */ 
function Slider(parameters) {
  // 'this' refers to the object. We Store the passed parameters as properties of the new object
  for (var x in parameters) {
    this[x] = parameters[x];
  }
  var obj = this;

  var input = '#' + obj.id + ' .range-value';

  //initialize the slider
  $('#' + this.id).slider({
    range: "min", //starts slider from min to max
    max: obj.max,
    min: obj.min,
    value: 0,
    slide: function (e, ui) { // function to be called on execution of slider
      $(input).val(ui.value);
    }
  });

  //Event handler attached to .range-value of each slider
  //Purpose: If input is changed manually, set slider to that position
  $(input).change(function () {
    if ($(input).val() <= obj.max && $(input).val() >= obj.min) {
      $('#' + obj.id).slider('value', $(input).val());
    }
  });
}


/**
 * Sliders we use :
 */
var brakes = new Slider({
  'id': 'range1',
  'max': 38,
  'min': 8
});

var aux = new Slider({
  'id': 'range2',
  'max': 50,
  'min': 0
});

$('#range1 input').change(function() {
    sendCommand("3");
    sendCommand($(this).val());
}); 