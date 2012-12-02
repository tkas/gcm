#include <sqlite/Stmt.h>
#include <stl/String.h>
#include <util/Log.h>
#ifndef _WIN32
# include <sys/time.h>
#else
# include "gettimeofday.h"
#endif

using namespace GCM;
using namespace GCM::sqlite;

GCM_API Stmt::Stmt(sqlite3_stmt *stmt, String sql, GC<Database>db): mStmt(stmt), mSql(sql), mDb(db), finalized(false), usage(0), totalTime(0) {
}

int GCM_API Stmt::reset() {
	int result = sqlite3_reset(this->mStmt);
	return result;
}

int GCM_API Stmt::step() {
	struct timeval tmStart;
	struct timeval tmEnd;

	gettimeofday(&tmStart, NULL);
	int result = sqlite3_step(this->mStmt);
	gettimeofday(&tmEnd, NULL);
	++this->usage;

	this->totalTime += (tmEnd.tv_sec - tmStart.tv_sec) + (tmEnd.tv_usec - tmStart.tv_usec) / 1e6;

#ifdef SQLITE_DEBUG
	GCM::util::Log::d("SQLite", "Step %s", this->mSql->c_str());
#endif
	if (result != SQLITE_DONE && result != SQLITE_ROW) {
		this->mDb->handleError(mSql->c_str());
	}
	return result;
}

void GCM_API Stmt::finalize() {
	if (!this->finalized) {
		sqlite3_finalize(this->mStmt);
		this->finalized = true;
	}
}

int GCM_API Stmt::bind(const int id, const char *value) {
	int result = sqlite3_bind_text(this->mStmt, id, value, -1, SQLITE_TRANSIENT);
	if (result != SQLITE_OK) {
		this->mDb->handleError();
	}

	return result;
}

int GCM_API Stmt::bind(const int id, const char *value, size_t len) {
	int result = sqlite3_bind_text(this->mStmt, id, value, len, SQLITE_TRANSIENT);
	if (result != SQLITE_OK) {
		this->mDb->handleError();
	}

	return result;
}

int GCM_API Stmt::bind(const int id, const String value) {
	// Here we could probably use Garbage collector to not require SQLITE_TRANSIENT.
	// But for now, let's keep it the simple way.
	int result = sqlite3_bind_text(this->mStmt, id, value->c_str(), value->length(), SQLITE_TRANSIENT);
	if (result != SQLITE_OK) {
		this->mDb->handleError();
	}

	return result;
}

int GCM_API Stmt::bind(const int id, const int32_t value) {
	int result = sqlite3_bind_int(this->mStmt, id, value);
	if (result != SQLITE_OK) {
		this->mDb->handleError();
	}

	return result;
}

int GCM_API Stmt::bind(const int id, const uint32_t value) {
	int result = sqlite3_bind_int(this->mStmt, id, (int64_t)value);
	if (result != SQLITE_OK) {
		this->mDb->handleError();
	}

	return result;
}

int GCM_API Stmt::bind(const int id, const int64_t value) {
	int result = sqlite3_bind_int64(this->mStmt, id, value);
	if (result != SQLITE_OK) {
		this->mDb->handleError();
	}

	return result;
}

int GCM_API Stmt::bind(const int id, const uint64_t value) {
	int result = sqlite3_bind_int64(this->mStmt, id, value);
	if (result != SQLITE_OK) {
		this->mDb->handleError();
	}
	return result;
}

int GCM_API Stmt::bind(const int id, const double value) {
	int result = sqlite3_bind_double(this->mStmt, id, value);
	if (result != SQLITE_OK) {
		this->mDb->handleError();
	}

	return result;
}

int GCM_API Stmt::bind(const int id, const void *value, const size_t len) {
	int result = sqlite3_bind_blob(this->mStmt, id, value, len, SQLITE_TRANSIENT);
	if (result != SQLITE_OK) {
		this->mDb->handleError();
	}

	return result;
}

int GCM_API Stmt::bindNull(int id) {
	int result = sqlite3_bind_null(this->mStmt, id);
	if (result != SQLITE_OK) {
		this->mDb->handleError();
	}

	return result;
}

int GCM_API Stmt::bindIfModified(const int id, const Modifiable<GCM::String> &value) {
	if (value.isModified()) {
		return this->bind(id, value);
	} else {
		return this->bindNull(id);
	}
}

int GCM_API Stmt::bindIfModified(const int id, const Modifiable<int32_t> &value) {
	if (value.isModified()) {
		return this->bind(id, value);
	} else {
		return this->bindNull(id);
	}
}

int GCM_API Stmt::bindIfModified(const int id, const Modifiable<int64_t> &value) {
	if (value.isModified()) {
		return this->bind(id, value);
	} else {
		return this->bindNull(id);
	}
}

int GCM_API Stmt::bindIfModified(const int id, const Modifiable<double> &value) {
	if (value.isModified()) {
		return this->bind(id, value);
	} else {
		return this->bindNull(id);
	}
}

int GCM_API Stmt::bindIfModified(const int id, const Modifiable<bool> &value) {
	if (value.isModified()) {
		return this->bind(id, value.get());
	} else {
		return this->bindNull(id);
	}
}

int GCM_API Stmt::bindIfModified(const int id, const Modifiable<uint64_t> &value) {
	if (value.isModified()) {
		return this->bind(id, (int64_t)value.get());
	} else {
		return this->bindNull(id);
	}
}

int GCM_API Stmt::bindIfModified(const int id, const Modifiable<uint32_t> &value) {
	if (value.isModified()) {
		return this->bind(id, (int)value.get());
	} else {
		return this->bindNull(id);
	}
}

const char GCM_API *Stmt::columnString(int id) {
	return (const char *)sqlite3_column_text(this->mStmt, id);
}

int GCM_API Stmt::columnInt(int id) {
	return sqlite3_column_int(this->mStmt, id);
}

int64_t GCM_API Stmt::columnInt64(int id) {
	return sqlite3_column_int64(this->mStmt, id);
}

double GCM_API Stmt::columnDouble(int id) {
	return sqlite3_column_double(this->mStmt, id);
}

int GCM_API Stmt::columnBytes(int id) {
	return sqlite3_column_bytes(this->mStmt, id);
}

const void GCM_API *Stmt::columnBlob(int id) {
	return sqlite3_column_blob(this->mStmt, id);
}

const std::string Stmt::columnData(int id) {
	return std::string(
		(const char *)columnBlob(id),
		(size_t)columnBytes(id)
	);
}

GCM_API Stmt::~Stmt() {
	this->finalize();

	if (this->usage > 0) {
		GCM::util::Log::d("SQLite", "Statement %s used %d times, total %1.2fs.",
			this->mSql->c_str(), this->usage, this->totalTime);
	}
}
