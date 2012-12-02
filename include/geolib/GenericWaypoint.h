#ifndef _GCM_WAYPOINT_H
#	define _GCM_WAYPOINT_H

#include <stl/String.h>
#include <Object.h>
#include <stl/Modifiable.h>

// Forward of database declaration.
namespace GCM {
	namespace database {

class Database;

	}
}

#ifdef _WIN32
# include <database/Database.h>
#endif

namespace GCM {
	namespace geolib {

/**
 * Generic waypoint, that has coordinates and some basic information.
 */
class GCM_API GenericWaypoint: public Object {
	public:
		typedef enum {
			FMT_DD,
			FMT_DM,
			FMT_DMS
		} CoordsFormat;

		struct Distance {
			int distance;	/**< Distance in meters */
			int azimuth;	/**< Azimuth in tens of degree */
			Distance(int distance, int azimuth): distance(distance), azimuth(azimuth) {}
		};

	protected:
		Modifiable<int> latitudeE6;
		Modifiable<int> longitudeE6;
		Modifiable<int64_t> dtCreated;
		Modifiable<String> id;
		Modifiable<String> name;
		Modifiable<String> comment;
		Modifiable<String> type;

		GC<GCM::database::Database> dbProvider; /**< Database provider that stores this waypoint. */

		Distance distance;

		static const int RADIUS;
		static const double DEG2RAD;
		static const double RAD2DEG;

	public:
		GenericWaypoint();
		GenericWaypoint(const GenericWaypoint &wpt);

		virtual Modifiable<int> getLatitudeE6();
		virtual double getLatitude();
		virtual Modifiable<int> getLongitudeE6();
		virtual double getLongitude();
		virtual void setLatitudeE6(int latitude);
		virtual void setLongitudeE6(int longitude);

		virtual Modifiable<int64_t> getDtCreated();
		virtual void setDtCreated(int64_t time);

		virtual const Modifiable<String> getId();
		virtual void setId(String id);

		virtual const Modifiable<String> getName();
		virtual void setName(String name);

		virtual const Modifiable<String> getComment();
		virtual void setComment(String comment);

		virtual const Modifiable<String> getType();
		virtual void setType(String type);

		virtual GC<GCM::database::Database> getDbProvider();
		virtual void setDbProvider(GC<GCM::database::Database> db);

		virtual void setDistance(Distance distance);
		virtual void setDistance(int distance, int azimuth);
		virtual void setDistanceFrom(int latE6, int lonE6);
		virtual const Distance getDistance();

		static String formatCoords(int latE6, int lonE6, CoordsFormat format = FMT_DM);
		virtual String formatCoords(CoordsFormat format = FMT_DM);

		static String formatDistance(int distance, int azimuth);
		virtual String formatDistance();

		static Distance calcDistanceAzimuth(int latE61, int lonE61, int latE62, int lonE62);
		static Distance calcDistanceAzimuth(double dLat1, double dLon1, double dLat2, double dLon2);
		static int calcDistance(int latE61, int lonE61, int latE62, int lonE62);
		static int calcDistance(double dLat1, double dLon1, double dLat2, double dLon2);
		static int calcAzimuth(int latE61, int lonE61, int latE62, int lonE62);
		static int calcAzimuth(double dLat1, double dLon1, double dLat2, double dLon2);

		Distance calcDistanceAzimuth(int latE6, int lonE6);
		Distance calcDistanceAzimuth(double dLat, double dLon);
		int calcDistance(int latE6, int lonE6);
		int calcDistance(double dLat, double dLon);
		int calcAzimuth(int latE6, int lonE6);
		int calcAzimuth(double dLat, double dLon);
};

	}
}

#endif
