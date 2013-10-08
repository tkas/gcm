#include "GCMDatabase.h"
#include <GC.h>
#include <util/Log.h>
#include <list>

#include <geolib/GenericWaypoint.h>
#include <geolib/GeocacheWaypoint.h>
#include <geolib/Geocache.h>
#include <geolib/GeocacheList.h>
#include <geolib/Filter.h>
#include <geolib/ReferencePoint.h>

#include <config/AppConfig.h>
#include <stl/List.h>

using namespace GCM::sqlite;
using namespace GCM::pluginmanager;
using namespace GCM::database;
using namespace GCM::geolib;
using namespace GCM::config;
using namespace GCM;

void GCMDatabase::createPointsTable() {
	this->db->exec("CREATE TABLE IF NOT EXISTS \"points\" ("
			"\"id\" INTEGER PRIMARY KEY AUTOINCREMENT,"
			"\"latE6\" INT NULL,"
			"\"lonE6\" INT NULL,"
			"\"name\" TEXT NOT NULL,"
			"\"wpttype\" INT NOT NULL," // Waypoint type (geocache | waypoint | other)
			"\"subtype\" INT NULL," // The type of waypoint itself (traditional cache, final waypoint, ...)
			"\"ref_cache\" INT NULL,"
			"\"ref_waypoint\" INT NULL"
		")");
	//this->db->exec("CREATE UNIQUE INDEX IF NOT EXISTS \"points_id_idx\" ON \"points\" (\"id\")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"points_ref_cache_idx\" ON \"points\" (\"ref_cache\")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"points_ref_waypoint_idx\" ON \"points\" (\"ref_waypoint\")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"points_ref_cache_waypoint_idx\" ON \"points\" (\"ref_cache\", \"ref_waypoint\")");

	GC<Stmt> tblExists = this->db->prepare("SELECT DISTINCT tbl_name FROM sqlite_master WHERE tbl_name = ?");
	tblExists->bind(1, "points_idx");

	if (tblExists->step() != SQLITE_ROW) {
		this->db->exec("CREATE VIRTUAL TABLE \"points_idx\" USING rtree(id, minX, maxX, minY, maxY)");
	}

	tblExists->finalize();
}

void GCMDatabase::createGeocacheTable() {
	this->db->exec("CREATE TABLE IF NOT EXISTS \"geocaches\" ("
			"\"id\" INTEGER PRIMARY KEY,"
			"\"gcid\" TEXT NOT NULL,"
			"\"status\" INT,"
			"\"has_corrected\" INT,"
			"\"is_pmo\" INT,"
			"\"is_own\" INT,"
			"\"name\" TEXT NOT NULL,"
			"\"placed_by\" TEXT,"
			"\"owner\" TEXT,"
			"\"owner_id\" INT,"
			"\"type\" INT,"
			"\"size\" INT,"
			"\"difficulty\" INT,"
			"\"terrain\" INT,"
			"\"country\" TEXT,"
			"\"state\" TEXT,"
			"\"favorite_points\" INT,"
			"\"dtcreated\" INT,"
			"\"dtupdate\" INT,"
			"\"dtfound\" INT,"
			"\"comment\" TEXT,"
			"\"locked\" INT"
		")");
	//this->db->exec("CREATE INDEX IF NOT EXISTS \"geocaches_id_idx\" ON \"geocaches\" (\"id\")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"geocaches_type_idx\" ON \"geocaches\" (\"type\")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"geocaches_dtupdate_idx\" ON \"geocaches\" (\"dtupdate\")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"geocaches_dtfound_idx\" ON \"geocaches\" (\"dtfound\")");
}

void GCMDatabase::createLongDataTable() {
	this->db->exec("CREATE TABLE IF NOT EXISTS \"geocache_long_data\" ("
		"\"id\" INTEGER PRIMARY KEY,"
		"\"short_desc\" TEXT,"
		"\"long_desc\" TEXT,"
		"\"short_desc_html\" INT,"
		"\"long_desc_html\" INT,"
		"\"hint\" TEXT"
		")");
}

void GCMDatabase::createWaypointTable() {
	this->db->exec("CREATE TABLE IF NOT EXISTS \"waypoints\" ("
			"\"id\" INTEGER PRIMARY KEY AUTOINCREMENT,"
			"\"ref_cache\" INT,"
			"\"code\" TEXT,"
			"\"name\" TEXT,"
			"\"type\" INT,"
			"\"description\" TEXT,"
			"\"user_comment\" TEXT DEFAULT '0',"
			"\"user_flag\" INT DEFAULT '0',"
			"\"dtupdate\" INT,"
			"\"corrected\" INT"
		")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"wpt_ref_cache_idx\" ON \"waypoints\" (\"ref_cache\")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"wpt_code_idx\" ON \"waypoints\" (\"code\")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"wpt_type_idx\" ON \"waypoints\" (\"type\")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"wpt_dtupdate_idx\" ON \"waypoints\" (\"dtupdate\")");
}

void GCMDatabase::createAttributesTable() {
	this->db->exec("CREATE TABLE IF NOT EXISTS \"attributes\" ("
			"\"id\" INTEGER PRIMARY KEY AUTOINCREMENT,"
			"\"ref_cache\" INT,"
			"\"attribute_id\" INT,"
			"\"positive\" INT"
		")");
	//this->db->exec("CREATE INDEX IF NOT EXISTS \"attributes_id_idx\" ON \"attributes\" (\"id\")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"attributes_ref_cache_idx\" ON \"attributes\" (\"ref_cache\")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"attributes_attribute_id_idx\" ON \"attributes\" (\"attribute_id\")");
}

void GCMDatabase::createLogsTable() {
	this->db->exec("CREATE TABLE IF NOT EXISTS \"logs\" ("
			"\"id\" INTEGER PRIMARY KEY,"
			"\"ref_cache\" INT,"
			"\"date\" INT,"
			"\"type\" INT,"
			"\"user\" TEXT,"
			"\"user_id\" INT,"
			"\"text\" TEXT,"
			"\"isencoded\" INT,"
			"\"dtupdate\" INT"
		")");
	//this->db->exec("CREATE INDEX IF NOT EXISTS \"logs_id_idx\" ON \"logs\" (\"id\")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"logs_ref_cache_idx\" ON \"logs\" (\"ref_cache\")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"logs_user_id_idx\" ON \"logs\" (\"user_id\")");
}

void GCMDatabase::createTagsTable() {
	this->db->exec("CREATE TABLE IF NOT EXISTS \"tags\" ("
			"\"id\" INTEGER PRIMARY KEY AUTOINCREMENT,"
			"\"ref_cache\" INT,"
			"\"name\" TEXT,"
			"\"value\" TEXT"
		")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"tag_ref_cache_idx\" ON \"tags\" (\"ref_cache\")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"tag_name_idx\" ON \"tags\" (\"name\")");
}

void GCMDatabase::createFieldnotesTable() {
	this->db->exec("CREATE TABLE IF NOT EXISTS \"fieldnotes\" ("
			"\"id\" INTEGER PRIMARY KEY AUTOINCREMENT,"
			"\"ref_cache\" INT,"
			"\"date\" INT,"
			"\"type\" INT,"
			"\"text\" TEXT"
		")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"fieldnotes_ref_cache_idx\" ON \"fieldnotes\" (\"ref_cache\")");
	this->db->exec("CREATE INDEX IF NOT EXISTS \"fieldnotes_date_idx\" ON \"fieldnotes\" (\"date\")");
}

/**
 * Create the database scheme
 */
void GCMDatabase::createDb() {
	this->createPointsTable();
	this->createGeocacheTable();
	this->createLongDataTable();
	this->createWaypointTable();
	this->createAttributesTable();
	this->createLogsTable();
	this->createTagsTable();
	this->createFieldnotesTable();
}

