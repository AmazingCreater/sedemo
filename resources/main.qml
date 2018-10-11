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
           anchors.right: sysBtnGroup.left
           anchors.rightMargin: 50
           anchors.verticalCenter: parent.verticalCenter
           font.bold: true
           font.weight:Font.Bold
           color: "white"
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
         color: "#060205"
         Rectangle{
             id:rectChns
             width: parent.width/2
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
         Rectangle{
             id:rectContentRight
             anchors.left: rectChns.right
             anchors.top: parent.top
             anchors.bottom:parent.bottom
             anchors.right: parent.right
             color:"transparent"
             Rectangle{
                 id:rectQtAV
                 anchors.left: parent.left
                 anchors.right: parent.right
                 anchors.top: parent.top
                 anchors.bottom: parent.bottom
                 anchors.bottomMargin:300
                 color: "transparent"
                 Image {
                      id: imgDetail
                      cache: false
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

//                 Connections {
//                     target:picontrol
//                     onRefreshImage:{
//                         imgDetail.source = "image://imageProvider/detail"
//                         imgDetail.update();
//                     }
//                 }
             }
             Rectangle{
                 id:rectSelectedDetail
                 color:"transparent"
                 anchors.left: parent.left
                 anchors.right: parent.right
                 anchors.top:rectQtAV.bottom
                 anchors.bottom: parent.bottom
//                 GridView{
//                     id:grdSelctedDetail
//                     model: 10
//                     cellHeight: 100
//                     cellWidth: 200
//                     delegate:Image {
//                             id: imgObjs
//                             height: 100
//                             width: 200
//                             source: "images/car.jpg"
//                     }
//                 }
             }
         }

    }

}
