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

    property real numSamples: 20
    property variant coords: []
    property bool coordsReady: false
    property bool collecting: false
    property bool zeroWhenFinished: false
    property real xMean: 0
    property real yMean: 0
    property real thetaMean: 0
    property real xStdev: 1
    property real yStdev: 1
    property real thetaStdev: 1
    property real xRobotZero: 0
    property real yRobotZero: 0
    property real thetaRobotZero: 0
    property real numCountingSamples: 0

    Component.onCompleted: {
        var temp = new Array(0);
        for(var i=0;i<numSamples;i++)
            temp.push(Qt.vector3d(0,0,0));
        coords = temp;
    }

    Timer {
        id: numSampleTimer
        interval: 1000*10
        running: false
        repeat: false
        onTriggered: console.log("Number of samples collected in 10 seconds:"+numCountingSamples);
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
                text: "Start counting samples"
                onClicked: {
                    numCountingSamples = 0;
                    numSampleTimer.start();
                    console.log("Started counting samples...");
                }
            }

            Button {
                text: "Zero robot coords"
                onClicked: {
                    if(coordsReady){
                        xRobotZero = xMean;
                        yRobotZero = yMean;
                        thetaRobotZero = thetaMean;
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

            TextField{
                id: thetaRobotZeroField
                text: thetaRobotZero
                readOnly: true
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

            Button {
                text: "Measure and Zero"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    ready.color = "red";
                    coordsReady = false;
                    collecting = true;
                    zeroWhenFinished = true;
                }
            }

            CheckBox{
                id: logEverything
                checked: false
                text: "Log every pose"
                anchors.verticalCenter: parent.verticalCenter
            }

            CheckBox{
                id: logX
                checked: true
                text: "Log X"
                anchors.verticalCenter: parent.verticalCenter
            }

            CheckBox{
                id: logY
                checked: true
                text: "Log Y"
                anchors.verticalCenter: parent.verticalCenter
            }

            CheckBox{
                id: logTheta
                checked: true
                text: "Log Theta"
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Text{
            id: ready
            text: "Ready?"
            color: "red"
        }

        Text{
            text: robotComm.x*robotComm.gridSpacing + " " + robotComm.y*robotComm.gridSpacing + " " + robotComm.theta
        }
    }

    CelluloBluetooth{
        property real gridSpacing: 0.508
        property real currentIndex: 0
        property real collectStartIndex: -1

        id: robotComm
        macAddr: "00:06:66:74:48:A7"

        onPoseChanged: {
            coords[currentIndex] = Qt.vector3d(x*gridSpacing, y*gridSpacing, theta);

            numCountingSamples++;

            if(logEverything.checked)
                console.log(
                            (logX.checked ? (coords[currentIndex].x - xRobotZero) + " " : " ") +
                            (logY.checked ? (coords[currentIndex].y - yRobotZero) + " " : " ") +
                            (logTheta.checked ? (coords[currentIndex].z - thetaRobotZero) : " ")
                            );

            if(collecting){
                if(collectStartIndex < 0)
                    collectStartIndex = currentIndex;

                if((currentIndex + 1) % numSamples == collectStartIndex){
                    collectStartIndex = -1;

                    //Means
                    xMean = 0;
                    yMean = 0;
                    thetaMean = 0;
                    for(var i=0;i<numSamples;i++){
                        xMean += coords[i].x;
                        yMean += coords[i].y;
                        thetaMean += coords[i].z;
                    }
                    xMean /= numSamples;
                    yMean /= numSamples;
                    thetaMean /= numSamples;

                    //Stdevs
                    xStdev = 0;
                    yStdev = 0;
                    thetaStdev = 0;
                    for(var i=0;i<numSamples;i++){
                        xStdev += Math.pow(xMean - coords[i].x, 2);
                        yStdev += Math.pow(yMean - coords[i].y, 2);
                        thetaStdev += Math.pow(thetaMean - coords[i].z, 2);
                    }
                    xStdev = Math.sqrt(xStdev/(numSamples - 1));
                    yStdev = Math.sqrt(yStdev/(numSamples - 1));
                    thetaStdev = Math.sqrt(thetaStdev/(numSamples - 1));

                    //Record data
                    console.log(
                                (logX.checked ? (xMean - xRobotZero) + " " + xStdev + " " : " ") +
                                (logY.checked ? (yMean - yRobotZero) + " " + yStdev + " " : " ") +
                                (logTheta.checked ? (thetaMean - thetaRobotZero) + " " + thetaStdev : " ")
                                );

                    if(zeroWhenFinished){
                        xRobotZero = xMean;
                        yRobotZero = yMean;
                        thetaRobotZero = thetaMean;
                    }

                    collecting = false;
                    zeroWhenFinished = false;
                    ready.color = "green";
                    coordsReady = true;
                }
            }

            currentIndex = (currentIndex + 1) % numSamples;
        }
    }
}
