var net = require('net');
var fs = require('fs');
var formidable = require("formidable");
var io = require('socket.io');

//var io = require('socket.io');



// Backend Core Functionality
var team_id;
var stat;
var acceleration;
var position;
var velocity;
var battery_voltage;
var battery_current;
var battery_temperature;
var pod_temperature;
var stripe_count;

var no_data_packet = 0;

var input_base = "NULL";

var dataJSON = new Object();
dataJSON = {
    "no_data_packets" : 0,
    "Team_Id":'0',
    "Status_pod":'0',
    "Nav_Acceleration":'0',
    "Nav_Yaw":'0',
    "Nav_Pitch":'0',
    "Nav_Roll":'0',
    "Nav_Position":'0',
    "Nav_Velocity":'0',
    "Nav_LTS_Brake_1_2":'0',
    "Nav_LTS_Brake_3_4":'0',
    "Nav_RR_Strip_Count":'0',
    "PWR_Voltage_1":'0',
    "PWR_Current_1":'0',
    "PWR_Temperature_1":'0',
    "PWR_Voltage_2":'0',
    "PWR_Current_2":'0',
    "PWR_Temperature_2":'0',
    "CTRL_Temperature":'0',
    "CTRL_Temperature":'0',
    "CTRL_Pressure":'0',
    "CTRL_LTS_Height_1_2":'0',
    "CTRL_LTS_Height_3_4":'0'
};

function unpack(arr) {
    var count = 0;
    count += arr[0] + arr[1] * 256 + arr[2] * 256 * 256 + arr[3] * 256 * 256 * 256;
    return count;

}

function unpack8(arr) {
    var count = 0;
    count += arr[0];
    return count;
}



var server = net.createServer(function (socket) {
    // socket.setEncoding('utf8');
    socket.on('error', function () {
        console.log("error occured");
    });
    socket.on('end', function () {
        console.log("ended");
    });


    socket.on('data', function (data) {
        //console.dir(data);
        //console.log(data);
        dataJSON['no_data_packets'] = dataJSON['no_data_packets'] + 1;
        var arr = Array.prototype.slice.call(data, 0, 4);
        dataJSON.Team_Id = unpack(arr);
        arr = Array.prototype.slice.call(data, 4, 8);
        dataJSON.Status_pod = unpack(arr);
        //console.log(dataJSON.Status_pod);
        arr = Array.prototype.slice.call(data, 8, 12);
        dataJSON.Nav_Acceleration = unpack(arr);
        arr = Array.prototype.slice.call(data, 12, 16);
        dataJSON.Nav_Yaw = unpack(arr);
        arr = Array.prototype.slice.call(data, 16, 20);
        dataJSON.Nav_Pitch = unpack(arr);
        arr = Array.prototype.slice.call(data, 20, 24);
        dataJSON.Nav_Roll = unpack(arr);
        arr = Array.prototype.slice.call(data, 24, 28);
        dataJSON.Nav_Position = unpack(arr);
        arr = Array.prototype.slice.call(data, 28, 32);
        dataJSON.Nav_Velocity = unpack(arr);
        arr = Array.prototype.slice.call(data, 32, 36);
        dataJSON.Nav_LTS_Brake_1_2 = unpack(arr);
        arr = Array.prototype.slice.call(data, 36, 40);
        dataJSON.Nav_LTS_Brake_3_4 = unpack(arr);
        arr = Array.prototype.slice.call(data, 40, 44);
        dataJSON.Nav_RR_Strip_Count = unpack(arr);
        arr = Array.prototype.slice.call(data, 44, 48);
        dataJSON.PWR_Voltage_1 = unpack(arr);
        arr = Array.prototype.slice.call(data, 48, 52);
        dataJSON.PWR_Current_1 = unpack(arr);
        arr = Array.prototype.slice.call(data, 52, 56);
        dataJSON.PWR_Temperature_2 = unpack(arr);
        arr = Array.prototype.slice.call(data, 56, 60);
        dataJSON.PWR_Voltage_2 = unpack(arr);
        arr = Array.prototype.slice.call(data, 60, 64);
        dataJSON.PWR_Current_2 = unpack(arr);
        arr = Array.prototype.slice.call(data, 64, 68);
        dataJSON.PWR_Temperature_2 = unpack(arr);
        arr = Array.prototype.slice.call(data, 68, 72);
        dataJSON.CTRL_Temperature = unpack(arr);
        arr = Array.prototype.slice.call(data, 72, 76);
        dataJSON.CTRL_Pressure = unpack(arr);
        arr = Array.prototype.slice.call(data, 76, 80);
        dataJSON.CTRL_LTS_Height_1_2 = unpack(arr);
        arr = Array.prototype.slice.call(data, 80, 84);
        dataJSON.CTRL_LTS_Height_3_4 = unpack(arr);

        no_data_packet = no_data_packet + 1;
        //console.log(no_data_packet)
        //console.log("Data Packet No."+ no_data_packet)
        //console.log(dataJSON);  

        //   app.get('/data.html', browserdisp );
    });
    //socket.write("input_base");

    //console.log(input_base);

    setInterval(function () {
        if (input_base !== 'NULL') {
            socket.write(input_base);
            input_base = 'NULL'
            console.log(input_base);
        } else {

            socket.write("NULL");

        }
    }, 0);

});
/*var data_to_pod = {"Test1":"1","Test2":"2"};
var listener = io.listen(server);
listener.sockets.on('connection',function(socket_to_pod){

   setInterval(function(){
    socket_to_pod.emit('data_send',data_to_pod);
    console.log(typeof data_to_pod.Test1);
   },0); 


});*/