void GCMDatabase::prepareStatements() {
	this->stmtQuerySingleCacheForImport = this->db->prepare("SELECT \"id\", \"status\", \"locked\" FROM \"geocaches\" WHERE \"id\" = ?");

	this->stmtInsertCoords = this->db->prepare("INSERT INTO \"points\" ("
		"\"latE6\", \"lonE6\", \"name\", \"wpttype\", \"subtype\", \"ref_cache\", "
		"\"ref_waypoint\""
	") VALUES (?, ?, ?, ?, ?, ?, ?)");

	this->stmtInsertSingleCache = this->db->prepare("INSERT INTO \"geocaches\" ("
		"\"id\", \"gcid\", \"status\", \"has_corrected\", \"is_pmo\", \"name\", "
		"\"placed_by\", \"owner\", \"owner_id\", \"type\", \"size\", "
		"\"difficulty\", \"terrain\", \"country\", \"state\", \"favorite_points\", "
		"\"dtcreated\", \"dtupdate\", \"dtfound\", \"comment\", \"locked\", \"is_own\""
	") VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");

	this->stmtQueryCacheCoords = this->db->prepare("SELECT \"wpttype\", \"id\", \"latE6\", \"lonE6\", \"name\", \"subtype\" FROM \"points\" WHERE \"ref_cache\" = ? AND \"ref_waypoint\" IS NULL");
	this->stmtInsertCoordsIndex = this->db->prepare("INSERT INTO \"points_idx\" (\"id\", \"minX\", \"minY\", \"maxX\", \"maxY\") VALUES (?, ?, ?, ?2, ?3)");
	this->stmtUpdateCoords = this->db->prepare("UPDATE \"points\" SET \"latE6\" = ?, \"lonE6\" = ? WHERE \"id\" = ?");
	this->stmtUpdateCoordsType = this->db->prepare("UPDATE \"points\" SET \"wpttype\" = ? WHERE \"id\" = ?");
	this->stmtUpdateCoordsAndType = this->db->prepare("UPDATE \"points\" SET \"latE6\" = ?, \"lonE6\" = ?, \"wpttype\" = ? WHERE \"id\" = ?");
	this->stmtUpdateCoordsIndex = this->db->prepare("UPDATE \"points_idx\" SET \"minX\" = ?, \"minY\" = ?, \"maxX\" = ?1, \"maxY\" = ?2 WHERE \"id\" = ?3");
	this->stmtDeleteCoords = this->db->prepare("DELETE FROM \"points\" WHERE \"id\" = ?");
	this->stmtDeleteCoordsIndex = this->db->prepare("DELETE FROM \"points_idx\" WHERE \"id\" = ?");
	this->stmtUpdateCoordsTypeByCacheAndType = this->db->prepare("UPDATE \"points\" SET \"wpttype\" = ? WHERE \"ref_cache\" = ? AND \"wpttype\" = ?");
	this->stmtUpdateCoordsNameAndSubtype = this->db->prepare("UPDATE \"points\" SET \"name\" = ?, \"subtype\" = ? WHERE \"id\" = ?");
	this->stmtUpdateCoordsName = this->db->prepare("UPDATE \"points\" SET \"name\" = ? WHERE \"id\" = ?");

	this->stmtUpdateCacheInfo = this->db->prepare("UPDATE \"geocaches\" SET "
			"status = IFNULL(?, status), "
			"name = IFNULL(?, name), "
			"placed_by = IFNULL(?, placed_by), "
			"owner = IFNULL(?, owner), "
			"owner_id = IFNULL(?, owner_id), "
			"type = IFNULL(?, type), "
			"size = IFNULL(?, size), "
			"difficulty = IFNULL(?, difficulty), "
			"terrain = IFNULL(?, terrain), "
			"country = IFNULL(?, country), "
			"state = IFNULL(?, state), "
			"favorite_points = IFNULL(?, favorite_points), "
			"dtcreated = IFNULL(?, dtcreated), "
			"dtupdate = IFNULL(?, dtupdate), "
			"dtfound = IFNULL(?, dtfound), "
			"comment = IFNULL(?, comment), "
			"locked = IFNULL(?, locked), "
			"has_corrected = IFNULL(?, has_corrected), "
			"is_own = IFNULL(?, is_own) "
			"WHERE id = ?");

	this->stmtUpdateCacheNonbasicInfo = this->db->prepare("UPDATE \"geocaches\" SET "
			"status = IFNULL(?, status), "
			"name = IFNULL(?, name), "
			"placed_by = IFNULL(?, placed_by), "
			"owner = IFNULL(?, owner), "
			"owner_id = IFNULL(?, owner_id), "
			"country = IFNULL(?, country), "
			"state = IFNULL(?, state), "
			"favorite_points = IFNULL(?, favorite_points), "
			"dtupdate = IFNULL(?, dtupdate), "
			"dtfound = IFNULL(?, dtfound), "
			"comment = IFNULL(?, comment), "
			"locked = IFNULL(?, locked), "
			"has_corrected = IFNULL(?, has_corrected), "
			"is_own = IFNULL(?, is_own) "
			"WHERE id = ?");

	this->stmtUpdateNotLockableCacheInfo = this->db->prepare("UPDATE \"geocaches\" SET "
		"favorite_points = IFNULL(?, favorite_points), "
		"dtupdate = IFNULL(?, dtupdate), "
		"dtfound = IFNULL(?, dtfound), "
		"comment = IFNULL(?, comment), "
		"locked = IFNULL(?, locked), "
		"has_corrected = IFNULL(?, has_corrected), "
		"is_own = IFNULL(?, is_own) "
		"WHERE id = ?");

	this->stmtInsertLongData = this->db->prepare("INSERT OR REPLACE INTO \"geocache_long_data\" (\"id\", \"short_desc\", \"long_desc\", \"short_desc_html\", \"long_desc_html\", \"hint\") VALUES (?,?,?,?,?,?)");
	this->stmtQueryLongData = this->db->prepare("SELECT \"short_desc\", \"long_desc\", \"short_desc_html\", \"long_desc_html\", \"hint\" FROM \"geocache_long_data\" WHERE \"id\" = ?");

	this->stmtQueryWaypoint = this->db->prepare("SELECT \"id\", \"user_flag\" FROM \"waypoints\" WHERE \"code\" = ?");
	this->stmtInsertWaypoint = this->db->prepare("INSERT INTO \"waypoints\" (\"ref_cache\", \"code\", \"name\", \"type\", \"description\", \"user_flag\", \"user_comment\", \"dtupdate\", \"corrected\") VALUES (?,?,?,?,?,?,?,?,?)");
	this->stmtUpdateWaypoint = this->db->prepare("UPDATE \"waypoints\" SET \"name\" = ?, \"type\" = ?, \"description\" = ?, \"user_flag\" = ?, \"user_comment\" = ?, \"dtupdate\" = ?, \"corrected\" = ? WHERE \"id\" = ?");
	this->stmtQueryWaypointCoords = this->db->prepare("SELECT \"id\" FROM \"points\" WHERE \"ref_waypoint\" = ?");
	this->stmtUpdateWaypointCoords = this->db->prepare("UPDATE \"points\" SET \"latE6\" = ?, \"lonE6\" = ?, \"subtype\" = ? WHERE \"ref_waypoint\" = ?");
	this->stmtQueryCacheWaypoints = this->db->prepare("SELECT \"waypoints\".\"id\", \"code\", \"waypoints\".\"name\", \"type\", \"description\", \"user_comment\", \"user_flag\", \"corrected\", \"dtupdate\", \"latE6\", \"lonE6\"  FROM \"waypoints\" LEFT JOIN \"points\" ON (\"points\".\"ref_waypoint\" = \"waypoints\".\"id\") WHERE \"waypoints\".\"ref_cache\" = ?");

	this->stmtClearLogs = this->db->prepare("DELETE FROM \"logs\" WHERE \"ref_cache\" = ? AND \"id\" NOT IN (SELECT \"id\" FROM \"logs\" WHERE \"ref_cache\" = ?1 ORDER BY \"date\" DESC LIMIT ?) AND \"id\" NOT IN (SELECT \"id\" FROM \"logs\" WHERE \"ref_cache\" = ?1 AND \"user_id\" = ?)");
	this->stmtInsertLog = this->db->prepare("INSERT OR REPLACE INTO \"logs\" (\"id\", \"ref_cache\", \"date\", \"type\", \"user\", \"user_id\", \"text\", \"isencoded\", \"dtupdate\") VALUES (?,?,?,?,?,?,?,?,?)");
	this->stmtQueryLogs = this->db->prepare("SELECT \"id\", \"date\", \"type\", \"user\", \"user_id\", \"text\", \"isencoded\", \"dtupdate\" FROM \"logs\" WHERE \"ref_cache\" = ? ORDER BY \"date\" DESC, \"id\" DESC LIMIT ?");

	this->stmtQueryAttributes = this->db->prepare("SELECT \"attribute_id\", \"positive\", \"id\" FROM \"attributes\" WHERE \"ref_cache\" = ?");
	this->stmtDeleteAttribute = this->db->prepare("DELETE FROM \"attributes\" WHERE \"id\" = ?");
	this->stmtInsertAttribute = this->db->prepare("INSERT INTO \"attributes\" (\"ref_cache\", \"attribute_id\", \"positive\") VALUES (?, ?, ?)");

	this->stmtStatsCaches = this->db->prepare("SELECT COUNT(*) FROM \"geocaches\"");
	this->stmtStatsWaypoints = this->db->prepare("SELECT COUNT(*) FROM \"waypoints\"");
}

GCMDatabase::GCMDatabase(const char *path): db(new GCM::sqlite::Database()), statsValid(false), currentHint(GCM::database::Database::AH_NONE) {
	if (this->db->open(path, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE) != SQLITE_OK) {
		throw GCM::database::DatabaseLoadFailed();
	}

	this->file = path;

	this->createDb();
	this->prepareStatements();
	this->getStats();
}

String GCMDatabase::getConnectionString() {
	return this->file;
}

String GCMDatabase::getId() {
	return "gcm";
}

void GCMDatabase::actionHint(GCM::database::Database::ActionHint hint) {
	if (hint == AH_NONE) {
		stopAction();
	} else {
		startAction(hint);
	}
}

void GCMDatabase::stopAction() {
	switch (currentHint) {
		case AH_IMPORT:
			stopImportAction();
			break;

		default:
			break;
	}

	currentHint = AH_NONE;
}

void GCMDatabase::startAction(ActionHint hint) {
	currentHint = hint;
	switch (currentHint) {
		case AH_IMPORT:
			startImportAction();
			break;

		default:
			break;
	}
}

