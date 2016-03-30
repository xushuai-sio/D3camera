#include "settingwindow.h"
#include "d3cam.h"
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QtWidgets>
#include <QDebug>
#include <cstdlib>
#include <iostream>

settingwindow::settingwindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	init();

	putty = "./putty.exe";

	ui.sliderExp->setRange(0, 100 * EXPO_TIMES);
	ui.sliderGain->setRange(0, 3);



}

settingwindow::~settingwindow()
{

}

void settingwindow::on_downloadImgBtn_clicked()
{
	QDir dir(imgDownloadDir);
	if (!dir.exists() || imgDownloadDir.isEmpty())
	{

		QMessageBox::warning(this,tr("warning!"),QStringLiteral("下载路径设置出错"));
		return;
	}

	if (!checkDirValid(imgDownloadDir))
	{
		QMessageBox::warning(this, tr("warning!"), QStringLiteral("请勿包含空格"));
		return;
	}

	QString imgDir = getValidDir(imgDownloadDir);

	QString str = "WinSCP.exe /console "
		+ tr("/command ")
		+ "\"option batch continue\" "
		+ "\"option confirm off\" "
		+ "\"open root:vrmagic@192.168.0.2:22\" "
		+ "\"option transfer binary\" "
		+ tr("\"get /mnt/sdc/ %1\"").arg(imgDir);

	system(str.toStdString().c_str());
}

void settingwindow::on_btnDeleteImgs_clicked()
{
	QMessageBox box;
	box.setIcon(QMessageBox::Warning);
	box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	box.setDefaultButton(QMessageBox::Yes);
	box.setText(QStringLiteral("是否删除sd卡上的所有图片？"));
	if (box.exec() == QMessageBox::No)
	{
		return;
	}

	QString str = "WinSCP.exe /console "
		+ tr("/command ")
		+ "\"option batch continue\" "
		+ "\"option confirm off\" "
		+ "\"open root:vrmagic@192.168.0.2:22\" "
		+ "\"option transfer binary\" "
		+ tr("\"rm /mnt/sdc/*.* \"");

	int k = system(str.toStdString().c_str());
}

void settingwindow::on_uploadProgramBtn_clicked()
{
	QString path = QFileDialog::getOpenFileName(this,
		tr("choose"),
		".",
		0);
	QFile file(path);
	if (!file.exists())
	{
		QMessageBox::warning(this, tr("warning"), QStringLiteral("未选择任何程序！"));
		return;
	}

	//winscp
	QString str = "WinSCP.exe /console "
		+ tr("/command ")
		+ "\"option batch continue\" "
		+ "\"option confirm off\" "
		+ "\"open root:vrmagic@192.168.0.2:22\" "   //root：vrmagic->账号：密码
		+ "\"option transfer binary\" "
		+ tr("\"put %1 /root/Trigger_img\"").arg(path);

	system(str.toStdString().c_str());

	//showMsg(QStringLiteral("上传完成"));
}

void settingwindow::on_btnChooseImgDownloadDir_clicked()
{
	QString saveDir = QFileDialog::getExistingDirectory(this, QStringLiteral("选择文件夹"),
		lastDir,
		QFileDialog::ShowDirsOnly);

	if (!saveDir.isEmpty()){
		lastDir = saveDir;
	}

	imgDownloadDir = saveDir;
	ui.editImgDownloadDir->setText(saveDir);
}

void settingwindow::on_sliderExp_valueChanged(int expTime)
{
	if (NULL != vrCam){
		float value = (float)expTime / EXPO_TIMES;
		vrCam->setExposure(value);
		ui.editExp->setText(tr("%1").arg(value));
	}
}


void settingwindow::on_editExp_returnPressed()
{
	if (NULL != vrCam){
		float value = ui.editExp->text().toFloat();
		vrCam->setExposure(value);
		ui.sliderExp->setValue(value * EXPO_TIMES);
	}
}

void settingwindow::on_sliderGain_valueChanged(int gain)
{
	if (NULL != vrCam){
		vrCam->setGain(gain);
		ui.editGain->setText(tr("%1").arg(gain));
	}
}

void settingwindow::on_editGain_returnPressed()
{
	if (NULL != vrCam){
		int value = ui.editGain->text().toInt();
		vrCam->setGain(value);
		ui.sliderGain->setValue(value);
	}
}

void settingwindow::on_radioBtnAutoWhiteOpen_clicked()
{
	if (NULL != vrCam){
		vrCam->setAutoBalance(true);
	}
}

void settingwindow::on_radioBtnAutoWhiteClose_clicked()
{
	if (NULL != vrCam){
		vrCam->setAutoBalance(false);
	}
}

void settingwindow::on_btnResetWhiteBalance_clicked()
{
	if (NULL != vrCam){
		vrCam->resetAutoBalance();
		
	}
}

void settingwindow::on_btnSaveCamConfig_clicked()
{
	if (NULL != vrCam){
		vrCam->saveCamConfig();
		QMessageBox::about(this, NULL, QStringLiteral("参数保存成功！"));
	}

}

