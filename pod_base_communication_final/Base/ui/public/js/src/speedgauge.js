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
    for(var i=0; i<49; i++) {
        html += '<span class="tick '+this.id+'"></span>';
    }
    html += '</div>' + '<div class="digit-ring '+this.id+'">';

    for(var i=1; i<=9; i++) {
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
    value: 0.1
});
v.initGauge();

var a = new SpeedGauge({
    id: 'acceleration',
    label: 'Acceleration',
    unit: 'm/s/s',
    value: 0.1
});
a.initGauge();