server.listen(3000, function () {
    var address = server.address().port;
    console.log("server is listening at " + address);
    //192.168.0.144
});

//Frontend - Webapp
var http = require("http"),
    express = require('express'),
    bodyParser = require('body-parser'),
    path = require('path'); // Also loading the path module

var app = express();

app.use(bodyParser.urlencoded({
    extended: true
}));

app.use(bodyParser.json());

// The static middleware

// ui dependencies 
app.use('/jquery',express.static('node_modules/jquery/dist/'));
app.use('/jquery-ui',express.static('node_modules/jquery-ui-dist/'));
app.use('/materialize',express.static('node_modules/materialize-css/dist/'));
app.use('/svg-gauge',express.static('node_modules/svg-gauge/dist/'));
app.use('/material-icons',express.static('node_modules/material-design-icons/iconfont/'));
app.use('/gsap',express.static('node_modules/gsap/src/minified'));

// All other static assets like images and javascript files 
app.use(express.static(path.join(__dirname, '/ui/public')));

var server1 = http.createServer(app);

app.get('/', function (req, res) {

    res.send("Base Station!");

});

app.get('/index.html', function (req, res) {


    fs.readFile(__dirname + "/index.html", function (error, data) {
        res.writeHead(200, {
            "Content-Type": "text/html"
        });
        res.write(data, "utf8");
        res.end();
    });

});

app.get('/base.html', function (req, res) {


    fs.readFile(__dirname + "/ui/base.html", function (error, data) {
        res.writeHead(200, {
            "Content-Type": "text/html"
        });
        res.write(data, "utf8");
        res.end();
    });

});

app.post('/index.html', function (request, response) {

    //console.log("Got Post!!");
    console.log('COMMAND: ' + request.body.name);
    input_base = 'Y' + request.body.name + 'Z';
    response.end('Thanks');
});

var listener = io.listen(server1);
listener.sockets.on('connection', function (socket) {

    socket.on('join', function (data_client) {
        console.log(data_client);
        console.log("Client Connected!!")
    });
    socket.on('pod_command', function (command_name) {
        input_base = 'Y' + command_name; 
        console.log(command_name);
    });
    setInterval(function () {
        socket.emit('data_send', dataJSON);
    },5000);

});

server1.listen(8081, "0.0.0.0");
console.log("Server listening at 8081");