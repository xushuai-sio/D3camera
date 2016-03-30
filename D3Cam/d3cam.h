#ifndef D3CAM_H
#define D3CAM_H

#define EXPO_TIMES 100
#include "settingwindow.h"
#include <QtWidgets/QDialog>
#include "ui_d3cam.h"
#include "VR_Head.h"
#include <QTimer>
#include "VRCam.h"
#include "ProcessThread.h"
#include <QProcess>
#include "CaptureThread.h"
#include <QList>


class D3Cam : public QDialog
{
	Q_OBJECT

public:
	D3Cam(QWidget *parent = 0);
	~D3Cam();

	private slots:
	void on_startSysBtn_clicked();
	void on_stopSysBtn_clicked();
	void on_btnSaveImg_clicked();
	void on_settingBtn_clicked();

	//从CaptureThread接收处理好的图像数据
	void updateFrame(const QImage& frame);
	///向消息窗口显示消息
	void showMsg(QString msg);
	void deviceConnect(bool isConnect);
	void on_helpBtn_clicked();

private:
	Ui::D3CamClass ui;
	///释放相机内存，并关闭相机
	bool releaseVRcam();
	///创建VRcam对象，并建立信号和槽
	QString getSaveFileName();
	void createVRCam();
	settingwindow setting;

	///获得当前时间的字符串，格式  h:m:s.ms
	QString getCurrentTime();
	///获得当前时间的字符串，格式  年月日-时分秒
	QString getCurrentTime2();
	///图像缓存
	MyBuffer<QImage>* imgBuffer;
	///相机对象
	VRCam* vrCam;
	///图像处理线程
	ProcessThread* processThread;
	///图像采集线程
	CaptureThread* captureThread;
	///相机是否初始化标志位
	bool systemInited;

	QString lastDir;
	
};

#endif // D3CAM_H
