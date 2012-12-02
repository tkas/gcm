#include "QtGeolib.h"
#include <templater/Templater.h>
#include <QDate>
#include <time.h>

bool QtGeolib::iconsInitialized;
QIcon QtGeolib::iconCito;
QIcon QtGeolib::iconEarth;
QIcon QtGeolib::iconEvent;
QIcon QtGeolib::iconLetterbox;
QIcon QtGeolib::iconMegaevent;
QIcon QtGeolib::iconMulti;
QIcon QtGeolib::iconTraditional;
QIcon QtGeolib::iconUnknown;
QIcon QtGeolib::iconVirtual;
QIcon QtGeolib::iconWebcam;
QIcon QtGeolib::iconWherigo;

QIcon QtGeolib::iconFinal;
QIcon QtGeolib::iconQta;
QIcon QtGeolib::iconSom;
QIcon QtGeolib::iconReference;
QIcon QtGeolib::iconParking;
QIcon QtGeolib::iconTrailhead;

using namespace GCM::geolib;

void QtGeolib::loadIcons() {
	if (!iconsInitialized) {
		iconCito = QIcon(":/icons/16x16/" + cacheIconQString(Geocache::T_CITO));
		iconEarth = QIcon(":/icons/16x16/" + cacheIconQString(Geocache::T_EARTH));
		iconEvent = QIcon(":/icons/16x16/" + cacheIconQString(Geocache::T_EVENT));
		iconLetterbox = QIcon(":/icons/16x16/" + cacheIconQString(Geocache::T_LETTERBOX));
		iconMegaevent = QIcon(":/icons/16x16/" + cacheIconQString(Geocache::T_MEGAEVENT));
		iconMulti = QIcon(":/icons/16x16/" + cacheIconQString(Geocache::T_MULTI));
		iconTraditional = QIcon(":/icons/16x16/" + cacheIconQString(Geocache::T_TRADITIONAL));
		iconUnknown = QIcon(":/icons/16x16/" + cacheIconQString(Geocache::T_UNKNOWN));
		iconVirtual = QIcon(":/icons/16x16/" + cacheIconQString(Geocache::T_VIRTUAL));
		iconWebcam = QIcon(":/icons/16x16/" + cacheIconQString(Geocache::T_WEBCAM));
		iconWherigo = QIcon(":/icons/16x16/" + cacheIconQString(Geocache::T_WHERIGO));

		iconFinal = QIcon(":/icons/16x16/" + waypointIconQString(GeocacheWaypoint::T_FINAL));
		iconQta = QIcon(":/icons/16x16/" + waypointIconQString(GeocacheWaypoint::T_QTA));
		iconSom = QIcon(":/icons/16x16/" + waypointIconQString(GeocacheWaypoint::T_SOM));
		iconReference = QIcon(":/icons/16x16/" + waypointIconQString(GeocacheWaypoint::T_REFERENCE));
		iconParking = QIcon(":/icons/16x16/" + waypointIconQString(GeocacheWaypoint::T_PARKING));
		iconTrailhead = QIcon(":/icons/16x16/" + waypointIconQString(GeocacheWaypoint::T_TRAILHEAD));

		iconsInitialized = true;

		GCM::templater::Templater::registerFormatFunc("cacheTypeToIcon", cacheTypeToIcon);
		GCM::templater::Templater::registerFormatFunc("starsToIcon", starsToIcon);
		GCM::templater::Templater::registerFormatFunc("formatDate", formatDate);
		GCM::templater::Templater::registerFormatFunc("waypointTypeToIcon", waypointTypeToIcon);
	}
}

const char *QtGeolib::cacheIconChar(Geocache::Type type) {
	switch (type) {
	case Geocache::T_CITO: return "ic_cache_cito.png";
	case Geocache::T_EARTH: return "ic_cache_earth.png";
	case Geocache::T_EVENT: return "ic_cache_event.png";
	case Geocache::T_LETTERBOX: return "ic_cache_letterbox.png";
	case Geocache::T_MEGAEVENT: return "ic_cache_megaevent.png";
	case Geocache::T_MULTI: return "ic_cache_multi.png";
	case Geocache::T_TRADITIONAL: return "ic_cache_traditional.png";
	case Geocache::T_UNKNOWN: return "ic_cache_mystery.png";
	case Geocache::T_VIRTUAL: return "ic_cache_virtual.png";
	case Geocache::T_WEBCAM: return "ic_cache_webcam.png";
	case Geocache::T_WHERIGO: return "ic_cache_wherigo.png";
	default: return "";
	}
}

