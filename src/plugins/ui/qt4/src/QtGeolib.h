#ifndef QTGEOLIB_H
#define QTGEOLIB_H

#include <geolib/Geocache.h>
#include <geolib/GeocacheWaypoint.h>
#include <stl/String.h>
#include <templater/Variable.h>

#include <QString>
#include <QIcon>

class QtGeolib {
	protected:
		static bool iconsInitialized;

		static QIcon iconCito;
		static QIcon iconEarth;
		static QIcon iconEvent;
		static QIcon iconLetterbox;
		static QIcon iconMegaevent;
		static QIcon iconMulti;
		static QIcon iconTraditional;
		static QIcon iconUnknown;
		static QIcon iconVirtual;
		static QIcon iconWebcam;
		static QIcon iconWherigo;

		static QIcon iconFinal;
		static QIcon iconQta;
		static QIcon iconSom;
		static QIcon iconReference;
		static QIcon iconParking;
		static QIcon iconTrailhead;

	public:
		static void loadIcons();

		static const char *cacheIconChar(GCM::geolib::Geocache::Type type);
		static QString cacheIconQString(GCM::geolib::Geocache::Type type);
		static GCM::String cacheIconString(GCM::geolib::Geocache::Type type);

		static QIcon cacheIconSmall(GCM::GC<GCM::geolib::Geocache> cache);

		static const char *waypointIconChar(GCM::geolib::GeocacheWaypoint::Type type);
		static QString waypointIconQString(GCM::geolib::GeocacheWaypoint::Type type);
		static GCM::String waypointIconString(GCM::geolib::GeocacheWaypoint::Type type);

		static QIcon waypointIconSmall(GCM::GC<GCM::geolib::GeocacheWaypoint> waypoint);

		static GCM::GC<GCM::templater::Variable> cacheTypeToIcon(GCM::GC<GCM::templater::Variable> var);
		static GCM::GC<GCM::templater::Variable> starsToIcon(GCM::GC<GCM::templater::Variable> var);
		static GCM::GC<GCM::templater::Variable> formatDate(GCM::GC<GCM::templater::Variable> var);
		static GCM::GC<GCM::templater::Variable> waypointTypeToIcon(GCM::GC<GCM::templater::Variable> var);
};

#endif // QTGEOLIB_H
