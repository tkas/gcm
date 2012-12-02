#include "ProgressDialog.h"
#include "ui_ProgressDialog.h"
#include <util/Log.h>

ProgressDialog::ProgressDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ProgressDialog),
	value(0),
	maxVal(-1),
	maxChanged(true),
	cancellable(false),
	iscancelled(false)
{
	ui->setupUi(this);
	this->timer = new QTimer(this);
	this->timer->setInterval(100);
	this->ui->btnCancel->setVisible(false);

	connect(this->timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	connect(this->ui->btnCancel, SIGNAL(pressed()), this, SLOT(onCancel()));

	this->setWindowFlags(
		Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::CustomizeWindowHint
	);
}

ProgressDialog::~ProgressDialog()
{
	delete ui;
}

void ProgressDialog::onCancel() {
	this->iscancelled = true;
	this->events.trigger("cancelled", NULL);
	GCM::util::Log::d("Qt4|ProgressDialog", "Cancel button pressed.");
}

bool ProgressDialog::isCancellable() {
	return this->cancellable;
}

bool ProgressDialog::isCancelled() {
	return this->iscancelled;
}

void ProgressDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void ProgressDialog::setCancellable(bool cancellable) {
	this->cancellable = cancellable;
	this->ui->btnCancel->setVisible(cancellable);
}

void ProgressDialog::showEvent(QShowEvent *e) {
	QDialog::showEvent(e);
	this->timer->start();
}

void ProgressDialog::hideEvent(QHideEvent *e) {
	this->timer->stop();
	QDialog::hideEvent(e);
}

void ProgressDialog::setMessage(const char *text) {
	this->message = QString::fromUtf8(text);
	if (!this->timer->isActive()) {
		this->ui->label->setText(this->message);
	}
}

void ProgressDialog::setProgress(int value) {
	this->value = value;
	if (!this->timer->isActive()) {
		this->ui->progressBar->setValue(value);
	}
}

void ProgressDialog::setMaxProgress(int value) {
	//this->ui->progressBar->setMaximum(value);
	this->maxVal = value;
	this->maxChanged = true;
}

void ProgressDialog::onTimer() {
	if (maxChanged) {
		this->ui->progressBar->setMaximum(this->maxVal);
	}

	this->ui->label->setText(this->message);
	this->ui->progressBar->setValue(this->value);
	this->update();
}

void ProgressDialog::accept() {
}

void ProgressDialog::reject() {
}
