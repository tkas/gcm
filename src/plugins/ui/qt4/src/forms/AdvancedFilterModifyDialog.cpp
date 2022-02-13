#include "AdvancedFilterModifyDialog.h"
#include "ui_AdvancedFilterModifyDialog.h"

//#include <QtGui/QMessageBox>

using namespace GCM::geolib::filter;

AdvancedFilterModifyDialog::AdvancedFilterModifyDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AdvancedFilterModifyDialog)
{
	ui->setupUi(this);
	this->ui->radioAdd->setChecked(true);
}

AdvancedFilterModifyDialog::~AdvancedFilterModifyDialog()
{
	delete ui;
}

void AdvancedFilterModifyDialog::changeEvent(QEvent *e)
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

void AdvancedFilterModifyDialog::accept() {
	if (this->ui->filterName->text().trimmed() == "") {
		this->ui->filterName->setFocus(Qt::OtherFocusReason);
		QMessageBox msg(this);
		msg.setWindowTitle("Error");
		msg.setText("You must provide filter name.");
		msg.setIcon(QMessageBox::Critical);
		msg.exec();
		return;
	}

	QDialog::accept();
}

QString AdvancedFilterModifyDialog::getFilterName() {
	return this->ui->filterName->text();
}

AdvancedFilterItem::Mode AdvancedFilterModifyDialog::getFilterMode() {
	if (this->ui->radioAdd->isChecked()) {
		return AdvancedFilterItem::Add;
	} else if (this->ui->radioSubtract->isChecked()) {
		return AdvancedFilterItem::Subtract;
	} else if (this->ui->radioIntersect->isChecked()) {
		return AdvancedFilterItem::Intersect;
	} else {
		return AdvancedFilterItem::Initial;
	}
}

void AdvancedFilterModifyDialog::setFilterName(QString name) {
	this->ui->filterName->setText(name);
}

void AdvancedFilterModifyDialog::setFilterMode(AdvancedFilterItem::Mode mode) {
	switch (mode) {
	case AdvancedFilterItem::Add:
		this->ui->radioAdd->setChecked(true);
		break;

	case AdvancedFilterItem::Subtract:
		this->ui->radioSubtract->setChecked(true);
		break;

	case AdvancedFilterItem::Intersect:
		this->ui->radioIntersect->setChecked(true);
		break;

	// Do not allow initial items to be changed type.
	case AdvancedFilterItem::Initial:
		this->ui->radioAdd->setEnabled(false);
		this->ui->radioSubtract->setEnabled(false);
		this->ui->radioIntersect->setEnabled(false);
		break;
	}
}
