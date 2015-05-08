import QtQuick 2.0

Item {
    property int imgWidth: 752/2
    property int imgHeight: 480/2

    property bool expectCamStream
    property bool pixelBufferReady: true
    property var pixelIndex
    property var pixelBuffer
    property var frame

    signal frameArrived()

    Component.onCompleted: {
        pixelBuffer = new Array(imgWidth*imgHeight*2);
        frame = 0;
    }

    function initCamStream(){
        pixelBufferReady = false;
        pixelIndex = 0;
        expectCamStream = true;
    }

    function processIncoming(data){

        //Process cam image stream
        if(expectCamStream){
            var c;
            for(var i=0;i<data.length;i++){
                c = data.charCodeAt(i);
                if((c >= 48 && c <= 57) || (c >= 65 && c <= 70)){
                    pixelBuffer[pixelIndex] = c;
                    pixelIndex++;
                }
            }

            if(pixelIndex >= imgWidth*imgHeight*2){
                expectCamStream = false;
                pixelBufferReady = true;
                console.log("Frame " + frame + " arrived");
                frame++;
                frameArrived();
            }
        }

        //Process other command responses or messages
        else{
            console.log(data);
        }
    }
}
