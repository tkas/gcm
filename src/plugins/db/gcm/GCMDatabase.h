#ifndef _GCM_GCMDATABASE_H
# define _GCM_GCMDATABASE_H

#include <database/Database.h>
#include <sqlite/Database.h>
#include <GC.h>
#include <geolib/Geocache.h>
#include <map>

class GCMDatabase: public GCM::database::Database {
	public:
		typedef enum {
			WPT_GEOCACHE,
			WPT_GEOCACHE_INITIAL,
			WPT_GEOCACHE_CORRECTED,
			WPT_WAYPOINT
		} PointsWptTypes;

	private:
		typedef enum GCM::database::Database::ActionHint ActionHint;

		GCM::String file;
		GCM::GC< GCM::sqlite::Database > db;

		// Query for caches
		GCM::GC< GCM::sqlite::Stmt > stmtQuerySingleCacheForImport;
		GCM::GC< GCM::sqlite::Stmt > stmtQueryLongData;

		// Insert cache
		GCM::GC< GCM::sqlite::Stmt > stmtInsertSingleCache;
		GCM::GC< GCM::sqlite::Stmt > stmtInsertLongData;

		// Update cache
		GCM::GC< GCM::sqlite::Stmt > stmtUpdateCacheInfo;
		GCM::GC< GCM::sqlite::Stmt > stmtUpdateCacheNonbasicInfo;
		GCM::GC< GCM::sqlite::Stmt > stmtUpdateNotLockableCacheInfo;

		// For working with cache coordinates in points table
		GCM::GC< GCM::sqlite::Stmt > stmtQueryCacheCoords;
		GCM::GC< GCM::sqlite::Stmt > stmtInsertCoords;
		GCM::GC< GCM::sqlite::Stmt > stmtInsertCoordsIndex;
		GCM::GC< GCM::sqlite::Stmt > stmtUpdateCoords;
		GCM::GC< GCM::sqlite::Stmt > stmtUpdateCoordsType;
		GCM::GC< GCM::sqlite::Stmt > stmtUpdateCoordsAndType;
		GCM::GC< GCM::sqlite::Stmt > stmtUpdateCoordsIndex;
		GCM::GC< GCM::sqlite::Stmt > stmtDeleteCoords;
		GCM::GC< GCM::sqlite::Stmt > stmtDeleteCoordsIndex;
		GCM::GC< GCM::sqlite::Stmt > stmtUpdateCoordsTypeByCacheAndType;
		GCM::GC< GCM::sqlite::Stmt > stmtUpdateCoordsNameAndSubtype;
		GCM::GC< GCM::sqlite::Stmt > stmtUpdateCoordsName;

		// Waypoint
		GCM::GC< GCM::sqlite::Stmt > stmtQueryWaypoint;
		GCM::GC< GCM::sqlite::Stmt > stmtInsertWaypoint;
		GCM::GC< GCM::sqlite::Stmt > stmtQueryWaypointCoords;
		GCM::GC< GCM::sqlite::Stmt > stmtUpdateWaypoint;
		GCM::GC< GCM::sqlite::Stmt > stmtUpdateWaypointCoords;
		GCM::GC< GCM::sqlite::Stmt > stmtQueryCacheWaypoints;

		// Logs
		GCM::GC< GCM::sqlite::Stmt > stmtClearLogs;
		GCM::GC< GCM::sqlite::Stmt > stmtInsertLog;
		GCM::GC< GCM::sqlite::Stmt > stmtQueryLogs;

		// Attributes
		GCM::GC< GCM::sqlite::Stmt > stmtQueryAttributes;
		GCM::GC< GCM::sqlite::Stmt > stmtDeleteAttribute;
		GCM::GC< GCM::sqlite::Stmt > stmtInsertAttribute;

		// Stats
		GCM::GC< GCM::sqlite::Stmt > stmtStatsCaches;
		GCM::GC< GCM::sqlite::Stmt > stmtStatsWaypoints;

		Database::Stats stats;
		bool statsValid;

		GCM::database::Database::ActionHint currentHint;

		void createPointsTable();
		void createGeocacheTable();
		void createLongDataTable();
		void createWaypointTable();
		void createAttributesTable();
		void createLogsTable();
		void createTagsTable();
		void createFieldnotesTable();
		void createDb();

		void insertCoordIndex(int id, int latE6, int lonE6);

		void prepareStatements();
		void updateLogs(GCM::GC<GCM::geolib::Geocache> cache);
		void updateAttributes(GCM::GC<GCM::geolib::Geocache> cache);

		GCM::database::Database::Result updateCache(GCM::GC<GCM::geolib::Geocache> cache);
		GCM::database::Database::Result insertCache(GCM::GC<GCM::geolib::Geocache> cache);

		void remWpt(int id);
		GCM::GC<GCM::geolib::Geocache> buildCacheFromStmt(GCM::GC<GCM::sqlite::Stmt> stmt);

		void stopAction();
		void startAction(ActionHint hint);
		void startImportAction();
		void stopImportAction();

		std::map<int, GCM::geolib::Geocache::Status> importCache;

		struct WaypointMeta {
			int id;
			int user_flag;
			int point_id;
			WaypointMeta(int id, int user_flag, int point_id): id(id), user_flag(user_flag), point_id(point_id) {}
			WaypointMeta(): id(0), user_flag(0), point_id(0) {}
		};

		std::map<std::string, WaypointMeta> importWaypoints;

	public:
		GCMDatabase(const char *path);

		virtual GCM::String getConnectionString();
		virtual GCM::String getId();

		virtual GCM::database::Database::Result saveCache(GCM::GC<GCM::geolib::Geocache> cache);
		virtual GCM::database::Database::Result saveCacheWaypoint(GCM::GC<GCM::geolib::GeocacheWaypoint> waypoint);
		virtual GCM::database::Database::Result saveGenericWaypoint(GCM::GC<GCM::geolib::GenericWaypoint> waypoint);

		virtual void removeCache(GCM::GC<GCM::geolib::Geocache> cache);
		virtual void removeCacheWaypoint(GCM::GC<GCM::geolib::GeocacheWaypoint> waypoint);

		virtual GCM::GC<GCM::geolib::GeocacheWaypointList> getCacheWaypoints(GCM::GC<GCM::geolib::Geocache> cache);

		virtual void beginTransaction();
		virtual void commit();
		virtual void rollback();
		virtual void actionHint(GCM::database::Database::ActionHint hint);

		const GCM::database::Database::Stats &getStats();

		virtual GCM::GC<GCM::geolib::GeocacheList> listCaches(GCM::GC<GCM::geolib::Filter> f, ptrDialog progressDialog);

		virtual void loadLongData(GCM::GC<GCM::geolib::Geocache> cache);
		virtual void loadAttributes(GCM::GC<GCM::geolib::Geocache> cache);
		virtual void loadLogs(GCM::GC<GCM::geolib::Geocache> cache, int count = -1);
		virtual void loadWaypoints(GCM::GC<GCM::geolib::Geocache> cache);

		virtual GCM::GC<GCM::geolib::Geocache> getByGCID(GCM::String gcId);
		virtual GCM::GC<GCM::geolib::GeocacheList> getByName(GCM::String name);
};

#endif
