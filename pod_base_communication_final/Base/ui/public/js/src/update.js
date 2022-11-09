var pod_state = [
  "LVElec",
  "Idle",
  "Ready",
  "AwaitPusherAttach",
  "Pushing",
  "LevAndBraking",
  "DescAndBrakeRetr",
  "Rolling",
  "LSD",
  "PodStop",
  "EmStop",
  "Fault",
  "EB0",
  "EB1",
  "EB2",
  "PowerOff"
];

function update(data){
    // comms
    if(data.comm_primary)  
      $('#primary-channel').removeClass().addClass('connected');
    else
      $('#primary-channel').removeClass();

    if(data.comm_secondary)  
      $('#secondary-channel').removeClass().addClass('connected');
    else(data.comm_secondary)  
      $('#secondary-channel').removeClass();
    
    $('#current-channel').text(data.comm_current);

    // team id
    if(data.Team_Id)
      $('#team_id').text(parseFloat(data.Team_Id));
    
    // pod-status
    if(data.Status_pod)
      $('#pod-status').text(pod_state[data.Status_pod]);

    // console
    if(data.console_entry)
      $('.console .display').append('<p class="console-entry">['+data.timestamp+'] '+data.console_entry+'</p>');
    
    
    // primary battery
    for(var i=1;i<=2;i++){
        var bat = $('#b-p'+(i));
        bat.find('.temp').text(data['PWR_Temperature_'+i]);
        bat.find('.current').text(data['PWR_Current_'+i]);
        bat.find('.voltage').text(data['PWR_Voltage_'+i]);
        //bat.find('.battery').removeClass().addClass('battery soc--'+data.battery_primary_soc[i]);
        bat.find('.battery').removeClass().addClass('battery soc--4');

      }
    // secondary battery
    /*$('#b-sec').find('.battery').removeClass().addClass('battery soc--'+data.battery_secondary_soc);
    $('#b-sec').find('.temp').text(data.battery_secondary_temperature);
    $('#b-sec').find('.current').text(data.battery_secondary_current);
    // backup battery
    $('#b-bk').find('.value').text((data.battery_backup)?'ON':'OFF');
    */   
    // temperature
    if(data.CTRL_Temperature)
      $('#temperature').text(data.CTRL_Temperature);
    
    // pressure
    if(data.CTRL_Pressure)
      $('#pressure').text(data.CTRL_Pressure);
    

    // strip count
    if(data.Nav_RR_Strip_Count)
      p.updateStripCount(data.Nav_RR_Strip_Count);
    // position
    if(data.Nav_Position)
      p.updatePosition(data.Nav_Position);
    // velocity
    if(data.Nav_Velocity)
      v.setStatValue(data.Nav_Velocity);
    // acceleration
    if(data.Nav_Acceleration)
      a.setStatValue(data.Nav_Acceleration);
    
    // yaw pitch roll
    if(data.Nav_Yaw)
      yaw.setValueAnimated(data.Nav_Yaw,0.5);
    
    if(data.Nav_Pitch)
      pitch.setValueAnimated(data.Nav_Pitch, 0.5);
    
    if(data.Nav_Roll)
      roll.setValueAnimated(data.Nav_Roll, 0.5);
    
    // levitation
    if(data.CTRL_LTS_Height_1_2)
      $('#lev1').text(data.CTRL_LTS_Height_1_2);
    
    if(data.CTRL_LTS_Height_3_4)
      $('#lev2').text(data.CTRL_LTS_Height_3_4);

    // brake pads
    if(data.Nav_LTS_Brake_1_2 && data.Nav_LTS_Brake_3_4){ 
      var avg_Brake_Pad = ( data.Nav_LTS_Brake_1_2 + data.Nav_LTS_Brake_3_4 )/2;
      $('#brake-pad').text(avg_Brake_Pad);
    }
   
    //pusher
    /*if(!data.pusher || data.pusher==0){
        $('.pusher').text('Disengaged')
          .removeClass('engage')
          .addClass('disengage');
    }
    else {
       $('.pusher').text('Engaged')
        .removeClass('disengage')
        .addClass('engage');
    }*/
}

var socket = io.connect();
socket.on('connect', function() {
  console.log("Connected!"); 
});

socket.on('data_send', function(data) { 
  update(data);
  DATA = data;
});

function sendCommand(command_name) {
  socket.emit('pod_command', command_name);
}