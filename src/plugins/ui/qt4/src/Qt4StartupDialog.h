#ifndef QT4STARTUPDIALOG_H
#define QT4STARTUPDIALOG_H

#include <ui.h>
#include "forms/StartupDialog.h"
#include "forms/MainWindow.h"

class Qt4StartupDialog: public GCM::ui::Dialog {
private:
	StartupDialog *dialog;
	MainWindow *wnd;

public:
	Qt4StartupDialog(MainWindow *wnd);
	virtual void show();
	virtual void update();
	virtual void dismiss();
};

#endif // QT4STARTUPDIALOG_H