void GCMDatabase::startImportAction() {
	GC<Stmt> stmt = this->db->prepare("SELECT \"g\".\"id\", \"g\".\"state\" FROM \"geocaches\" \"g\"");
	while (stmt->step() == SQLITE_ROW) {
		importCache[stmt->columnInt(0)] = (GCM::geolib::Geocache::Status)stmt->columnInt(1);
	}

	stmt = this->db->prepare("SELECT \"w\".\"code\", \"w\".\"id\", \"w\".\"user_flag\", \"p\".\"id\" FROM \"waypoints\" \"w\" LEFT JOIN \"points\" \"p\" ON (\"p\".\"ref_waypoint\" = \"w\".\"id\")");
	while (stmt->step() == SQLITE_ROW) {
		importWaypoints[stmt->columnString(0)] = WaypointMeta(stmt->columnInt(1), stmt->columnInt(2), stmt->columnInt(3));
	}

	this->db->exec("PRAGMA synchronous=OFF");

	GCM::util::Log::d("GCMDatabase", "Loaded %d waypoints to cache.", importWaypoints.size());
}

void GCMDatabase::stopImportAction() {
	importCache.clear();
	importWaypoints.clear();
	this->db->exec("PRAGMA synchronous=ON");
}

GCM::database::Database::Result GCMDatabase::updateCache(GC<Geocache> cache) {
	this->db->exec("SAVEPOINT \"update_cache\"");

	// Original cache data are in this->stmtQuerySingleCacheForImport.
	int cacheId = cache->getRowId();
	int lockFlags = cache->getLockFlags();

	GC<Stmt> stmt;

	stmt = this->stmtQueryCacheCoords;
	stmt->bind(1, cacheId);

	bool correctedSaved = false;

	while (stmt->step() == SQLITE_ROW) {
		switch (stmt->columnInt(0)) {
			// Has only initial coordinates, not corrected.
			case WPT_GEOCACHE:
				// Cache has only initial coordinates. Compare and if differs,
				// update.
				if (!cache->hasCorrectedCoords()) {
					// First test if coords are locked, then if coords differ.
					if ((lockFlags & Geocache::LOCK_COORDS) == 0 && (cache->getLatitudeE6() != stmt->columnInt(2) || cache->getLongitudeE6() != stmt->columnInt(3))) {
						// UPDATE \"points\" SET \"latE6\" = ?, \"lonE6\" = ? WHERE \"id\" = ?
						GC<Stmt> stmtCoords = this->stmtUpdateCoords;
						stmtCoords->bind(1, cache->getLatitudeE6());
						stmtCoords->bind(2, cache->getLongitudeE6());
						stmtCoords->bind(3, stmt->columnInt(1));
						if (stmtCoords->step() == SQLITE_DONE) {
							stmtCoords->reset();

							// UPDATE \"points_idx\" SET \"minX\" = ?, \"minY\" = ?, \"maxX\" = ?, \"maxY\" = ? WHERE \"id\" = ?
							stmtCoords = this->stmtUpdateCoordsIndex;
							stmtCoords->bind(1, cache->getLatitude());
							stmtCoords->bind(2, cache->getLongitude());
							stmtCoords->bind(3, stmt->columnInt(1));
							stmtCoords->step();
							stmtCoords->reset();
						} else {
							stmtCoords->reset();
						}
					}
				} else {
					// Coords differs
					if ((lockFlags & Geocache::LOCK_COORDS) == 0 && (cache->getInitialLatitudeE6() != stmt->columnInt(2) || cache->getInitialLongitudeE6() != stmt->columnInt(3))) {
						// UPDATE \"points\" SET \"latE6\" = ?, \"lonE6\" = ?, \"wpttype\" = ? WHERE \"id\" = ?
						GC<Stmt> stmtCoords = this->stmtUpdateCoordsAndType;
						stmtCoords->bind(1, cache->getInitialLatitudeE6());
						stmtCoords->bind(2, cache->getInitialLongitudeE6());
						stmtCoords->bind(3, WPT_GEOCACHE_INITIAL);
						stmtCoords->bind(4, stmt->columnInt(1));

						if (stmtCoords->step() == SQLITE_DONE) {
							stmtCoords->reset();
							stmtCoords = this->stmtUpdateCoordsIndex;
							stmtCoords->bind(1, cache->getInitialLatitude());
							stmtCoords->bind(2, cache->getInitialLongitude());
							stmtCoords->bind(3, stmt->columnInt(1));

							stmtCoords->step();
							stmtCoords->reset();
						} else {
							stmtCoords->reset();
						}

					// Coords are same or locked, update only type
					} else {
						// UPDATE \"points\" SET \"wpttype\" = ? WHERE \"id\" = ?
						GC<Stmt> stmtCoords = this->stmtUpdateCoordsType;
						stmtCoords->bind(1, WPT_GEOCACHE_INITIAL);
						stmtCoords->bind(2, stmt->columnInt(1));

						stmtCoords->step();
						stmtCoords->reset();
					}
				}
				break;

			// Has initial coordinates.
			case WPT_GEOCACHE_INITIAL:
				// Coords not locked and valid.
				if ((lockFlags & Geocache::LOCK_COORDS) == 0 && cache->getInitialLatitudeE6() != 0 && cache->getInitialLongitudeE6() != 0) {
					if ((cache->getInitialLatitudeE6() != stmt->columnInt(2) || cache->getInitialLongitudeE6() != stmt->columnInt(3))) {
						// UPDATE \"points\" SET \"latE6\" = ?, \"lonE6\" = ? WHERE \"id\" = ?
						GC<Stmt> stmtCoords = this->stmtUpdateCoords;
						stmtCoords->bind(1, cache->getInitialLatitudeE6());
						stmtCoords->bind(2, cache->getInitialLongitudeE6());
						stmtCoords->bind(3, stmt->columnInt(1));
						if (stmtCoords->step() == SQLITE_DONE) {
							stmtCoords->reset();

							stmtCoords->reset();
							stmtCoords = this->stmtUpdateCoordsIndex;
							stmtCoords->bind(1, cache->getInitialLatitude());
							stmtCoords->bind(2, cache->getInitialLongitude());
							stmtCoords->bind(3, stmt->columnInt(1));
						} else {
							stmtCoords->reset();
						}
					}
				}
				break;

			case WPT_GEOCACHE_CORRECTED:
				if (cache->hasCorrectedCoords() && (cache->getLatitudeE6() != stmt->columnInt(2) || cache->getLongitudeE6() != stmt->columnInt(3))) {

					// A little hack to remove corrected coordinates, if the are nulled.
					if (cache->getLatitudeE6() == 0 && cache->getLongitudeE6() == 0) {
						// Remove corrected coordinates.
						// "DELETE FROM \"points\" WHERE \"id\" = ?"
						GCM::GC<Stmt> stmtCoords = this->stmtDeleteCoords;
						stmtCoords->bind(1, stmt->columnInt(1));
						if (stmtCoords->step() == SQLITE_DONE) {
							stmtCoords->reset();

							// Remove index
							// "DELETE FROM \"points_idx\" WHERE \"id\" = ?"
							stmtCoords = this->stmtDeleteCoordsIndex;
							stmtCoords->bind(1, stmt->columnInt(1));
							stmtCoords->step();
							stmtCoords->reset();

							// Revert initial coordinates back to cache coordinates.
							//"UPDATE \"points\" SET \"wpttype\" = ? WHERE \"ref_cache\" = ? AND \"wpttype\" = ?"
							stmtCoords = this->stmtUpdateCoordsTypeByCacheAndType;
							stmtCoords->bind(1, WPT_GEOCACHE);
							stmtCoords->bind(2, cache->getRowId());
							stmtCoords->bind(3, WPT_GEOCACHE_INITIAL);
							stmtCoords->step();
							stmtCoords->reset();
						} else {
							stmtCoords->reset();
						}
					} else {
						GC<Stmt> stmtCoords = this->stmtUpdateCoords;
						stmtCoords->bind(1, cache->getLatitudeE6());
						stmtCoords->bind(2, cache->getLongitudeE6());
						stmtCoords->bind(3, stmt->columnInt(1));
						if (stmtCoords->step() == SQLITE_DONE) {
							stmtCoords->reset();

							stmtCoords->reset();
							stmtCoords = this->stmtUpdateCoordsIndex;
							stmtCoords->bind(1, cache->getLatitude());
							stmtCoords->bind(2, cache->getLongitude());
							stmtCoords->bind(3, stmt->columnInt(1));
						} else {
							stmtCoords->reset();
						}
					}

					correctedSaved = true;
				}
				break;
		}

		// Name and type can be updated
		if (
			(lockFlags & Geocache::LOCK_INFO) == 0
			&& (
				stmt->columnInt(5) != (int)cache->getCacheType()
				|| !cache->getName()->equals(stmt->columnString(4))
			))
		{
			GCM::GC< Stmt> stmtCoords = stmtUpdateCoordsNameAndSubtype;

			// "UPDATE \"points\" SET \"name\" = ?, \"subtype\" = ? WHERE \"id\" = ?"
			stmtCoords->bind(1, cache->getName());
			stmtCoords->bind(2, (int)cache->getCacheType());
			stmtCoords->bind(3, stmt->columnInt(1));
			stmtCoords->reset();
		// Only name can be updated
		} else if ((lockFlags & Geocache::LOCK_NONBASIC) == 0 && !cache->getName()->equals(stmt->columnString(4))) {
			// "UPDATE \"points\" SET \"name\" = ? WHERE \"id\" = ?"
			GCM::GC< Stmt> stmtCoords = stmtUpdateCoordsName;
			stmtCoords->bind(1, cache->getName());
			stmtCoords->bind(2, stmt->columnInt(1));
			stmtCoords->step();
			stmtCoords->reset();
		}
	}
	stmt->reset();

	// Create new corrected coordinates entry.
	if (!correctedSaved && cache->hasCorrectedCoords()) {
		// INSERT INTO \"points\" (\"latE6\", \"lonE6\", \"name\", \"wpttype\", \"subtype\", \"ref_cache\", \"ref_waypoint\") VALUES (?, ?, ?, ?, ?, ?, ?)
		stmt = this->stmtInsertCoords;
		stmt->bind(1, cache->getLatitudeE6());
		stmt->bind(2, cache->getLongitudeE6());
		stmt->bind(3, cache->getName());
		stmt->bind(4, WPT_GEOCACHE_CORRECTED);
		stmt->bind(5, (int)cache->getCacheType());
		stmt->bind(6, cache->getRowId());
		stmt->bindNull(7);

		if (stmt->step() == SQLITE_DONE) {
			insertCoordIndex(this->db->lastInsertId(), cache->getLatitudeE6(), cache->getLongitudeE6());
		}
	}

	/*if (lockFlags & Geocache::LOCK_COORDS) == 0) {
		bool hasCorrected = false;
		//bool hasInitial = false;
		bool initialNeedConv = false;
		int correctedId = 0;
		int initialId = 0;

		// Query for coordinates that are valid for this cache.
		stmt = this->stmtQueryCacheCoords;
		stmt->bind(1, cacheId);

		while (stmt->step() == SQLITE_ROW) {
			if (stmt->columnInt(0) == WPT_GEOCACHE) {
				//hasInitial = true;
				initialNeedConv = true;
				initialId = stmt->columnInt(1);
			} else if (stmt->columnInt(0) == WPT_GEOCACHE_INITIAL) {
				//hasInitial = true;
				initialId = stmt->columnInt(1);
			} else if (stmt->columnInt(0) == WPT_GEOCACHE_CORRECTED) {
				hasCorrected = true;
				correctedId = stmt->columnInt(1);
			}
		}

		stmt->reset();

		if (!cache->hasCorrectedCoords() || hasCorrected) {
			// Don't have corrected coordinates at all, or
			// has corrected coordinates already stored.

			if (!cache->hasCorrectedCoords()) {
				stmt = this->stmtUpdateInitialCacheCoords;
			} else {
				stmt = this->stmtUpdateCorrectedCacheCoords;
			}

			stmt->bind(1, cache->getLatitudeE6());
			stmt->bind(2, cache->getLongitudeE6());
			stmt->bind(3, cacheId);

			if (!cache->hasCorrectedCoords()) {
				stmt->bind(4, WPT_GEOCACHE);
				stmt->bind(5, WPT_GEOCACHE_INITIAL);
			} else {
				stmt->bind(4, WPT_GEOCACHE_CORRECTED);
			}

			stmt->step();
			stmt->reset();

			int updateIndex;
			if (!cache->hasCorrectedCoords()) {
				// Update the index for initial coordinates.
				updateIndex = initialId;
			} else {
				updateIndex = correctedId;
			}

			// Update the index
			stmt = this->stmtUpdateCoordsIndex;
			stmt->bind(1, cache->getLatitudeE6());
			stmt->bind(2, cache->getLongitudeE6());
			stmt->bind(3, cache->getLatitudeE6());
			stmt->bind(4, cache->getLongitudeE6());
			stmt->bind(5, updateIndex);
			stmt->step();
			stmt->reset();
		} else {
			// Now has corrected, in db has only initial. Create new entry
			// for corrected coords.

			// Initial coordinates are without the "has corrected" flag.
			// Need to convert it to contain the flag.
			if (initialNeedConv) {
				stmt = this->stmtUpdateInitialCoordsType;
				stmt->bind(1, WPT_GEOCACHE_INITIAL);
				stmt->bind(2, cacheId);
				stmt->bind(3, WPT_GEOCACHE);
				stmt->step();
				stmt->reset();
			}

			stmt = this->stmtInsertCoords;
			stmt->bind(1, cache->getLatitudeE6());
			stmt->bind(2, cache->getLongitudeE6());
			stmt->bind(3, cache->getName());
			stmt->bind(4, WPT_GEOCACHE_CORRECTED);
			stmt->bind(5, cache->getCacheType());
			stmt->bind(6, cacheId);
			stmt->bindNull(7); // NULL for waypoint type.
			stmt->step(); // Save the coords.
			stmt->reset();

			stmt = this->stmtInsertCoordsIndex;
			stmt->bind(1, this->db->lastInsertId());
			stmt->bind(2, cache->getLatitudeE6());
			stmt->bind(3, cache->getLongitudeE6());
			stmt->bind(4, cache->getLatitudeE6());
			stmt->bind(5, cache->getLongitudeE6());
			stmt->step();
			stmt->reset();
		}
	} // !LOCK_COORDS
	*/

	if ((lockFlags & Geocache::LOCK_INFO) == 0) {
		// Update all cache info
		stmt = this->stmtUpdateCacheInfo;

		// UPDATE \"geocaches\" SET status = ?, name = ?, placed_by = ?, owner = ?, owner_id = ?, type = ?, size = ?, difficulty = ?, terrain = ?, country = ?, state = ?, favorite_points = ?, dtcreated = ?, dtupdate = ?, dtfound = ?, comment = ?, locked = ?, has_corrected = ?, is_own = ? WHERE id = ?
		stmt->bind(1, 1 * cache->isDisabled() + 2 * cache->isArchived());
		stmt->bindIfModified(2, cache->getName());
		stmt->bindIfModified(3, cache->getPlacedBy());
		stmt->bindIfModified(4, cache->getOwner());
		stmt->bindIfModified(5, cache->getOwnerId());
		stmt->bindIfModified(6, cache->getCacheType().get<int>());
		stmt->bindIfModified(7, cache->getSize().get<int>());
		stmt->bindIfModified(8, cache->getDifficulty());
		stmt->bindIfModified(9, cache->getTerrain());
		stmt->bindIfModified(10, cache->getCountry());
		stmt->bindIfModified(11, cache->getState());
		stmt->bindIfModified(12, cache->getFavPoints());
		stmt->bindIfModified(13, cache->getDtCreated());
		stmt->bind(14, (int64_t)time(NULL));
		stmt->bindIfModified(15, cache->getDtFound());
		stmt->bindIfModified(16, cache->getComment());
		stmt->bindIfModified(17, cache->getLockFlags());
		stmt->bindIfModified(18, cache->hasCorrectedCoords());
		stmt->bindIfModified(19, cache->isOwn());
		stmt->bind(20, cacheId);

		stmt->step();
		stmt->reset();
	} else if ((lockFlags & Geocache::LOCK_NONBASIC) == 0) {
		// Update only non-basic info
		stmt = this->stmtUpdateCacheNonbasicInfo;

		stmt->bind(1, 1 * cache->isDisabled() + 2 * cache->isArchived());
		stmt->bindIfModified(2, cache->getName());
		stmt->bindIfModified(3, cache->getPlacedBy());
		stmt->bindIfModified(4, cache->getOwner());
		stmt->bindIfModified(5, cache->getOwnerId());
		stmt->bindIfModified(6, cache->getCountry());
		stmt->bindIfModified(7, cache->getState());
		stmt->bindIfModified(8, cache->getFavPoints());
		stmt->bind(9, (int64_t)time(NULL));
		stmt->bindIfModified(10, cache->getDtFound());
		stmt->bindIfModified(11, cache->getComment());
		stmt->bindIfModified(12, cache->getLockFlags());
		stmt->bindIfModified(13, cache->hasCorrectedCoords());
		stmt->bindIfModified(14, cache->isOwn());
		stmt->bind(15, cacheId);

		stmt->step();
		stmt->reset();
	} else {
		stmt = this->stmtUpdateNotLockableCacheInfo;

		stmt->bindIfModified(1, cache->getFavPoints());
		stmt->bind(2, (int64_t)time(NULL));
		stmt->bindIfModified(3, cache->getDtFound());
		stmt->bindIfModified(4, cache->getComment());
		stmt->bindIfModified(5, cache->getLockFlags());
		stmt->bindIfModified(6, cache->hasCorrectedCoords());
		stmt->bindIfModified(7, cache->isOwn());
		stmt->bind(8, cacheId);

		stmt->step();
		stmt->reset();
	}

	if ((lockFlags & Geocache::LOCK_LISTING) == 0) {
		if (cache->isLongLoaded()) {
			stmt = this->stmtInsertLongData;
			stmt->bind(1, cache->getRowId());
			stmt->bind(2, cache->getShortDescription());
			stmt->bind(3, cache->getLongDescription());
			stmt->bind(4, cache->getSDIsHTML());
			stmt->bind(5, cache->getLDIsHTML());
			stmt->bind(6, cache->getHint());

			stmt->step();
			stmt->reset();
		}
	} // !LOCK_LISTING

	cache->setDtLastUpdate(time(NULL));

	this->db->exec("RELEASE \"update_cache\"");

	return GCM::database::Database::ST_UPDATED;
}

