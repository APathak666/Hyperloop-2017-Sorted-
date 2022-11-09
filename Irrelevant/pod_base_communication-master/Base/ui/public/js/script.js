/**********************************
Start/Emergency Stop Button
**********************************/
$(".btn1").click(function ()
{
    if ($(this).text()=="Stop")
    {
        sendCommand()
        $(this).text("Start");
        sendCommand("Pod Start");
        $(this).removeClass('red');
        $(this).addClass('teal');
      }
    else
    {
        $(this).text("Stop");
        sendCommand("Pod Stop");
        $(this).removeClass('teal');
        $(this).addClass('red');
    }
});
/**********************************
Toggle Button
**********************************/
//this is triggered for every input with type=checkbox
$('input:checkbox').change( function(){
    if($(this).is(':checked')) {
        //add class checked (used for css styling)
        $(this).closest('div').addClass('checked').trigger('switch');
        
        //if this switch is a command switch then send the string stored in the on-command attribute of the switch
        if($(this).closest('div').attr('command')!=null) {
            sendCommand($(this).closest('div').attr('on-command'));
        }

    } 
    else {
        $(this).closest('div').removeClass('checked').trigger('switch');
        
        //if this switch is a command switch then send the string stored in the off-command attribute of the switch
        if($(this).closest('div').attr('command')!=null) {
            sendCommand($(this).closest('div').attr('off-command'));
        }
    }
});


//append 4 bars to each battery
$('.battery').each(function(i) {
    for(var x=1; x<=4; x++){
        $(this).append('<div class="bar">');
    }
});

/**
 * **************************
 * Gauge for Yaw Pitch Roll
 * **************************
 * 
 * This uses the SVG Gauge library 
 * https://github.com/naikus/svg-gauge
*/
var Gauge = window.Gauge;

var yaw = Gauge(
  document.getElementById("yaw"), {
    max: 360,
    dialStartAngle: -90,
    dialEndAngle: -90.001,
    value: 100,
    label: function(value) {
      return value+String.fromCharCode(176);
    }
  }
);

var pitch = Gauge(
  document.getElementById("pitch"), {
    max: 360,
    dialStartAngle: -90,
    dialEndAngle: -90.001,
    value: 100,
    label: function(value) {
      return value+String.fromCharCode(176);
    }
  }
);

var roll = Gauge(
  document.getElementById("roll"), {
    max: 360,
    dialStartAngle: -90,
    dialEndAngle: -90.001,
    value: 100,
    label: function(value) {
      return value+String.fromCharCode(176);
    }
  }
);
var checks = [ 
  "Computer Mode Operational",
  "Telemetry Stream Active",
  "Emergency brakes - disengaged",
  "UI reading of Pod relay state",
  "Battery packs sufficiently charged.",
  "UI reading of Battery temperature",
  "Brake pads are still at 32 mm",
  "Llevitation module is at 45 mm",
  "Zero acceleration in all axes",
  "Zero velocity in all axes",
  "External pressure is 860Pa",
  "Nominal temperatures",
  "SpaceX Pusher Status - Ready"
];

var html;
for(var i in checks) {
  // append each check name into the health display
  $('#health-display').append('<div class="check-item" id="chk'+i+'"><i class="status material-icons">cached</i><span class="check-item-name">'+checks[i]+'</span></div>');
}

$('#start-check').click(function() {
  $('.health .collapsible').collapsible('open', 0);
  startCheck(0);
});

function startCheck(count) {
  if(count > checks.length) {
    $('#health-display').append('<p  class="check-item">All Health Checks Passed. Pod is ready for Launch! </p>');
    return;
  }
  $('#chk'+count+' .status')
    .html('check')
    .css({
      'color': 'green',
    });
  console.log( $('#chk'+count+'.status'));
  setTimeout(function(){startCheck(count+1)},300);
}
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

/**
 * ***************************************
 * Position and Strip Count Indicator Code
 * ***************************************
 * We define a new class, with methods to create and update the indicator
 * The object is declared using this class at the end of the file
 */

/**
 * This is the constructor function for the class - It initializes a new Position Indicator object
 * parameters - An object with id, label, unit, count, and count_label
 */
