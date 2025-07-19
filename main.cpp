#include <QtWidgets/QApplication>
#include <QFontDatabase>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	
	QApplication::setApplicationVersion("1.0");
	int font_id = QFontDatabase::addApplicationFont(":/Resource/font/flache.ttf");
	QString family = QFontDatabase::applicationFontFamilies(font_id).at(0);
	QFont font;
	font.setHintingPreference(QFont::PreferNoHinting);
	font.setFamily(family);
	a.setFont(font);

	MainWindow mainWindow(argc, argv);
	mainWindow.show();
	
	return a.exec();
}