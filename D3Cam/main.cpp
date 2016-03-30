#include "d3cam.h"
#include <QtWidgets/QApplication>
#include "settingwindow.h"
#include <QSplashScreen>
#include <QElapsedTimer>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QPixmap pixmap("splashlogo.png");
	QSplashScreen screen(pixmap);
	screen.show();

	QElapsedTimer t;
	t.start();
	while (t.elapsed()<1000)
	{
		QCoreApplication::processEvents();
	}

	D3Cam w;
	screen.finish(&w);
	w.show();
	return a.exec();
}
  