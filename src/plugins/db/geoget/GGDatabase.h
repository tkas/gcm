#ifndef _GCM_GGDATABASE_H
# define _GCM_GGDATABASE_H

#include <database/Database.h>
#include <sqlite/Database.h>
#include <GC.h>
#include <geolib/Geocache.h>
#include <geolib/LogList.h>

class GGDatabase: public GCM::database::Database {
	private:
		GCM::String file;
		GCM::GC< GCM::sqlite::Database > db;

		Database::Stats stats;
		bool statsValid;

		GCM::GC<GCM::sqlite::Stmt> stmtInsertLong;

		void createGeocacheTable();
		void createGeolistTable();
		void createGeologTable();
		void createTagsTables();
		void createMetaTable();
		void createWaypointTable();

		void createDb();
		void prepareStatements();

		/**
		 * Convert time_t to GeoGet date of form YYYYMMDD
		 */
		static GCM::String timeToGGDate(time_t time);

		/**
		 * Convert time_t to GeoGet time of form HMM
		 */
		static GCM::String timeToGGTime(time_t time);

		/**
		 * Convert time_t to GeoGet date of form days-since-1900.fraction of day
		 */
		static double timeToGGDate2(time_t time);

		/**
		 * Convert time_t to GeoGet date of form YYYY-MM-DD HH:MM:SS.SSS
		 */
		static GCM::String timeToGGDate3(time_t time);

		/**
		 * Convert GG date of form YYYYMMDD to time_t.
		 */
		static time_t ggDateToTime(GCM::String ggDate);

		/**
		 * Convert GG date of form YYYYMMDD and time of form HMM to time_t.
		 */
		static time_t ggDateToTime(GCM::String ggDate, GCM::String ggTime);

		/**
		 * Convert GG date of form days-since-1900.fraction of day to time_t.
		 */
		static time_t ggDate2ToTime(double ggDate);

		static GCM::String GZDecompress(GCM::String str);
		static GCM::String GZCompress(GCM::String str);

		GCM::database::Database::Result createCache(GCM::GC<GCM::geolib::Geocache> cache);
		GCM::database::Database::Result updateCache(GCM::GC<GCM::geolib::Geocache> cache);

		void updateLogs(GCM::GC<GCM::geolib::LogList> list);

		static const unsigned int HASH_SEED;
		uint32_t computeHashLong(GCM::GC<GCM::geolib::Geocache> cache);
		uint32_t computeHashLog(GCM::GC<GCM::geolib::Log> log);

	public:
		GGDatabase(const char *path);

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

		virtual void actionHint(ActionHint hint);

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
