var all_checks = [ 
  "Computer Mode Operational",
  "Telemetry Stream Active",
  "Emergency brakes - disengaged",
  "UI reading of Pod relay state",
  "Battery packs sufficiently charged.",
  "UI reading of Battery temperature",
  "Brake pads are still at 32 mm",
  "Levitation module is at 45 mm",
  "Zero acceleration in all axes",
  "Zero velocity in all axes",
  "External pressure is 860Pa",
  "Nominal temperatures",
  "SpaceX Pusher Status - Ready"
];

var checks = [ 
  "Telemetry Stream Active",
  "Battery packs sufficiently charged.",
  "UI reading of Battery temperature",
  "Brake pads are still at 32 mm",
  "Levitation module is at 45 mm",
  "Zero acceleration in all axes",
  "Zero velocity in all axes",
  "External pressure is 860Pa",
  "Nominal temperatures",
];

// constants 
var MIN_VOLTAGE = 0.1;
var BRAKE_PAD = 32;
var LEVITATION = 45;
var MAX_PRESSURE = 860;
var NOM_TEMP = 30;  

for(var i in checks) {
  // append each check name into the health display
  $('#health-display').append('<div class="check-item" id="chk'+i+'"><i class="status material-icons">cached</i><span class="check-item-name">'+checks[i]+'</span></div>');
}

$('#start-check').click(function() {
 // $('.health .collapsible').collapsible('open', 0);
  setTimeout(function(){startCheck()},400);
});
var count;
  
function startCheck() {
  var data = DATA;
  markCheck(0, (data)?true:false) ;
  
  markCheck(1, (data.PWR_Voltage_1 && data.PWR_Voltage_2 && data.PWR_Voltage_1 > MIN_VOLTAGE && data.PWR_Voltage_2 > MIN_VOLTAGE)?true:false);
  
  markCheck(2, (data.PWR_Temperature_1 && data.PWR_Temperature_2)?true:false);

  markCheck(3, (data.NAV_LTS_Brake_1_2 && data.NAV_LTS_Brake_3_4 && data.NAV_LTS_Brake_1_2>BRAKE_PAD && data.NAV_LTS_Brake_3_4 > BRAKE_PAD)?true:false);
  
  markCheck(4, (data.CTRL_LTS_Height_1_2 && data.CTRL_LTS_Height_3_4 && data.CTRL_LTS_Height_1_2>LEVITATION && data.CTRL_LTS_Height_3_4 > LEVITATION)?true:false);
  
  markCheck(5, (data.Nav_Acceleration > 0)?true:false);
  
  markCheck(6, (data.Nav_Velocity > 0)?true:false);
  
  markCheck(7, (data.CTRL_Pressure > MAX_PRESSURE)?true:false);
  
  markCheck(8, (data.CTRL_Temperature > NOM_TEMP)?true:false);
  
  if(count == checks.length) {
    $('#health-display').append('<p  class="check-item">All Health Checks Passed. Pod is ready for Launch! </p>');
  }
}

function markCheck(checkno,stat) {
  stat?(count++):0;
  $('#chk'+checkno+' .status')
    .html(stat?'check':'close')
    .css({
      'color': stat?'green':'red',
    });
}