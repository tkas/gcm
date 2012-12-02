#include "AboutDialog.h"
#include "ui_AboutDialog.h"

#include <config.h>
#include <gcm.h>

AboutDialog::AboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AboutDialog)
{
	ui->setupUi(this);
	this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	connect(this->ui->btnClose, SIGNAL(clicked()), this, SLOT(onBtnCloseClick()));

	this->ui->labelVersion->setText(QString(gcm_version()));
}

AboutDialog::~AboutDialog()
{
	delete ui;
}

void AboutDialog::changeEvent(QEvent *e)
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

void AboutDialog::mouseEvent(QMouseEvent *e) {
	if (e->button() == Qt::LeftButton) {
		emit this->accept();
	}
}

void AboutDialog::onBtnCloseClick() {
	emit this->accept();
}