GCM::database::Database::Result GCMDatabase::insertCache(GC<Geocache> cache) {
	/* INSERT INTO \"geocaches\" ("
		"\"id\", \"gcid\", \"status\", \"has_corrected\", \"is_pmo\", \"name\", "
		"\"placed_by\", \"owner\", \"owner_id\", \"type\", \"size\", "
		"\"difficulty\", \"terrain\", \"country\", \"state\", \"favorite_points\", "
		"\"dtcreated\", \"dtupdate\", \"dtfound\", \"comment\", \"locked\""
	") VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)*/
	GC<Stmt> stmt = this->stmtInsertSingleCache;

	this->db->exec("SAVEPOINT \"insert_cache\"");

	stmt->bind(1, cache->getRowId()); // id
	stmt->bind(2, cache->getId()); // gcid

	int status = 1 * cache->isDisabled() + 2 * cache->isArchived();
	stmt->bind(3, status); // status

	stmt->bind(4, cache->hasCorrectedCoords()?1:0); // has_corrected
	stmt->bind(5, 0); // TODO: is_pmo
	stmt->bind(6, cache->getName()); // name
	stmt->bind(7, cache->getPlacedBy()); // placed by
	stmt->bind(8, cache->getOwner()); // owner
	stmt->bind(9, cache->getOwnerId()); // owner id
	stmt->bind(10, (int)cache->getCacheType()); // type
	stmt->bind(11, (int)cache->getSize()); // size
	stmt->bind(12, cache->getDifficulty()); // difficulty
	stmt->bind(13, cache->getTerrain()); // terrain
	stmt->bind(14, cache->getCountry()); // country
	stmt->bind(15, cache->getState()); // state
	stmt->bind(16, cache->getFavPoints()); // favorite points
	stmt->bind(17, (int64_t)cache->getDtCreated()); // dtcreated
	stmt->bind(18, (int64_t)time(NULL)); // dtupdate
	stmt->bind(19, (int64_t)cache->getDtFound()); // dtfound
	stmt->bind(20, cache->getComment()); // comment
	stmt->bind(21, cache->getLockFlags()); // locked
	stmt->bind(22, cache->isOwn());

	GCM::database::Database::Result result = GCM::database::Database::ST_FAILED;

	if (stmt->step() == SQLITE_DONE) {
		stmt->reset();

		// Insert whole long data...
		if (cache->isLongLoaded()) {
			stmt = this->stmtInsertLongData;

			stmt->bind(1, cache->getRowId());
			stmt->bind(2, cache->getShortDescription());
			stmt->bind(3, cache->getLongDescription());
			stmt->bind(4, cache->getSDIsHTML());
			stmt->bind(5, cache->getLDIsHTML());
			stmt->bind(6, cache->getHint());

			stmt->step();
			stmt->reset();
		}

		// Insert the cache coordinates.
		stmt = this->stmtInsertCoords;
		stmt->bind(3, cache->getName()); // Cache name
		stmt->bind(5, (int)cache->getCacheType()); // Subtype = cache type
		stmt->bind(6, cache->getRowId()); // Reference cache
		stmt->bindNull(7); // Reference waypoint is NULL for the cache.

		// Insert corrected
		if (cache->hasCorrectedCoords() && (cache->getLatitudeE6() != 0 || cache->getLongitudeE6() != 0)) {
			stmt->bind(1, cache->getLatitudeE6());
			stmt->bind(2, cache->getLongitudeE6());
			stmt->bind(4, WPT_GEOCACHE_CORRECTED);
			if (stmt->step()) {
				insertCoordIndex(this->db->lastInsertId(), cache->getLatitudeE6(), cache->getLongitudeE6());
			}
			stmt->reset();

			stmt->bind(4, WPT_GEOCACHE_INITIAL);
		} else {
			stmt->bind(4, WPT_GEOCACHE);
		}

		// Insert initial
		stmt->bind(1, cache->getInitialLatitudeE6());
		stmt->bind(2, cache->getInitialLongitudeE6());

		if (stmt->step() == SQLITE_DONE) {
			stmt->reset();
			this->insertCoordIndex(this->db->lastInsertId(), cache->getInitialLatitudeE6(), cache->getInitialLongitudeE6());
			result = GCM::database::Database::ST_CREATED;
		} else {
			stmt->reset();
			result = GCM::database::Database::ST_FAILED;
		}
	} else {
		stmt->reset();
	}

	if (result == GCM::database::Database::ST_FAILED) {
		this->db->exec("ROLLBACK TO \"insert_cache\"");
	} else {
		this->db->exec("RELEASE \"insert_cache\"");
	}

	return result;
}

