#include "ExportDialog.h"
#include "ui_ExportDialog.h"

#include <QListWidgetItem>
#include <QList>
#include <QPushButton>
#include <QItemDelegate>

class ExportItem: public QListWidgetItem {
public:
	ExportItem(GCM::GC<GCM::ExportInfo> ei, QListWidget *parent = 0): QListWidgetItem(parent), ei(ei) {
		this->setText(QString::fromUtf8(ei->formatDescription->c_str()));
	}

	GCM::GC<GCM::ExportInfo> exportInfo() {
		return this->ei;
	}

private:
	GCM::GC<GCM::ExportInfo> ei;
};

ExportDialog::ExportDialog(GCM::GC< GCM::List<GCM::ExportInfo> > allExports, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ExportDialog),
	allExports(allExports)
{
	ui->setupUi(this);

	GCM::List<GCM::ExportInfo>::iterator i = allExports->begin();
	while (i != allExports->end()) {
		this->ui->exportPlugins->addItem(new ExportItem(*i, this->ui->exportPlugins));
		i++;
	}

	connect(this->ui->exportPlugins, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onExportDoubleClick(QModelIndex)));
	connect(this->ui->exportPlugins, SIGNAL(currentRowChanged(int)), this, SLOT(onCurrentRowChanged()));
	this->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	this->ui->exportPlugins->setItemDelegate(this->delegate = new ExportDelegate());
}

ExportDialog::~ExportDialog()
{
	delete ui;
	delete delegate;
}

void ExportDialog::changeEvent(QEvent *e)
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

GCM::GC<GCM::ExportInfo> ExportDialog::selected() {
	QModelIndex selected = this->ui->exportPlugins->currentIndex();
	if (selected.isValid()) {
		return ((ExportItem *)selected.internalPointer())->exportInfo();
	} else {
		return NULL;
	}
}

void ExportDialog::onCurrentRowChanged() {
	if (this->ui->exportPlugins->currentIndex().isValid()) {
		this->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	} else {
		this->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	}
}

void ExportDialog::onExportDoubleClick(QModelIndex index) {
	if (index.isValid()) {
		emit this->accept();
	}
}
