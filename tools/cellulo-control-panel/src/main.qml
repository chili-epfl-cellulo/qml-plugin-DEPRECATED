import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Controls.Styles 1.3
import Cellulo 1.0

ApplicationWindow {
    id: window
    visible: true

    function em(x){ return Math.round(x*TextSingleton.font.pixelSize); }

    property bool mobile: Qt.platform.os === "android"
    property real gWidth: mobile ? Screen.width : 640
    width: gWidth
    height: mobile ? Screen.desktopAvailableHeight : itemsCol.height

    ScrollView {
        anchors.fill: parent
        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        verticalScrollBarPolicy: mobile ? Qt.ScrollBarAsNeeded : Qt.ScrollBarAlwaysOff

        Column{
            id: itemsCol

            GroupBox {
                id: addressBox
                title: "Robot Address"
                width: gWidth

                MacAddrSelector{
                    addresses: [
                        "00:06:66:74:40:D2",
                        "00:06:66:74:40:D4",
                        "00:06:66:74:40:D5",
                        "00:06:66:74:40:DB",
                        "00:06:66:74:40:DC",
                        "00:06:66:74:40:E4",
                        "00:06:66:74:40:EC",
                        "00:06:66:74:40:EE",
                        "00:06:66:74:41:04",
                        "00:06:66:74:41:14",
                        "00:06:66:74:41:4C",
                        "00:06:66:74:43:00",
                        "00:06:66:74:46:58",
                        "00:06:66:74:46:60",
                        "00:06:66:74:48:A7"
                    ]
                    onConnectRequested: robotComm.macAddr = selectedAddress
                    onDisconnectRequested: robotComm.disconnectFromServer()
                    connectionStatus: robotComm.connectionStatus
                }
            }

            GroupBox {
                id: colorBox
                title: "Color Effects"
                width: gWidth

                RowLayout{
                    anchors.left: parent.left
                    anchors.right: parent.right
                    spacing: 5

                    ComboBox {
                        id: effect
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
                        Layout.fillWidth: true
                        Slider{
                            id: redSlider
                            anchors.left: parent.left
                            anchors.right: parent.right
                            minimumValue: 0
                            maximumValue: 255
                            stepSize: 1
                            value: 128
                            style: SliderStyle {
                                groove: Rectangle {
                                    implicitHeight: 8
                                    color: "red"
                                }
                            }
                        }
                        Slider{
                            id: greenSlider
                            anchors.left: parent.left
                            anchors.right: parent.right
                            minimumValue: 0
                            maximumValue: 255
                            stepSize: 1
                            value: 128
                            style: SliderStyle {
                                groove: Rectangle {
                                    implicitHeight: 8
                                    color: "green"
                                }
                            }
                        }
                        Slider{
                            id: blueSlider
                            anchors.left: parent.left
                            anchors.right: parent.right
                            minimumValue: 0
                            maximumValue: 255
                            stepSize: 1
                            value: 128
                            style: SliderStyle {
                                groove: Rectangle {
                                    implicitHeight: 8
                                    color: "blue"
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
                        width: em(3)
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
                width: gWidth

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
                id: motorBox
                title: "Locomotion"
                width: gWidth

                Column{
                    anchors.fill: parent

                    RowLayout{
                        spacing: 5
                        anchors.left: parent.left
                        anchors.right: parent.right

                        Label{
                            text: "Motor 1 output:"
                        }
                        Slider{
                            id: motor1Slider
                            Layout.fillWidth: true
                            minimumValue: -0xFFF
                            maximumValue: 0xFFF
                            stepSize: 1
                            value: 0
                            style: SliderStyle {
                                groove: Rectangle {
                                    implicitHeight: 8
                                }
                            }
                            onValueChanged: robotComm.setMotor1Output(value)
                        }
                        Button{
                            text: "Zero"
                            onClicked: motor1Slider.value = 0
                        }
                    }

                    RowLayout{
                        spacing: 5
                        anchors.left: parent.left
                        anchors.right: parent.right

                        Label{
                            text: "Motor 2 output:"
                        }
                        Slider{
                            id: motor2Slider
                            Layout.fillWidth: true
                            minimumValue: -0xFFF
                            maximumValue: 0xFFF
                            stepSize: 1
                            value: 0
                            style: SliderStyle {
                                groove: Rectangle {
                                    implicitHeight: 8
                                }
                            }
                            onValueChanged: robotComm.setMotor2Output(value)
                        }
                        Button{
                            text: "Zero"
                            onClicked: motor2Slider.value = 0
                        }
                    }

                    RowLayout{
                        spacing: 5
                        anchors.left: parent.left
                        anchors.right: parent.right

                        Label{
                            text: "Motor 3 output:"
                        }
                        Slider{
                            id: motor3Slider
                            Layout.fillWidth: true
                            minimumValue: -0xFFF
                            maximumValue: 0xFFF
                            stepSize: 1
                            value: 0
                            style: SliderStyle {
                                groove: Rectangle {
                                    implicitHeight: 8
                                }
                            }
                            onValueChanged: robotComm.setMotor3Output(value)
                        }
                        Button{
                            text: "Zero"
                            onClicked: motor3Slider.value = 0
                        }
                    }

                    Row{
                        spacing: 5

                        Column{
                            Label{
                                text: "Pose goal:"
                            }

                            TextField{
                                id: goalPoseX
                                placeholderText: "x (grid coords)"
                            }

                            TextField{
                                id: goalPoseY
                                placeholderText: "y (grid coords)"

                            }

                            TextField{
                                id: goalPoseTheta
                                placeholderText: "theta (degrees)"

                            }

                            TextField{
                                id: goalPoseMaxV
                                placeholderText: "v (mm/s)"

                            }

                            TextField{
                                id: goalPoseMaxW
                                placeholderText: "w (rad/s)"

                            }

                            Button{
                                text: "Go"
                                onClicked: robotComm.setGoalPose(parseFloat(goalPoseX.text), parseFloat(goalPoseY.text), parseFloat(goalPoseTheta.text),
                                                                 parseFloat(goalPoseMaxV.text), parseFloat(goalPoseMaxW.text));
                            }
                        }

                        Column{
                            Label{
                                text: "Position goal:"
                            }

                            TextField{
                                id: goalPositionX
                                placeholderText: "x (grid coords)"
                            }

                            TextField{
                                id: goalPositionY
                                placeholderText: "y (grid coords)"

                            }

                            TextField{
                                id: goalPositionMaxV
                                placeholderText: "v (mm/s)"

                            }

                            Button{
                                text: "Go"
                                onClicked: robotComm.setGoalPosition(parseFloat(goalPositionX.text), parseFloat(goalPositionY.text), parseFloat(goalPositionMaxV.text));
                            }
                        }


                        Column{
                            Label{
                                text: "Goal velocity:"
                            }

                            TextField{
                                id: goalVelX
                                placeholderText: "vx (mm/s)"
                            }

                            TextField{
                                id: goalVelY
                                placeholderText: "vy (mm/s)"

                            }

                            TextField{
                                id: goalW
                                placeholderText: "w (rad/s)"

                            }

                            Button{
                                text: "Set"
                                onClicked: robotComm.setGoalVelocity(parseFloat(goalVelX.text), parseFloat(goalVelY.text), parseFloat(goalW.text));
                            }
                        }
                    }
                }
            }

            GroupBox {
                id: statusBox
                title: "Status"
                width: gWidth

                Column{
                    spacing: 5

                    Row{
                        spacing: 5

                        Text{
                            text: "Battery State: " + robotComm.batteryState.toString()
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
                    }
                    Row{
                        spacing: 5

                        Text{
                            text: "Kidnapped?"
                            color: robotComm.kidnapped ? "red" : "green"
                        }
                        Text{
                            text: "X=" + robotComm.x.toFixed(2) + " Y=" + robotComm.y.toFixed(2) + " Theta=" + robotComm.theta.toFixed(1)
                        }
                    }
                }
            }

            GroupBox{
                id: profilingBox
                title: "Profiling"
                width: gWidth

                Column{
                    spacing: 5

                    Row{
                        CheckBox{
                            id: timestampingCheckbox
                            text: "Get onboard timestamps"
                        }
                    }

                    Row{
                        Text{
                            text: "Framerate: " + robotComm.framerate.toFixed(2) + " fps"
                        }
                    }

                    Row{
                        Text{
                            text: "Last timestamp: " + robotComm.lastTimestamp + " ms"
                        }
                    }

                    Row{
                        Text{
                            text: "Pose broadcast period:"
                        }
                        TextField{
                            id: poseBcastPeriodField
                            placeholderText: "(ms)"
                            width: em(5)
                        }
                        Button{
                            text: "Set"
                            onClicked: robotComm.setPoseBcastPeriod(parseFloat(poseBcastPeriodField.text))
                        }
                    }
                }
            }

            GroupBox{
                id: cameraImageBox
                title: "Camera image"
                width: gWidth

                Row{
                    spacing: 5

                    Image{
                        id: cameraImage
                        source: "image://cameraFrame/"
                        cache: false
                        function reload() {
                            var oldSource = source;
                            source = "";
                            source = oldSource;
                        }

                        fillMode: Image.PreserveAspectFit
                        width: gWidth*0.4 - 10
                        height: width/188*120
                    }

                    Column{
                        spacing: 5

                        Button{
                            text: "Grab one frame"
                            onClicked: robotComm.requestFrame();
                        }

                        ProgressBar{
                            value: robotComm.cameraImageProgress
                            width: gWidth*0.6 - 10
                        }
                    }
                }
            }
        }
    }

    CelluloBluetooth{
        id: robotComm
        timestampingEnabled: timestampingCheckbox.checked
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
        onFrameReady: cameraImage.reload()
    }
}