QString QtGeolib::cacheIconQString(Geocache::Type type) {
	return QString::fromAscii(cacheIconChar(type));
}

GCM::String QtGeolib::cacheIconString(Geocache::Type type) {
	return GCM::String(cacheIconChar(type));
}

QIcon QtGeolib::cacheIconSmall(GCM::GC<GCM::geolib::Geocache> cache) {
	loadIcons();

	switch ((Geocache::Type)cache->getCacheType()) {
		case GCM::geolib::Geocache::T_CITO: return iconCito;
		case GCM::geolib::Geocache::T_EARTH: return iconEarth;
		case GCM::geolib::Geocache::T_EVENT: return iconEvent;
		case GCM::geolib::Geocache::T_LETTERBOX: return iconLetterbox;
		case GCM::geolib::Geocache::T_MEGAEVENT: return iconMegaevent;
		case GCM::geolib::Geocache::T_MULTI: return iconMulti;
		case GCM::geolib::Geocache::T_TRADITIONAL: return iconTraditional;
		case GCM::geolib::Geocache::T_UNKNOWN: return iconUnknown;
		case GCM::geolib::Geocache::T_VIRTUAL: return iconVirtual;
		case GCM::geolib::Geocache::T_WEBCAM: return iconWebcam;
		case GCM::geolib::Geocache::T_WHERIGO: return iconWherigo;
		default: return QIcon();
	}
}

const char *QtGeolib::waypointIconChar(GCM::geolib::GeocacheWaypoint::Type type) {
	switch (type) {
	case GeocacheWaypoint::T_FINAL: return "ic_wpt_final.png";
	case GeocacheWaypoint::T_PARKING: return "ic_wpt_parking.png";
	case GeocacheWaypoint::T_QTA: return "ic_wpt_qta.png";
	case GeocacheWaypoint::T_REFERENCE: return "ic_wpt_reference.png";
	case GeocacheWaypoint::T_SOM: return "ic_wpt_stage.png";
	case GeocacheWaypoint::T_TRAILHEAD: return "ic_wpt_trailhead.png";
	default: return "";
	}
}

QString QtGeolib::waypointIconQString(GCM::geolib::GeocacheWaypoint::Type type) {
	return QString::fromAscii(waypointIconChar(type));
}

GCM::String QtGeolib::waypointIconString(GCM::geolib::GeocacheWaypoint::Type type) {
	return GCM::String(waypointIconChar(type));
}

QIcon QtGeolib::waypointIconSmall(GCM::GC<GCM::geolib::GeocacheWaypoint> waypoint) {
	loadIcons();

	switch (waypoint->getWptType()) {
		case GCM::geolib::GeocacheWaypoint::T_FINAL: return iconFinal;
		case GCM::geolib::GeocacheWaypoint::T_PARKING: return iconParking;
		case GCM::geolib::GeocacheWaypoint::T_QTA: return iconQta;
		case GCM::geolib::GeocacheWaypoint::T_REFERENCE: return iconReference;
		case GCM::geolib::GeocacheWaypoint::T_SOM: return iconSom;
		case GCM::geolib::GeocacheWaypoint::T_TRAILHEAD: return iconTrailhead;
		default: return QIcon();
	}
}

GCM::GC<GCM::templater::Variable> QtGeolib::cacheTypeToIcon(GCM::GC<GCM::templater::Variable> var) {
	return new GCM::templater::Variable(cacheIconString((Geocache::Type)var->toInt()));
}

GCM::GC<GCM::templater::Variable> QtGeolib::starsToIcon(GCM::GC<GCM::templater::Variable> var) {
	return new GCM::templater::Variable(GCM::String::format("stars_%02d.png", (int)(var->toDouble() * 10)));
}

GCM::GC<GCM::templater::Variable> QtGeolib::formatDate(GCM::GC<GCM::templater::Variable> var) {
	int64_t time = var->toInt64();
	struct tm local;
	localtime_r((time_t *)&time, &local);
	QDate dt(local.tm_year + 1900, local.tm_mon + 1, local.tm_mday);
	return new GCM::templater::Variable(GCM::String(dt.toString(Qt::DefaultLocaleShortDate).toUtf8().data()));
}

GCM::GC<GCM::templater::Variable> QtGeolib::waypointTypeToIcon(GCM::GC<GCM::templater::Variable> var) {
	return new GCM::templater::Variable(waypointIconString((GeocacheWaypoint::Type)var->toInt()));
}
