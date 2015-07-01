import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtBluetooth 5.2
import Cellulo 1.0

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

        Button {
            text: "Test rainbow"
            onClicked:{
                robotComm.setVisualEffect(1,"#FFA00000",0);
                robotComm.setVisualEffect(1,"#FF00A000",1);
                robotComm.setVisualEffect(1,"#FF0000A0",2);
                robotComm.setVisualEffect(1,"#FFA0A000",3);
                robotComm.setVisualEffect(1,"#FFA000A0",4);
                robotComm.setVisualEffect(1,"#FF00A0A0",5);
            }
        }

        Button {
            text: "Test waiting"
            onClicked: robotComm.setVisualEffect(5,"#FFFF8080",0);
        }

        Button {
            text: "Test breathe"
            onClicked: robotComm.setVisualEffect(8,"#FF80FF80",0);
        }

        anchors.left: cameraFrameImage.right
    }

    CelluloBluetooth{
        id: robotComm
        macAddr: "00:06:66:74:40:E4"
        onFrameReady: cameraFrameImage.reload();
    }
}
