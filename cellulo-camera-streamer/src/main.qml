import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtBluetooth 5.2

Window {
    id: window
    visible: true
    height: 300
    width: 500

    function frameArrived(){
        canvas.requestPaint();
        robot.getCamSnapshot();
    }

    Component.onCompleted: {
        robot.frameArrived.connect(frameArrived);
    }

    Canvas{
        id: canvas
        width: 752/2
        height: 480/2
        onPaint:{
            console.log("Drawing frame...");
            var context = canvas.getContext('2d');
            var img = context.createImageData(canvas.width, canvas.height);
            var high, low, grayscale;
            for(var i=0;i<canvas.width*canvas.height;i++){
                high = robot.pixelBuffer[2*i] - 48;
                if(high > 9)
                    high -= 7;
                low = robot.pixelBuffer[2*i+1] - 48;
                if(low > 9)
                    low -= 7;
                grayscale = 16*high + low;
                img.data[4*i+0] = grayscale;
                img.data[4*i+1] = grayscale;
                img.data[4*i+2] = grayscale;
                img.data[4*i+3] = 255;
            }
            context.drawImage(img,0,0);
        }
    }

    Column{
        spacing: 10

        Button {
            id: pingButton
            text: "Ping"
            onClicked: robot.ping();
        }

        Button {
            id: snapshotButton
            text: "(Re)start stream"
            onClicked: robot.getCamSnapshot();
        }
    }

    Column{
        anchors.right: parent.right
        spacing: 10

        CelluloRobot{
            id:  robot
            macAddress:  "00:06:66:74:41:14"
        }
    }
}
