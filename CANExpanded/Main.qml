import QtQuick
import CANproject
import QtQuick.Controls
Window {
    width: 1000
    height: 800
    visible: true
    title: qsTr("CAN listener")

    //colors for buttons
    //turn lamp
    property color turnLampLeft: "white"
    property color turnLampRight: "white"
    property color turnLampOff: "white"
    property color turnLampLaneLeft: "white"
    property color turnLampLaneRight: "white"
    property int turnLampLeftF: 0
    property int turnLampRightF: 0
    property int turnLampOffF: 0
    property int turnLampLaneLeftF: 0
    property int turnLampLaneRightF: 0
    //high beam
    property color highBeamL: "white"
    property int highBeamF: 0
    //flashBeam
    property color flashBeam: "white"
    property int flashBeamF: 0
    //fwasher
    property color fwasher: "white"
    property int fwasherF: 0
    //fwiper
    property color fwiperOff: "white"
    property color fwiperLow: "white"
    property color fwiperHigh: "white"
    property color fwiperInt: "white"
    property int fwiperOffF: 0
    property int fwiperLowF: 0
    property int fwiperHighF: 0
    property int fwiperIntF: 0
    //fwiperSens
    property color fwipeSensCenter: "white"
    property color fwipeSensUp: "white"
    property color fwipeSensDown: "white"
    property int fwipeSensCenterF: 0
    property int fwipeSensUpF: 0
    property int fwipeSensDownF: 0
    //parked
    property color isParked: "white"
    property int isParkedF: 0

    //gear pos
    property color gearCenter: "white"
    property color gearUp: "white"
    property color gearUp2: "white"
    property color gearDown: "white"
    property color gearDown2: "white"
    property int gearCenterF: 0
    property int gearUpF: 0
    property int gearUp2F: 0
    property int gearDownF: 0
    property int gearDown2F: 0

    Button {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 25
        anchors.rightMargin: 50
        text: qsTr("初始化")
        onClicked: {
            controller.initPort()
        }
    }

    CanController {
        id: controller
        Component.onCompleted: {
            controller.initPort()
            controller.getMsg()
            conTimer.start()
        }
        onTurnLampChanged: {
            turnLampLeft: "white"
            turnLampRight: "white"
            turnLampOff: "white"
            turnLampLaneLeft: "white"
            turnLampLaneRight: "white"
            if(turnLamp === 0){
                turnLampOff: "yellow"
                turnLampOffF++;
            } else if(turnLamp === 1){
                turnLampLeftF++
                turnLampLeft: "yellow"
            } else if(turnLamp === 2){
                turnLampRight: "yellow"
                turnLampRightF++
            } else if(turnLamp === 3){
                turnLampLaneLeft: "yellow"
                turnLampLaneLeftF++
            } else {
                turnLampLaneRight: "yellow"
                turnLampLaneRightF++
            }
        }

        onHighBeamSig: {
            highBeamF++
            highBeamL = "yellow"
            highBeamTimer.restart();
        }

        onFlashBeamSig: {
            flashBeamF++
            flashBeam = "yellow"
            flashBeamTimer.restart();
        }

        onFwasherSig: {
            fwasherF++
            fwasher = "yellow"
            fwasherTimer.restart();
        }

        onFwiperChanged: {
            fwiperOff = "white"
            fwiperLow = "white"
            fwiperHigh = "white"
            fwiperInt = "white"
            if(fwiper === 0){
                fwiperOffF++
                fwiperOff = "yellow"
            } else if(fwiper === 1){
                fwiperLowF++
                fwiperLow = "yellow"
            } else if(fwiper === 2){
                fwiperHigh = "yellow"
                fwiperHighF++
            } else {
                fwiperInt = "yellow"
                fwiperIntF++
            }
        }

        onFwiperSensChanged: {
            fwipeSensCenter = "white"
            fwipeSensUp = "white"
            fwipeSensDown = "white"
            if(fwiperSens === 0){
                fwipeSensCenterF++
                fwipeSensCenter = "yellow"
            } else if(fwiperSens === 1){
                fwipeSensUp = "yellow"
                fwipeSensUpF++
            } else if(fwipeSens === 2){
                fwipeSensDown = "yellow"
                fwipeSensDownF++;
            }
        }

        onParkSig: {
            isParkedF++
            isParked = "yellow"
            parkedTimer.restart();
        }

        onShifterPosChanged: {
            gearCenter = "white"
            gearUp = "white"
            gearUp2 = "white"
            gearDown = "white"
            gearDown2 = "white"
            if(shifterPos === 0){
                gearCenterF++
                gearCenter = "yellow"
            } else if(shifterPos === 1){
                gearUpF++
                gearUp = "yellow"
            } else if(shifterPos === 2){
                gearUp2 = "yellow"
                gearUp2F++
            } else if(shifterPos === 3){
                gearDown = "yellow"
                gearDownF++
            } else if(shifterPos === 4){
                gearDown2 = "yellow"
                gearDown2F++
            }
        }
    }

    Timer {
        id: highBeamTimer
        interval: 20
        onTriggered: {
            highBeamL: "white"
        }
    }
    Timer {
        id: flashBeamTimer
        interval: 20
        onTriggered: {
            flashBeam: "white"
        }
    }
    Timer {
        id: fwasherTimer
        interval: 20
        onTriggered: {
            fwasher = "white"
        }
    }
    Timer {
        id: parkedTimer
        interval: 20
        onTriggered: {
            isParked = "white"
        }
    }

    Timer {
        id: conTimer
        interval: 20
        onTriggered: {
            controller.getMsg()
            restart()
        }
    }

    Text {
        id: tally
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.topMargin: 25
        required property int data
        data: controller.framesSent
        text: qsTr("收到帧数: " + data)
    }

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 100
        anchors.top: parent.top
        anchors.topMargin: 50
        property int size
        size: 20
        Text {
            text: "左边"
            font.pixelSize: parent.size
        }
        Text {
            text: "右边"
            font.pixelSize: parent.size
        }
    }

    Text {
        anchors.fill: parent
        anchors.topMargin: 100
        anchors.leftMargin: 20
        text: qsTr("转向/变道灯开关信号(TurnLampSwSt):")
        font.pixelSize: 15
    }

    Row {
        anchors.fill: parent
        anchors.topMargin: 125
        anchors.leftMargin: 20
        spacing: 20
        Repeater {
            model: ["关闭", "左转", "右转" ,"左变道", "右变道"]
            Column {
                required property string modelData
                required property int index
                spacing: 10
                Text {
                    leftPadding: 10
                    text: parent.modelData
                }
                Text {
                    property int data
                    data: {
                        if(parent.index === 0)
                            turnLampOffF
                        else if(parent.index === 1)
                            turnLampLeftF
                        else if(parent.index === 2)
                            turnLampRightF
                        else if(parent.index === 3)
                            turnLampLaneLeftF
                        else if(parent.index === 4)
                            turnLampLaneRightF
                    }
                    text: "帧: " + data
                }
                Rectangle {
                    radius: 27.5
                    width: 55
                    height: 55
                    border.color: "black"
                    border.width: 1
                }
            }
        }
    }
    Row {
        anchors.fill: parent
        anchors.leftMargin: 20
        anchors.topMargin: 250
        spacing: 20
        Column {
            spacing: 15
            Text {
                text: "远光灯开关"
            }
            Text {
                text: "帧: " + highBeamF
            }

            Rectangle {
                radius: 27.5
                width: 55
                height: 55
                border.color: "black"
                border.width: 1
            }
        }
        Column {
            spacing: 15
            Text {
                text: "超车灯开关"
            }
            Text {
                text: "帧: " + flashBeamF
            }

            Rectangle {
                radius: 27.5
                width: 55
                height: 55
                border.color: "black"
                border.width: 1
            }
        }
        Column {
            spacing: 15
            Text {
                text: "前雨刮摆动/洗涤"
            }
            Text {
                text: "帧: " + fwasherF
            }

            Rectangle {
                radius: 27.5
                width: 55
                height: 55
                border.color: "black"
                border.width: 1
            }
        }
    }
    Row {
        anchors.fill: parent
        anchors.leftMargin: 20
        anchors.topMargin: 375
        Column {
            spacing: 15
            Text {
                text: qsTr("前雨刮挡位选择(MFS_FwiperSwSts): ")
                font.pixelSize: 15
            }
            Row {
                spacing: 15
                Column {
                        spacing: 15
                    Text {
                        text: "旋钮OFF位"
                    }
                    Text {
                        text: "帧: " + fwiperOffF
                    }

                    Rectangle {
                        radius: 27.5
                        width: 55
                        height: 55
                        border.color: "black"
                        border.width: 1
                    }
                }
                Column {
                    spacing: 15
                    Text {
                        text: "旋钮Low位"
                    }
                    Text {
                        text: "帧: " + fwiperLowF
                    }

                    Rectangle {
                        radius: 27.5
                        width: 55
                        height: 55
                        border.color: "black"
                        border.width: 1
                    }
                }
                Column {
                    spacing: 15
                    Text {
                        text: "旋钮High位"
                    }
                    Text {
                        text: "帧: " + fwiperHighF
                    }

                    Rectangle {
                        radius: 27.5
                        width: 55
                        height: 55
                        border.color: "black"
                        border.width: 1
                    }
                }
                Column {
                    spacing: 15
                    Text {
                        text: "旋钮INT位"
                    }
                    Text {
                        text: "帧: " + fwiperIntF
                    }

                    Rectangle {
                        radius: 27.5
                        width: 55
                        height: 55
                        border.color: "black"
                        border.width: 1
                    }
                }
            }
        }
    }


    Row {
        anchors.fill: parent
        anchors.leftMargin: 20
        anchors.topMargin: 550
        spacing: 30
        Column {
            spacing: 15
            Text {
                text: "前雨刮自动雨刮灵敏度调节旋钮"
                font.pixelSize: 15
            }
            Row {
                spacing: 20
                Column{
                    spacing: 15
                    Text {
                        text: "Center位"
                    }
                    Text {
                        text: "帧: " + fwipeSensCenterF
                    }

                    Rectangle {
                        radius: 27.5
                        width: 55
                        height: 55
                        border.color: "black"
                        border.width: 1
                    }
                }
                Column{
                    spacing: 15
                    Text {
                        text: "上拨"
                    }
                    Text {
                        text: "帧: " + fwipeSensUpF
                    }

                    Rectangle {
                        radius: 27.5
                        width: 55
                        height: 55
                        border.color: "black"
                        border.width: 1
                    }
                }
                Column{
                    spacing: 15
                    Text {
                        text: "下拨"
                    }
                    Text {
                        text: "帧: " + fwipeSensDownF
                    }

                    Rectangle {
                        radius: 27.5
                        width: 55
                        height: 55
                        border.color: "black"
                        border.width: 1
                    }
                }
            }
        }
        Column {
            spacing: 15
            Text {
                text: "P档开关信号"
                font.pixelSize: 15
            }
            Row {
                Column{
                    spacing: 15
                    Text {
                        text: "按下"
                    }
                    Text {
                        text: "帧: " + isParkedF
                    }

                    Rectangle {
                        radius: 27.5
                        width: 55
                        height: 55
                        border.color: "black"
                        border.width: 1
                    }
                }
            }
        }
    }
    Row {
        anchors.top: parent.top
        anchors.topMargin: 300
        anchors.right: parent.right
        anchors.rightMargin: 20
        Column {
            spacing: 15
            Text {
                text: qsTr("档位开关信号(MFS_ShifterPosSt): ")
                font.pixelSize: 15
            }
            Row {
                spacing: 20
                Column{
                    spacing: 15
                    Text {
                        text: "向上二个档位"
                    }
                    Text {
                        text: "帧: " + gearUp2F
                    }

                    Rectangle {
                        radius: 27.5
                        width: 55
                        height: 55
                        border.color: "black"
                        border.width: 1
                    }
                }
                Column{
                    spacing: 15
                    Text {
                        text: "向上一个档位"
                    }
                    Text {
                        text: "帧: " + gearUpF
                    }

                    Rectangle {
                        radius: 27.5
                        width: 55
                        height: 55
                        border.color: "black"
                        border.width: 1
                    }
                }
                Column{
                    spacing: 15
                    Text {
                        text: "初始位"
                    }
                    Text {
                        text: "帧: " + gearCenterF
                    }

                    Rectangle {
                        radius: 27.5
                        width: 55
                        height: 55
                        border.color: "black"
                        border.width: 1
                    }
                }
                Column{
                    spacing: 15
                    Text {
                        text: "向下一个档位"
                    }
                    Text {
                        text: "帧: " + gearDownF
                    }

                    Rectangle {
                        radius: 27.5
                        width: 55
                        height: 55
                        border.color: "black"
                        border.width: 1
                    }
                }
                Column{
                    spacing: 15
                    Text {
                        text: "向下二个档位"
                    }
                    Text {
                        text: "帧: " + gearDown2F
                    }

                    Rectangle {
                        radius: 27.5
                        width: 55
                        height: 55
                        border.color: "black"
                        border.width: 1
                    }
                }
            }
        }
    }
}