void GCMDatabase::insertCoordIndex(int id, int latE6, int lonE6) {
	// Create the index...
	GC<Stmt> stmt = this->stmtInsertCoordsIndex;
	stmt->bind(1, id);
	stmt->bind(2, latE6 / 1E6);
	stmt->bind(3, lonE6 / 1E6);
	stmt->step();
	stmt->reset();
}

void GCMDatabase::updateLogs(GC<Geocache> cache) {
	GC<Stmt> stmt;

	LogList::iterator i = cache->getLogs()->begin();

	while (i != cache->getLogs()->end()) {
		GC<Log> log = *i;

		// Update / insert new logs...
		/*stmt = this->stmtQueryLog;
		stmt->bind(1, log->getId());

		if (stmt->step() == SQLITE_ROW) {
			stmt->reset();

			stmt = this->stmtUpdateLog;
			stmt->bind(1, log->getTime());
			stmt->bind(2, log->getType());
			stmt->bind(3, log->getFinder());
			stmt->bind(4, log->getText());
			stmt->bind(5, time(NULL));
			stmt->bind(6, log->getId());
			stmt->step();
			stmt->reset();

		} else {
			stmt->reset();

			stmt = this->stmtInsertLog;
			stmt->bind(1, log->getId());
			stmt->bind(2, cache->getId());
			stmt->bind(3, log->getTime());
			stmt->bind(4, log->getType());
			stmt->bind(5, log->getFinder());
			stmt->bind(6, log->getFinderId());
			stmt->bind(7, log->getText());
			stmt->bind(8, time(NULL));
			stmt->step();
			stmt->reset();
		}*/

		stmt = this->stmtInsertLog;
		stmt->bind(1, log->getId());
		stmt->bind(2, cache->getRowId());
		stmt->bind(3, (int64_t)log->getTime());
		stmt->bind(4, log->getType());
		stmt->bind(5, log->getFinder());
		stmt->bind(6, log->getFinderId());
		stmt->bind(7, log->getText());
		stmt->bind(8, log->isTextEncoded());
		stmt->bind(9, (int64_t)time(NULL));
		stmt->step();
		stmt->reset();

		++i;
	}

	// Delete all logs that are older than last five logs.
	stmt = this->stmtClearLogs;
	stmt->bind(1, cache->getId());
	stmt->bind(2, 5);
	stmt->bind(3, cache->getOwnerId());
	stmt->step();
	stmt->reset();
}

GCM::database::Database::Result GCMDatabase::saveCache(GC<Geocache> cache) {
	GCM::database::Database::Result result;

	// Query for single cache if not importing
	if (currentHint != AH_IMPORT) {
		this->stmtQuerySingleCacheForImport->bind(1, cache->getRowId());
	}

	if ((currentHint == AH_IMPORT && importCache.find(cache->getRowId()) != importCache.end()) || (currentHint != AH_IMPORT && this->stmtQuerySingleCacheForImport->step() == SQLITE_ROW)) {
		result = this->updateCache(cache);
	} else {
		result = this->insertCache(cache);

		if (result != Database::ST_FAILED && this->statsValid) {
			this->stats.numCaches++;
		}
	}

	if (currentHint != AH_IMPORT) {
		this->stmtQuerySingleCacheForImport->reset();
	}

	if (result != Database::ST_FAILED) {
		if (cache->isLogsLoaded()) {
			this->updateLogs(cache);
		}

		if (cache->isAttributesLoaded()) {
			this->updateAttributes(cache);
		}
		
    if (cache->isWaypointsLoaded()) {
	    GC<GeocacheWaypointList> list = cache->getWaypoints();
      GeocacheWaypointList::iterator i = list->begin();
    	while (i != list->end()) {
		    this->saveCacheWaypoint(*i);
	  	  i++;
	    }
		}

		// TODO: this->updateTags(cache);

		cache->setDbProvider(this);
	} else {
		GCM::util::Log::e("GCMDatabase", "Save cache failed.");
	}

	return result;
}

