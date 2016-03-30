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

	void on_openBtn_clicked();  //�򿪴���
	void on_sendBtn1_clicked();  // ���Ͱ�ť����
	void on_sendBtn2_clicked();
	void on_downBtn_clicked(); //�رմ���
	void receive_data();       //�������ݿ�

	void on_btnShutDownCam_clicked();//�ϵ�
	void on_btnSaveCamConfig_clicked();//��������

	void on_minButton_clicked(); 
	void on_closeButton_clicked();


private:
	Ui::settingwindow ui;
	///�������sd���е�ͼ��ʱ��pc��ѡ��ı���·�����ܺ��пո�
	///�ú������ѡ���·�����Ƿ��пո�
	bool checkDirValid(QString imgDownloadDir);
	///windows��·����б�ܺ�linux�е�б�����෴��
	///�ú�����������sd��ͼƬ����·���е�б��
	QString getValidDir(QString imgDownloadDir);
	///ͼ������·��
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
