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
        $('.pusher').removeClass('engage');
        $('.pusher').addClass('disengage');
    }
    else {
        $('.pusher').removeClass('disengage');
        $('.pusher').addClass('engage');
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