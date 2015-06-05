import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtBluetooth 5.2
import CelluloBluetooth 1.0

Window {
    id: window
    visible: true
    height: 510
    width: 500

    Image{
        id: cameraFrameImage
        source: "image://cameraFrame/"
        cache: false
        function reload() {
            var oldSource = source;
            source = "";
            source = oldSource;
        }
    }

    Rectangle{
        id: paper
        anchors.top: cameraFrameImage.bottom
        width: 210
        height: 270
        color: "white"
        border.color: "black"
        border.width: 2
        Rectangle{
            id: robotDummy
            width: 20
            height: 20
            radius: 5
            x: robotComm.x*0.5 - 10
            y: robotComm.y*0.5 - 10
            rotation: 360 - robotComm.theta
            color: robotComm.kidnapped ? "red" : "green"
        }
    }

    Column{
        spacing: 10

        Button {
            text: "Frame"
            onClicked: robotComm.requestFrame();
        }

        Button {
            text: "Reset"
            onClicked: robotComm.reset();
        }

        Button {
            text: "Test color high"
            onClicked: robotComm.setVisualEffect(0,"#FFFFFFFF",0);
        }

        Button {
            text: "Test color mid"
            onClicked: robotComm.setVisualEffect(0,"#FF808080",0);
        }

        Button {
            text: "Test color low"
            onClicked: robotComm.setVisualEffect(0,"#FF000000",0);
        }

        anchors.left: cameraFrameImage.right
    }

    CelluloBluetooth{
        id: robotComm
        macAddr: "00:06:66:74:43:00"
        onFrameReady: cameraFrameImage.reload();
    }
}
