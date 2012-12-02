#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

#include <QModelIndex>
#include <QModelIndexList>
#include <QTableWidgetItem>
#include <QMessageBox>

#include <config/AppConfig.h>
#include <stl/List.h>
#include <geolib/ReferencePoint.h>
#include <geolib/GenericWaypoint.h>

#include "AddRefPointDialog.h"
#include "AddOtherNickDialog.h"

using namespace GCM;
using namespace GCM::config;
using namespace GCM::geolib;

class RefPointItem: public QTableWidgetItem {
public:
	RefPointItem(GCM::GC<ReferencePoint> point):
			QTableWidgetItem(QString::fromUtf8(point->getName()->c_str())),
			point(point) {
	}

	GCM::GC<ReferencePoint> point;
};

class RefPointCoords: public QTableWidgetItem {
public:
	RefPointCoords(GCM::GC<ReferencePoint> point):
			QTableWidgetItem(QString::fromUtf8(GenericWaypoint::formatCoords(
				point->getLatitudeE6(),
				point->getLongitudeE6()
			)->c_str())),
			point(point) {

		coords.dlat = point->getLatitudeE6() / 1E6;
		coords.dlon = point->getLongitudeE6() / 1E6;
	}

	void setCoords(Coords c) {
		this->coords = c;
		this->setText(QString::fromUtf8(GenericWaypoint::formatCoords(
				this->coords.dlat * 1E6,
				this->coords.dlon * 1E6
			)->c_str()));
	}

	GCM::GC<ReferencePoint> point;
	Coords coords;
};

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);

	AppConfig *cfg = AppConfig::getInstance();

	this->ui->myNick->setText(cfg->getString("username", ""));

	int x = 0;
	const char *nick = cfg->getString("othernick_0", NULL);
	while (nick != NULL) {
		this->ui->otherNicks->addItem(nick);

		x++;
		nick = cfg->getString(String::format("othernick_%d", x)->c_str(), NULL);
	}

	// Setup reference points
	this->refPoints = cfg->getReferencePoints();

	ArrayList<ReferencePoint>::iterator i = this->refPoints->begin();

	i = this->refPoints->begin();
	int size = 0;
	while (i != this->refPoints->end()) {
		if (!(*i)->isTemporary()) size++;
		i++;
	}

	i = this->refPoints->begin();

	this->ui->referencePoints->setRowCount(size);
	int rowNum = 0;
	while (i != this->refPoints->end()) {
		if (!(*i)->isTemporary()) {
			this->ui->referencePoints->setItem(
				rowNum,
				0,
				new RefPointItem(*i)
			);
			this->ui->referencePoints->setItem(
				rowNum,
				1,
				new RefPointCoords(*i)
			);
			rowNum++;
		}
		i++;
	}

	this->ui->referencePoints->setSortingEnabled(true);

	connect(this->ui->btnAddNick, SIGNAL(clicked()), this, SLOT(onBtnAddNickClick()));
	connect(this->ui->btnModifyNick, SIGNAL(clicked()), this, SLOT(onBtnModifyNickClick()));
	connect(this->ui->btnRemoveNick, SIGNAL(clicked()), this, SLOT(onBtnRemoveNickClick()));
	connect(this->ui->otherNicks, SIGNAL(itemSelectionChanged()), this, SLOT(onNicksSelChanged()));

	connect(this->ui->btnAddRefPt, SIGNAL(clicked()), this, SLOT(onBtnAddRefPointClick()));
	connect(this->ui->btnModifyRefPt, SIGNAL(clicked()), this, SLOT(onBtnModifyRefPointClick()));
	connect(this->ui->btnRemoveRefPt, SIGNAL(clicked()), this, SLOT(onBtnRemoveRefPointClick()));
	connect(this->ui->referencePoints, SIGNAL(itemSelectionChanged()), this, SLOT(onRefPtSelChanged()));
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::changeEvent(QEvent *e)
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

void SettingsDialog::onNicksSelChanged() {
	if (this->ui->otherNicks->selectionModel()->selectedRows().count() > 0) {
		this->ui->btnModifyNick->setEnabled(true);
		this->ui->btnRemoveNick->setEnabled(true);
	} else {
		this->ui->btnModifyNick->setEnabled(false);
		this->ui->btnRemoveNick->setEnabled(false);
	}
}

void SettingsDialog::onRefPtSelChanged() {
	if (this->ui->referencePoints->selectionModel()->selectedRows().count() > 0) {
		this->ui->btnModifyRefPt->setEnabled(true);
		this->ui->btnRemoveRefPt->setEnabled(true);
	} else {
		this->ui->btnModifyRefPt->setEnabled(false);
		this->ui->btnRemoveRefPt->setEnabled(false);
	}
}


