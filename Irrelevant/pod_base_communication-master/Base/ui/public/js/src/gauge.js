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