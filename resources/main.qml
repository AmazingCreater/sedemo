import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import "."
import CambriconControl 1.0
ApplicationWindow {
    id:mainwindiow
    visible: true
    width: 1920
    height: 1080
    color: "#080403"
    title: qsTr("CMABRICON")
    flags: Qt.FramelessWindowHint|Qt.Window|Qt.WindowSystemMenuHint|Qt.WindowMinimizeButtonHint

    property point startPoint1: Qt.point(0,0)
    property point offsetPoint1: Qt.point(0,0)
    property bool isMoveWindow1: false
    property bool isScale1: false
    property int enableSize1: 4
    property bool maxornormal1: true

    Rectangle{
        id: rectTitleBar
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width
        height: 67
        color: "#19212f"
        z: 2

        /*界面拖动*/
        MouseArea{
            id: mouseMoveWindowArea
            anchors.fill: parent

            /*鼠标点击后初始化开始移动的起点,移动窗体isMoveWindow置为true*/
            onPressed: {
                cursorShape = Qt.SizeAllCursor
                startPoint1 = Qt.point(mouseX,mouseY)
                isMoveWindow1 = !isMoveWindow1
            }

            /*鼠标按住移动计算位置偏移量,窗体随之移动*/
            onPositionChanged: {
                offsetPoint1 = Qt.point(mouseX-startPoint1.x,mouseY-startPoint1.y)
                if(isMoveWindow1){
                    mainwindiow.x += offsetPoint1.x
                    mainwindiow.y += offsetPoint1.y
                }
            }

            /*释放时isMoveWindow置为false*/
            onReleased: {
                cursorShape = Qt.ArrowCursor
                isMoveWindow1=!isMoveWindow1
            }
        }
       Text {
            id: txtMainTitle
            y: 26
            color: "#e6e5e5"
            text: qsTr("CAMBRICON")
            anchors.left: parent.left
            anchors.leftMargin: 40
            font.bold: true
            font.weight: Font.Bold
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 30
        }
       Text {
           id: txtModels
           text: qsTr("Resnet34-SSD")
           anchors.right: txtFps.left
           anchors.rightMargin: 50
           anchors.verticalCenter: parent.verticalCenter
           font.bold: true
           font.weight:Font.Bold
           color: "white"
       }
       Text {
           id: txtFps
           text: qsTr("500FPS")
           objectName: "fps_view"
           anchors.right: sysBtnGroup.left
           anchors.rightMargin: 50
           anchors.verticalCenter: parent.verticalCenter
           font.bold: true
           font.weight:Font.Bold
           color: "white"
           Connections {
               target: fps_updater
               onSig_refresh_fps: {
                   txtFps.text = "TOTAL FPS: " + fps;
               }
           }
       }


       SysBtnGroup {
            id: sysBtnGroup
            x: 1163
            y: 25
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: parent.verticalCenter

            onMin: mainwindiow.showMinimized()
            onClose: {
                Qt.quit();
                mainwindiow.close()
            }
            onMax: {
                mainwindiow.maxornormal1?mainwindiow.showMaximized():mainwindiow.showNormal()
                mainwindiow.maxornormal1 = !mainwindiow.maxornormal1
            }
        }
    }
    Rectangle {
        id: rectSeparator
        width: parent.width
        height: 3
        color: "#080404"
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.top: rectTitleBar.bottom
        anchors.topMargin: 0
    }
    Rectangle{
        id:rectContent
        anchors.top: rectSeparator.bottom
        anchors.left:parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
         color: "#19212e"
         Rectangle{
             id:rectChns
             width: parent.width/2.5
             height: parent.height
             color: "#19212e"
             border.width: 0
             anchors.left: parent.left
             anchors.leftMargin: 0
             anchors.bottom: parent.bottom
             anchors.bottomMargin: 0
             anchors.top: parent.top
             anchors.topMargin: 0
             VideoCapture{
                 anchors.fill:parent
                 id : videoArea
                 objectName: "videoView"
                 MouseArea{
                     anchors.fill: parent
                     onClicked: {
                         videoArea.selectChns(mouseX,mouseY);
                     }
                 }
             }
         }
         Rectangle {
             id:border1
             anchors.left: rectChns.right
             anchors.top: parent.top
             anchors.bottom:parent.bottom
             width: 10
             color:"transparent"
         }

         Rectangle{
             id:rectContentRight
             anchors.left: border1.right
             anchors.top: parent.top
             anchors.bottom:parent.bottom
             anchors.right: parent.right
             color:"transparent"
             Rectangle{
                 id:rectQtAV
                 anchors.left: parent.left
                 anchors.right: parent.right
                 anchors.top: parent.top
                 height: parent.height / 1.5

                 color: "transparent"
                 Image {
                     anchors.left: parent.left
                     anchors.right: parent.right
                     anchors.top:  parent.top
                     anchors.bottom: parent.bottom
                     fillMode: Image.Stretch
                     clip: true
                      id: imgDetail
                      cache: false
                 }
             }
             Rectangle {
                 id:border2
                 color:"transparent"
                 anchors.left: parent.left
                 anchors.right: parent.right
                 anchors.top:rectQtAV.bottom
                 height: 10
             }
             Rectangle{
                 id:rectSelectedDetail
                 color:"transparent"
                 anchors.left: parent.left
                 anchors.right: parent.right
                 anchors.top:border2.bottom
                 anchors.bottom: parent.bottom
                 Image {
                      width: (parent.width - 3 * 5) / 4
                      height: (parent.height - 10) / 2
                      x: 0 * (5 + width)
                      y: 0
                      fillMode: Image.PreserveAspectFit
                      clip: true
                      id: obj0
                      cache: false
                 }
                 Image {
                     width: (parent.width - 3 * 5) / 4
                     height: (parent.height - 10) / 2
                     x: 1 * (5 + width)
                     y: 0
                      fillMode: Image.PreserveAspectFit
                      clip: true
                      id: obj1
                      cache: false
                 }
                 Image {
                     width: (parent.width - 3 * 5) / 4
                     height: (parent.height - 10) / 2
                     x: 2 * (5 + width)
                     y: 0
                      fillMode: Image.PreserveAspectFit
                      clip: true
                      id: obj2
                      cache: false
                 }
                 Image {
                     width: (parent.width - 3 * 5) / 4
                     height: (parent.height - 10) / 2
                     x: 3 * (5 + width)
                     y: 0
                     fillMode: Image.PreserveAspectFit
                     clip: true
                      id: obj3
                      cache: false
                 }
                 Image {
                     width: (parent.width - 3 * 5) / 4
                     height: (parent.height - 10) / 2
                     x: 0 * (5 + width)
                     y: 5 + height
                     fillMode: Image.PreserveAspectFit
                     clip: true
                      id: obj4
                      cache: false
                 }
                 Image {
                     width: (parent.width - 3 * 5) / 4
                     height: (parent.height - 10) / 2
                     x: 1 * (5 + width)
                     y: 5 + height
                     fillMode: Image.PreserveAspectFit
                     clip: true
                      id: obj5
                      cache: false
                 }
                 Image {
                     width: (parent.width - 3 * 5) / 4
                     height: (parent.height - 10) / 2
                     x: 2 * (5 + width)
                     y: 5 + height
                     fillMode: Image.PreserveAspectFit
                     clip: true
                      id: obj6
                      cache: false
                 }
                 Image {
                     width: (parent.width - 3 * 5) / 4
                     height: (parent.height - 10) / 2
                     x: 3 * (5 + width)
                     y: 5 + height
                     fillMode: Image.PreserveAspectFit
                     clip: true
                      id: obj7
                      cache: false
                 }
                 Rectangle {
                     height: 5
                 }
             }
             Timer{
                 //定时器触发时间 单位毫秒
                 interval: 30;
                 //触发定时器
                 running: true;
                 //不断重复
                 repeat: true;
                 //定时器触发时执行
                 onTriggered: {
                     imgDetail.source = "";
                     imgDetail.source = "image://imageProvider/detail";
                 }
             }
             Timer{
                 //定时器触发时间 单位毫秒
                 interval: 400;
                 //触发定时器
                 running: true;
                 //不断重复
                 repeat: true;
                 //定时器触发时执行
                 onTriggered: {
                     obj0.source = "";
                     obj0.source = "image://imageProvider/obj0";
                     obj1.source = "";
                     obj1.source = "image://imageProvider/obj1";
                     obj2.source = "";
                     obj2.source = "image://imageProvider/obj2";
                     obj3.source = "";
                     obj3.source = "image://imageProvider/obj3";
                     obj4.source = "";
                     obj4.source = "image://imageProvider/obj4";
                     obj5.source = "";
                     obj5.source = "image://imageProvider/obj5";
                     obj6.source = "";
                     obj6.source = "image://imageProvider/obj6";
                     obj7.source = "";
                     obj7.source = "image://imageProvider/obj7";
                 }
             }
         }

    }

}
