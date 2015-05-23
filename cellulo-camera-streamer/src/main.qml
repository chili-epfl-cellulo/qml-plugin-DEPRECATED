import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtBluetooth 5.2
import CelluloBluetooth 1.0

Window {
    id: window
    visible: true
    height: 240
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

        anchors.left: cameraFrameImage.right
    }

    CelluloBluetooth{
        id: robotComm
        macAddr: "00:06:66:74:41:04" //"00:06:66:74:41:14"
        onFrameReady: cameraFrameImage.reload();
    }
}
