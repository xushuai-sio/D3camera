#include "ProcessThread.h"

ProcessThread::ProcessThread(MyBuffer<QImage>* imgBuffer)
	: QThread()
	, imgBuffer(imgBuffer)
{
	doStop = false;
	doImgProcess = false;
	doProbePnt = false;
	connect(this, &ProcessThread::finished, this, &ProcessThread::deleteLater);
}

ProcessThread::~ProcessThread()
{

}

void ProcessThread::run()
{
	while (1)
	{
		doStopMutex.lock();
		if (doStop)
		{
			doStop = false;
			doStopMutex.unlock();
			break;
		}
		doStopMutex.unlock();
		currentframe = imgBuffer->get();

		processingMutex.lock();
		if (doImgProcess)
		{

		}
		// frame = QtCommon::mat2QImage(currentFrame);

		processingMutex.unlock();

		emit newFrame(currentframe);
	}
}

void ProcessThread::stop()
{
	QMutexLocker locker(&doStopMutex);
	doStop = true;
}

void ProcessThread::enablePImgProcess(bool doProcess)
{
	QMutexLocker locker(&doImgProcessMutex);
	doImgProcess = doProcess;
}