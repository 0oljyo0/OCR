#include "MyDesign.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MyDesign w;
	w.show();
	return a.exec();
}
