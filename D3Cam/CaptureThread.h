#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QThread>
#include "MyBuffer.h"
#include "VRCam.h"

class CaptureThread : public QThread
{
	Q_OBJECT

public:
	CaptureThread(VRCamDevice vrCamDevice,
		MyBuffer<QImage>* imgBuffer);
	~CaptureThread();

	void stop();
	void saveImg(const QString& imgPath);
	void prepareImg4Save();

private:
	QString savePath;
	QMutex grabMutex;
	MyBuffer<QImage>* imgBuffer;
	VRCamDevice vrCamDevice;

	QMutex doStopMutex;
	volatile bool doStop;
	volatile bool doSaveImg;
	volatile bool doPrepareImg4Save;

	QString proStartDate;
	QString featureName;

	QImage img4Save;

protected:
	void run();

signals:
	void sendMsg(QString msg);
};

#endif // CAPTURETHREAD_H
