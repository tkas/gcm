#include <pluginmanager/Plugin.h>
#include <database/Database.h>
#include <GC.h>
#include "GGDatabase.h"
#include <util/Log.h>
#include <sqlite/Database.h>
#include <sqlite/Stmt.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

using namespace GCM::pluginmanager;
using namespace GCM;
using namespace GCM::sqlite;

GC<GCM::database::Database> openDb(const char *path) {
	GCM::util::Log::d("GCM", "Open database %s.", path);
	GGDatabase *db = new GGDatabase(path);
	return db;
}

class MyDbInfo: public GCM::database::DatabaseInfo {
	private:
		bool tableExists(GC<Stmt> stmt, const char *tblName) {
			stmt->bind(1, tblName);
			if (stmt->step() != SQLITE_ROW) {
				stmt->reset();
				return false;
			} else {
				stmt->reset();
				return true;
			}
		}

	public:
		MyDbInfo(String id, String name): GCM::database::DatabaseInfo(id, name) {}

		virtual GC<database::Database> open(const char *fileName) {
			return openDb(fileName);
		}

		virtual bool accept(const char *fileName) {
			// Try to open the database
			FILE *f = fopen(fileName, "rb");
			if (f) {
				char buff[16];
				fgets(buff, 16, f);
				fclose(f);

				if (strncmp(buff, "SQLite format 3", 15) == 0) {
					GC<Database> db = new Database();
					if (db->open(fileName, SQLITE_OPEN_READONLY) != SQLITE_OK) {
						return false;
					}

					// Test if some of GCM tables exists...
					GC<Stmt> stmt = db->prepare("SELECT name FROM sqlite_master WHERE type = 'table' AND name = ?");
					bool result = tableExists(stmt, "geocache") && tableExists(stmt, "waypoint") &&
						tableExists(stmt, "geolist") && tableExists(stmt, "geolog") &&
						tableExists(stmt, "geotag") && tableExists(stmt, "geotagcategory") &&
						tableExists(stmt, "geotagvalue") && tableExists(stmt, "coord_index") && tableExists(stmt, "wpt_coord_index");
						
					db = NULL;
					
					return result;
				} else {
					return false;
				}
			} else {
				return false;
			}
		}
};

GC< List<GCM::database::DatabaseInfo> > getDbInfo() {
	GC< List<GCM::database::DatabaseInfo> > out = new List<GCM::database::DatabaseInfo>();
	out->append(new MyDbInfo("ggdb", "GeoGet Database"));
	return out;
}

PLUGIN_EXPORT void gcm_pluginInit(Plugin *plugin) {
	plugin->author = "Niximor";
	plugin->version.major = 1;
	plugin->version.minor = 0;
	plugin->manager.registerFunction(plugin, "Database", callback(getDbInfo));
}