void settingwindow::on_btnShutDownCam_clicked()
{


	QStringList arguments;
	arguments.append("-pw");
	arguments.append("vrmagic");
	arguments.append("-m");
	arguments.append("./shutDownFile");
	arguments.append("root@192.168.0.2");

	shutdownProc.start(putty, arguments);
	if (!shutdownProc.waitForStarted()){
		QMessageBox::warning(this, tr("warning!"), QStringLiteral("关机失败"));
		return;
	}
}

bool settingwindow::checkDirValid(QString imgDownloadDir) //图像下载路径是否有空格
{
	for (int i = 0; i < imgDownloadDir.count(); i++){
		if (imgDownloadDir[i] == ' '){
			return false;
		}
	}
	return true;
}

QString settingwindow::getValidDir(QString imgDownloadDir)
{
	QString str;
	for (int i = 0; i < imgDownloadDir.count(); i++){
		if (imgDownloadDir[i] == '/'){
			str.push_back("\\");
		}
		else{
			str.push_back(imgDownloadDir[i]);
		}
	}
	return str;
}

void settingwindow::on_openBtn_clicked()   //打开串口
{
	//定时器设置串口名、波特率等参数
	QString portName = ui.portnamecomboBox->currentText();
	my_serial->setReadBufferSize(2048);
	my_serial->setPortName(portName);  //串口名
	my_serial->setBaudRate(9600);      //波特率
	my_serial->setDataBits(QSerialPort::Data8);
	my_serial->setParity(QSerialPort::NoParity);
	my_serial->setStopBits(QSerialPort::OneStop);
	my_serial->setFlowControl(QSerialPort::NoFlowControl);
	connect(my_serial, &QIODevice::readyRead, this, &settingwindow::receive_data);
	my_serial->open(QIODevice::ReadWrite);

	ui.openBtn->setEnabled(false);
	ui.downBtn->setEnabled(true);
	ui.sendBtn1->setEnabled(true);
	ui.sendBtn2->setEnabled(true);
	ui.textBrowser_send1->setEnabled(true);
	ui.textBrowser_send2->setEnabled(true);
	ui.textBrowser_receive->setEnabled(true);
	ui.portnamecomboBox->setEnabled(false);
}

void settingwindow::on_sendBtn1_clicked()  //发送工作时间给串口
{
	QString  send_data;
	send_data = ui.textBrowser_send1->toPlainText();
	send_data += "\r\n";
	QByteArray temp;
	temp = send_data.toLatin1();
	char *ch;
	ch = temp.data();
	my_serial->write(ch);
}

void settingwindow::on_sendBtn2_clicked()  //发送间隔时间给串口
{
	QString send_data;
	send_data = ui.textBrowser_send2->toPlainText();
	send_data += "\r\n";
	QByteArray temp;
	temp = send_data.toLatin1();
	char *ch;
	ch = temp.data();
	my_serial->write(ch);
}

void settingwindow::receive_data()  //接收串口返回数据
{
	QByteArray array = my_serial->readAll();
	ui.textBrowser_receive->append(array);
}

void settingwindow::on_downBtn_clicked()  //关闭串口
{
	my_serial->close();
	ui.openBtn->setEnabled(true);
	ui.downBtn->setEnabled(false);
	ui.sendBtn1->setEnabled(false);
	ui.sendBtn2->setEnabled(false);
	ui.textBrowser_send1->setEnabled(false);
	ui.textBrowser_send2->setEnabled(false);
	ui.textBrowser_receive->setEnabled(false);
	ui.portnamecomboBox->setEnabled(true);
}

void settingwindow::on_minButton_clicked()
{
	this->showMinimized();
}

void settingwindow::on_closeButton_clicked()
{
	this->close();
}

void settingwindow::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		mouse_press = true;
		move_point = event->pos();
	}
}

void settingwindow::mouseMoveEvent(QMouseEvent *event)
{
	if (mouse_press)
	{
		QPoint move_pos = event->globalPos();
		this->move(move_pos - move_point);
	}
}

void settingwindow::mouseReleaseEvent(QMouseEvent *event)
{
	mouse_press = false;
}

void settingwindow::init()
{
	ui.downBtn->setEnabled(false);
	ui.sendBtn1->setEnabled(false);
	ui.sendBtn2->setEnabled(false);
	ui.textBrowser_send1->setEnabled(false);
	ui.textBrowser_send2->setEnabled(false);
	ui.textBrowser_receive->setEnabled(false);

	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);//无边框
	setMouseTracking(true);
	int width = this->width();
	QPixmap minPix = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
	QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
	ui.minButton->setIcon(minPix);
	ui.closeButton->setIcon(closePix);
	ui.minButton->setGeometry(width - 46, 5, 20, 10);
	ui.closeButton->setGeometry(width - 25, 5, 20, 10);
	ui.minButton->setStyleSheet("background-color:transparent;");
	ui.closeButton->setStyleSheet("background-color:transparent;");

	QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");//窗口淡出
	animation->setDuration(2500);
	animation->setStartValue(0);
	animation->setEndValue(1);
	animation->start();


}

