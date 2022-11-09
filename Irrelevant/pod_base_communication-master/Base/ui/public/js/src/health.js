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