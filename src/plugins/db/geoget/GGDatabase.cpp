#include "GGDatabase.h"

#include <geolib/Geocache.h>
#include <geolib/GeocacheList.h>
#include <geolib/Log.h>
#include <geolib/GeocacheWaypoint.h>
#include <geolib/Filter.h>
#include <geolib/ReferencePoint.h>

#include <config/AppConfig.h>

#include <string.h>
#include <sstream>
#include <zlib.h>

#include "MurmurHash2.h"

using namespace GCM::database;
using namespace GCM::geolib;
using namespace GCM::sqlite;
using namespace GCM::config;
using namespace GCM;

void GGDatabase::createDb() {
	this->createGeocacheTable();
	this->createGeolistTable();
	this->createGeologTable();
	this->createTagsTables();
	this->createMetaTable();
	this->createWaypointTable();
}


void GGDatabase::createGeocacheTable() {
	this->db->exec("CREATE TABLE IF NOT EXISTS geocache ("
		"key INTEGER PRIMARY KEY, "
		"id TEXT, "
		"guid TEXT, "
		"x TEXT, "
		"y TEXT, "
		"name TEXT, "
		"author TEXT, "
		"cachetype TEXT, "
		"cachesize TEXT, "
		"difficulty TEXT, "
		"terrain TEXT, "
		"inventory INTEGER DEFAULT \"0\", "
		"cachestatus INTEGER DEFAULT \"0\", "
		"userstatus INTEGER DEFAULT \"0\", "
		"dthidden INTEGER DEFAULT \"0\", "
		"dtlast INTEGER DEFAULT \"0\", "
		"dtupdate INTEGER DEFAULT \"0\", "
		"comment TEXT, "
		"dtfoundtime INTEGER DEFAULT \"0\", "
		"dtfound INTEGER DEFAULT \"0\", "
		"country TEXT, "
		"state TEXT, "
		"gs_cacheid TEXT, "
		"gs_ownerid TEXT, "
		"dtupdate2 REAL DEFAULT 0);"
	);

	this->db->exec("CREATE INDEX IF NOT EXISTS  csizeidx ON geocache (cachesize);");
	this->db->exec("CREATE INDEX IF NOT EXISTS  ctypeidx ON geocache (cachetype);");
	this->db->exec("CREATE INDEX IF NOT EXISTS  dthiidx ON geocache (dthidden);");
	this->db->exec("CREATE INDEX IF NOT EXISTS  dtupidx ON geocache (dtupdate);");
	this->db->exec("CREATE INDEX IF NOT EXISTS  ididx ON geocache (id);");

	GC<Stmt> tblExists = this->db->prepare("SELECT DISTINCT tbl_name FROM sqlite_master WHERE tbl_name = ?");
	tblExists->bind(1, "coord_index");

	if (tblExists->step() != SQLITE_ROW) {
		this->db->exec("CREATE VIRTUAL TABLE coord_index USING rtree(key, minX, maxX, minY, maxY);");
	}

	tblExists->finalize();
}

void GGDatabase::createGeolistTable() {
	this->db->exec("CREATE TABLE IF NOT EXISTS  geolist ("
		"key INTEGER PRIMARY KEY, "
		"id TEXT, "
		"shortdesc TEXT, "
		"shortdescflag INTEGER DEFAULT \"0\", "
		"longdesc TEXT, "
		"longdescflag INTEGER DEFAULT \"0\", "
		"hint TEXT, "
		"hash INTEGER DEFAULT 0, "
		"dtupdate2 REAL DEFAULT 0);"
	);

	this->db->exec("CREATE INDEX IF NOT EXISTS  lididx ON geolist (id);");
}

void GGDatabase::createGeologTable() {
	this->db->exec("CREATE TABLE IF NOT EXISTS  geolog ("
		"key INTEGER PRIMARY KEY, "
		"id TEXT, "
		"dt TEXT, "
		"type TEXT, "
		"finder TEXT, "
		"logtext TEXT, "
		"gs_logid TEXT, "
		"gs_finderid TEXT, "
		"hash INTEGER DEFAULT 0, "
		"dtupdate2 REAL DEFAULT 0);");

	this->db->exec("CREATE INDEX IF NOT EXISTS  logididx ON geolog (gs_logid);");
	this->db->exec("CREATE INDEX IF NOT EXISTS  logidx ON geolog (id);");
}

void GGDatabase::createTagsTables() {
	this->db->exec("CREATE TABLE IF NOT EXISTS  geotag ("
		"key INTEGER PRIMARY KEY, "
		"id TEXT, "
		"flag INTEGER DEFAULT \"0\", "
		"ptrkat INTEGER DEFAULT \"0\", "
		"ptrvalue INTEGER DEFAULT \"0\");");

	this->db->exec("CREATE INDEX IF NOT EXISTS  gtididx ON geotag (id);");
	this->db->exec("CREATE INDEX IF NOT EXISTS  gtptrkatidx ON geotag (ptrkat);");
	this->db->exec("CREATE INDEX IF NOT EXISTS  gtptrvalueidx ON geotag (ptrvalue);");

	this->db->exec("CREATE TABLE IF NOT EXISTS  geotagcategory ("
		"key INTEGER PRIMARY KEY, "
		"value TEXT, "
		"flag INTEGER DEFAULT \"0\");");

	this->db->exec("CREATE INDEX IF NOT EXISTS  gtcvalueidx ON geotagcategory (value);");

	this->db->exec("CREATE TABLE IF NOT EXISTS  geotagvalue ("
		"key INTEGER PRIMARY KEY, "
		"value TEXT, "
		"flag INTEGER DEFAULT \"0\");");

	this->db->exec("CREATE INDEX IF NOT EXISTS  gtvvalueidx ON geotagvalue (value);");
}

