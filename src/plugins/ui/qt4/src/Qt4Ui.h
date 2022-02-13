#ifndef QT4UI_H
#define QT4UI_H

#include <QtGui/QGuiApplication>
#include <QtWidgets>
#include <ui.h>
#include "forms/MainWindow.h"

class Qt4Ui: public GCM::ui::UI {
private:
	QApplication *app;
	MainWindow *mainWindow;

public:
	Qt4Ui(int argc, char **argv);
	virtual void init();
	virtual void processEvents();
	virtual void start();
	virtual ~Qt4Ui();
	virtual ptrDialogBuilder getDialogBuilder();
};


#endif // QT4UI_H
