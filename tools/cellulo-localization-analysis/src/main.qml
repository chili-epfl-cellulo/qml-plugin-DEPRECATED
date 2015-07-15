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
    minimumHeight: height
    minimumWidth: width

    property real numSamples: 100
    property variant coords: []
    property bool coordsReady: false
    property bool collecting: false
    property real xMean: 0
    property real yMean: 0
    property real xStdev: 1
    property real yStdev: 1
    property real xRobotZero: 0
    property real yRobotZero: 0
    property real xCNC: xCNCField.value
    property real yCNC: yCNCField.value

    Component.onCompleted: {
        var temp = new Array(0);
        for(var i=0;i<numSamples;i++)
            temp.push(Qt.vector2d(0,0));
        coords = temp;
    }

    Column{
        spacing: 5

        Text{
            text: "Connected?"
            color: robotComm.connected ? "green" : "red"
        }

        Row{
            spacing: 5

            Button {
                text: "Zero robot coords"
                onClicked: {
                    if(coordsReady){
                        xRobotZero = xMean;
                        yRobotZero = yMean;
                    }
                }
            }

            TextField{
                id: xRobotZeroField
                text: xRobotZero
                readOnly: true
                anchors.verticalCenter: parent.verticalCenter
            }

            TextField{
                id: yRobotZeroField
                text: yRobotZero
                readOnly: true
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Row{
            spacing: 5

            Text{
                text: "CNC Coordinates:"
                anchors.verticalCenter: parent.verticalCenter
            }

            SpinBox{
                id: xCNCField
                value: 0
                stepSize: 1
                minimumValue: 0
                maximumValue: 300
                anchors.verticalCenter: parent.verticalCenter
            }

            SpinBox{
                id: yCNCField
                value: 0
                stepSize: 1
                minimumValue: 0
                maximumValue: 300
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Row{
            spacing: 5

            Button {
                text: "Measure"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    ready.color = "red";
                    coordsReady = false;
                    collecting = true;
                }
            }

            CheckBox{
                id: autoincrementCNCY
                anchors.verticalCenter: parent.verticalCenter
                checked: false
                text: "Autoincrement CNC y after measurement"
            }
        }

        Text{
            id: ready
            text: "Ready?"
            color: "red"
        }

        Text{
            text: robotComm.x + " " + robotComm.y
        }
    }

    CelluloBluetooth{
        property real gridSpacing: 0.508
        property real currentIndex: 0
        property real collectStartIndex: -1

        id: robotComm
        macAddr: "00:06:66:74:48:A7"

        onPoseChanged: {
            coords[currentIndex] = Qt.vector2d(x*gridSpacing, y*gridSpacing);

            if(collecting){
                if(collectStartIndex < 0)
                    collectStartIndex = currentIndex;

                if((currentIndex + 1) % numSamples == collectStartIndex){
                    collectStartIndex = -1;

                    //Means
                    xMean = 0;
                    yMean = 0;
                    for(var i=0;i<numSamples;i++){
                        xMean += coords[i].x;
                        yMean += coords[i].y;
                    }
                    xMean /= numSamples;
                    yMean /= numSamples;

                    //Stdevs
                    xStdev = 0;
                    yStdev = 0;
                    for(var i=0;i<numSamples;i++){
                        xStdev += Math.pow(xMean - coords[i].x, 2);
                        yStdev += Math.pow(yMean - coords[i].y, 2);
                    }
                    xStdev = Math.sqrt(xStdev/(numSamples - 1));
                    yStdev = Math.sqrt(yStdev/(numSamples - 1));

                    collecting = false;
                    ready.color = "green";
                    coordsReady = true;
                    if(autoincrementCNCY)
                        xCNCField.__increment();
                }
            }

            currentIndex = (currentIndex + 1) % numSamples;
        }
    }
}
