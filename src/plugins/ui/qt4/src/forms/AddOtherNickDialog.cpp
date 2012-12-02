#include "AddOtherNickDialog.h"
#include "ui_AddOtherNickDialog.h"

AddOtherNickDialog::AddOtherNickDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AddOtherNickDialog),
	editMode(false),
	item(NULL)
{
	ui->setupUi(this);
}

AddOtherNickDialog::AddOtherNickDialog(QListWidgetItem *item, QWidget *parent):
	QDialog(parent),
	ui(new Ui::AddOtherNickDialog),
	editMode(true),
	item(item)
{
	ui->setupUi(this);
}

AddOtherNickDialog::~AddOtherNickDialog()
{
	delete ui;
}

void AddOtherNickDialog::changeEvent(QEvent *e)
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

void AddOtherNickDialog::accept() {
	if (this->editMode) {
		this->item->setText(this->ui->userName->text());
	}

	QDialog::accept();
}

QString AddOtherNickDialog::getNick() {
	return this->ui->userName->text();
}
