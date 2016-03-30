#pragma once
#include <QThread>
#include "MyBuffer.h"
#include "VR_Head.h"
#include <QImage>
#include <QTimer>
#include <vector>


struct VRCamDevice
{
	VRmUsbCamDevice device;
	VRmImageFormat target_format;
	VRmDWORD port;
};

class VRCam : public QObject
{
	Q_OBJECT

public:
	VRCam();
	~VRCam(void);

	bool init();
	bool release();

	VRCamDevice getVRCamDevice();

	void setExposure(float expo);
	float getExposure();
	void setGain(int value);
	int getGain();
	bool getIsAutoWhite();
	void setAutoBalance(bool b);
	void resetAutoBalance();

	void readCamConfig();
	void saveCamConfig();

	void checkDeviceConnect();

	HWND getPropertyPageHandle();

	static VRCam* pThis;
	static void VRCAMStaticCallback();

signals:
	void sendMsg(QString msg);
	void deviceConnect(bool isConnected);

private:
	QString savePath;
	QMutex grabMutex;


	VRCamDevice vrCamDevice;
	VRmUsbCamDevice device;
	VRmImageFormat target_format;
	VRmDWORD port;

	bool deviceConnected;
	bool disConnectMsgShowed;
	bool systemInited;
	VRmStaticCallback fp_callback;
	VRmDevicePropertyPage* lp_page;
	QTimer timer;
};

