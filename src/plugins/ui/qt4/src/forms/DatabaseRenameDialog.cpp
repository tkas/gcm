#include "DatabaseRenameDialog.h"
#include "ui_DatabaseRenameDialog.h"

DatabaseRenameDialog::DatabaseRenameDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DatabaseRenameDialog)
{
	ui->setupUi(this);
}

DatabaseRenameDialog::~DatabaseRenameDialog()
{
	delete ui;
}

void DatabaseRenameDialog::changeEvent(QEvent *e)
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

QString DatabaseRenameDialog::getDatabaseName() {
	return this->ui->dbName->text();
}

void DatabaseRenameDialog::setDatabaseName(QString dbName) {
	this->ui->dbName->setText(dbName);

	QString title;
	title.sprintf("Rename database %s", dbName.toUtf8().data());
	this->setWindowTitle(title);
}
