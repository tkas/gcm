#ifndef WAYPOINTDIALOG_H
#define WAYPOINTDIALOG_H

#include <QDialog>
#include <GC.h>
#include <geolib/Geocache.h>

namespace Ui {
	class WaypointDialog;
}

class WaypointDialog : public QDialog {
	Q_OBJECT
public:
	WaypointDialog(GCM::GC<GCM::geolib::Geocache> cache, QWidget *parent = 0);
	WaypointDialog(GCM::GC<GCM::geolib::GeocacheWaypoint> waypoint, QWidget *parent = 0);
	~WaypointDialog();

protected:
	void changeEvent(QEvent *e);
	void setupUi();

private:
	Ui::WaypointDialog *ui;
	bool editMode;
	GCM::GC<GCM::geolib::Geocache> cache;
	GCM::GC<GCM::geolib::GeocacheWaypoint> waypoint;

public slots:
	void onAccepted();
	void wptTypeChanged(int);
};

#endif // WAYPOINTDIALOG_H
