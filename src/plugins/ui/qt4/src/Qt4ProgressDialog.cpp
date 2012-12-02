#include "Qt4ProgressDialog.h"
#include <util/Log.h>

Qt4ProgressDialog::Qt4ProgressDialog(QWidget *wnd): GCM::ui::Dialog(), dialog(NULL), wnd(wnd) {
}

Qt4ProgressDialog::~Qt4ProgressDialog() {
	if (this->dialog != NULL) {
		delete this->dialog;
	}
}

void Qt4ProgressDialog::show() {
	if (this->dialog == NULL) {
		this->dialog = new ProgressDialog(wnd);
		this->dialog->setCancellable(this->isCancellable());
		this->dialog->events.bind("cancelled", new GCM::Callback<Qt4ProgressDialog>(this, &Qt4ProgressDialog::onCancel));
	}

	this->dialog->setMessage(this->getMessage()->c_str());
	this->dialog->setMaxProgress(this->getMaxProgress());
	this->dialog->show();
}

void Qt4ProgressDialog::setMaxProgress(int maxProgress) {
	GCM::ui::Dialog::setMaxProgress(maxProgress);
	if (this->dialog != NULL) {
		this->dialog->setMaxProgress(maxProgress);
	}
}

void Qt4ProgressDialog::update() {
	if (this->dialog) {
		this->dialog->setMessage(this->getMessage()->c_str());

		if (this->getMaxProgress() > 0) {
			this->dialog->setProgress(this->getProgress());
		}
	}
}

void Qt4ProgressDialog::dismiss() {
	if (this->dialog) {
		this->dialog->hide();
	}
}

void Qt4ProgressDialog::setCancellable(bool cancellable) {
	GCM::ui::Dialog::setCancellable(cancellable);

	if (this->dialog) {
		this->dialog->setCancellable(cancellable);
	}
}

void Qt4ProgressDialog::onCancel(void *) {
	GCM::util::Log::d("Qt4|Qt4ProgressDialog", "Cancelled event triggered.");
	this->setCancelled(true);
}