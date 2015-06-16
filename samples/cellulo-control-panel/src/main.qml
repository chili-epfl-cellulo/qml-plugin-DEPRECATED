import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.3
import QtBluetooth 5.2
import Cellulo 1.0

ApplicationWindow {
    id: window
    visible: true

    GroupBox {
        id: addressBox
        title: "Robot Address"

        Row{
            spacing: 5

            Label{
                text: "00:06:66:74:"
                anchors.verticalCenter: macAddrRight.verticalCenter
            }
            TextField{
                id: macAddrRight
                placeholderText: "XX:XX"
            }
            Button {
                text: "Connect"
                onClicked: robotComm.macAddr =  "00:06:66:74:" + macAddrRight.text;
            }
        }
    }

    GroupBox {
        id: colorBox
        title: "Color Effects"
        anchors.top: addressBox.bottom

        Row{
            spacing: 5

            ComboBox {
                id: effect
                width: 200
                model: [
                    "CONST_ALL",
                    "CONST_SINGLE",
                    "ALERT_ALL",
                    "ALERT_SINGLE",
                    "PROGRESS",
                    "WAITING",
                    "DIRECTION",
                    "BLINK",
                    "BREATHE",
                    "PULSE"
                ]
                currentIndex: 0
            }
            Column{
                Slider{
                    id: redSlider
                    minimumValue: 0
                    maximumValue: 255
                    stepSize: 1
                    value: 128
                    style: SliderStyle {
                        groove: Rectangle {
                            implicitWidth: 200
                            implicitHeight: 8
                            color: "red"
                            radius: 8
                        }
                    }
                }
                Slider{
                    id: greenSlider
                    minimumValue: 0
                    maximumValue: 255
                    stepSize: 1
                    value: 128
                    style: SliderStyle {
                        groove: Rectangle {
                            implicitWidth: 200
                            implicitHeight: 8
                            color: "green"
                            radius: 8
                        }
                    }
                }
                Slider{
                    id: blueSlider
                    minimumValue: 0
                    maximumValue: 255
                    stepSize: 1
                    value: 128
                    style: SliderStyle {
                        groove: Rectangle {
                            implicitWidth: 200
                            implicitHeight: 8
                            color: "blue"
                            radius: 8
                        }
                    }
                }
            }
            Label{
                text: "Value:"
                anchors.verticalCenter: effectValue.verticalCenter
            }
            SpinBox {
                id: effectValue
                minimumValue: 0
                maximumValue: 255
                value: 0
            }
            Button {
                text: "Send"
                onClicked:robotComm.setVisualEffect(effect.currentIndex, "#FF"
                                                    + (redSlider.value < 16 ? "0" : "") + redSlider.value.toString(16).toUpperCase()
                                                    + (greenSlider.value < 16 ? "0" : "") + greenSlider.value.toString(16).toUpperCase()
                                                    + (blueSlider.value < 16 ? "0" : "") + blueSlider.value.toString(16).toUpperCase(),
                                                    effectValue.value);
            }
        }
    }

    GroupBox {
        id: powerBox
        title: "Power"
        anchors.top: colorBox.bottom
        Row{
            spacing: 5

            Button{
                text: "Reset"
                onClicked: robotComm.reset();
            }
            Button{
                text: "Shutdown"
                onClicked: robotComm.shutdown();
            }
            Button {
                text: "Query Battery State"
                onClicked: robotComm.queryBatteryState();
            }
        }
    }

    GroupBox {
        id: statusBox
        title: "Status"
        anchors.top: powerBox.bottom
        Row{
            spacing: 5
            Text{
                text: "Connected?"
                color: robotComm.connected ? "green" : "red"
            }
            Text{
                text: "Battery State: " + robotComm.batteryState
            }
            Text{
                id: k0
                text: "K0"
                color: "black"
            }
            Text{
                id: k1
                text: "K1"
                color: "black"
            }
            Text{
                id: k2
                text: "K2"
                color: "black"
            }
            Text{
                id: k3
                text: "K3"
                color: "black"
            }
            Text{
                id: k4
                text: "K4"
                color: "black"
            }
            Text{
                id: k5
                text: "K5"
                color: "black"
            }
            Text{
                text: "Kidnapped?"
                color: robotComm.kidnapped ? "red" : "green"
            }
            Text{
                text: "X=" + robotComm.x + " Y=" + robotComm.y + " Theta=" + robotComm.theta
            }
        }
    }

    CelluloBluetooth{
        id: robotComm
        onTouchBegan:{
            switch(key){
            case 0: k0.color = "yellow"; break;
            case 1: k1.color = "yellow"; break;
            case 2: k2.color = "yellow"; break;
            case 3: k3.color = "yellow"; break;
            case 4: k4.color = "yellow"; break;
            case 5: k5.color = "yellow"; break;
            }
        }
        onLongTouch:{
            switch(key){
            case 0: k0.color = "green"; break;
            case 1: k1.color = "green"; break;
            case 2: k2.color = "green"; break;
            case 3: k3.color = "green"; break;
            case 4: k4.color = "green"; break;
            case 5: k5.color = "green"; break;
            }
        }
        onTouchReleased:{
            switch(key){
            case 0: k0.color = "black"; break;
            case 1: k1.color = "black"; break;
            case 2: k2.color = "black"; break;
            case 3: k3.color = "black"; break;
            case 4: k4.color = "black"; break;
            case 5: k5.color = "black"; break;
            }
        }
    }
}
