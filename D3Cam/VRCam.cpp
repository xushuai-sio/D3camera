#include "VRCam.h"

VRCam* VRCam::pThis = NULL;

VRCam::VRCam()
	: deviceConnected(false)
	, systemInited(false)
	, disConnectMsgShowed(false)
	, lp_page(0)
	, device(NULL)
{
	pThis = this;

	//【需要测试是用定时器还是用回调函数比较好】

	VRmUsbCamEnableLoggingEx("VRmagic.log");

}

VRCam::~VRCam(void)
{
	release();
}

void VRCam::VRCAMStaticCallback(){
	if (pThis == NULL)
		return;

	pThis->sendMsg("callback");
	//检查连接的设备
	VRmDWORD size = 0;
	(VRmUsbCamGetDeviceKeyListSize(&size));
	pThis->sendMsg(tr("%1").arg(size));

	if (size == 0){
		if (false == pThis->disConnectMsgShowed){
			pThis->sendMsg(QStringLiteral("设备未连接"));
			pThis->disConnectMsgShowed = true;
			pThis->deviceConnected = false;
		}
		if (pThis->systemInited){
			pThis->release();
			pThis->sendMsg(QStringLiteral("设备已断开"));
			emit pThis->deviceConnect(false);
		}
	}
	else{
		if (false == pThis->deviceConnected){
			pThis->sendMsg(QStringLiteral("已连接1个设备"));
			pThis->deviceConnected = true;
			pThis->disConnectMsgShowed = false;
			emit pThis->deviceConnect(true);
		}
	}
}

bool VRCam::init()
{
	if (systemInited == true)
		return false;

	atexit(VRmUsbCamCleanup);

	//更新设备列表
	if (!VRmUsbCamUpdateDeviceKeyList()){
		sendMsg("update Device failed");
		return false;        
	}

	//检查连接的设备
	VRmDWORD size = 0;
	if (!(VRmUsbCamGetDeviceKeyListSize(&size))){
		sendMsg("Get Device Size failed");
		return false;
	}

	if (size == 0){
		sendMsg("0 Cam Connect");
		return false;
	}

	sendMsg(tr("%1 Cam Find").arg(size));

	//打开第一个可用的设备
	VRmDeviceKey* p_device_key = 0;
	for (VRmDWORD i = 0; i < size; ++i)
	{
		VRMEXECANDCHECK(VRmUsbCamGetDeviceKeyListEntry(i, &p_device_key));
		if (!p_device_key->m_busy)
		{
			if (!(VRmUsbCamOpenDevice(p_device_key, &device)))
			{
				sendMsg("Open Cam Failed");
			}
		}
		else
		{
			sendMsg("Device is busy now");
		}
		VRMEXECANDCHECK(VRmUsbCamFreeDeviceKey(&p_device_key));
	}

	if (!device)
	{
		return false;
	}

	//now get the first sensor port
	VRMEXECANDCHECK(VRmUsbCamGetSensorPortListEntry(device, 0, &port));

	// get the current source format
	VRmImageFormat source_format;
	VRMEXECANDCHECK(VRmUsbCamGetSourceFormatEx(device, port, &source_format));
	const char *source_color_format_str;
	VRMEXECANDCHECK(VRmUsbCamGetStringFromColorFormat(source_format.m_color_format, &source_color_format_str));


	VRmDWORD number_of_target_formats, i;
	VRMEXECANDCHECK(VRmUsbCamGetTargetFormatListSizeEx2(device, port, &number_of_target_formats));
	for (i = 0; i < number_of_target_formats; ++i)
	{
		VRMEXECANDCHECK(VRmUsbCamGetTargetFormatListEntryEx2(device, port, i, &target_format));
		if (target_format.m_color_format == VRM_ARGB_4X8)
			break;
	}

	systemInited = true;

	return true;
}

bool VRCam::release()
{
	if (device)
	{

		if (!(VRmUsbCamCloseDevice(device)))
		{
			sendMsg("close failed");
			return false;
		}


		device = NULL;
		systemInited = false;


		return true;
	}
	else{
		return false;
	}
}

void VRCam::setExposure(float expo)
{
	VRmUsbCamSetPropertyValueF(device, VRM_PROPID_CAM_EXPOSURE_TIME_F, &expo);
}

void VRCam::setGain(int value)
{
	VRmUsbCamSetPropertyValueI(device, VRM_PROPID_CAM_GAIN_MONOCHROME_I, &value);
}

float VRCam::getExposure()
{
	float value;
	VRmUsbCamGetPropertyValueF(device, VRM_PROPID_CAM_EXPOSURE_TIME_F, &value);
	return value;
}

int VRCam::getGain()
{
	int gain;
	VRmUsbCamGetPropertyValueI(device, VRM_PROPID_CAM_GAIN_MONOCHROME_I, &gain);
	return gain;
}

void VRCam::readCamConfig()
{

	VRMEXECANDCHECK(VRmUsbCamLoadConfig(device, 1));
}

void VRCam::saveCamConfig()
{
	VRMEXECANDCHECK(VRmUsbCamSaveConfig(device, 1));
}

void VRCam::setAutoBalance(bool b)
{
	VRmBOOL _b = b ? 1 : 0;
	VRmUsbCamSetPropertyValueB(device, VRM_PROPID_PLUGIN_AUTO_WHITE_BALANCE_FILTER_B, &_b);
}

bool VRCam::getIsAutoWhite()
{
	VRmBOOL _b;
	VRmUsbCamGetPropertyValueB(device, VRM_PROPID_PLUGIN_AUTO_WHITE_BALANCE_FILTER_B, &_b);
	return _b == 1 ? true : false;
}

void VRCam::checkDeviceConnect()
{

	VRmUsbCamUpdateDeviceKeyList();
	sendMsg("check");

}

void VRCam::resetAutoBalance()
{
	
	VRmBOOL _b = 0;
	VRmUsbCamSetPropertyValueB(device, VRM_PROPID_PLUGIN_AUTO_RESET_LEVEL_B, &_b);
}

HWND VRCam::getPropertyPageHandle()
{
	return lp_page->m_handle;
}

VRCamDevice VRCam::getVRCamDevice()
{
	vrCamDevice.device = device;
	vrCamDevice.port = port;
	vrCamDevice.target_format = target_format;
	return vrCamDevice;
}


