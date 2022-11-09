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