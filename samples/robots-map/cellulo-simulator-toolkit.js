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
    DIRECTION: 6        ///< Point toward one direction; value is from 00 to FF where 00 is 0 degrees and FF is 358.59375 degrees
}

var commands = {"ping": /^P$/,
                "frame": /^F$/,
                "battery": /^B$/,
                "visual": /^V(\d)$/,
                "effect": /^E(\d)([0-9A-F]{6})([0-9A-F]{2})$/,
                "reset": /^R$/,
                "shutdown": /^S$/
                }

function grid2mm(x, y, playground) {
    return {x: x * playground.width / (playground.gridRight - playground.gridLeft),
            y: y * playground.height / (playground.gridBottom - playground.gridTop)}
}

function mm2grid(x, y, playground) {
    if (!playground) return {x:"", y:""};

    return {x: zeroPad(Number(x / (playground.width / (playground.gridRight - playground.gridLeft))).toFixed(0), 8),
            y: zeroPad(Number(y / (playground.height / (playground.gridBottom - playground.gridTop))).toFixed(0), 8)}
}

// Taken from: http://stackoverflow.com/questions/1267283/how-can-i-create-a-zerofilled-value-using-javascript
function zeroPad(num, numZeros) {
    var n = Math.abs(num);
    var zeros = Math.max(0, numZeros - Math.floor(n).toString().length );
    var zeroString = Math.pow(10,zeros).toString().substr(1);
    if( num < 0 ) {
        zeroString = '-' + zeroString;
    }

    return zeroString+n;
}