void GGDatabase::createMetaTable() {
	this->db->exec("CREATE TABLE IF NOT EXISTS  metadata ("
		"key INTEGER PRIMARY KEY, "
		"name TEXT, "
		"value TEXT, "
		"UNIQUE(name)"
	");");

	// Insert metadata...
	GC<Stmt> stmt = this->db->prepare("INSERT OR IGNORE INTO metadata (key, name, value) VALUES (?, ?, ?)");
	stmt->bind(1, 1);
	stmt->bind(2, "create_application");
	stmt->bind(3, "GeoGet Database for GCManager");
	stmt->step();
	stmt->reset();

	stmt->bind(1, 4);
	stmt->bind(2, "create_datetime");
	stmt->bind(3, GGDatabase::timeToGGDate3(time(NULL)));
	stmt->step();
	stmt->reset();

	stmt->bind(1, 3);
	stmt->bind(2, "create_dt");
	stmt->bind(3, GGDatabase::timeToGGDate2(time(NULL)));
	stmt->step();
	stmt->reset();

	stmt->bind(1, 2);
	stmt->bind(2, "create_level");
	stmt->bind(3, "2");
	stmt->step();
	stmt->reset();

	stmt->bind(1, 5);
	stmt->bind(2, "upgrade_application");
	stmt->bind(3, "GeoGet Database for GCManager");
	stmt->step();
	stmt->reset();

	stmt->bind(1, 7);
	stmt->bind(2, "upgrade_datetime");
	stmt->bind(3, this->timeToGGDate3(time(NULL)));
	stmt->step();
	stmt->reset();

	stmt->bind(1, 6);
	stmt->bind(2, "upgrade_dt");
	stmt->bind(3, this->timeToGGDate2(time(NULL)));
	stmt->step();
	stmt->reset();
}

void GGDatabase::createWaypointTable() {
	this->db->exec("CREATE TABLE IF NOT EXISTS  waypoint ("
		"key INTEGER PRIMARY KEY, "
		"id TEXT, "
		"guid TEXT, "
		"x TEXT, "
		"y TEXT, "
		"name TEXT, "
		"prefixid TEXT, "
		"lookup TEXT, "
		"wpttype TEXT, "
		"cmt TEXT, "
		"comment TEXT, "
		"flag INTEGER DEFAULT \"0\", "
		"dtupdate INTEGER DEFAULT \"0\", "
		"dtupdate2 REAL DEFAULT 0);");

	this->db->exec("CREATE INDEX IF NOT EXISTS wptdtupidx ON waypoint (dtupdate);");
	this->db->exec("CREATE INDEX IF NOT EXISTS wptididx ON waypoint (id);");

	GC<Stmt> tblExists = this->db->prepare("SELECT DISTINCT tbl_name FROM sqlite_master WHERE tbl_name = ?");
	tblExists->bind(1, "wpt_coord_index");

	if (tblExists->step() != SQLITE_ROW) {
		this->db->exec("CREATE VIRTUAL TABLE wpt_coord_index USING rtree(key, minX, maxX, minY, maxY);");
	}

	tblExists->finalize();
}

