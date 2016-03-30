#include "d3cam.h"
#include "settingwindow.h"
#include <QDebug>
#include <cstdlib>
#include <iostream>
#include <QTime>
#include <QDialog>
#include <QFileDialog>
#include <QWindow>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QList>
#include <QtWidgets>

void LogExit()
{
	std::cerr << "VRmUsbCam Error:" << VRmUsbCamGetLastError();
}

D3Cam::D3Cam(QWidget *parent)
	: QDialog(parent)
	, vrCam(NULL)
	, processThread(NULL)
	, captureThread(NULL)
	, imgBuffer(NULL)
	, systemInited(false)

{
	ui.setupUi(this);
	ui.viewLabel->setScaledContents(true);
	
	//ui.settingBtn->setEnabled(false);
	this->setFixedSize(771,568);
	imgBuffer = new MyBuffer<QImage>(10);
}

D3Cam::~D3Cam()
{
	releaseVRcam();
}

bool D3Cam::releaseVRcam()
{
	if (systemInited == false)
	{
		return false;
	}

	if (captureThread != NULL)
	{
		if (captureThread->isRunning()){
			captureThread->stop();
			captureThread->wait();
			captureThread = NULL;
		}
	}
	if (processThread != NULL)
	{
		if (processThread->isRunning())
		{
			processThread->stop();
			processThread = NULL;
		}
	}

	if (vrCam != NULL)
	{
		if (!vrCam->release())
		{
			return false;
		}
		delete vrCam;
		vrCam = NULL;
	}

	systemInited = false;
	return true;
}

void D3Cam::on_settingBtn_clicked()
{
	setting.show();

}

void D3Cam::on_startSysBtn_clicked()
{
	createVRCam();
	if (vrCam->init())
	{
		showMsg(QStringLiteral("相机初始化成功！"));
		
		ui.btnSaveImg->setEnabled(true);
		systemInited = true;
	}
	else
	{
		showMsg(QStringLiteral("相机初始化失败！"));
		delete vrCam;
		vrCam = NULL;
		return;
	}
	
	captureThread = new CaptureThread(vrCam->getVRCamDevice(), imgBuffer);
	processThread = new ProcessThread(imgBuffer);
	connect(processThread, &ProcessThread::newFrame, this, &D3Cam::updateFrame);

	captureThread->start();
	processThread->start();
	
	//gui
	ui.stopSysBtn->setEnabled(true);
	ui.startSysBtn->setEnabled(false);
	//ui.settingBtn->setEnabled(true);
}

void D3Cam::showMsg(QString msg)
{
	QString timeStr = getCurrentTime();
	ui.textEdit->setTextColor(QColor(0, 0, 255));
	ui.textEdit->insertPlainText(timeStr);
	ui.textEdit->setTextColor(QColor(0, 0, 0));
	ui.textEdit->insertPlainText(QStringLiteral("%1\n").arg(msg));
}

QString D3Cam::getCurrentTime()
{
	QTime time;
	int hour = time.currentTime().hour();
	int minute = time.currentTime().minute();
	int second = time.currentTime().second();
	int msec = time.currentTime().msec();

	return QString("%1:%2:%3.%4> ").arg(hour).arg(minute).arg(second).arg(msec, 3);
}

void D3Cam::on_stopSysBtn_clicked()
{
	if (releaseVRcam())
	{
		showMsg(QStringLiteral("断开成功！"));
		ui.stopSysBtn->setEnabled(false);
		ui.startSysBtn->setEnabled(true);
		
		ui.btnSaveImg->setEnabled(true);
		systemInited = false;
	}
	else
	{
		showMsg(QStringLiteral("断开失败！"));
	}
}

void D3Cam::updateFrame(const QImage& frame)
{
	ui.viewLabel->setPixmap(QPixmap::fromImage(frame));
}

void D3Cam::on_btnSaveImg_clicked()
{
	if (NULL != captureThread){
		captureThread->prepareImg4Save();
		QString fileName = getSaveFileName();
		if (fileName.isEmpty()){
			return;
		}
		captureThread->saveImg(fileName);
	}
}

QString D3Cam::getSaveFileName()
{
	QString fileName = QFileDialog::getSaveFileName(this, QStringLiteral("保存截图"),
		lastDir + getCurrentTime2() + ".bmp",
		tr("Images(*.bmp *.png)"));

	if (!fileName.isEmpty())
	{
		QDir file(fileName);
		lastDir = file.absolutePath();
	}

	return fileName;
}

QString D3Cam::getCurrentTime2()
{
	QTime time;
	QDate date;

	int year = date.currentDate().year();
	int month = date.currentDate().month();
	int day = date.currentDate().day();
	int hour = time.currentTime().hour();
	int minute = time.currentTime().minute();
	int second = time.currentTime().second();

	return QString("%1-%2-%3 %4-%5-%6").arg(year).arg(month).arg(day).arg(hour).arg(minute).arg(second);
}

void D3Cam::deviceConnect(bool isConnect)
{
	if (isConnect){
		ui.startSysBtn->setEnabled(true);
	}
	else{
		ui.startSysBtn->setEnabled(false);
	}
}

void D3Cam::createVRCam()
{
	vrCam = new VRCam();
	connect(vrCam, &VRCam::deviceConnect, this, &D3Cam::deviceConnect);
	connect(vrCam, &VRCam::sendMsg, this, &D3Cam::showMsg);
}

void D3Cam::on_helpBtn_clicked()
{
	QString runPath = QCoreApplication::applicationDirPath(); //获得当前路径
	QString helpName = "help.pdf";
	QString helpPath = QString("%1/%2").arg(runPath).arg(helpName);
	QFile bfilePath(helpPath);
	if (!bfilePath.exists())
	{
		return;
	}
	QString filePath = "file:///" + helpPath;
	QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}