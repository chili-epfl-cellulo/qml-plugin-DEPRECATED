.pragma library

var BatteryEnum = {
    DISCHARGING: 0,      ///< No charger present, battery draining
    LOW: 1,                  ///< No charger present, battery low
    CHARGING: 2,             ///< Charger present, battery charging
    CHARGED: 3,              ///< Charger present, battery full
    SHUTDOWN: 4,             ///< Charger charging disabled, voltage too low, or battery not present
    ERROR: 5,                ///< Thermal fault or charge timeout
    INVALID: 6               ///< Invalid state
}

var VisualStateEnum = {
    RESPONSIVE: 0,    //LEDs respond to touches
    ABSOLUTE: 1          //LEDs do not respond to touches
}

var VisualEffectEnum = {
    CONST_ALL: 0,       ///< Constant color for all LEDS
    CONST_SINGLE: 1,     ///< Constant color for single LED, doesn't modify other LEDs
    ALERT_ALL: 2,        ///< Alert flashing in all LEDs, goes back to previous effect when done
    ALERT_SINGLE: 3,    ///< Alert flashing in one LED, goes back to previous effect when done
    PROGRESS: 4,         ///< Indicates a static percentage/progress circularly, needs 2 digit hex progress value (out of FF)
    WAITING: 5,          ///< Circular waiting/processing animation
    DIRECTION: 6,        ///< Point toward one direction; value is from 00 to FF where 00 is 0 degrees and FF is 358.59375 degrees
    BLINK: 7,            ///< Alert flashing in all LEDs forever; value is from 00 to FF where 20*value is the LED on time in milliseconds
    BREATHE: 8,          ///< Breathing animation, given brightness is used as the highest brightness
    PULSE: 9            ///< A slower breathe-like animation
}


var commands = {"ack": /^A$/,
                "rebootcomplete": /^O$/,
                "wakeupcomplete": /^H$/,
                "goingtosleep": /^D$/,
                "goingtosleepbattery": /^W$/,
                "battery": /^B(\d)$/,
                "touch": /^T(\d)$/,
                "longtouch": /^L(\d)$/,
                "released": /^R(\d)$/,
                "pose": /^PX(\d+)Y(\d+)R(\d+)$/,
                "kidnapped": /^K$/
                }


// Check if a point lies inside a (possibly concave) polygon.
//
// See algo: http://en.wikipedia.org/wiki/Point_in_polygon
//
// Implementation based on:
//+ Jonas Raoni Soares Silva
//@ http://jsfromhell.com/math/is-point-in-poly [rev. #0]
function isPointInPoly(poly, pt){
    for(var c = false, i = -1, l = poly.length, j = l - 1; ++i < l; j = i)
        ((poly[i].y <= pt.y && pt.y < poly[j].y) || (poly[j].y <= pt.y && pt.y < poly[i].y))
        && (pt.x < (poly[j].x - poly[i].x) * (pt.y - poly[i].y) / (poly[j].y - poly[i].y) + poly[i].x)
        && (c = !c);
    return c;
}

function polyCenter(poly){

    var l = poly.length;
    var x = 0;
    var y = 0;

    for(var i =0;  i < l; i++){
        x+=poly[i].x;
        y+=poly[i].y;
    }

    return {x: x/l, y:y/l};

}

function distance(p1, p2) {
    return Math.sqrt(Math.pow(p1.x-p2.x,2) + Math.pow(p1.y-p2.y,2));
}

function grid2mm(x, y, playground) {
    return {x: x * playground.width / (playground.gridRight - playground.gridLeft),
            y: y * playground.height / (playground.gridBottom - playground.gridTop)}
}

function shuffle(array) {
    var rndarray = array.slice(0); //deep copy the array
    for (var i = array.length - 1; i > 0; i--) {
        var j = Math.floor(Math.random() * (i + 1));
        var temp = rndarray[i];
        rndarray[i] = rndarray[j];
        rndarray[j] = temp;
    }
    return rndarray;
}

function formatColor(color) {
    var hr = Math.round(color.r * 255).toString(16)
    hr = hr.length == 1 ? "0" + hr:hr
    var hg = Math.round(color.g * 255).toString(16)
    hg = hg.length == 1 ? "0" + hg:hg
    var hb = Math.round(color.b * 255).toString(16)
    hb = hb.length == 1 ? "0" + hb:hb

    return (hr+hg+hb).toUpperCase();
}

function partial(fn /*, args...*/) {
  // A reference to the Array#slice method.
  var slice = Array.prototype.slice;
  // Convert arguments object to an array, removing the first argument.
  var args = slice.call(arguments, 1);

  return function() {
    // Invoke the originally-specified function, passing in all originally-
    // specified arguments, followed by any just-specified arguments.
    return fn.apply(this, args.concat(slice.call(arguments, 0)));
  };
}

// attach the .equals method to Array's prototype to call it on any array
Array.prototype.equals = function (array) {
    // if the other array is a falsy value, return
    if (!array)
        return false;

    // compare lengths - can save a lot of time
    if (this.length != array.length)
        return false;

    for (var i = 0, l=this.length; i < l; i++) {
        // Check if we have nested arrays
        if (this[i] instanceof Array && array[i] instanceof Array) {
            // recurse into the nested arrays
            if (!this[i].equals(array[i]))
                return false;
        }
        else if (this[i] != array[i]) {
            // Warning - two different object instances will never be equal: {x:20} != {x:20}
            return false;
        }
    }
    return true;
}

var HEATMAP = [
          "#0080FF",
          "#178BE7",
          "#2E97D0",
          "#45A2B9",
          "#5CAEA2",
          "#73B98B",
          "#8BC573",
          "#A2D05C",
          "#B9DC45",
          "#D0E72E",
          "#E7F317",
          "#FFFF00",
          "#FFF300",
          "#FFE700",
          "#FFDC00",
          "#FFD000",
          "#FFC500",
          "#FFB900",
          "#FFAE00",
          "#FFA200",
          "#FF9700",
          "#FF8B00",
          "#FF8000",
          "#FF7000",
          "#FF6000",
          "#FF5000",
          "#FF4000",
          "#FF3000",
          "#FF2000",
          "#FF1000",
          "#FF0000"
    ]

var HEATMAP_BLUE_VIOLET = [
    "#0000FF",
    "#0C00F8",
    "#1800F2",
    "#2400EC",
    "#3000E6",
    "#3C00E0",
    "#4800DA",
    "#5500D4",
    "#6100CE",
    "#6D00C8",
    "#7900C2",
    "#8500BC",
    "#9100B6",
    "#9D00B0",
    "#AA00AA",
    "#B600A4",
    "#C2009E",
    "#CE0098",
    "#DA0092",
    "#E6008C",
    "#F20086",
    "#FF0080"
]

function colorFromRamp(alpha, ramp) {
    var idx = Math.round(alpha * ramp.length);
    if (idx === ramp.length) idx = ramp.length - 1;
    return ramp[idx];
}
