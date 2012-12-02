#include "Qt4Ui.h"
#include "Qt4DialogBuilder.h"
#include "main.h"

#include "tasks/InitTask.h"

Qt4Ui::Qt4Ui(int argc, char **argv): UI(argc, argv), app(NULL), mainWindow(NULL) {
}

void Qt4Ui::init() {
	this->app = new QApplication(this->argc, this->argv);
	this->mainWindow = new MainWindow();
}

void Qt4Ui::processEvents() {
	QApplication::processEvents();
	UI::processEvents();
}

void Qt4Ui::start() {
	//this->mainWindow->show();
	this->addTask(new InitTask(this->mainWindow));
	UI::start();
}

Qt4Ui::~Qt4Ui() {
	if (mainWindow != NULL) {
		delete this->mainWindow;
	}

	if (app != NULL) {
		delete this->app;
	}
}

ptrDialogBuilder Qt4Ui::getDialogBuilder() {
	return new Qt4DialogBuilder(this->mainWindow);
}
