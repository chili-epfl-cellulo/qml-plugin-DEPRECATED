import QtQuick 2.0
import QtBluetooth 5.2

BluetoothSocket{
    property string macAddress

    signal dataReceived(string data);
    signal destroying();
    signal connected_(); //There is already a property called 'connected'

    service: BluetoothService{
        deviceAddress: macAddress
        serviceProtocol: BluetoothService.RfcommProtocol
        serviceUuid: "00001101-0000-1000-8000-00805F9B34FB" //Serial port UUID
    }

    onSocketStateChanged: {
        console.log(service.deviceAddress + " is now " + socketState + ", connected: " + connected);
        if(socketState == 0){
            destroying();
            destroy();
        }
        else
            connected_();
    }

    onStringDataChanged: console.log(stringData)
}
