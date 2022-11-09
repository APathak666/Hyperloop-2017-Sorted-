var PORT = 3000;
var HOST = '127.0.0.1';

var dgram = require('dgram');
var server = dgram.createSocket('udp4');

var team_id;
var  status;
var acceleration;
var position;
var velocity;
var battery_voltage;
var battery_current;
var battery_temperature;
var pod_temperature;
var stripe_count;

var no_data_packet =0;

var dataJSON=new Object();
dataJSON={ "team_id":'0',
            "status":'0',
            "acceleration":'0',
            "position":'0',
            "velocity":'0',
            "battery_voltage":'0',
            "battery_current":'0',
            "battery_temperature":'0',
            "pod_temperature":'0',
            "stripe_count":'0'
        };
function unpack(arr) 
    {var count = 0;
        count+=arr[0]+arr[1]*256+arr[2]*256*256+arr[3]*256*256*256;    
return count;

     }
function unpack8(arr) 
    {var count = 0;
        count+=arr[0];
        return count;    
     }


server.on('listening', function () {
    var address = server.address();
    console.log('UDP Server listening on ' + address.address + ":" + address.port);
});

server.on('message', function (message, remote) {
    //console.log(remote.address + ':' + remote.port +' - ' + message);

    var arr = Array.prototype.slice.call(message, 0,1);
                		 dataJSON.team_id=unpack8(arr);
                        arr = Array.prototype.slice.call(message, 1,2);
                         dataJSON.status=unpack8(arr);
                         arr = Array.prototype.slice.call(message, 2,6);
                        dataJSON.acceleration=unpack(arr);
                         arr = Array.prototype.slice.call(message, 6,10);
                         dataJSON.position =unpack(arr);
                        arr = Array.prototype.slice.call(message, 10,14);
                         dataJSON.velocity=unpack(arr);
                         arr = Array.prototype.slice.call(message,14,18);
                        dataJSON.battery_voltage=unpack(arr);
                         arr = Array.prototype.slice.call(message, 18,22);
                         dataJSON.battery_current=unpack(arr);
                          arr = Array.prototype.slice.call(message, 22,26);
                         dataJSON.battery_temperature=unpack(arr);
                         arr = Array.prototype.slice.call(message, 26,30);
                        dataJSON.pod_temperature=unpack(arr);
                         arr = Array.prototype.slice.call(message,30,34);
                        dataJSON.stripe_count=unpack(arr);

                        no_data_packet = no_data_packet +1;
                        //console.log(no_data_packet)
                        console.log("Data Packet No."+ no_data_packet)
                        console.log(dataJSON); 	

});

server.bind(PORT, HOST);