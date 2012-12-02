#include "Qt4StartupDialog.h"

Qt4StartupDialog::Qt4StartupDialog(MainWindow *wnd): GCM::ui::Dialog(), dialog(NULL), wnd(wnd) {
}

void Qt4StartupDialog::show() {
	if (this->dialog == NULL) {
		this->dialog = new StartupDialog(wnd);
	}

	this->dialog->setMessage(this->getMessage()->c_str());
	this->dialog->show();
}

void Qt4StartupDialog::update() {
	this->dialog->setMessage(this->getMessage()->c_str());
}

void Qt4StartupDialog::dismiss() {
	if (this->dialog != NULL) {
		this->dialog->hide();
		delete this->dialog;
		this->dialog = NULL;
		this->wnd->show();
	}
}
