#include "CaptureThread.h"

CaptureThread::CaptureThread(VRCamDevice vrCamDevice,
	MyBuffer<QImage>* imgBuffer)
	: QThread()
	, vrCamDevice(vrCamDevice)
	, imgBuffer(imgBuffer)
{
	doStop = false;
	doSaveImg = false;
	doPrepareImg4Save = false;
	connect(this, &CaptureThread::finished, this, &CaptureThread::deleteLater);
}

CaptureThread::~CaptureThread()
{

}

void CaptureThread::run()
{
	VRmUsbCamStart(vrCamDevice.device);
	VRmImage* p_source_img = 0;
	VRmDWORD frames_dropped = 0;
	QImage img;

	while (true)
	{
		doStopMutex.lock();
		if (doStop)
		{
			doStop = false;
			(VRmUsbCamStop(vrCamDevice.device));
			doStopMutex.unlock();
			break;
		}
		doStopMutex.unlock();

		if (VRmUsbCamLockNextImageEx2(vrCamDevice.device, vrCamDevice.port, &p_source_img, &frames_dropped, 1000))
		{
			VRmImage* p_target_img = 0;
			VRmUsbCamNewImage(&p_target_img, vrCamDevice.target_format);
			VRmUsbCamConvertImage(p_source_img, p_target_img);
			VRmUsbCamUnlockNextImage(vrCamDevice.device, &p_source_img);

			img = QImage(p_target_img->mp_buffer,
				vrCamDevice.target_format.m_width,
				vrCamDevice.target_format.m_height,
				p_target_img->m_pitch,
				QImage::Format_ARGB32_Premultiplied);

			imgBuffer->add(img.copy(), true);
			if (doPrepareImg4Save){
				img4Save = img.copy();
				doPrepareImg4Save = false;
			}
			VRmUsbCamFreeImage(&p_target_img);
		}
		else{
			continue;
		}

		if (doSaveImg)
		{
			if (img4Save.save(savePath))
			{
				sendMsg(QStringLiteral("%1-保存成功").arg(savePath));
			}
			else
				sendMsg(QStringLiteral("%1-保存失败").arg(savePath));
			doSaveImg = false;
		}
	}
}

void CaptureThread::stop()
{
	QMutexLocker locker(&doStopMutex);
	doStop = true;
}

void CaptureThread::saveImg(const QString& imgPath)
{
	QMutexLocker locker(&grabMutex);
	doSaveImg = true;
	savePath = imgPath;
}

void CaptureThread::prepareImg4Save()
{
	QMutexLocker locker(&grabMutex);
	doPrepareImg4Save = true;
}