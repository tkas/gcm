#ifndef CACHEEDITDIALOG_H
#define CACHEEDITDIALOG_H

#include <QDialog>
#include <GC.h>
#include <geolib/Geocache.h>
#include <QMap>
#include <QString>
#include <QStringList>
#include "../widgets/AttributeChecked.h"

namespace Ui {
	class CacheEditDialog;
}

class CacheEditDialog : public QDialog {
	Q_OBJECT
public:
	CacheEditDialog(GCM::GC<GCM::geolib::Geocache> cache, QWidget *parent = 0);
	~CacheEditDialog();

	static QMap<QString, QStringList> countrys;
	static bool countrysInitialized;
	static void initCountrys();

protected:
	void changeEvent(QEvent *e);
	void setupUi();

private:
	Ui::CacheEditDialog *ui;
	GCM::GC<GCM::geolib::Geocache> cache;
	GCM::GC<GCM::geolib::GeocacheWaypointList> waypoints;
	bool wereLongLoaded;
	bool wereAttributesLoaded;
	QMap<int, AttributeChecked *> attributes;

public slots:
	void onDiffChanged(int value);
	void onTerrChanged(int value);
	void onCbFoundClick(int state);
	void onLockAllClick(int state);
	void onCountryChanged(QString country);
	void onAccepted();
	void onWptChanged(int index);
};

#endif // CACHEEDITDIALOG_H
