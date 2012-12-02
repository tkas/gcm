#include "DatabaseSelectDialog.h"
#include "ui_DatabaseSelectDialog.h"

#include <QStyledItemDelegate>
#include <QListWidgetItem>
#include <QDialogButtonBox>
#include <QFileDialog>

#include <GC.h>
#include <pluginmanager/PluginManager.h>
#include <database/Database.h>
#include <util/File.h>
#include <stl/String.h>

#include "CreateDatabaseDialog.h"
#include "../main.h"
#include "../tasks/SwitchDatabaseTask.h"

using namespace GCM;
using namespace GCM::pluginmanager;
using namespace GCM::database;
using namespace GCM::io;

class DbListDelegate: public QStyledItemDelegate {
public:
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
		QSize size = QStyledItemDelegate::sizeHint(option, index);
		return QSize(size.width(), size.height() * 1.5);
	}
};


DatabaseSelectDialog::DatabaseSelectDialog(MainWindow *parent) :
	QDialog(parent),
	ui(new Ui::DatabaseSelectDialog),
	force(false),
	wnd(parent)
{
	ui->setupUi(this);

	this->ui->databaseList->setItemDelegate(new DbListDelegate());
	this->ui->databaseList->setModel(wnd->databaseModel);
	wnd->databaseModel->updateFiles();

	connect(this->ui->databaseList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onItemDblClick(QModelIndex)));
	connect(this->ui->btnCreate, SIGNAL(clicked()), this, SLOT(onDbCreateClick()));
	connect(this->ui->btnBrowse, SIGNAL(clicked()), this, SLOT(onBrowseClick()));
}

DatabaseSelectDialog::~DatabaseSelectDialog()
{
	delete ui;
}

void DatabaseSelectDialog::changeEvent(QEvent *e)
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

void DatabaseSelectDialog::onItemDblClick(QModelIndex index) {
	if (index.isValid()) {
		this->ui->databaseList->setCurrentIndex(index);
		emit this->accept();
	}
}

GCM::io::File DatabaseSelectDialog::getDatabaseFile() {
	if (this->browseFile) {
		return this->browseFile;
	} else {
		DbSelectItemBundle *item = (DbSelectItemBundle *)this->ui->databaseList->currentIndex().internalPointer();
		if (item) {
			return item->file;
		} else {
			return NULL;
		}
	}
}

void DatabaseSelectDialog::setForce(bool force) {
	this->force = force;

	if (!force) {
		this->ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	} else {
		this->ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok);
	}
}

void DatabaseSelectDialog::onDbCreateClick() {
	CreateDatabaseDialog dlg(this);
	if (dlg.exec() == QDialog::Accepted) {
		GCM::GC<GCM::database::DatabaseInfo> info = dlg.getDatabaseInfo();
		if (info) {
			// Clear old cache list before new one is loaded to save memory.
			wnd->setCaches(NULL);

			::ui->addTask(new SwitchDatabaseTask(wnd, info, File(dlg.getDatabaseName().append(".db3").toUtf8().data())));

			this->force = false;
			emit this->reject();
		}
	}
}

void DatabaseSelectDialog::reject() {
	if (!this->force) {
		QDialog::reject();
	}
}

void DatabaseSelectDialog::accept() {
	if (this->browseFile || this->ui->databaseList->currentIndex().isValid()) {
		QDialog::accept();
	}
}

void DatabaseSelectDialog::onBrowseClick() {
	QString file = QFileDialog::getOpenFileName(this, "Select database file", QString(), "Database files (*.db3)", NULL);
	if (file != "") {
		this->browseFile = File(file.toUtf8().data());
		emit this->accept();
	}
}
