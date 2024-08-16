import QtQuick
import CANproject
import QtQuick.Controls
Window {
    width: 650
    height: 500
    visible: true
    title: qsTr("CAN listener")
    property color pressedColor: "white"
    property color forwardOneColor: "white"
    property color forwardTwoColor: "white"
    property color backwardOneColor: "white"
    property color backwardTwoColor: "white"
    property color normalColor: "white"
    property int gear: 0 // forward1 = 4, forward2 = 5, backward1 = 2, backward2 = 1, normal = 3
    property double pressedC: 0
    property double forwardOneC: 0
    property double forwardTwoC: 0
    property double normalC: 0
    property double backwardOneC: 0
    property double backwardTwoC: 0

    //gears
    property color pColor: "white"
    property color nColor: "yellow"
    property color rColor: "white"
    property color dColor: "white"

    Timer {
        id: reset
        interval: 50
        onTriggered: {
            forwardOneColor = "white"
            forwardTwoColor = "white"
            backwardOneColor = "white"
            backwardTwoColor = "white"
            normalColor = "white"
            gear = 0
        }
    }

    Button {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 50
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
            timer.start()
            timer1.start()
        }

        onPressed: {
            pressedColor = "green"
            pressedC++
            pressedReset.restart()

        }

        onForwardOne: {
            gear = 4
            forwardOneColor = "green"
            forwardOneC++
            forwardTwoColor = "white"
            backwardOneColor = "white"
            backwardTwoColor = "white"
            normalColor = "white"
            reset.restart()
        }

        onForwardTwo: {
            gear = 5
            forwardTwoColor = "green"
            forwardTwoC++
            forwardOneColor = "white"
            backwardOneColor = "white"
            backwardTwoColor = "white"
            normalColor = "white"
            reset.restart()
        }

        onBackWardOne: {
            backwardOneColor = "green"
            gear = 2
            backwardOneC++
            forwardOneColor = "white"
            forwardTwoColor = "white"
            backwardTwoColor = "white"
            normalColor = "white"
            reset.restart()
        }

        onBackWardTwo: {
            backwardTwoColor = "green"
            gear = 1
            backwardTwoC++
            forwardOneColor = "white"
            forwardTwoColor = "white"
            backwardOneColor = "white"
            normalColor = "white"
            reset.restart()
        }

        onNormal: {
            normalColor = "green"
            gear = 3
            normalC++;
            forwardOneColor = "white"
            forwardTwoColor = "white"
            backwardOneColor = "white"
            backwardTwoColor = "white"
            reset.restart()
        }

        onGearsChanged: {
            pColor = "white"
            nColor = "white"
            rColor = "white"
            dColor = "white"
            if(gears === 0)
                pColor = "yellow"
            else if(gears === 1)
                dColor = "yellow"
            else if(gears === 2)
                nColor = "yellow"
            else if(gears === 3)
                rColor = "yellow"
        }
    }

    Timer {
        id: pressedReset
        interval: 50
        onTriggered: {
            pressedColor = "white"
        }
    }

    Timer {
        id: timer
        interval: 20
        onTriggered: {
            controller.getMsg()
        }
        repeat: true

    }

    Timer {
        id: timer1
        interval: 100
        onTriggered: {
            controller.udsRead()
        }
        repeat: true
    }

    Text {
        id: tally
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.topMargin: 50
        required property int data
        data: controller.framesSent
        text: qsTr("收到帧数: " + data)
    }

    Row {
        anchors.fill: parent
        anchors.topMargin: 80
        anchors.leftMargin: 60
        spacing: 15
        Repeater {
            id: repeat
            model: ["P按键","B2", "B1", "H","F1", "F2"]
            Column {
                required property string modelData
                required property int index //for future use
                spacing: 10

                Text {
                    leftPadding: 17
                    width: 20
                    font.pixelSize: 12
                    text: modelData
                    color: "black"
                }

                Text {
                    required property int data
                    data: {
                        if(index === 0)
                            pressedC
                        else if(index === 1)
                            backwardTwoC
                        else if(index === 2)
                            backwardOneC
                        else if(index === 3)
                            normalC
                        else if(index === 4)
                            forwardOneC
                        else if(index === 5)
                            forwardTwoC
                    }
                    font.pixelSize: 12
                    text: "收到帧数：" + data

                }

                Rectangle {
                    width: 50
                    height: 50
                    radius: 25
                    border.color: "black"
                    border.width: 1
                    color: {
                        if(index === 0)
                            pressedColor
                        else if(index === 1)
                            backwardTwoColor
                        else if(index === 2)
                            backwardOneColor
                        else if(index === 3)
                            normalColor
                        else if(index === 4)
                            forwardOneColor
                        else if(index === 5)
                            forwardTwoColor
                    }
                }
            }
        }
    }

    Text {
        anchors.fill: parent
        anchors.topMargin: 220
        anchors.leftMargin: 120
        text: qsTr("档位:")
        font.pixelSize: 15
    }

    Row {
        topPadding: 50
        spacing: 30
        anchors.centerIn: parent
        Repeater {
            model: ["P", "D", "N", "R"]
            Column {
                required property string modelData
                required property int index
                spacing: 15
                Text {
                    leftPadding: 17
                    text: parent.modelData
                    font.pixelSize: 15
                }
                Rectangle {
                    radius: 25
                    width: 50
                    height: 50
                    border.width: 1
                    color: {
                        switch(parent.index){
                            case 0:
                                pColor
                                break
                            case 1:
                                dColor
                                break
                            case 2:
                                nColor
                                break
                            case 3:
                                rColor
                                break
                        }
                    }
                }
            }
        }
    }

    Text {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 150
        anchors.left: parent.left
        anchors.leftMargin: 15
        text: qsTr("原始和物理值: ")
        font.pixelSize: 20
    }

    Row {
        spacing: 50
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 125
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
            text: qsTr("原始值: ") + controller.orig
            font.pixelSize: 15
        }
        Text {
            text: qsTr("实时角度: ") + controller.physical
            font.pixelSize: 15
        }
    }

}
