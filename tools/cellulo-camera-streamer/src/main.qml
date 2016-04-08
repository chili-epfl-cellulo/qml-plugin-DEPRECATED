import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtBluetooth 5.2
import Cellulo 1.0

Window {
    id: window
    visible: true
    height: 480
    width: 640

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
            connected: robotComm.connected
            connecting: robotComm.connecting
        }

        CheckBox {
            id: enable
            text: "Enable image streaming"
            checked: false
        }

        Button {
            text: "Get frame"
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
        onFrameReady: cameraFrameImage.reload();
        imageStreamingEnabled: enable.checked
    }
}
