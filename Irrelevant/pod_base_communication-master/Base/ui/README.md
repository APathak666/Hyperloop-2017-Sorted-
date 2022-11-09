# Base Station UI

## How the build works 

We are using Gulp to run the build tasks.  CSS and JS files are compiled from multiple source files and minified. 

To run the build tasks, run 

    gulp 

from the `/ui/` folder

## Directory Structure 

    /ui/
        base.html               HTML for the UI
        gulpfile.js             Contains build tasks for gulp
        index.html              Basic UI
        server_forpy.js         The nodeJS server

    ui/public/                  Contains all the static assets for ui - css, images and client js
    
    ui/public/img/              Images used in the site
   
    ui/public/css/              Compiled CSS. DO NOT EDIT
    ui/public/scss/             Source SCSS files. Edit these to make css changes.

    ui/public/js/
        script.js               Compiled JS. DO NOT EDIT
        script.min.js           Compiled + Minified. DO NOT EDIT
        script.min.js.map       Mapfile generated during compilation. DO NOT EDIT
    
    ui/public/js/src/           Source JS Files. Edit these to make js changes
        buttons.js              For Buttons
        gauge.js                Yaw Pitch Roll gauges
        health.js               Healthcheck UI
        layout.js               Inserts SVG images for levitation and braking
        position.js             Code for Position and Stripcount Indicator
        speedgauge.js           Accleration and Velocity gauges
        update.js               Handles sockets and updation


## Instructions for Editing javasctipt files  

The `public/js/` folder has all the javascript files used for the UI. scripts.js and scripts.min.js files are compiled by gulp. Do not edit those. 

`public/js/src` contains the actual source files that should be edited. 

#### DO NOT MAKE CHANGES TO **scripts.js** OR **scripts.min.js** FOUND IN THE `public/js` FOLDER.

One file -  `public/js/src/update.js` - handles the connection to server over socket.io. All the other files are handle UI functionality.