GCM::database::Database::Result GCMDatabase::saveCacheWaypoint(GC<GeocacheWaypoint> waypoint) {
	GC<Stmt> stmt;

	this->db->exec("SAVEPOINT \"waypoint\"");

	Database::Result result = Database::ST_FAILED;

	if (currentHint != AH_IMPORT) {
		stmt = this->stmtQueryWaypoint;
		stmt->bind(1, waypoint->getId());
	}

	std::map<std::string, WaypointMeta>::iterator i;

	if (
		(currentHint == AH_IMPORT && (i = importWaypoints.find(waypoint->getId()->c_str())) != importWaypoints.end())
		|| (currentHint != AH_IMPORT && stmt->step() == SQLITE_ROW))
	{
		int rowId, user_flag;

		if (currentHint != AH_IMPORT) {
			rowId = stmt->columnInt(0);
			user_flag = stmt->columnInt(1);

			stmt->reset();
		} else {
			rowId = i->second.id;
			user_flag = i->second.user_flag;
		}

		if (!user_flag || waypoint->getUserFlag()) {
			stmt = this->stmtUpdateWaypoint;
			stmt->bind(1, waypoint->getName());
			stmt->bind(2, waypoint->getWptType());
			stmt->bind(3, waypoint->getComment());
			stmt->bind(4, waypoint->getUserFlag());
			stmt->bind(5, waypoint->getUserComment());
			stmt->bind(6, (int64_t)time(NULL));
			stmt->bind(7, waypoint->useAsCorrected());
			stmt->bind(8, rowId);

			if (stmt->step() == SQLITE_DONE) {
				// Updated, update coords...
				stmt->reset();

				if (currentHint != AH_IMPORT) {
					stmt = this->stmtQueryWaypointCoords;
					stmt->bind(1, rowId);
				}

				if ((currentHint == AH_IMPORT && i->second.point_id > 0) || stmt->step() == SQLITE_ROW) {
					int pointRowId;

					if (currentHint != AH_IMPORT) {
						pointRowId = stmt->columnInt(0);
						stmt->reset();
					} else {
						pointRowId = i->second.point_id;
					}

					stmt = this->stmtUpdateWaypointCoords;
					stmt->bind(1, waypoint->getLatitudeE6());
					stmt->bind(2, waypoint->getLongitudeE6());
					stmt->bind(3, waypoint->getWptType());
					stmt->bind(4, rowId);

					if (stmt->step() == SQLITE_DONE) {
						stmt->reset();

						stmt = this->stmtUpdateCoordsIndex;
						stmt->bind(1, waypoint->getLatitude());
						stmt->bind(2, waypoint->getLongitude());
						stmt->bind(3, pointRowId);

						if (stmt->step()) {
							stmt->reset();
							result = Database::ST_UPDATED;
						} else {
							stmt->reset();
						}
					} else {
						stmt->reset();
					}
				} else {
					stmt = this->stmtInsertCoords;
					stmt->bind(1, waypoint->getLatitudeE6());
					stmt->bind(2, waypoint->getLongitudeE6());
					stmt->bind(3, waypoint->getName());
					stmt->bind(4, WPT_WAYPOINT);
					stmt->bind(5, waypoint->getType());
					stmt->bind(6, Geocache::gcIdToInt(waypoint->getId()));
					stmt->bind(7, this->db->lastInsertId());

					if (stmt->step() == SQLITE_DONE) {
						stmt->reset();

						this->insertCoordIndex(this->db->lastInsertId(), waypoint->getLatitudeE6(), waypoint->getLongitudeE6());
						result = Database::ST_CREATED;
					}
				}
			} else {
				stmt->reset();
			}
		} else {
			// Skip the waypoint, because user flag locks it.
		}
	} else {
		// Insert waypoint
		if (currentHint != AH_IMPORT) {
			stmt->reset();
		}

		GCM::util::Log::d("GCMDatabase", "DNF waypoint match.");

		stmt = this->stmtInsertWaypoint;

		stmt->bind(1, waypoint->getRefCacheId());
		stmt->bind(2, waypoint->getId()); // Code (prefix + cache_no)
		stmt->bind(3, waypoint->getName());
		stmt->bind(4, waypoint->getWptType());
		stmt->bind(5, waypoint->getComment());
		stmt->bind(6, waypoint->getUserFlag());
		stmt->bind(7, waypoint->getUserComment());
		stmt->bind(8, (int64_t)time(NULL));
		stmt->bind(9, waypoint->useAsCorrected());

		if (stmt->step() == SQLITE_DONE) {
			stmt->reset();

			stmt = this->stmtInsertCoords;
			stmt->bind(1, waypoint->getLatitudeE6());
			stmt->bind(2, waypoint->getLongitudeE6());
			stmt->bind(3, waypoint->getName());
			stmt->bind(4, WPT_WAYPOINT);
			stmt->bind(5, waypoint->getWptType());
			stmt->bind(6, waypoint->getRefCacheId());
			stmt->bind(7, this->db->lastInsertId());

			if (stmt->step() == SQLITE_DONE) {
				stmt->reset();

				this->insertCoordIndex(this->db->lastInsertId(), waypoint->getLatitudeE6(), waypoint->getLongitudeE6());
				result = Database::ST_CREATED;
			}
		} else {
			stmt->reset();
		}
	}

	if (result == Database::ST_FAILED) {
		this->db->exec("ROLLBACK TO \"waypoint\"");
	} else {
		waypoint->setDbProvider(this);
		this->db->exec("RELEASE \"waypoint\"");

		if (this->statsValid) {
			this->stats.numWaypoints++;
		}
	}

	return result;
}

// Generic waypoints are not supported by the database... at least for now.
GCM::database::Database::Result GCMDatabase::saveGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return Database::ST_FAILED;
	(void)waypoint;
}

void GCMDatabase::beginTransaction() {
	this->db->beginTransaction();
}

void GCMDatabase::commit() {
	this->db->commit();
}

void GCMDatabase::rollback() {
	this->db->rollback();
}

struct Coords {
	int latE6;
	int lonE6;

	Coords(): latE6(0), lonE6(0) {}
	Coords(const Coords &c): latE6(c.latE6), lonE6(c.lonE6) {}
	Coords(int lat, int lon): latE6(lat), lonE6(lon) {}
};

typedef std::map<int, Coords> CoordsMap;

GC<GeocacheList> GCMDatabase::listCaches(GC<Filter> f, ptrDialog progressDialog) {
	GC<Stmt> stmt = this->db->prepare("SELECT \"geocaches\".\"id\", \"gcid\", \"status\", \"name\", \"placed_by\", \"owner\", \"owner_id\", \"type\", \"size\", \"difficulty\", \"terrain\", \"country\", \"state\", \"favorite_points\", \"dtcreated\", \"dtupdate\", \"dtfound\", \"comment\", \"locked\", \"is_own\" FROM \"geocaches\"");

	GC<Stmt> stmtWpt = this->db->prepare("SELECT \"ref_cache\", \"id\", \"code\", \"name\", \"type\", \"description\", \"user_comment\", \"user_flag\", \"dtupdate\" FROM \"waypoints\"");

	GC<Stmt> stmtPoints = this->db->prepare("SELECT \"ref_cache\", \"ref_waypoint\", \"wpttype\", \"latE6\", \"lonE6\" FROM \"points\"");

	GC<GeocacheList> out = new GeocacheList();

	// Get active reference point to calc distances of points.
	GC<ReferencePoint> rp = AppConfig::getInstance()->getActiveRefPt();

	CoordsMap cacheInitialCoords;
	CoordsMap cacheCorrectedCoords;
	CoordsMap waypointCoords;

	int loaded = 0;

	if (progressDialog) {
		const Stats &st = this->getStats();
		progressDialog->setMaxProgress(st.numCaches + st.numWaypoints);
	}

	while (stmtPoints->step() == SQLITE_ROW) {
		if (stmtPoints->columnInt(1) == 0) {
			// Geocache coordinates
			switch (stmtPoints->columnInt(2)) {
				case WPT_GEOCACHE:
				case WPT_GEOCACHE_INITIAL:
					cacheInitialCoords[stmtPoints->columnInt(0)] = Coords(stmtPoints->columnInt(3), stmtPoints->columnInt(4));
					break;

				case WPT_GEOCACHE_CORRECTED:
					cacheCorrectedCoords[stmtPoints->columnInt(0)] = Coords(stmtPoints->columnInt(3), stmtPoints->columnInt(4));
					break;
			}
		} else {
			// Waypoint coordinates
			waypointCoords[stmtPoints->columnInt(1)] = Coords(stmtPoints->columnInt(3), stmtPoints->columnInt(4));
		}
	}

	CoordsMap::iterator i;

	while (stmt->step() == SQLITE_ROW) {
		GC<Geocache> cache = new Geocache();
		cache->setRowId(stmt->columnInt(0));
		cache->setId(stmt->columnString(1));

		if ((stmt->columnInt(2) & 1) == 1) cache->setCacheStatus(Geocache::ST_DISABLED);
		else if ((stmt->columnInt(2) & 2) == 2) cache->setCacheStatus(Geocache::ST_ARCHIVED);
		else cache->setCacheStatus(Geocache::ST_AVAILABLE);

		cache->setName(stmt->columnString(3));
		cache->setPlacedBy(stmt->columnString(4));
		cache->setOwner(stmt->columnString(5));
		cache->setOwnerId(stmt->columnInt(6));
		cache->setCacheType((Geocache::Type)stmt->columnInt(7));
		cache->setSize((Geocache::Size)stmt->columnInt(8));
		cache->setDifficulty(stmt->columnInt(9));
		cache->setTerrain(stmt->columnInt(10));
		cache->setCountry(stmt->columnString(11));
		cache->setState(stmt->columnString(12));
		cache->setFavPoints(stmt->columnInt(13));
		cache->setDtCreated(stmt->columnInt64(14));
		cache->setDtLastUpdate(stmt->columnInt64(15));
		cache->setDtFound(stmt->columnInt64(16));
		cache->setComment(stmt->columnString(17));
		cache->setLockFlags(stmt->columnInt(18));
		cache->setOwn(stmt->columnInt(19));
		cache->setDbProvider(this);

		// Try to load initial coordinates.
		i = cacheInitialCoords.find(cache->getRowId());
		if (i != cacheInitialCoords.end()) {
			cache->setInitialLatitudeE6(i->second.latE6);
			cache->setInitialLongitudeE6(i->second.lonE6);
		}

		// Try to load corrected coordinates.
		i = cacheCorrectedCoords.find(cache->getRowId());
		if (i != cacheCorrectedCoords.end()) {
			cache->setHasCorrectedCoords(true);
			cache->setLatitudeE6(i->second.latE6);
			cache->setLongitudeE6(i->second.lonE6);
		}

		cache->setMapLoaded(true);
		cache->setLoaded(true);

		if (rp) {
			cache->setDistanceFrom(rp->getLatitudeE6(), rp->getLongitudeE6());
		}

		if (!f || f->matchCache(cache)) {
			out->append(cache);
			loaded++;
		}

		if (progressDialog) {
			progressDialog->setProgress(progressDialog->getProgress() + 1);
		}
	}

	while (stmtWpt->step() == SQLITE_ROW) {
		int gcId = stmtWpt->columnInt(0);
		GC<Geocache> cache = out->getCache(gcId);
		if (cache) {
			GC<GeocacheWaypoint> wpt = new GeocacheWaypoint();
			wpt->setCache(cache);
			wpt->setRowId(stmtWpt->columnInt(1));
			wpt->setId(stmtWpt->columnString(2));
			wpt->setName(stmtWpt->columnString(3));
			wpt->setWptType((GeocacheWaypoint::Type)stmtWpt->columnInt(4));
			wpt->setComment(stmtWpt->columnString(5));
			wpt->setUserComment(stmtWpt->columnString(6));
			wpt->setUserFlag(stmtWpt->columnInt(7));
			wpt->setDtLastUpdate(stmtWpt->columnInt64(8));
			wpt->setDbProvider(this);

			i = waypointCoords.find(wpt->getRowId());
			if (i != waypointCoords.end()) {
				wpt->setLatitudeE6(i->second.latE6);
				wpt->setLongitudeE6(i->second.lonE6);
			}

			if (rp) {
				wpt->setDistanceFrom(rp->getLatitudeE6(), rp->getLongitudeE6());
			}

			cache->setWaypointsLoaded(true);

			if (!f || f->matchWaypoint(wpt)) {
				cache->getWaypoints()->append(wpt);
			}
		}

		if (progressDialog) {
			progressDialog->setProgress(progressDialog->getProgress() + 1);
		}
	}

	GCM::util::Log::d("GCMDatabase", "Loaded %d caches.", loaded);

	return out;
}

