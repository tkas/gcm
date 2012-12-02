#include "CreateDatabaseDialog.h"
#include "ui_CreateDatabaseDialog.h"

#include <QMessageBox>

#include <pluginmanager/PluginManager.h>
#include <database/Database.h>
#include <stl/List.h>

using namespace GCM;
using namespace GCM::pluginmanager;
using namespace GCM::database;

CreateDatabaseDialog::CreateDatabaseDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CreateDatabaseDialog)
{
	ui->setupUi(this);

	PluginManager::PluginFunctionList &funcs = PluginManager::Instance()->listPlugins("Database");
	PluginManager::PluginFunctionList::iterator i = funcs.begin();
	while (i != funcs.end()) {
		GCM::GC< List<DatabaseInfo> > info = ((fDatabase)(i->second))();

		List<DatabaseInfo>::iterator ii = info->begin();
		while (ii != info->end()) {
			allInfo.append(*ii);
			ii++;
		}

		i++;
	}

	ArrayList<DatabaseInfo>::iterator ii = allInfo.begin();
	while (ii != allInfo.end()) {
		this->ui->databaseFormat->addItem(QString::fromUtf8((*ii)->name->c_str()));
		ii++;
	}
}

CreateDatabaseDialog::~CreateDatabaseDialog()
{
	delete ui;
}

void CreateDatabaseDialog::accept() {
	if (this->ui->databaseName->text().trimmed() == "") {
		this->ui->databaseName->setFocus(Qt::OtherFocusReason);
		QMessageBox msg(this);
		msg.setWindowTitle("Error");
		msg.setText("You must provide database name.");
		msg.setIcon(QMessageBox::Critical);
		msg.exec();
		return;
	}

	QDialog::accept();
}

void CreateDatabaseDialog::changeEvent(QEvent *e)
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

QString CreateDatabaseDialog::getDatabaseName() {
	return this->ui->databaseName->text();
}

GCM::GC<database::DatabaseInfo> CreateDatabaseDialog::getDatabaseInfo() {
	if (this->ui->databaseFormat->currentIndex() >= 0) {
		return this->allInfo[this->ui->databaseFormat->currentIndex()];
	} else {
		return NULL;
	}
}
