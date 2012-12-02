#ifndef QT4DIALOGBUILDER_H
#define QT4DIALOGBUILDER_H

#include <ui.h>
#include "forms/MainWindow.h"

class Qt4DialogBuilder: public GCM::ui::DialogBuilder {
private:
	MainWindow *mainWindow;

public:
	Qt4DialogBuilder(MainWindow *mainWindow);
	virtual ptrDialog buildStartupDialog();
	virtual ptrDialog buildProgressDialog();
	virtual ptrDialog buildAlertDialog();
};


#endif // QT4DIALOGBUILDER_H
