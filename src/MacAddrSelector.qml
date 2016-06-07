import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Controls.Styles 1.3
import QtQuick.Dialogs 1.2
import Cellulo 1.0

Row {
    spacing: 5

    function em(x){ return Math.round(x*TextSingleton.font.pixelSize); }

    property variant addresses: []
    property string selectedAddress: selectionBox.currentText
    property var connectionStatus: CelluloBluetooth.ConnectionStatusDisconnected

    signal connectRequested()
    signal disconnectRequested()

    ComboBox{
        id: selectionBox

        editable: true
        width: em(12)
        model: addresses

        onAccepted: {
            if (currentIndex == -1) {
                var tempModel = [];
                for(var i=0;i<model.length;i++)
                    tempModel.push(model[i]);
                tempModel.push(editText);
                model = tempModel;
                currentIndex = count - 1;
            }
        }
    }

    Button{
        text: "Connect"
        onClicked: {
            if(selectionBox.acceptableInput)
                connectRequested();
        }
    }

    Button{
        text: "Disconnect"
        onClicked: disconnectRequested();
    }

    Label{
        text: connectionStatus
        color: {
            switch(connectionStatus){
            case CelluloBluetooth.ConnectionStatusDisconnected:
                return "red";
            case CelluloBluetooth.ConnectionStatusConnecting:
                return "yellow";
            case CelluloBluetooth.ConnectionStatusConnected:
                return "green";
            }
        }
    }
}