function PositionIndicator(parameters) {
    for (var x in parameters) {
        this[x] = parameters[x];
    }
}
/**
 * init()
 * Create the Position Indicator and append it to the HTML element whose ID is supplied with parameters
 */
PositionIndicator.prototype.init = function () {
    /** If you change the total number of strips, then the angles in addCSS() also need to be changed */
    var strips = 41;

    var html = '<section class="position-indicator-container"' + this.id + '>' +
        '<div class="position-indicator ' + this.id + '">' +
        '<div class="outer-ring ' + this.id + '">';
    for (var i = 0; i < strips; i++) {
        html += '<span class="tick ' + this.id + '"></span>';
    }

    html += '</div>' + '<div class="details ' + this.id + '">';

    html += '<p class="count-label ' + this.id + '">' + this.count_label + ' <span class="count"> ' + this.count + '</span></p>' +
        '<p class="label ' + this.id + '">' + this.label + '</p>' +
        '<p class="value ' + this.id + '">' + this.value + '</p>' +
        '<p class="unit ' + this.id + '">' + this.unit + '</p>' +
        '</div>' +
        '</section>';

    $('#' + this.id).append(html);

    this.addCSS();
}
/**
 * addCSS()
 * Add css transforms to the ticks
 */
PositionIndicator.prototype.addCSS = function () {
    this.ticks = $('.tick.' + this.id);
    this.details = $('.details.' + this.id);
    this.outerRingRadius = 85;

    var obj = this;
    this.ticks.each(function (i) {
        var angle = 210 - i * 6;
        var theta = deg2rad(angle);
        var radius = obj.outerRingRadius;
        var x = Math.cos(theta) * radius;
        var y = Math.sin(theta) * -radius;
        var transform = ['translate(' + x + 'px, ' + y + 'px)', 'rotate(' + -angle + 'deg)'].join(' ');
        $(this).css({
            '-webkit-transform': transform,
            '-moz-transform': transform,
            'transform': transform,
        });
    });

    this.updateStripCount();
    this.updatePosition();
}
/**
 * updateStripCount()
 * Update the strip count
 */
PositionIndicator.prototype.updateStripCount = function (count) {
    if (count)
        this.count = count;
    var obj = this;
    this.ticks.each(function (i) {
        if (i + 1 <= obj.count) {
            $(this).css({
                'border-color': '#fff'
            });
        } else {
            $(this).css({
                'border-color': '#3b3d45'
            });
        }
    });
    this.details.find('.count').text(this.count);
}
/**
 * updatePosition()
 * Update the position
 */
PositionIndicator.prototype.updatePosition = function (position) {
    if (position)
        this.value = position;
    this.details.find('.value').text(this.position);
}

/**
 * This an object initialized using class PositionIndicator
 */
var p = new PositionIndicator({
    id: 'position',
    label: 'Position',
    unit: 'm',
    value: 0,
    count: 0,
    count_label: 'Strip'
});

p.init();
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
      //sendCommand(command_name);
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
'use strict';


function SpeedGauge(parameters) {
    for(var x in parameters) {
        this[x] = parameters[x];
    }
}

