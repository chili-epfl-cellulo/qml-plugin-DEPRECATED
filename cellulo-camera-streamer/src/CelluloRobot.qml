import QtQuick 2.0
import QtQuick.Controls 1.2

Rectangle{
    color: "red"
    width: 10
    height: 10

    property string macAddress
    property var socket
    property bool ready

    function connectToDevice(){
        ready = false;
        color = "red";
        console.log("Connecting to " + macAddress);
        var socketComponent = Qt.createComponent("DisposableBluetoothSocket.qml");
        socket = socketComponent.createObject(this, {"macAddress":macAddress, "connected": true});
        socket.destroying.connect(connectToDevice); //Reconnect everytime we are destroyed, this is the only way to reconnect when dropped
        socket.connected_.connect(initTimer.start); //Workaround for first command waiting
    }

    function sendData(str){
        socket.stringData = str;
    }

    onMacAddressChanged: {
        connectToDevice();
    }

    Timer{
        id: initTimer
        interval: 500
        repeat: false
        running: false
        onTriggered: {
            parent.ready = true;
            color = "green";
        }
    }
}
