#include "Qt4DialogBuilder.h"
#include "Qt4StartupDialog.h"
#include "Qt4ProgressDialog.h"

Qt4DialogBuilder::Qt4DialogBuilder(MainWindow *window): GCM::ui::DialogBuilder(), mainWindow(window) {
}

ptrDialog Qt4DialogBuilder::buildStartupDialog() {
	return new Qt4StartupDialog(mainWindow);
}

ptrDialog Qt4DialogBuilder::buildProgressDialog() {
	return new Qt4ProgressDialog(mainWindow);
}

ptrDialog Qt4DialogBuilder::buildAlertDialog() {
	return NULL;
}

