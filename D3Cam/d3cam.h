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

	//��CaptureThread���մ���õ�ͼ������
	void updateFrame(const QImage& frame);
	///����Ϣ������ʾ��Ϣ
	void showMsg(QString msg);
	void deviceConnect(bool isConnect);
	void on_helpBtn_clicked();

private:
	Ui::D3CamClass ui;
	///�ͷ�����ڴ棬���ر����
	bool releaseVRcam();
	///����VRcam���󣬲������źźͲ�
	QString getSaveFileName();
	void createVRCam();
	settingwindow setting;

	///��õ�ǰʱ����ַ�������ʽ  h:m:s.ms
	QString getCurrentTime();
	///��õ�ǰʱ����ַ�������ʽ  ������-ʱ����
	QString getCurrentTime2();
	///ͼ�񻺴�
	MyBuffer<QImage>* imgBuffer;
	///�������
	VRCam* vrCam;
	///ͼ�����߳�
	ProcessThread* processThread;
	///ͼ��ɼ��߳�
	CaptureThread* captureThread;
	///����Ƿ��ʼ����־λ
	bool systemInited;

	QString lastDir;
	
};

#endif // D3CAM_H
