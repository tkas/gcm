#include "WaypointDialog.h"
#include "ui_WaypointDialog.h"
#include <config/AppConfig.h>
#include <geolib/Geocache.h>
#include <geolib/GeocacheWaypoint.h>
#include "../widgets/CoordinatesWidget.h"

using namespace GCM::geolib;

WaypointDialog::WaypointDialog(GCM::GC<Geocache> cache, QWidget *parent):
	QDialog(parent),
	ui(new Ui::WaypointDialog),
	cache(cache)
{
	ui->setupUi(this);
	this->editMode = false;

	QString title = QString::fromUtf8("New waypoint for cache ");
	title.append(QString::fromUtf8(cache->getName()->c_str()));

	this->setWindowIcon(QIcon(":/icons/16x16/ic_waypoint_add.png"));

	this->setWindowTitle(title);
	this->setupUi();

	this->ui->cbUserWaypoint->setChecked(true);

	this->ui->waypointName->setText("Final"); // Final waypoint
	this->ui->waypointPrefix->setText("FI"); // Default prefix for final waypoint
	this->ui->waypointType->setCurrentIndex(0); // Final location
}

/**
 * Constructor for edit mode dialog.
 */
WaypointDialog::WaypointDialog(GCM::GC<GeocacheWaypoint> waypoint, QWidget *parent):
	QDialog(parent),
	ui(new Ui::WaypointDialog),
	cache(waypoint->getCache()),
	waypoint(waypoint)
{
	ui->setupUi(this);
	this->editMode = true;

	QString title = QString::fromUtf8("Modify waypoint ");
	title.append(QString::fromUtf8(waypoint->getName()->c_str()));
	title.append(" for cache ");
	title.append(QString::fromUtf8(cache->getName()->c_str()));

	this->setWindowTitle(title);
	this->setWindowIcon(QIcon(":/icons/16x16/ic_waypoint_modify.png"));
	this->setupUi();

	// Fill in items.
	this->ui->waypointName->setText(waypoint->getName()->c_str());
	this->ui->waypointName->setModified(true);

	this->ui->waypointComment->document()->setPlainText(waypoint->getComment()->c_str());

	this->ui->waypointPrefix->setText(waypoint->getId()->c_str());
	this->ui->waypointPrefix->setModified(true);

	switch (waypoint->getWptType()) {
	case GeocacheWaypoint::T_FINAL: this->ui->waypointType->setCurrentIndex(0); break;
	case GeocacheWaypoint::T_PARKING: this->ui->waypointType->setCurrentIndex(4); break;
	case GeocacheWaypoint::T_QTA: this->ui->waypointType->setCurrentIndex(1); break;
	case GeocacheWaypoint::T_REFERENCE: this->ui->waypointType->setCurrentIndex(3); break;
	case GeocacheWaypoint::T_SOM: this->ui->waypointType->setCurrentIndex(2); break;
	case GeocacheWaypoint::T_TRAILHEAD: this->ui->waypointType->setCurrentIndex(5); break;
	}

	this->ui->cbUserWaypoint->setChecked(waypoint->getUserFlag());
	this->ui->userNote->document()->setPlainText(waypoint->getUserComment()->c_str());

	Coords c;
	c.dlat = waypoint->getLatitudeE6() / 1E6;
	c.dlon = waypoint->getLongitudeE6() / 1E6;
	c.mlat = (c.dlat - (int)c.dlat) * 60;
	c.mlon = (c.dlon - (int)c.dlon) * 60;
	c.slat = (c.mlat - (int)c.mlat) * 60;
	c.slon = (c.mlon - (int)c.mlon) * 60;
	this->ui->coords->setCoords(c);
}

WaypointDialog::~WaypointDialog()
{
	delete ui;
}

void WaypointDialog::changeEvent(QEvent *e)
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

void WaypointDialog::setupUi() {
	connect(this, SIGNAL(accepted()), this, SLOT(onAccepted()));
	connect(this->ui->waypointType, SIGNAL(currentIndexChanged(int)), this, SLOT(wptTypeChanged(int)));
}

