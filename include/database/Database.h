/**
 * \file
 * Contains base class for all database plugins.
 *
 * @version 1.0
 * @author Michal Kuchta
 */

#ifndef _GCM_DATABASE_DATABASE_H
#	define _GCM_DATABASE_DATABASE_H

#include <gcm.h>
#include <exception>

#include <pluginmanager/Plugin.h>
#include <GC.h>
#include <Object.h>
#include <stl/Exception.h>
#include <stl/List.h>
#include <ui.h>

namespace GCM {
	namespace geolib {

class Geocache;
class GeocacheWaypoint;
class GenericWaypoint;
class Filter;
class GeocacheList;
//class GeocacheWaypointList;
typedef ArrayList<GeocacheWaypoint> GeocacheWaypointList;

	}
}

namespace GCM {
	namespace database {

/**
 * Abstract class that defines database operations.
 */
class Database: public Object {
	public:
		typedef enum {
			ST_FAILED,
			ST_CREATED,
			ST_UPDATED,
			ST_AVAIL_TO_DIS,
			ST_DIS_TO_ARCHIVE,
			ST_ARCHIVE_TO_DIS,
			ST_DIS_TO_AVAIL,
			ST_AVAIL_TO_ARCHIVE,
			ST_ARCHIVE_TO_AVAIL,
			ST_UP_TO_DATE
		} Result;

		/**
		 * Action hints that tells database to prepare for specific actions.
		 */
		enum ActionHint {
			AH_NONE,		/**< None hint - used primarily to tell
							'action done' after last action hint was set. */
			AH_IMPORT		/**< Big import is comming. */
		};

		struct Stats {
			int numCaches;
			int numWaypoints;

			Stats(): numCaches(0), numWaypoints(0) {}
		};

	public:
		virtual ~Database() {};

		virtual String getConnectionString() = 0;
		virtual String getId() = 0;

		virtual Result saveCache(GC<GCM::geolib::Geocache> cache) = 0;
		virtual Result saveCacheWaypoint(GC<GCM::geolib::GeocacheWaypoint> waypoint) = 0;
		virtual Result saveGenericWaypoint(GC<GCM::geolib::GenericWaypoint> waypoint) = 0;

		virtual void removeCache(GC<GCM::geolib::Geocache> cache) = 0;
		virtual void removeCacheWaypoint(GC<GCM::geolib::GeocacheWaypoint> waypoint) = 0;

		virtual GC<GCM::geolib::GeocacheWaypointList> getCacheWaypoints(GC<GCM::geolib::Geocache> cache) = 0;

		virtual void beginTransaction() = 0;
		virtual void commit() = 0;
		virtual void rollback() = 0;

		virtual void actionHint(ActionHint action) = 0;

		virtual const Stats &getStats() = 0;

		/**
		 * Load all caches matching specified filter.
		 */
		virtual GC<GCM::geolib::GeocacheList> listCaches(GC<GCM::geolib::Filter> f, ptrDialog progressDialog = NULL) = 0;

		/**
		 * Load long cache data (listing, hint)
		 */
		virtual void loadLongData(GC<GCM::geolib::Geocache> cache) = 0;

		/**
		 * Load attributes for specified cache.
		 */
		virtual void loadAttributes(GC<GCM::geolib::Geocache> cache) = 0;

		/**
		 * Load logs for specified cache.
		 * @param cache Cache to load logs
		 * @param count Optional number of last logs to load. If not specified,
		 *  all logs are loaded.
		 */
		virtual void loadLogs(GC<GCM::geolib::Geocache> cache, int count = -1) = 0;

		/**
		 * Load waypoints for specified cache.
		 */
		virtual void loadWaypoints(GC<GCM::geolib::Geocache> cache) = 0;

		virtual GCM::GC<GCM::geolib::Geocache> getByGCID(GCM::String gcId) = 0;
		virtual GCM::GC<GCM::geolib::GeocacheList> getByName(GCM::String name) = 0;
};

class DatabaseLoadFailed: public Exception {
	const char *what() const throw() {
		return "Database failed to load.";
	}
};

class DatabaseInfo: public GCM::Object {
	public:
		String id;
		String name;
		virtual GC<Database> open(const char *filename) = 0;
		virtual bool accept(const char *filename) = 0;

		DatabaseInfo(String id, String name): id(id), name(name) {}
};

typedef GC< List<DatabaseInfo> > (*fDatabase)();

	} // namespace database
} // namespace GCM

#endif
