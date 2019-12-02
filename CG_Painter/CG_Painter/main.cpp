#include "CG_Painter.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CG_Painter w;
	w.show();	
	return a.exec();
}