void GCMDatabase::remWpt(int id) {
	this->db->exec("SAVEPOINT \"rem_waypoint\"");

	GC<Stmt> stmt = this->db->prepare("DELETE FROM \"waypoints\" WHERE \"id\" = ?");
	stmt->bind(1, id);

	if (stmt->step() == SQLITE_DONE) {
		stmt->reset();

		stmt = this->db->prepare("SELECT \"id\" FROM \"points\" WHERE \"ref_waypoint\" = ?");
		stmt->bind(1, id);

		GC<Stmt> stmtDelIndex = this->db->prepare("DELETE FROM \"points_idx\" WHERE \"id\" = ?");

		while (stmt->step() == SQLITE_ROW) {
			stmtDelIndex->bind(1, stmt->columnInt(0));
			stmtDelIndex->step();
			stmtDelIndex->reset();
		}
		stmt->reset();

		stmt = this->db->prepare("DELETE FROM \"points\" WHERE \"ref_waypoint\" = ?");
		stmt->bind(1, id);
		stmt->step();
		stmt->reset();
	} else {
		stmt->reset();
	}

	if (this->statsValid) {
		this->stats.numWaypoints--;
	}

	this->db->exec("RELEASE \"rem_waypoint\"");
}

void GCMDatabase::removeCache(GC<GCM::geolib::Geocache> cache) {
	this->db->exec("SAVEPOINT \"rem_cache\"");

	// Remove all waypoints
	GC<Stmt> stmt = this->db->prepare("SELECT \"id\" FROM \"waypoints\" WHERE \"ref_cache\" = ?");
	stmt->bind(1, cache->getRowId());
	while (stmt->step() == SQLITE_ROW) {
		this->remWpt(stmt->columnInt(0));
	}
	stmt->reset();

	// Remove the cache itself.
	stmt = this->db->prepare("DELETE FROM \"geocaches\" WHERE \"id\" = ?");
	stmt->bind(1, cache->getRowId());
	stmt->step();
	stmt->reset();

	// Remove cache coordinates
	stmt = this->db->prepare("SELECT \"id\" FROM \"points\" WHERE \"ref_cache\" = ?");
	stmt->bind(1, cache->getRowId());

	GC<Stmt> delPoint = this->db->prepare("DELETE FROM \"points_idx\" WHERE \"id\" = ?");

	while (stmt->step() == SQLITE_ROW) {
		delPoint->bind(1, stmt->columnInt(0));
		delPoint->step();
		delPoint->reset();
	}

	stmt->reset();

	stmt = this->db->prepare("DELETE FROM \"points\" WHERE \"ref_cache\" = ?");
	stmt->bind(1, cache->getRowId());
	stmt->step();
	stmt->reset();

	if (this->statsValid) {
		this->stats.numWaypoints--;
	}

	this->db->exec("RELEASE \"rem_cache\"");
}

void GCMDatabase::removeCacheWaypoint(GC<GCM::geolib::GeocacheWaypoint> waypoint) {
	this->remWpt(waypoint->getRowId());

	// Remove cache waypoint from the cache itself.
	GC<Geocache> cache = waypoint->getCache();
	GC<GeocacheWaypointList> list = cache->getWaypoints();
	GeocacheWaypointList::iterator i = list->begin();
	while (i != list->end()) {
		if ((*i)->getRowId() == waypoint->getRowId() && (*i)->getDbProvider() == waypoint->getDbProvider()) {
			list->erase(i);
			break;
		}

		i++;
	}

	// TODO: Remove from database.
}

GC<GeocacheWaypointList> GCMDatabase::getCacheWaypoints(GC<GCM::geolib::Geocache> cache) {
	// SELECT \"id\", \"code\", \"name\", \"type\", \"description\", \"user_comment\", \"user_flag\", \"corrected\", \"dtupdate\" FROM \"waypoints\" WHERE \"ref_cache\" = ?
	GC<Stmt> stmt = this->stmtQueryCacheWaypoints;
	stmt->bind(1, cache->getRowId());

	GC<GeocacheWaypointList> wptList = new GeocacheWaypointList();

	while (stmt->step() == SQLITE_ROW) {
		GC<GeocacheWaypoint> waypoint = new GeocacheWaypoint();
		waypoint->setCache(cache);
		waypoint->setRowId(stmt->columnInt(0));
		waypoint->setId(stmt->columnString(1));
		waypoint->setName(stmt->columnString(2));
		waypoint->setWptType((GeocacheWaypoint::Type)stmt->columnInt(3));
		waypoint->setComment(stmt->columnString(4));
		waypoint->setUserComment(stmt->columnString(5));
		waypoint->setUserFlag(stmt->columnInt(6));
		waypoint->setUseAsCorrected(stmt->columnInt(7));
		waypoint->setDtLastUpdate(stmt->columnInt64(8));
		waypoint->setLatitudeE6(stmt->columnInt(9));
		waypoint->setLongitudeE6(stmt->columnInt(10));
		waypoint->setDbProvider(this);
		wptList->push_back(waypoint);
	}

	stmt->reset();

	return wptList;
}

void GCMDatabase::loadLongData(GC<GCM::geolib::Geocache> cache) {
	// SELECT \"short_desc\", \"long_desc\", \"short_desc_html\", \"long_desc_html\", \"hint\" FROM \"geocache_long_data\" WHERE \"id\" = ?
	GC<Stmt> stmt = this->stmtQueryLongData;
	stmt->bind(1, cache->getRowId());

	if (stmt->step() == SQLITE_ROW) {
		cache->setShortDescription(stmt->columnString(0));
		cache->setLongDescription(stmt->columnString(1));
		cache->setSDIsHTML(stmt->columnInt(2));
		cache->setLDIsHTML(stmt->columnInt(3));
		cache->setHint(stmt->columnString(4));
		cache->setLongLoaded(true);
	}
	stmt->reset();
}

void GCMDatabase::loadAttributes(GC<GCM::geolib::Geocache> cache) {
	GC<AttributeSet> attributes = cache->getAttributes();
	attributes->clear();

	GC<Stmt> stmt = this->stmtQueryAttributes;
	stmt->bind(1, cache->getRowId());

	Attribute *attribute;
	while (stmt->step() == SQLITE_ROW) {
		attribute = new Attribute();
		attribute->setId(stmt->columnInt(0));
		attribute->setPositive(stmt->columnInt(1));
		attributes->append(attribute);
	}

	cache->setAttributesLoaded(true);

	stmt->reset();
}