void GGDatabase::prepareStatements() {
	// TODO: Convert statements from methods to global prepared values
	stmtInsertLong = this->db->prepare("INSERT INTO \"geolist\" ("
			"\"id\", \"shortdesc\", \"shortdescflag\", \"longdesc\", "
			"\"longdescflag\", \"hint\", \"hash\", \"dtupdate2\") VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
}

String GGDatabase::timeToGGDate(time_t time) {
	struct tm timeinfo;
	localtime_r(&time, &timeinfo);

	return String::format("%04d%02d%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
}

String GGDatabase::timeToGGTime(time_t time) {
	struct tm timeinfo;
	localtime_r(&time, &timeinfo);

	return String::format("%d", timeinfo.tm_hour * 100 + timeinfo.tm_min);
}

double GGDatabase::timeToGGDate2(time_t time) {
	return 25569.0 + time / 86400.0;
}

String GGDatabase::timeToGGDate3(time_t time) {
	struct tm timeinfo;
	localtime_r(&time, &timeinfo);

	return String::format("%04d-%02d-%02d %02d:%02d:%02d.000", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

/**
 * Convert GG date of form YYYYMMDD to time_t.
 */
time_t GGDatabase::ggDateToTime(String ggDate) {
	long int dt = strtol(ggDate->c_str(), NULL, 10);
	int year = dt / 10000;
	dt -= year * 10000;
	int mon = dt / 100;
	dt -= mon * 100;
	int day = dt;

	struct tm t;
	memset(&t, 0, sizeof(struct tm));
	t.tm_year = year - 1900;
	t.tm_mon = mon - 1;
	t.tm_mday = day;

	return mktime(&t);
}

time_t GGDatabase::ggDateToTime(String ggDate, String ggTime) {
	int time = atoi(ggTime->c_str());
	int hour = time / 100;
	time -= hour * 100;
	int min = time;

	return ggDateToTime(ggDate) + hour * 3600 + min * 60;
}

time_t GGDatabase::ggDate2ToTime(double ggDate) {
	return (ggDate - 25569) * 86400;
}

GGDatabase::GGDatabase(const char *path): db(new GCM::sqlite::Database()), statsValid(false) {
	if (this->db->open(path, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE) != SQLITE_OK) {
		throw GCM::database::DatabaseLoadFailed();
	}

	this->file = path;

	this->createDb();
	this->prepareStatements();

	this->getStats();
}

String GGDatabase::getConnectionString() {
	return this->file;
}

String GGDatabase::getId() {
	return "ggdb";
}

void GGDatabase::actionHint(ActionHint hint) {
	(void)hint;
}

struct LogMeta {
	int key;
	uint32_t hash;
	int dt;

	LogMeta(int key, int hash, int dt)
		: key(key), hash(hash), dt(dt)
	{}
};

void GGDatabase::updateLogs(GCM::GC<GCM::geolib::LogList> list) {
	GC<Stmt> stmt = this->db->prepare("SELECT \"gs_logid\", \"hash\", \"dt\" FROM \"geolog\" WHERE \"id\" = ? ORDER BY \"dt\" DESC, \"key\" DESC");
	LogList::iterator i = list->begin();

	GC<Stmt> stmtCreateLog = this->db->prepare("INSERT INTO \"geolog\" (\"id\", \"dt\", \"type\", "
		"\"finder\", \"logtext\", \"gs_logid\", \"gs_finderid\", \"hash\", \"dtupdate2\" "
		"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
	GC<Stmt> stmtUpdateLog = this->db->prepare("UPDATE \"geolog\" SET \"dt\" = ?, \"type\" = ?, "
		"\"finder\" = ?, \"dtupdate2\" = ?, \"logtext\" = ?, \"hash\" = ? WHERE \"gs_logid\" = ?");
	GC<Stmt> stmtUpdateLogNoText = this->db->prepare("UPDATE \"geolog\" SET \"dt\" = ?, \"type\" = ?, "
		"\"finder\" = ?, \"dtupdate2\" = ? WHERE \"gs_logid\" = ?");

	std::list<LogMeta> logs;

	while (i != list->end()) {
		GC<Log> log = *i;

		// First entry, load all existing logs for the cache
		if (i == list->begin()) {
			stmt->bind(1, log->getCache()->getId());
			while (stmt->step() == SQLITE_ROW) {
				logs.push_back(LogMeta(
					stmt->columnInt(0),
					stmt->columnInt64(1),
					stmt->columnInt(2)
				));uint32_t computeHashLong(GCM::GC<GCM::geolib::Geocache> cache);
			}
		}

		// Compute hash of the log text
		// ( const void * key, int len, uint32_t seed, void * out );
		uint32_t hash = this->computeHashLog(log);

		std::list<LogMeta>::iterator mi = logs.begin();
		bool found = false;
		while (mi != logs.end()) {
			if (log->getId() == mi->key) {
				// Found log, update...

				if (hash != mi->hash) {
					// Need to update the text
					stmtUpdateLog->bind(1, GGDatabase::timeToGGDate(log->getText()));
					stmtUpdateLog->bind(2, Log::typeToGpxString(log->getType()));
					stmtUpdateLog->bind(3, log->getFinder());
					stmtUpdateLog->bind(4, GGDatabase::timeToGGDate2(time(NULL)));
					stmtUpdateLog->bind(5, GZCompress(log->getText()));
					stmtUpdateLog->bind(6, (int64_t)hash);
					stmtUpdateLog->step();
					stmtUpdateLog->reset();
				} else {
					// Does not need to update the text
					stmtUpdateLogNoText->bind(1, GGDatabase::timeToGGDate(log->getText()));
					stmtUpdateLogNoText->bind(2, Log::typeToGpxString(log->getType()));
					stmtUpdateLogNoText->bind(3, log->getFinder());
					stmtUpdateLogNoText->bind(4, GGDatabase::timeToGGDate2(time(NULL)));
					stmtUpdateLogNoText->step();
					stmtUpdateLogNoText->reset();
				}

				found = true;
				break;
			}
			++mi;
		}

		if (!found) {
			logs.push_front(LogMeta(
				log->getId(),
				hash,
				GGDatabase::timeToGGDate(log->getTime())
			));

			stmtCreateLog->bind(1, log->getCache()->getId()); // id
			stmtCreateLog->bind(2, GGDatabase::timeToGGDate(log->getTime())); // dt
			stmtCreateLog->bind(3, Log::typeToGpxString(log->getType())); // type
			stmtCreateLog->bind(4, log->getFinder()); // finder
			stmtCreateLog->bind(5, GZCompress(log->getText())); // logtext
			stmtCreateLog->bind(6, log->getId()); // gs_logid
			stmtCreateLog->bind(7, log->getFinderId()); // gs_finderid
			stmtCreateLog->bind(8, (int64_t)hash); // hash
			stmtCreateLog->bind(9, GGDatabase::timeToGGDate2(time(NULL))); // dtupdate2

			stmtCreateLog->step();
			stmtCreateLog->reset();
		}

		++i;
	}
}

// Magic HASH_SEED to "fine tune" the hash for GeoGet DB. This must be the same
// as GG has.
const int GGDatabase::HASH_SEED = 3314489979;

uint32_t GGDatabase::computeHashLong(GCM::GC<GCM::geolib::Geocache> cache) {
	String toHash = "";
	toHash->reserve(
		cache->getShortDescription()->length() +
		cache->getLongDescription()->length() +
		cache->getHint()->length()
	);
	toHash->append(*(cache->getShortDescription().get().getObj()));
	toHash->append(*(cache->getLongDescription().get().getObj()));
	toHash->append(*(cache->getHint().get().getObj()));

	return MurmurHash2(toHash->c_str(), toHash->length(), HASH_SEED);
}

uint32_t GGDatabase::computeHashLog(GCM::GC<GCM::geolib::Log> log) {
	std::stringstream query;
	query.str("");
	query
		<< *(GGDatabase::timeToGGDate(log->getTime()).getObj()) // date
		<< *(GCM::geolib::Log::typeToGpxString(log.getType()).getObj()) // type
		<< log->getText(); // log text

	std::string s = query.str();

	return MurmurHash2(s.c_str(), s.length(), HASH_SEED);
}

GCM::database::Database::Result GGDatabase::createCache(GCM::GC<GCM::geolib::Geocache> cache) {
	GC<Stmt> stmt = this->db->prepare("INSERT INTO \"geocache\" (\"id\", \"x\", \"y\", \"name\", "
		"\"author\", \"cachetype\", \"cachesize\", \"difficulty\", \"terrain\", \"inventory\", "
		"\"cachestatus\", \"userstatus\", \"dthidden\", \"dtlast\", \"dtupdate\", \"comment\", "
		"\"dtfoundtime\", \"dtfound\", \"country\", \"state\", \"gs_cacheid\", \"gs_ownerid\", "
		"\"dtupdate2\") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
		"?, ?, ?, ?, ?, ?, ?)");

	this->db->beginTransaction();

	stmt->bind(1, cache->getId());
	stmt->bind(2, cache->getLatitude());
	stmt->bind(3, cache->getLongitude());
	stmt->bind(4, cache->getName());
	stmt->bind(5, cache->getPlacedBy());
	stmt->bind(6, Geocache::typeToGpxString(cache->getCacheType()));
	stmt->bind(7, Geocache::sizeToGpxString(cache->getSize()));
	stmt->bind(8, cache->getDifficulty() / 10);
	stmt->bind(9, cache->getTerrain() / 10);
	stmt->bind(10, 0); // inventory is unimplemented in the core for now...
	stmt->bind(11, (cache->isArchived())?2:(cache->isDisabled())?1:0);
	stmt->bind(12, 0); // user status is what?!
	stmt->bind(13, GGDatabase::timeToGGDate(cache->getDtCreated()));
	stmt->bind(14, 0); // Last found
	stmt->bind(15, GGDatabase::timeToGGDate(cache->getDtLastUpdate()));
	stmt->bind(16, cache->getComment());

	if (cache->isFound()) {
		stmt->bind(17, GGDatabase::timeToGGTime(cache->getDtFound()));
		stmt->bind(18, GGDatabase::timeToGGDate(cache->getDtFound()));
	} else {
		stmt->bind(17, 0);
		stmt->bind(18, 0);
	}

	stmt->bind(19, cache->getCountry());
	stmt->bind(20, cache->getState());
	stmt->bind(21, Geocache::gcIdToInt(cache->getId()));
	stmt->bind(22, cache->getOwnerId());
	stmt->bind(23, GGDatabase::timeToGGDate2(cache->getDtLastUpdate()));

	if (stmt->step() != SQLITE_DONE) {
		this->db->rollback();
		return GCM::database::Database::ST_FAILED;
	} else {
		cache->setRowId(this->db->lastInsertId());
	}

	// Store waypoints
	if (cache->isWaypointsLoaded()) {
		GeocacheWaypointList::iterator i = cache->getWaypoints()->begin();
		while (i != cache->getWaypoints()->end()) {
			this->saveCacheWaypoint(*i);
		}
	}

	// Store listing
	if (cache->isLongLoaded()) {
		stmt = this->stmtInsertLong;

		stmt->bind(1, cache->getId());
		stmt->bind(2, GZCompress(cache->getShortDescription()));
		stmt->bind(3, cache->getSDIsHTML());
		stmt->bind(4, GZCompress(cache->getLongDescription()));
		stmt->bind(5, cache->getLDIsHTML());
		stmt->bind(6, cache->getHint());

		uint32_t hash = this->computeHashLong(cache);

		stmt->bind(7, (int64_t)hash);
		stmt->bind(8, GGDatabase::timeToGGDate2(cache->getDtLastUpdate()));

		if (stmt->step() != SQLITE_DONE) {
			this->db->rollback();
			return GCM::database::Database::ST_FAILED;
		}

		stmt->reset();
	}

	// Store logs
	if (cache->isLogsLoaded()) {
		updateLogs(cache->getLogs());
	}

	// Store attributes
	// TODO: Attributes unimplemented

	// Store tags
	// TODO: Tags unimplemented

	return GCM::database::Database::ST_CREATED;
}

GCM::database::Database::Result GGDatabase::updateCache(GCM::GC<GCM::geolib::Geocache> cache) {
	GC<Stmt> stmt = this->db->prepare("UPDATE \"geocache\" SET "
		"\"x\" = ?," // 1
		"\"y\" = ?," // 2
		"\"name\" = ?," // 3
		"\"author\" = ?," // 4
		"\"cachetype\" = ?," // 5
		"\"cachesize\" = ?," // 6
		"\"difficulty\" = ?," // 7
		"\"terrain\" = ?," // 8
		"\"cachestatus\" = ?," // 9
		"\"dthidden\" = ?," // 10
		"\"dtlast\" = ?," // 11
		"\"dtupdate\" = ?," // 12
		"\"comment\" = ?," // 13
		"\"dtfoundtime\" = ?," // 14
		"\"dtfound\" = ?," // 15
		"\"country\" = ?," // 16
		"\"state\" = ?," // 17
		"\"gs_cacheid\" = ?," // 18
		"\"gs_ownerid\" = ?," // 19
		"\"dtupdate2\" = ? " // 20
		"WHERE \"key\" = ?"); // 21

	stmt->bind(1, cache->getLatitude());
	stmt->bind(2, cache->getLongitude());
	stmt->bind(3, cache->getName());
	stmt->bind(4, cache->getPlacedBy());
	stmt->bind(5, Geocache::typeToGpxString(cache->getCacheType()));
	stmt->bind(6, Geocache::sizeToGpxString(cache->getSize()));
	stmt->bind(7, cache->getDifficulty() / 10.0);
	stmt->bind(8, cache->getTerrain() / 10.0);
	stmt->bind(9, (cache->isArchived()?2:(cache->isDisabled()?1:0)));
	stmt->bind(10, GGDatabase::timeToGGDate(cache->getDtCreated()));
	stmt->bind(11, 0); // TODO: Date of last found
	stmt->bind(12, GGDatabase::timeToGGDate(cache->getDtLastUpdate()));
	stmt->bind(13, cache->getComment());

	if (cache->isFound()) {
		stmt->bind(14, GGDatabase::timeToGGDate(cache->getDtFound()));
		stmt->bind(15, GGDatabase::timeToGGTime(cache->getDtFound()));
	} else {
		stmt->bind(14, 0);
		stmt->bind(15, 0);
	}

	stmt->bind(16, cache->getCountry());
	stmt->bind(17, cache->getState());
	stmt->bind(18, Geocache::gcIdToInt(cache->getId()));
	stmt->bind(19, cache->getOwnerId());
	stmt->bind(20, GGDatabase::timeToGGDate2(cache->getDtLastUpdate()));

	stmt->bind(21, cache->getRowId());

	if (stmt->step() != SQLITE_DONE) return GCM::database::Database::ST_FAILED;

	// Store waypoints
	if (cache->isWaypointsLoaded()) {
		GeocacheWaypointList::iterator i = cache->getWaypoints()->begin();
		while (i != cache->getWaypoints()->end()) {
			this->saveCacheWaypoint(*i);
		}
	}

	// Store long data
	if (cache->isLongLoaded()) {
		uint32_t hash = this->computeHashLong(cache);
		GC<Stmt> stmtQueryLong = this->db->prepare("SELECT \"key\", \"hash\" FROM \"geolist\" WHERE \"id\" = ?");
		stmtQueryLong->bind(1, cache->getId());

		if (stmtQueryLong->step() == SQLITE_ROW) {
			uint32_t existingHash = stmtQueryLong->columnInt64(0);

			if (existingHash != hash) {
				GC<Stmt> stmtUpdateLong = this->db->prepare("UPDATE \"geolist\" SET "
					"\"shortdesc\" = ?, " // 1
					"\"longdesc\" = ?, " // 2
					"\"shortdescflag\" = ?, " // 3
					"\"longdescflag\" = ?, " // 4
					"\"hint\" = ?, " // 5
					"\"hash\" = ?, " // 6
					"\"dtupdate2\" = ? " // 7
					"WHERE \"id\" = ?"); // 8

				stmtUpdateLong->bind(1, GZCompress(cache->getShortDescription()));
				stmtUpdateLong->bind(2, GZCompress(cache->getLongDescription()));
				stmtUpdateLong->bind(3, cache->getSDIsHTML());
				stmtUpdateLong->bind(4, cache->getLDIsHTML());
				stmtUpdateLong->bind(5, cache->getHint());
				stmtUpdateLong->bind(6, (int64_t)hash);
				stmtUpdateLong->bind(7, GGDatabase::timeToGGDate2(cache->getDtLastUpdate()));
				stmtUpdateLong->bind(8, cache->getId());

				stmtUpdateLong->step();
			} else {
				GC<Stmt> stmtUpdateLong2 = this->db->prepare("UPDATE \"geolist\" SET "
					"\"shortdescflag\" = ?, \"longdescflag\" = ?, \"dtupdate2\" = ? WHERE \"id\" = ?");

				stmtUpdateLong2->bind(1, cache->getSDIsHTML());
				stmtUpdateLong2->bind(2, cache->getLDIsHTML());
				stmtUpdateLong2->bind(3, GGDatabase::timeToGGDate2(cache->getDtLastUpdate()));
				stmtUpdateLong2->bind(4, cache->getId());

				stmtUpdateLong2->step();
			}
		} else {
			// Does not have long data, insert...
			stmtInsertLong->bind(1, cache->getId());
			stmtInsertLong->bind(2, GZCompress(cache->getShortDescription()));
			stmtInsertLong->bind(3, cache->getSDIsHTML());
			stmtInsertLong->bind(4, GZCompress(cache->getLongDescription()));
			stmtInsertLong->bind(5, cache->getLDIsHTML());
			stmtInsertLong->bind(6, cache->getHint());

			uint32_t hash = this->computeHashLong(cache);

			stmtInsertLong->bind(7, (int64_t)hash);
			stmtInsertLong->bind(8, GGDatabase::timeToGGDate2(cache->getDtLastUpdate()));

			stmtInsertLong->step();
		}
	}

	// Store logs
	if (cache->isLogsLoaded()) {
		updateLogs(cache->getLogs());
	}

	// TODO: Update attributes
	// TODO: Update tags

	return GCM::database::Database::ST_UPDATED;
}

GCM::database::Database::Result GGDatabase::saveCache(GCM::GC<GCM::geolib::Geocache> cache) {
	// Only update caches that originates from itself
	if (cache->getRowId() > 0
		&& cache->getDbProvider()
		&& cache->getDbProvider()->getConnectionString() == this->getConnectionString())
	{
		return this->updateCache(cache);
	} else {
		return this->createCache(cache);
	}
}

GCM::database::Database::Result GGDatabase::saveCacheWaypoint(GCM::GC<GCM::geolib::GeocacheWaypoint> waypoint) {
	GC<Stmt> stmt;

	this->db->beginTransaction();

	if (waypoint->getRowId() > 0) {
		// Waypoint exists, update.
		stmt = this->db->prepare("UPDATE \"waypoint\" SET "
				"\"id\" = ?, " // 1
				"\"x\" = ?, " // 2
				"\"y\" = ?, " // 3
				"\"name\" = ?, " // 4
				"\"prefixid\" = ?, " // 5
				"\"wpttype\" = ?, " // 6
				"\"cmt\" = ?, " // 7
				"\"comment\" = ?, " // 8
				"\"flag\" = ?, " // 9
				"\"dtupdate\" = ?, " // 10
				"\"dtupdate2\" = ? " // 11
			"WHERE \"key\" = ?"); // 12

		stmt->bind(1, waypoint->getCache()->getId());
		stmt->bind(2, waypoint->getLatitude());
		stmt->bind(3, waypoint->getLongitude());
		stmt->bind(4, waypoint->getName());
		stmt->bind(5, waypoint->getId()->substring(0, 2));
		stmt->bind(6, GeocacheWaypoint::typeToGpxString(waypoint->getWptType()));
		stmt->bind(7, waypoint->getComment());
		stmt->bind(8, waypoint->getUserComment());
		stmt->bind(9, waypoint->getUserFlag());
		stmt->bind(10, GGDatabase::timeToGGDate(waypoint->getDtLastUpdate()));
		stmt->bind(11, GGDatabase::timeToGGDate2(waypoint->getDtLastUpdate()));
		stmt->bind(12, waypoint->getRowId());

		if (stmt->step() != SQLITE_DONE) {
			this->db->rollback();
			return GCM::database::Database::ST_FAILED;
		}

		// Update r-tree
		stmt = this->db->prepare("UPDATE \"wpt_coord_index\" SET "
				"\"minX\" = ?, "
				"\"minY\" = ?, "
				"\"maxX\" = ?, "
				"\"maxY\" = ? "
			"WHERE \"key\" = ?");
		stmt->bind(1, waypoint->getLatitude());
		stmt->bind(2, waypoint->getLongitude());
		stmt->bind(3, waypoint->getLatitude());
		stmt->bind(4, waypoint->getLongitude());
		stmt->bind(5, waypoint->getRowId());

		if (stmt->step() != SQLITE_DONE) {
			this->db->rollback();
			return GCM::database::Database::ST_FAILED;
		}

		this->db->commit();
		return GCM::database::Database::ST_CREATED;
	} else {
		// Waypoint does not exists, create.
		stmt = this->db->prepare("INSERT INTO \"waypoint\" ("
			"\"id\", \"x\", \"y\", \"name\", \"prefixid\", \"wpttype\", "
			"\"cmt\", \"comment\", \"flag\", \"dtupdate\", \"dtupdate2\") VALUES "
			"(?,?,?,?,?,?,?,?,?,?,?)");
		stmt->bind(1, waypoint->getCache()->getId());
		stmt->bind(2, waypoint->getLatitude());
		stmt->bind(3, waypoint->getLongitude());
		stmt->bind(4, waypoint->getName());this->db->commit();
		stmt->bind(5, waypoint->getId()->substring(0, 2));
		stmt->bind(6, GeocacheWaypoint::typeToGpxString(waypoint->getWptType()));
		stmt->bind(7, waypoint->getComment());
		stmt->bind(8, waypoint->getUserComment());
		stmt->bind(9, waypoint->getUserFlag());
		stmt->bind(10, GGDatabase::timeToGGDate(waypoint->getDtLastUpdate()));
		stmt->bind(11, GGDatabase::timeToGGDate2(waypoint->getDtLastUpdate()));
		stmt->bind(12, waypoint->getRowId());

		if (stmt->step() != SQLITE_DONE) {
			this->db->rollback();
			return GCM::database::Database::ST_FAILED;
		}

		waypoint->setRowId(this->db->lastInsertId());

		// Create r-tree entry.
		stmt = this->db->prepare("INSERT INTO \"wpt_coord_index\" ("
			"\"key\", \"minX\", \"maxX\", \"minY\", \"maxY\") VALUES (?, ?, ?, ?, ?)");

		stmt->bind(1, waypoint->getRowId());
		stmt->bind(2, waypoint->getLatitude());
		stmt->bind(3, waypoint->getLongitude());
		stmt->bind(4, waypoint->getLatitude());
		stmt->bind(5, waypoint->getLongitude());

		if (stmt->step() != SQLITE_DONE) {
			this->db->rollback();
			return GCM::database::Database::ST_FAILED;
		}

		this->db->commit();
		return GCM::database::Database::ST_CREATED;
	}
}

GCM::database::Database::Result GGDatabase::saveGenericWaypoint(GCM::GC<GCM::geolib::GenericWaypoint> waypoint) {
	// GeoGet database does not support saving generic waypoints.
	return GCM::database::Database::ST_FAILED;
	(void)waypoint;
}

void GGDatabase::removeCache(GCM::GC<GCM::geolib::Geocache> cache) {
	this->db->beginTransaction();

	// Remove all waypoints with coord index
	GC<GeocacheWaypointList> wpts = cache->getWaypoints();
	GeocacheWaypointList::iterator i = wpts->begin();
	while (i != wpts->end()) {
		this->removeCacheWaypoint(*i);
		++i;
	}

	// Remove cache
	GC<Stmt> stmt = this->db->prepare("DELETE FROM \"geocache\" WHERE \"key\" = ?");
	stmt->bind(1, cache->getRowId());

	if (stmt->step() != SQLITE_DONE) {
		this->db->rollback();
		return;
	}

	// Remove cache coord index
	stmt = this->db->prepare("DELETE FROM \"coord_index\" WHERE \"key\" = ?");
	stmt->bind(1, cache->getRowId());

	if (stmt->step() != SQLITE_DONE) {
		this->db->rollback();
		return;
	}

	// Remove logs
	stmt = this->db->prepare("DELETE FROM \"geolog\" WHERE \"id\" = ?");
	stmt->bind(1, cache->getId());

	if (stmt->step() != SQLITE_DONE) {
		this->db->rollback();
		return;
	}

	// Remove listing
	stmt = this->db->prepare("DELETE FROM \"geolist\" WHERE \"id\" = ?");
	stmt->bind(1, cache->getId());

	if (stmt->step() != SQLITE_DONE) {
		this->db->rollback();
		return;
	}

	// Remove tags
	stmt = this->db->prepare("DELETE FROM \"geotag\" WHERE \"id\" = ?");
	stmt->bind(1, cache->getId());

	if (stmt->step() != SQLITE_DONE) {
		this->db->rollback();
		return;
	}

	this->db->commit();
}

void GGDatabase::removeCacheWaypoint(GCM::GC<GCM::geolib::GeocacheWaypoint> waypoint) {
	GC<Stmt> stmtDelWptIndex = this->db->prepare("DELETE FROM \"wpt_coord_index\" WHERE \"key\" = ?");
	GC<Stmt> stmtDelWpt = this->db->prepare("DELETE FROM \"waypoint\" WHERE \"key\" = ?");

	stmtDelWptIndex->bind(1, waypoint->getRowId());
	stmtDelWpt->bind(1, waypoint->getRowId());

	stmtDelWptIndex->step();
	stmtDelWpt->step();

	waypoint->setDbProvider(NULL);
}

GCM::GC<GCM::geolib::GeocacheWaypointList> GGDatabase::getCacheWaypoints(GCM::GC<GCM::geolib::Geocache> cache) {
	GCM::GC<GeocacheWaypointList> out = new GeocacheWaypointList();

	GC<Stmt> stmtWpt = this->db->prepare("SELECT "
		"key, " // 0
		"id, " // 1
		"x, " // 2
		"y, " // 3
		"name, " // 4
		"prefixid, " // 5
		"lookup, " // 6
		"wpttype, " // 7
		"cmt, " // 8
		"comment, " // 9
		"flag, " // 10
		"dtupdate2 " // 11
		"FROM waypoint WHERE id = ?");

	stmtWpt->bind(1, cache->getId());

	GC<ReferencePoint> rp = AppConfig::getInstance()->getActiveRefPt();

	while (stmtWpt->step() == SQLITE_ROW) {
		GC<GeocacheWaypoint> wpt = new GeocacheWaypoint();

			wpt->setCache(cache);
			wpt->setRowId(stmtWpt->columnInt(0));
			wpt->setLatitudeE6(stmtWpt->columnDouble(2) * 1E6);
			wpt->setLongitudeE6(stmtWpt->columnDouble(3) * 1E6);
			wpt->setName(stmtWpt->columnString(4));
			wpt->setWptType(GeocacheWaypoint::gpxStringToType(stmtWpt->columnString(7)));

			const char *comment = stmtWpt->columnString(8);
			if (comment) {
				wpt->setComment(comment);
			}

			const char *userComment = stmtWpt->columnString(9);
			if (userComment) {
				wpt->setUserComment(userComment);
			}

			wpt->setUserFlag(stmtWpt->columnInt(10));
			wpt->setDtLastUpdate(ggDate2ToTime(stmtWpt->columnDouble(11)));
			wpt->setDbProvider(this);

			if (rp) {
				wpt->setDistanceFrom(rp->getLatitudeE6(), rp->getLongitudeE6());
			}

			out->push_back(wpt);
	}

	return out;
}

void GGDatabase::beginTransaction() {
	this->db->beginTransaction();
}

void GGDatabase::commit() {
	this->db->commit();
}

void GGDatabase::rollback() {
	this->db->rollback();
}

const GCM::database::Database::Stats &GGDatabase::getStats() {
	if (!this->statsValid) {
		GC<Stmt> stmt = this->db->prepare("SELECT COUNT(*) FROM geocache");
		if (stmt->step() == SQLITE_ROW) {
			this->stats.numCaches = stmt->columnInt(0);
		}

		stmt = this->db->prepare("SELECT COUNT(*) FROM waypoint");
		if (stmt->step() == SQLITE_ROW) {
			this->stats.numWaypoints = stmt->columnInt(0);
		}

		this->statsValid = true;
	}

	return this->stats;
}

GCM::GC<GCM::geolib::GeocacheList> GGDatabase::listCaches(GCM::GC<GCM::geolib::Filter> f, ptrDialog progressDialog) {
	GC<Stmt> stmtCache = this->db->prepare("SELECT "
		"key, " // 0
		"id, " // 1
		"CAST (x AS DOUBLE), " // 2
		"CAST (y AS DOUBLE), " // 3
		"name, " // 4
		"author, " // 5
		"cachetype, " // 6
		"cachesize, " // 7
		"difficulty, " // 8
		"terrain, " // 9
		"cachestatus, " // 10
		"userstatus, " // 11
		"dthidden, " // 12
		"comment, " // 13
		"dtfoundtime, " // 14
		"dtfound, " // 15
		"country, " // 16
		"state, " // 17
		"gs_ownerid, " // 18
		"dtupdate2 " // 19
		"FROM geocache");

	GC<Stmt> stmtWpt = this->db->prepare("SELECT "
		"key, " // 0
		"id, " // 1
		"x, " // 2
		"y, " // 3
		"name, " // 4
		"prefixid, " // 5
		"lookup, " // 6
		"wpttype, " // 7
		"cmt, " // 8
		"comment, " // 9
		"flag, " // 10
		"dtupdate2 " // 11
		"FROM waypoint");

	GC<GeocacheList> out = new GeocacheList();

	if (!stmtCache || !stmtWpt) return out;

	GC<ReferencePoint> rp = AppConfig::getInstance()->getActiveRefPt();

	const Stats &st = this->getStats();
	progressDialog->setMaxProgress(
		st.numCaches + st.numWaypoints
	);

	while (stmtCache->step() == SQLITE_ROW) {
		GC<Geocache> gc = new Geocache();
		gc->setRowId(stmtCache->columnInt(0));
		gc->setId(stmtCache->columnString(1));
		gc->setLatitudeE6(stmtCache->columnDouble(2) * 1E6);
		gc->setLongitudeE6(stmtCache->columnDouble(3) * 1E6);
		gc->setName(stmtCache->columnString(4));
		gc->setPlacedBy(stmtCache->columnString(5));
		gc->setOwner(stmtCache->columnString(5));
		gc->setCacheType(Geocache::gpxStringToType(stmtCache->columnString(6)));
		gc->setSize(Geocache::gpxStringToSize(stmtCache->columnString(7)));
		gc->setDifficulty(stmtCache->columnDouble(8) * 10);
		gc->setTerrain(stmtCache->columnDouble(9) * 10);

		switch (stmtCache->columnInt(10)) {
			case 0:
				gc->setCacheStatus(Geocache::ST_AVAILABLE);
				break;

			case 1:
				gc->setCacheStatus(Geocache::ST_DISABLED);
				break;

			case 2:
				gc->setCacheStatus(Geocache::ST_ARCHIVED);
				break;
		}

		gc->setDtCreated(GGDatabase::ggDateToTime(stmtCache->columnString(12)));
		const char *comment = stmtCache->columnString(13);
		if (comment) {
			gc->setComment(comment);
		} else {
			gc->setComment("");
		}

		// If cache is found...
		if (stmtCache->columnInt(15) > 0) {
			gc->setDtFound(GGDatabase::ggDateToTime(stmtCache->columnString(14), stmtCache->columnString(15)));
		}

		gc->setCountry(stmtCache->columnString(16));
		gc->setState(stmtCache->columnString(17));
		gc->setOwnerId(stmtCache->columnInt(18));
		gc->setDtLastUpdate(GGDatabase::ggDate2ToTime(stmtCache->columnDouble(19)));

		gc->setMapLoaded(true);
		gc->setLoaded(true);

		gc->setDbProvider(this);

		if (rp) {
			gc->setDistanceFrom(rp->getLatitudeE6(), rp->getLongitudeE6());
		}

		if (!f || f->matchCache(gc)) {
			out->append(gc);
		}

		if (progressDialog) {
			progressDialog->setProgress(progressDialog->getProgress() + 1);
		}
	}

	while (stmtWpt->step() == SQLITE_ROW) {
		/*
		"key, " // 0
		"id, " // 1
		"x, " // 2
		"y, " // 3
		"name, " // 4
		"prefixid, " // 5
		"wpttype, " // 7
		"cmt, " // 8
		"comment, " // 9
		"flag, " // 10
		"dtupdate2 " // 11
		*/

		GC<Geocache> cache = out->getCache(stmtWpt->columnString(1));
		if (cache) {
			GC<GeocacheWaypoint> wpt = new GeocacheWaypoint();

			wpt->setCache(cache);
			wpt->setRowId(stmtWpt->columnInt(0));
			wpt->setLatitudeE6(stmtWpt->columnDouble(2) * 1E6);
			wpt->setLongitudeE6(stmtWpt->columnDouble(3) * 1E6);
			wpt->setName(stmtWpt->columnString(4));
			wpt->setWptType(GeocacheWaypoint::gpxStringToType(stmtWpt->columnString(7)));

			const char *comment = stmtWpt->columnString(8);
			if (comment) {
				wpt->setComment(comment);
			}

			const char *userComment = stmtWpt->columnString(9);
			if (userComment) {
				wpt->setUserComment(userComment);
			}

			wpt->setUserFlag(stmtWpt->columnInt(10));
			wpt->setDtLastUpdate(ggDate2ToTime(stmtWpt->columnDouble(11)));
			wpt->setDbProvider(this);

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

	return out;
}

void GGDatabase::loadLongData(GCM::GC<GCM::geolib::Geocache> cache) {
	GC<Stmt> stmt = this->db->prepare("SELECT shortdesc, shortdescflag, longdesc, longdescflag, hint FROM geolist WHERE id = ?");
	stmt->bind(1, cache->getId());

	if (stmt->step() == SQLITE_ROW) {
		GCM::util::Log::d("GeoGet", "Got request for listing... and listing was found.");

		cache->setShortDescription(GZDecompress(stmt->columnData(0)));
		cache->setSDIsHTML(stmt->columnInt(1) == 1);

		cache->setLongDescription(GZDecompress(stmt->columnData(2)));
		cache->setLDIsHTML(stmt->columnInt(3) == 1);

		cache->setHint(stmt->columnString(4));

		cache->setLongLoaded(true);
	} else {
		GCM::util::Log::d("GeoGet", "Got request for listing... and listing was NOT found.");
	}
}

void GGDatabase::loadAttributes(GCM::GC<GCM::geolib::Geocache> cache) {
	// TODO: Unimplemented
	(void)cache;
}

void GGDatabase::loadLogs(GCM::GC<GCM::geolib::Geocache> cache, int count) {
	GC<Stmt> stmt;

	// Load all logs for given cache
	if (count < 0) {
		stmt = this->db->prepare("SELECT dt, type, finder, logtext, gs_logid, gs_finderid FROM geolog WHERE id = ? ORDER BY dt DESC");
		stmt->bind(1, cache->getId());
	// Load only last <count> logs for the cache
	} else {
		stmt = this->db->prepare("SELECT dt, type, finder, logtext, gs_logid, gs_finderid FROM geolog WHERE id = ? ORDER BY dt DESC LIMIT 0, ?");
		stmt->bind(1, cache->getId());
		stmt->bind(2, count);
	}

	while (stmt->step() == SQLITE_ROW) {
		GC<Log> log = new Log();
		// TODO: Unzip

		log->setId(stmt->columnInt(4));
		log->setTime(GGDatabase::ggDateToTime(stmt->columnString(0)));
		log->setType(Log::gpxStringToType(stmt->columnString(1)));
		log->setFinderId(stmt->columnInt(5));
		log->setFinder(stmt->columnString(2));
		log->setText(GZDecompress(stmt->columnData(3)));
		log->setTextEncoded(false);

		cache->getLogs()->append(log);
	}

	cache->setLogsLoaded(true);
}

void GGDatabase::loadWaypoints(GCM::GC<GCM::geolib::Geocache> cache) {
	GCM::GC<GCM::geolib::GeocacheWaypointList> list = this->getCacheWaypoints(cache);
	GCM::GC<GCM::geolib::GeocacheWaypointList> wpts = cache->getWaypoints();
	wpts->clear();
	wpts->insert(wpts->begin(), list->begin(), list->end());

	cache->setWaypointsLoaded(true);
}

GCM::GC<GCM::geolib::Geocache> GGDatabase::getByGCID(GCM::String gcId) {
	GC<Stmt> stmtCache = this->db->prepare("SELECT "
		"key, " // 0
		"id, " // 1
		"CAST (x AS DOUBLE), " // 2
		"CAST (y AS DOUBLE), " // 3
		"name, " // 4
		"author, " // 5
		"cachetype, " // 6
		"cachesize, " // 7
		"difficulty, " // 8
		"terrain, " // 9
		"cachestatus, " // 10
		"userstatus, " // 11
		"dthidden, " // 12
		"comment, " // 13
		"dtfoundtime, " // 14
		"dtfound, " // 15
		"country, " // 16
		"state, " // 17
		"gs_ownerid, " // 18
		"dtupdate2 " // 19
		"FROM geocache WHERE ic = ?");

	stmtCache->bind(1, gcId);
	if (stmtCache->step() == SQLITE_ROW) {
		GC<Geocache> gc = new Geocache();

		gc->setRowId(stmtCache->columnInt(0));
		gc->setId(stmtCache->columnString(1));
		gc->setLatitudeE6(stmtCache->columnDouble(2) * 1E6);
		gc->setLongitudeE6(stmtCache->columnDouble(3) * 1E6);
		gc->setName(stmtCache->columnString(4));
		gc->setPlacedBy(stmtCache->columnString(5));
		gc->setOwner(stmtCache->columnString(5));
		gc->setCacheType(Geocache::gpxStringToType(stmtCache->columnString(6)));
		gc->setSize(Geocache::gpxStringToSize(stmtCache->columnString(7)));
		gc->setDifficulty(stmtCache->columnDouble(8) * 10);
		gc->setTerrain(stmtCache->columnDouble(9) * 10);

		switch (stmtCache->columnInt(10)) {
			case 0:
				gc->setCacheStatus(Geocache::ST_AVAILABLE);
				break;

			case 1:
				gc->setCacheStatus(Geocache::ST_DISABLED);
				break;

			case 2:
				gc->setCacheStatus(Geocache::ST_ARCHIVED);
				break;
		}

		gc->setDtCreated(GGDatabase::ggDateToTime(stmtCache->columnString(12)));
		const char *comment = stmtCache->columnString(13);
		if (comment) {
			gc->setComment(comment);
		} else {
			gc->setComment("");
		}

		// If cache is found...
		if (stmtCache->columnInt(15) > 0) {
			gc->setDtFound(GGDatabase::ggDateToTime(stmtCache->columnString(14), stmtCache->columnString(15)));
		}

		gc->setCountry(stmtCache->columnString(16));
		gc->setState(stmtCache->columnString(17));
		gc->setOwnerId(stmtCache->columnInt(18));
		gc->setDtLastUpdate(GGDatabase::ggDate2ToTime(stmtCache->columnDouble(19)));

		gc->setMapLoaded(true);
		gc->setLoaded(true);

		gc->setDbProvider(this);

		GC<ReferencePoint> rp = AppConfig::getInstance()->getActiveRefPt();

		if (rp) {
			gc->setDistanceFrom(rp->getLatitudeE6(), rp->getLongitudeE6());
		}

		return gc;
	} else {
		return NULL;
	}
}

GCM::GC<GCM::geolib::GeocacheList> GGDatabase::getByName(GCM::String name) {
	// TODO: Unimplemented
	return new GeocacheList();
	(void)name;
}

GCM::String GGDatabase::GZDecompress(GCM::String str) {
	z_stream stream;
	int ret;

	memset(&stream, 0, sizeof(z_stream));

	GCM::util::Log::d("GeoGet", "Decompressing string...");

	if ((ret = inflateInit(&stream)) != Z_OK) {
		GCM::util::Log::e("GeoGet", "Unable to decompress string (%d).", ret);
		return "";
	}

	std::stringstream ss;
	ss.str("");

	static const int CHUNK = 256 * 1024;
	char out[CHUNK];

	stream.avail_in = str->length();
	stream.next_in = (Bytef *)str->c_str();

	// Nothing to decompress... Not error, but nothing to do next.
	if (stream.avail_in == 0) {
		return "";
	}

	do {
		stream.avail_out = CHUNK;
		stream.next_out = (Bytef *)out;

		int ret = inflate(&stream, Z_NO_FLUSH);

		switch (ret) {
			case Z_STREAM_ERROR:
			case Z_NEED_DICT:
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				inflateEnd(&stream);
				GCM::util::Log::e("GeoGet", "Unable to decompress string (%d).", ret);
				return "";
		}

		int have = CHUNK - stream.avail_out;
		ss.write(out, have);
	} while (stream.avail_out == 0);

	inflateEnd(&stream);

	return ss.str();
}

GCM::String GGDatabase::GZCompress(GCM::String str) {
	z_stream stream;
	int ret;

	memset(&stream, 0, sizeof(z_stream));

	if ((ret = deflateInit(&stream, Z_BEST_SPEED)) != Z_OK) {
		GCM::util::Log::e("GeoGet", "Unable to compress string (%d).", ret);
		return "";
	}

	stream.next_in = (Bytef *)str->c_str();
	stream.avail_in = str->length();

	std::stringstream ss;
	static const int CHUNK = 256;
	char out[CHUNK];

	do {
		stream.avail_out = CHUNK;
		stream.next_out = (Bytef *)out;

		int ret = deflate(&stream, Z_NO_FLUSH);

		switch (ret) {
			case Z_STREAM_ERROR:
			case Z_NEED_DICT:
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				deflateEnd(&stream);
				GCM::util::Log::e("GeoGet", "Unable to decompress string (%d).", ret);
				return "";
		}

		int have = CHUNK - stream.avail_out;
		ss.write(out, have);

	} while (stream.avail_out == 0);

	deflateEnd(&stream);

	return ss.str();
}
