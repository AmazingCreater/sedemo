/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#include "qvideocapturethread.h"

#include "opencv2/video/video.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <QMutex>
#include <QTime>
#include <QTimer>
#include <QReadWriteLock>
#include <QWaitCondition>


//#define QVIDEO_CAPTURE_THREAD_DEBUG_FLAG
#ifdef QVIDEO_CAPTURE_THREAD_DEBUG_FLAG
#define QVIDEO_CAPTURE_THREAD_DEBUG(_param) qDebug() << (_param)
#else
#define QVIDEO_CAPTURE_THREAD_DEBUG(_param)
#endif


//using namespace cv;

class QVideoCaptureThreadPrivate{

public:
    cv::VideoCapture*  capture;
    QMat*          inactiveMat;

    bool           abord;
    bool           inactiveMatReady;

    int            width;
    int            height;

    int            seekRequest;

    QMutex         mutex;
    QWaitCondition condition;

};


/*!
  \class QVideoCaptureThread
  \internal
  \brief Internal video capture thread used by QVideoCapture.
 */

QVideoCaptureThread::QVideoCaptureThread(const QString &file, QObject *parent) :
    QThread(parent),
    m_file(file),
    m_paused(false),
    m_framePos(0),
    m_totalFrames(0),
    m_isSeeking(false),
    m_forceSeek(false),
    d_ptr(new QVideoCaptureThreadPrivate)
{
    Q_D(QVideoCaptureThread);
    d->inactiveMatReady = false;
    d->seekRequest      = -1;
    d->capture          = new cv::VideoCapture(file.toStdString());
    if ( d->capture->isOpened() )
        initializeMatSize();

    m_timer             = new QTimer;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(tick()));
}

qint64 QVideoCaptureThread::timeStamp1;

QVideoCaptureThread::~QVideoCaptureThread(){
    Q_D(QVideoCaptureThread);
    d->mutex.lock();
    d->abord = true;
    d->condition.wakeOne();
    d->mutex.unlock();
    wait(); // wait till thread finishes
    QVIDEO_CAPTURE_THREAD_DEBUG( QString("Video capture \"") + m_file + "\" thread released." );
    d->capture->release();
    delete m_timer;
    delete d->capture;
    delete d;
}

bool QVideoCaptureThread::isCaptureOpened(){
    Q_D(QVideoCaptureThread);
    if ( !d->capture->isOpened() )
        d->capture->open(file().toStdString());

    if ( d->capture->isOpened() )
        initializeMatSize();
    return d->capture->isOpened();
}

void QVideoCaptureThread::processNextFrame(){
    Q_D(QVideoCaptureThread);
    d->inactiveMatReady = false;
}

void QVideoCaptureThread::setLoop(bool loop){
    m_loop = loop;
}

void QVideoCaptureThread::tick(){
    Q_D(QVideoCaptureThread);
    QMutexLocker lock(&d->mutex);
    if ( !isRunning() ){
        start(NormalPriority);
    } else {
        if ( !d->inactiveMatReady ){
            d->condition.wakeOne();
        }
    }
}

void QVideoCaptureThread::seekTo(int frame){
    Q_D(QVideoCaptureThread);
    if ( frame != m_framePos && m_totalFrames != 0 ){
        d->seekRequest = frame;

        //tick();
    }
}


void QVideoCaptureThread::run(){
    Q_D(QVideoCaptureThread);

    d->abord = false;
}

void QVideoCaptureThread::initializeMatSize(){
    Q_D(QVideoCaptureThread);
    m_totalFrames = (int)d->capture->get(CV_CAP_PROP_FRAME_COUNT);
    d->width      = (int)d->capture->get(CV_CAP_PROP_FRAME_WIDTH);
    d->height     = (int)d->capture->get(CV_CAP_PROP_FRAME_HEIGHT);
    if ( d->width == 0 || d->height == 0 ){ // Sacrifice one frame to get width and height
        if ( d->capture->grab() ){
            cv::Mat firstFrame;
            d->capture->retrieve(firstFrame);
            d->width  = firstFrame.size().width;
            d->height = firstFrame.size().height;
            ++m_framePos;
        }
    }
}

void QVideoCaptureThread::beginSeek(){
    m_isSeeking = true;
    emit isSeekingChanged();
}

void QVideoCaptureThread::endSeek(){
    m_isSeeking = false;
    emit isSeekingChanged();
}

int QVideoCaptureThread::captureWidth() const{
    Q_D(const QVideoCaptureThread);
    return d->width;
}

int QVideoCaptureThread::captureHeight() const{
    Q_D(const QVideoCaptureThread);
    return d->height;
}

double QVideoCaptureThread::captureFps() const{
    Q_D(const QVideoCaptureThread);
    if ( d->capture->isOpened())
        return d->capture->get(CV_CAP_PROP_FPS);
    return 0;
}