void SettingsDialog::onBtnAddRefPointClick() {
	AddRefPointDialog dlg(this);
	if (dlg.exec() == QDialog::Accepted) {
		// Create new reference point
		GCM::GC<ReferencePoint> pt = new ReferencePoint();
		pt->setName(dlg.getName().toUtf8().data());

		Coords c = dlg.getCoords();
		pt->setLatitudeE6(c.dlat * 1E6);
		pt->setLongitudeE6(c.dlon * 1E6);

		//this->refPoints->append(pt);

		// Add point to the list
		this->ui->referencePoints->setSortingEnabled(false);

		int rowNum = this->ui->referencePoints->rowCount();
		this->ui->referencePoints->insertRow(rowNum);

		this->ui->referencePoints->setItem(
			rowNum,
			0,
			new RefPointItem(pt)
		);
		this->ui->referencePoints->setItem(
			rowNum,
			1,
			new RefPointCoords(pt)
		);

		this->ui->referencePoints->setSortingEnabled(true);
	}
}

void SettingsDialog::onBtnModifyRefPointClick() {
	this->ui->referencePoints->setSortingEnabled(false);

	QModelIndexList selected = this->ui->referencePoints->selectionModel()->selectedRows();
	QModelIndexList::iterator i = selected.begin();
	while (i != selected.end()) {
		if (i->isValid()) {
			RefPointItem *item = (RefPointItem *)this->ui->referencePoints->item(i->row(), 0);
			RefPointCoords *coords = (RefPointCoords *)this->ui->referencePoints->item(i->row(), 1);

			AddRefPointDialog dlg(item->point, item->text(), coords->coords, this);
			if (dlg.exec() == QDialog::Accepted) {
				// Update coordinates and name in both model and point itself.
				item->setText(dlg.getName());
				coords->setCoords(dlg.getCoords());
			}
			break;
		}

		i++;
	}

	this->ui->referencePoints->setSortingEnabled(true);
}

void SettingsDialog::onBtnRemoveRefPointClick() {
	this->ui->referencePoints->setSortingEnabled(false);

	QModelIndexList selected = this->ui->referencePoints->selectionModel()->selectedRows();
	QModelIndexList::iterator i = selected.begin();
	while (i != selected.end()) {
		if (i->isValid()) {
			if (QMessageBox::question(
				this,
				"Remove reference point?",
				"Do you really want to remove this reference point?",
				QMessageBox::Yes,
				QMessageBox::No) == QMessageBox::Yes) {

				this->ui->referencePoints->removeRow(i->row());
			}
			break;
		}

		i++;
	}

	this->ui->referencePoints->setSortingEnabled(true);
}

void SettingsDialog::accept() {
	// Save nicknames
	AppConfig *cfg = AppConfig::getInstance();
	cfg->set("username", this->ui->myNick->text().toUtf8().data());

	// Unset old nicks
	int x = 0;
	const char *nick = cfg->getString("othernick_0", NULL);
	while (nick != NULL) {
		cfg->unset(String::format("othernick_%d", x)->c_str());
		x++;
		nick = cfg->getString(String::format("othernick_%d", x)->c_str(), NULL);
	}

	// Set new nicks
	for (int x = 0; x < this->ui->otherNicks->count(); x++) {
		cfg->set(String::format("othernick_%d", x)->c_str(), this->ui->otherNicks->item(x)->text().toUtf8().data());
	}

	// Recreate reference points

	// Remove non-temporary items
	ArrayList<ReferencePoint>::iterator i = this->refPoints->begin();
	while (i != this->refPoints->end()) {
		if (!(*i)->isTemporary()) {
			this->refPoints->erase(i);
		} else {
			i++;
		}
	}

	int rc = this->ui->referencePoints->rowCount();
	for (int i = 0; i < rc; i++) {
		RefPointItem *item = (RefPointItem *)this->ui->referencePoints->item(i, 0);
		RefPointCoords *coords = (RefPointCoords *)this->ui->referencePoints->item(i, 1);

		item->point->setName(item->text().toUtf8().data());
		item->point->setLatitudeE6(coords->coords.dlat * 1E6);
		item->point->setLongitudeE6(coords->coords.dlon * 1E6);
		this->refPoints->append(item->point);
	}

	QDialog::accept();
}

void SettingsDialog::onBtnAddNickClick() {
	AddOtherNickDialog dlg(this);
	if (dlg.exec() == QDialog::Accepted) {
		this->ui->otherNicks->addItem(dlg.getNick());
	}
}

void SettingsDialog::onBtnModifyNickClick() {
	QModelIndexList rows = this->ui->otherNicks->selectionModel()->selectedRows();
	QModelIndexList::iterator i = rows.begin();
	while (i != rows.end()) {
		AddOtherNickDialog dlg(this->ui->otherNicks->item(i->row()), this);
		dlg.exec();
		i++;
	}
}

void SettingsDialog::onBtnRemoveNickClick() {
	QModelIndexList rows = this->ui->otherNicks->selectionModel()->selectedRows();
	QModelIndexList::iterator i = rows.begin();
	while (i != rows.end()) {
		this->ui->otherNicks->model()->removeRow(i->row(), i->parent());
		i++;
	}
}