void WaypointDialog::onAccepted() {
	// Fill in the cache waypoint.
	if (!this->waypoint) {
		this->waypoint = new GeocacheWaypoint();
		this->waypoint->setCache(this->cache);
		this->cache->getWaypoints()->append(this->waypoint);
	}

	this->waypoint->setName(this->ui->waypointName->text().toUtf8().constData());

	QString id = this->ui->waypointPrefix->text();
	id.append((const char *)(this->cache->getId()->c_str() + 2));

	this->waypoint->setId(id.toUtf8().constData());
	this->waypoint->setDtLastUpdate(time(NULL));
	this->waypoint->setComment(this->ui->waypointComment->document()->toPlainText().toUtf8().constData());
	this->waypoint->setUserComment(this->ui->userNote->document()->toPlainText().toUtf8().constData());
	this->waypoint->setUserFlag((this->ui->cbUserWaypoint->isChecked()));

	switch (this->ui->waypointType->currentIndex()) {
	case 0: this->waypoint->setWptType(GeocacheWaypoint::T_FINAL); break;
	case 1: this->waypoint->setWptType(GeocacheWaypoint::T_QTA); break;
	case 2: this->waypoint->setWptType(GeocacheWaypoint::T_SOM); break;
	case 3: this->waypoint->setWptType(GeocacheWaypoint::T_REFERENCE); break;
	case 4: this->waypoint->setWptType(GeocacheWaypoint::T_PARKING); break;
	case 5: this->waypoint->setWptType(GeocacheWaypoint::T_TRAILHEAD); break;
	default: this->waypoint->setWptType(GeocacheWaypoint::T_REFERENCE); break;
	}


	Coords c = this->ui->coords->getCoords();
	this->waypoint->setLatitudeE6(c.dlat * 1E6);
	this->waypoint->setLongitudeE6(c.dlon * 1E6);

	GCM::GC<GCM::database::Database> db = GCM::config::AppConfig::getInstance()->getWorkingDatabase();
	db->saveCacheWaypoint(this->waypoint);
}

void WaypointDialog::wptTypeChanged(int index) {
	if (/*!this->editMode && */!this->ui->waypointName->isModified()) {
		switch (index) {
			case 0: // Final
				this->ui->waypointName->setText("Final");
				break;

			case 1: // QTA
			case 2: // SOM
			{
				// Scan through existing waypoints and try to find next stage number.
				GeocacheWaypointList::iterator i = this->cache->getWaypoints()->begin();
				int stageNum = 0;
				while (i != this->cache->getWaypoints()->end()) {
					GCM::GC<GeocacheWaypoint> wpt = *i;
					int d;
					if (sscanf(wpt->getName()->c_str(), "Stage %d", &d) == 1) {
						if (d > stageNum) stageNum = d;
					}

					i++;
				}

				stageNum++;

				QString txt;
				txt.sprintf("Stage %d", stageNum);
				this->ui->waypointName->setText(txt);
				break;
			}

			case 3: // Ref
				this->ui->waypointName->setText("Reference point");
				break;

			case 4: // Park
				this->ui->waypointName->setText("Parking");
				break;

			case 5: // Trail
				this->ui->waypointName->setText("Trailhead");
				break;
		}
	}

	if (/*!this->editMode && */!this->ui->waypointPrefix->isModified()) {
		QString txt;
		switch (index) {
			case 0: // Final
				txt = "FI";
				break;

			case 4: // Parking
				txt = "PA";
				break;

			case 1: // QTA
			case 2: // SOM
			case 3: // Reference
			case 5: // Trailhead
				break;
		}

		if (txt.isEmpty()) {
			bool found = false;
			int d = 0;
			GeocacheWaypointList::iterator i;
			do {
				found = false;
				if (d > 99) {
					// Bail out if too many waypoints...
					txt = "";
					break;
				}

				txt.sprintf("%02d", d);

				i = this->cache->getWaypoints()->begin();
				while (i != this->cache->getWaypoints()->end()) {
					GCM::GC<GeocacheWaypoint> wpt = *i;
					if (strncmp(wpt->getId()->c_str(), txt.toAscii().data(), 2) == 0) {
						found = true;
						break;
					}
					i++;
				}

				d++;
			} while (found);
		}

		if (!txt.isEmpty()) {
			this->ui->waypointPrefix->setText(txt);
		}
	}
}
