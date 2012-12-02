#include "StartupDialog.h"
#include "ui_StartupDialog.h"

StartupDialog::StartupDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::StartupDialog)
{
	ui->setupUi(this);
	this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
}

StartupDialog::~StartupDialog()
{
	delete ui;
}

void StartupDialog::changeEvent(QEvent *e)
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

void StartupDialog::setMessage(const char *msg) {
	this->ui->showLabel->setText(msg);
}
