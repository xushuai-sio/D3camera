#ifndef PROCESSTHREAD_H
#define PROCESSTHREAD_H

#include <QThread>
#include "MyBuffer.h"
#include <QImage>

class ProcessThread : public QThread
{
	Q_OBJECT

public:
	ProcessThread(MyBuffer<QImage>* imgBuffer);
	~ProcessThread();

	void stop();
	void enablePImgProcess(bool doProcess);

protected:
	void run();

signals:
	void newFrame(const QImage &frame);
	void sendMsg(QString msg);

private:
	MyBuffer<QImage>* imgBuffer;
	QMutex processingMutex;
	//Mat currentFrame;
	QImage currentframe;

	QMutex doStopMutex;
	QMutex doImgProcessMutex;
	QMutex doProbePntMutex;
	volatile bool doStop;
	volatile bool doImgProcess;
	volatile bool doProbePnt;
};

#endif // PROCESSTHREAD_H