void GCMDatabase::updateAttributes(GCM::GC<GCM::geolib::Geocache> cache) {
	this->db->exec("SAVEPOINT \"attributes\"");

	// SELECT \"attribute_id\", \"positive\", \"id\" FROM \"attributes\" WHERE \"ref_cache\" = ?
	GC<Stmt> stmtQuery = this->stmtQueryAttributes;
	GC<Stmt> stmt;
	stmtQuery->bind(1, cache->getRowId());

	GC<AttributeSet> attributes = cache->getAttributes();
	std::vector<int> dontInsert(15);
	AttributeSet::iterator i;

	while (stmtQuery->step() == SQLITE_ROW) {
		// Attribute was not found in current cache instance OR it has other positivness, remove it.
		if ((i = attributes->find(stmtQuery->columnInt(0))) == attributes->end() || i->second->isPositive() != stmtQuery->columnInt(1)) {
			stmt = this->stmtDeleteAttribute;
			stmt->bind(1, stmtQuery->columnInt(2));
			stmt->step();
			stmt->reset();

		// Attribute was found, keep it.
		} else {
			dontInsert.push_back(stmtQuery->columnInt(0));
		}
	}

	stmtQuery->reset();

	i = attributes->begin();
	std::vector<int>::iterator di;
	while (i != attributes->end()) {
		di = dontInsert.begin();
		while (di != dontInsert.end()) {
			if (*di == i->second->getId()) {
				// Attribute was found in don't insert, skip processing.
				break;
			}

			di++;
		}

		if (di == dontInsert.end()) {
			// Not found in don't insert.
			// INSERT INTO \"attributes\" (\"ref_cache\", \"attribute_id\", \"positive\") VALUES (?, ?, ?)
			GC<Stmt> stmt = this->stmtInsertAttribute;
			stmt->bind(1, cache->getRowId());
			stmt->bind(2, i->second->getId());
			stmt->bind(3, i->second->isPositive());
			stmt->step();
			stmt->reset();
		}

		i++;
	}

	this->db->exec("RELEASE \"attributes\"");
}

void GCMDatabase::loadLogs(GC<GCM::geolib::Geocache> cache, int count) {
	// SELECT \"id\", \"date\", \"type\", \"user\", \"user_id\", \"text\", \"isencoded\", \"dtupdate\" FROM \"logs\" WHERE \"ref_cache\" = ? ORDER BY \"date\" DESC, \"id\" DESC LIMIT ?
	GC<Stmt> stmt = this->stmtQueryLogs;
	stmt->bind(1, cache->getRowId());

	if (count < 0) count = -1;
	stmt->bind(2, count);

	GC<LogList> logs = cache->getLogs();
	logs->clear();

	while (stmt->step() == SQLITE_ROW) {
		GC<Log> log = new Log();
		log->setId(stmt->columnInt(0));
		log->setTime(stmt->columnInt64(1));
		log->setType((Log::Type)stmt->columnInt(2));
		log->setFinder(stmt->columnString(3));
		log->setFinderId(stmt->columnInt(4));
		log->setText(stmt->columnString(5));
		log->setTextEncoded(stmt->columnInt(6));

		logs->push_back(log);
	}

	cache->setLogsLoaded(true);

	stmt->reset();
}

const GCM::database::Database::Stats &GCMDatabase::getStats() {
	if (this->statsValid) return this->stats;

	GC<Stmt> stmt = this->stmtStatsCaches;
	if (stmt->step() == SQLITE_ROW) {
		this->stats.numCaches = stmt->columnInt(0);
	}

	stmt = this->stmtStatsWaypoints;
	if (stmt->step() == SQLITE_ROW) {
		this->stats.numWaypoints = stmt->columnInt(0);
	}

	this->statsValid = true;
	return this->stats;
}

GC<GCM::geolib::Geocache> GCMDatabase::buildCacheFromStmt(GC<Stmt> stmt) {
	GC<Geocache> cache = new Geocache();
	cache->setRowId(stmt->columnInt(0));
	cache->setId(stmt->columnString(1));

	if ((stmt->columnInt(2) & 1) == 1) cache->setCacheStatus(Geocache::ST_DISABLED);
	else if ((stmt->columnInt(2) & 2) == 2) cache->setCacheStatus(Geocache::ST_ARCHIVED);
	else cache->setCacheStatus(Geocache::ST_AVAILABLE);

	cache->setName(stmt->columnString(3));
	cache->setPlacedBy(stmt->columnString(4));
	cache->setOwner(stmt->columnString(5));
	cache->setOwnerId(stmt->columnInt(6));
	cache->setCacheType((Geocache::Type)stmt->columnInt(7));
	cache->setSize((Geocache::Size)stmt->columnInt(8));
	cache->setDifficulty(stmt->columnInt(9));
	cache->setTerrain(stmt->columnInt(10));
	cache->setCountry(stmt->columnString(11));
	cache->setState(stmt->columnString(12));
	cache->setFavPoints(stmt->columnInt(13));
	cache->setDtCreated(stmt->columnInt64(14));
	cache->setDtLastUpdate(stmt->columnInt64(15));
	cache->setDtFound(stmt->columnInt64(16));
	cache->setComment(stmt->columnString(17));
	cache->setLockFlags(stmt->columnInt(18));
	cache->setOwn(stmt->columnInt(19));
	cache->setDbProvider(this);

	stmt = this->db->prepare("SELECT \"latE6\", \"lonE6\", \"wpttype\" FROM \"points\" WHERE \"ref_cache\" = ? AND \"ref_waypoint\" IS NULL");
	stmt->bind(1, cache->getRowId());

	while (stmt->step() == SQLITE_ROW) {
		switch (stmt->columnInt(2)) {
			case WPT_GEOCACHE_INITIAL:
				cache->setHasCorrectedCoords(true);

			case WPT_GEOCACHE:
				cache->setInitialLatitudeE6(stmt->columnInt(0));
				cache->setInitialLongitudeE6(stmt->columnInt(1));
				break;

			case WPT_GEOCACHE_CORRECTED:
				cache->setLatitudeE6(stmt->columnInt(0));
				cache->setLongitudeE6(stmt->columnInt(1));
				break;
		}
	}

	GC<ReferencePoint> rp = AppConfig::getInstance()->getActiveRefPt();
	if (rp) {
		cache->setDistanceFrom(rp->getLatitudeE6(), rp->getLongitudeE6());
	}

	cache->setLoaded(true);
	cache->setMapLoaded(true);

	return cache;
}

GCM::GC<GCM::geolib::Geocache> GCMDatabase::getByGCID(GCM::String gcId) {
	int rowId = Geocache::gcIdToInt(gcId);

	GC<Stmt> stmt = this->db->prepare("SELECT \"id\", \"gcid\", \"status\", "
		"\"name\", \"placed_by\", \"owner\", \"owner_id\", \"type\", \"size\", "
		"\"difficulty\", \"terrain\", \"country\", \"state\", \"favorite_points\", "
		"\"dtcreated\", \"dtupdate\", \"dtfound\", \"comment\", \"locked\", \"is_own\" FROM \"geocaches\" WHERE \"id\" = ?");
	stmt->bind(1, rowId);

	if (stmt->step() == SQLITE_ROW) {
		return buildCacheFromStmt(stmt);
	} else {
		return NULL;
	}
}

GCM::GC<GCM::geolib::GeocacheList> GCMDatabase::getByName(GCM::String name) {
	std::list<String> nameParts;

	const char *nm = name->c_str();
	size_t lastSpace = 0;
	for (size_t i = 0; i < name->length(); i++) {
		if (isspace(nm[i])) {
			if (lastSpace != i) {
				// Fill in part.
				nameParts.push_back(String(nm + lastSpace, i - lastSpace));
			}

			lastSpace = i + 1;
		}
	}

	// Last part
	if (lastSpace != name->length()) {
		nameParts.push_back(String(nm + lastSpace));
	}

	String sql = "SELECT \"id\", \"gcid\", \"status\", "
		"\"name\", \"placed_by\", \"owner\", \"owner_id\", \"type\", \"size\", "
		"\"difficulty\", \"terrain\", \"country\", \"state\", \"favorite_points\", "
		"\"dtcreated\", \"dtupdate\", \"dtfound\", \"comment\", \"locked\", \"is_own\" FROM \"geocaches\" WHERE ";

	std::list<String>::iterator i = nameParts.begin();
	while (i != nameParts.end()) {
		if (i != nameParts.begin()) {
			sql->append(" AND ");
		}
		sql->append("\"name\" LIKE ? COLLATE NOCASE");
		i++;
	}

	GC<Stmt> stmt = this->db->prepare(sql->c_str());

	i = nameParts.begin();
	int index = 1;
	while (i != nameParts.end()) {
		stmt->bind(index, String::format("%%%s%%", (*i)->c_str()));
		index++;
		i++;
	}

	GC<GeocacheList> out = new GeocacheList();
	while (stmt->step() == SQLITE_ROW) {
		out->append(buildCacheFromStmt(stmt));
	}
	return out;
}

void GCMDatabase::loadWaypoints(GCM::GC<GCM::geolib::Geocache> cache) {
	GCM::GC<GCM::geolib::GeocacheWaypointList> list = this->getCacheWaypoints(cache);
	GCM::GC<GCM::geolib::GeocacheWaypointList> wpts = cache->getWaypoints();
	wpts->clear();
	wpts->insert(wpts->begin(), list->begin(), list->end());

	cache->setWaypointsLoaded(true);
}
