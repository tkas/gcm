#include "SearchDialog.h"
#include "ui_SearchDialog.h"

SearchDialog::SearchDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SearchDialog)
{
	ui->setupUi(this);
}

SearchDialog::~SearchDialog()
{
	delete ui;
}

void SearchDialog::changeEvent(QEvent *e)
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

void SearchDialog::accept() {
	if (this->ui->searchString->text() != "") {
		QDialog::accept();
	} else {
		this->reject();
	}
}

QString SearchDialog::getSearchString() {
	return this->ui->searchString->text();
}
