/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "qvideocapture.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleMaterial>
#include <QtQuick/QSGSimpleTextureNode>
QVideoCapture::QVideoCapture(QQuickItem *parent)
    : QQuickItem(parent)
    , m_file("")
    , m_fps(0)
    , m_linearFilter(true)
    , m_loop(false)
	, qimg(NULL)
	,imgIpl(NULL)
	//, m_thread(0)
{

    setFlag(ItemHasContents, true);
    m_timer.start(intTimer);

    delete imgIpl;
    imgIpl = NULL;

    delete qimg;
    qimg = NULL;

    update();

}

qint64 QVideoCapture::timeStamp1;

void QVideoCapture::setFile(const QString &file){
	//实际设定ID 待改正
			delete imgIpl;
			imgIpl = NULL;

			delete qimg;
			qimg = NULL;

			update();

}

QSGNode *QVideoCapture::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData * x){
	if (!qimg ||!imgIpl)
	{
		return QQuickItem::updatePaintNode(node, x);
	}
	QSGSimpleTextureNode *texture = static_cast<QSGSimpleTextureNode*>(node);
    if (texture == nullptr) {
		texture = new QSGSimpleTextureNode();
	}

	QSGTexture *t = window()->createTextureFromImage(*qimg);


	if (t) {
		QSGTexture *tt = texture->texture();
		if (tt) {
			tt->deleteLater();
		}
        // QRectF rect;
        // rect.setRect(boundingRect().width() / 2 - intOrignWidth / 2, boundingRect().height()/2 - intOrignWidth*dblRate/2, intOrignWidth, intOrignWidth*dblRate);
        texture->setRect(boundingRect());
		texture->setTexture(t);
	}

    return reinterpret_cast<QSGNode*>(texture);
	
}


QVideoCapture::~QVideoCapture(){

}

void QVideoCapture::setScreenBlack()
{
	if (!qimg)
	{
		return;
	}
    qimg->fill("black");
	if (imgIpl)
	{
		if (imgIpl->imageData)
		{
			cvReleaseImage(&imgIpl);
			delete imgIpl;
            imgIpl = nullptr;
		}
	}

    update();
}

void QVideoCapture::receiveFrames(cv::Mat objMat)
{
     updateFps();
     //cv::imwrite("/home/cc/123/"+std::to_string(xxx)+".jpg",*(objMat));
        if (imgIpl)
        {
            //图片任务 大小不一致的图片处理
            if (imgIpl->width != objMat.cols || imgIpl->height != objMat.rows)
            {
                cvReleaseImage(&imgIpl);
                delete imgIpl;
                imgIpl = nullptr;

                delete qimg;
                qimg = nullptr;
            }
        }

        if (!imgIpl)
        {
            imgIpl = cvCreateImageHeader(cvSize(objMat.cols, objMat.rows), 8, 3);
            if (qimg)
            {
                delete qimg;
                qimg = nullptr;
            }
            if (!qimg)
            {
                qimg = new QImage(QSize(imgIpl->width, imgIpl->height), QImage::Format_RGB888);
                imgIpl->imageData = reinterpret_cast<char*>(qimg->bits());

            }
        }

            if (imgIpl->imageData)
            {
                IplImage ipl_img(objMat);
                cvCopy(&ipl_img, imgIpl, nullptr);
            }
        update();
}

void QVideoCapture::selectChns(int x, int y) {
    int select_chn = static_cast<int>(8.0 * y / height()) * 4 + static_cast<int>(x / width() * 4);
    if (select_chn > 31) select_chn = 31;
    qDebug() << "selected: " << select_chn;
    emit sig_change_chn(select_chn);
}

void QVideoCapture::updateFps() {
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> delay = end - m_start;
    int fps = static_cast<int>(1e3 / delay.count() * 32);
    emit sig_update_fps(fps);
    m_start = end;

}
