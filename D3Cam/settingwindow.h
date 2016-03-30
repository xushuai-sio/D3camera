#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#define EXPO_TIMES 100

#include <QWidget>
#include "ui_settingwindow.h"
#include <QProcess>
#include "VRCam.h"
#include <QtSerialPort/QSerialPort>
/*namespace ui{
	class settingwindow;
}*/

class settingwindow : public QWidget
{
	Q_OBJECT

public:
	settingwindow(QWidget *parent = 0);
	~settingwindow();
	private slots:

	void on_sliderExp_valueChanged(int expTime);
	void on_editExp_returnPressed();
	void on_sliderGain_valueChanged(int gain);
	void on_editGain_returnPressed();
	void on_radioBtnAutoWhiteOpen_clicked();
	void on_radioBtnAutoWhiteClose_clicked();
	void on_btnResetWhiteBalance_clicked();

	void on_btnChooseImgDownloadDir_clicked();
	void on_downloadImgBtn_clicked();
	void on_btnDeleteImgs_clicked();

	void on_uploadProgramBtn_clicked();

	void on_openBtn_clicked();  //打开串口
	void on_sendBtn1_clicked();  // 发送按钮数据
	void on_sendBtn2_clicked();
	void on_downBtn_clicked(); //关闭串口
	void receive_data();       //接收数据框

	void on_btnShutDownCam_clicked();//断电
	void on_btnSaveCamConfig_clicked();//保存设置

	void on_minButton_clicked(); 
	void on_closeButton_clicked();


private:
	Ui::settingwindow ui;
	///下载相机sd卡中的图像时，pc上选择的保存路径不能含有空格
	///该函数检查选择的路径中是否含有空格
	bool checkDirValid(QString imgDownloadDir);
	///windows中路径的斜杠和linux中的斜杠是相反的
	///该函数用来调整sd卡图片下载路径中的斜杠
	QString getValidDir(QString imgDownloadDir);
	///图像下载路径
	VRCam* vrCam;
	QSerialPort *my_serial = new QSerialPort;
	QString putty;
	QString imgDownloadDir;
	QProcess shutdownProc;
	QString lastDir;

	QPoint move_point;
	bool mouse_press;
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void init();
	
};

#endif // SETTINGWINDOW_H