SpeedGauge.prototype.initGauge = function() {
    var html = '<section class="speedometer-container"'+this.id+'>'
               + '<div class="speedometer '+this.id+'">'
               + '<div class="inner-ring '+this.id+'"></div>'
               + '<div class="outer-ring '+this.id+'">';
    for(var i =0; i<49; i++) {
        html += '<span class="tick '+this.id+'"></span>';
    }
    html += '</div>' + '<div class="digit-ring '+this.id+'">';

    for(var i=0; i<9; i++) {
        html += '<span class="digit '+this.id+'">'+(i-1)*20+'</span>';
    }
    html += '</div>' + '<div class="details '+this.id+'">';

    html += '<p class="label '+this.id+'">'+this.label+'</p>'
            + '<p class="speed '+this.id+'">'+this.value+'</p>'
            + '<p class="unit '+this.id+'">'+this.unit+'</p>'
            + '</div>'
            + '<div class="guage-progress '+this.id+'"></div>'
            + '</section>';

    $('#'+this.id).append(html);

    this.addCSS();
}
SpeedGauge.prototype.addCSS = function() {
    this.ticks = $('.tick.'+this.id);
    this.digits = $('.digit.'+this.id);
    this.details = $('.details.'+this.id);
    this.progress = $('.guage-progress.'+this.id);

    this.outerRingRadius = 85;
    this.digitRingRadius = 65;
    var obj = this;
        this.ticks.each(function (i) {
        var angle = 210 - i * 5;
        var theta = deg2rad(angle);
        var radius = obj.outerRingRadius + (i % 6 ? 0 : 4);
        var x = Math.cos(theta) * radius;
        var y = Math.sin(theta) * -radius;
        var transform = ['translate(' + x + 'px, ' + y + 'px)', 'rotate(' + -angle + 'deg)'].join(' ');
        $(this).css({
            '-webkit-transform': transform,
            '-moz-transform': transform,
            'transform': transform,
        });
    });

    this.digits.each(function (i) {
        var angle = 210 - i * 30;
        var theta = deg2rad(angle);
        var x = Math.cos(theta) * obj.digitRingRadius;
        var y = Math.sin(theta) * -obj.digitRingRadius;
        $(this).css({
            '-webkit-transform': 'translate(' + x + 'px, ' + y + 'px)',
            '-moz-transform': 'translate(' + x + 'px, ' + y + 'px)',
            'transform': 'translate(' + x + 'px, ' + y + 'px)'
        });
    });
    this.frameCount = 100;
    this.frameInterval = 0.3;
    this.digitValueMax = 160;
    this.statValueMax = this.value;
    this.statValueCurrent = 0;
    this.statValueInterval = this.statValueMax / this.frameCount;

    this.updateDetails();
}
SpeedGauge.prototype.updateDetails = function() {
    if (this.statValueCurrent.toFixed(1) > this.statValueMax) {
        return;
    }
    this.setStatValue(this.statValueCurrent.toFixed(1));
    this.statValueCurrent += this.statValueInterval;
    var obj = this;
    setTimeout(function() {
        obj.updateDetails();
    }, this.frameInterval);
}

SpeedGauge.prototype.setStatValue= function(value) {
    var angle = -120 + 240 * (value / this.digitValueMax);

    this.progress.css({
        'transform': 'rotate(' + angle + 'deg)'
    });
    this.details.find('.speed').text(value);
}

function deg2rad(angle) {
    return angle * (Math.PI / 180);
}

function rad2deg(angle) {
    return angle * (180 / Math.PI);
}

var v = new SpeedGauge({
    id: 'velocity',
    label: 'Velocity',
    unit: 'm/s',
    value: 160
});
v.initGauge();

var a = new SpeedGauge({
    id: 'acceleration',
    label: 'Acceleration',
    unit: 'm/s/s',
    value: 10
});
a.initGauge();

window.setInterval(function(){
    v.statValueCurrent = 0;
    v.statValueMax = Math.random() * (100-0);
    v.updateDetails();
}, 500000);

function update(data){
    $('#temperature').text(data.pod_temperature);
    $('#team_id').text(data.team_id);
    $('#pressure').text(data.pressure);
    for(var i=0;i<4;i++){
        $('#battery-current'+(i+1)).text(data.battery_current[i]);
        $('#batter-temperature'+(i+1)).text(data.battery_temperature[i]);
    }
    p.updateStripCount(data.stripe_count);
    p.updatePosition(data.position);
    v.setStatValue(data.velocity);
    a.setStatValue(data.acceleration);
    
    // setValueAnimated(value, durationInSecs);
    yaw.setValueAnimated(data.yaw,0.5);
    pitch.setValueAnimated(data.pitch, 0.5);
    roll.setValueAnimated(data.roll, 0.5);
    for (var i=0; i<4;i++ )
    {
      $('#lev'+(i+1)).text(data.levitation_value[i]);//Don't know how to update the values
    }
    $('#brake-pad').text(data.break_pad_distance);
    $('#pod-status').text(data.status);
    if(data.pusher_value==0){
        $('.pusher').text('Disengaged')
          .removeClass('engage')
          .addClass('disengage');
    }
    else {
       $('.pusher').text('Engaged')
        .removeClass('disengage')
        .addClass('engage');
    }
}

var socket = io.connect();
socket.on('connect', function() {
  console.log("Connected!"); 
});

socket.on('data_send', function(data) { 
  update(data);
});

function sendCommand(command_name) {
  socket.emit('pod_command', command_name);
}