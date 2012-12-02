#include <sqlite/Database.h>
#include <sqlite/Stmt.h>
#include <GC.h>
#include <util/Log.h>
#include <util/File.h>

using namespace GCM::sqlite;
using namespace GCM::io;
using namespace GCM;

GCM_API Database::Database(): db(NULL), opened(false) {

}

int GCM_API Database::open(const char *filename, int flags) {
	if (this->opened) {
		this->close();
	}

	// Try if path exists. If not, create.
	File f(filename);
	if (!f->getParentFile()->getAbsoluteFile()->exists()) {
		GCM::util::Log::d("Database", "Path %s does not exists, creating.", f->getParent()->c_str());
		f->mkdirs();
	}

	String localpath = f->getAbsolutePath();
	const char *utf8path;

#ifndef _WIN32
	utf8path = localpath->c_str();
#else
	// Convert from local encoding to UTF-8.
	wchar_t buffer[MAX_PATH];
	char utf8buffer[MAX_PATH];

	if (MultiByteToWideChar(CP_ACP, 0, localpath->c_str(), -1, buffer, MAX_PATH * sizeof(wchar_t)) == 0) {
		switch (GetLastError()) {
			case ERROR_INSUFFICIENT_BUFFER: GCM::util::Log::e("SQLite|MultiByteToWideChar", "A supplied buffer size was not large enough, or it was incorrectly set to NULL."); break;
			case ERROR_INVALID_FLAGS: GCM::util::Log::e("SQLite|MultiByteToWideChar", "The values supplied for flags were not valid."); break;
			case ERROR_INVALID_PARAMETER: GCM::util::Log::e("SQLite|MultiByteToWideChar", "Any of the parameter values was invalid."); break;
			case ERROR_NO_UNICODE_TRANSLATION: GCM::util::Log::e("SQLite|MultiByteToWideChar", "Invalid Unicode was found in a string."); break;
		}
	} else {
	}

	if (WideCharToMultiByte(CP_UTF8, 0, buffer, -1, utf8buffer, MAX_PATH, NULL, NULL) == 0) {
		switch (GetLastError()) {
			case ERROR_INSUFFICIENT_BUFFER: GCM::util::Log::e("SQLite|WideCharToMultiByte", "A supplied buffer size was not large enough, or it was incorrectly set to NULL."); break;
			case ERROR_INVALID_FLAGS: GCM::util::Log::e("SQLite|WideCharToMultiByte", "The values supplied for flags were not valid."); break;
			case ERROR_INVALID_PARAMETER: GCM::util::Log::e("SQLite|WideCharToMultiByte", "Any of the parameter values was invalid."); break;
			case ERROR_NO_UNICODE_TRANSLATION: GCM::util::Log::e("SQLite|WideCharToMultiByte", "Invalid Unicode was found in a string."); break;
		}
	} else {
	}
	utf8path = utf8buffer;
#endif

	GCM::util::Log::d("SQLite", "Open database file '%s'", utf8path);

	int result = sqlite3_open_v2(utf8path, &this->db, flags, NULL);
	if (result == SQLITE_OK) {
		this->opened = true;
	} else {
		this->handleError();
		sqlite3_close(this->db);
	}
	return result;
}

GC<Stmt> GCM_API Database::prepare(const char *statement) {
	if (!this->opened) {
		this->handleDbNotOpen();
		return NULL;
	}

	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(this->db, statement, -1, &stmt, NULL) == SQLITE_OK) {
		Stmt *out = new Stmt(stmt, statement, this);
		return GC<Stmt>(out);
	} else {
		this->handleError(statement);
		return NULL;
	}
}

int GCM_API Database::exec(const char *sql) {
	if (this->opened) {
		int result = sqlite3_exec(this->db, sql, NULL, NULL, NULL);
#ifdef SQLITE_DEBUG
		GCM::util::Log::d("SQLite", "Exec %s", sql);
#endif
		if (result != SQLITE_OK) {
			this->handleError(sql);
		}
		return result;
	} else {
		this->handleDbNotOpen();
		return SQLITE_ERROR;
	}
}

void GCM_API Database::close() {
	if (this->opened) {
		sqlite3_close(this->db);
		this->opened = false;
	}
}

int GCM_API Database::beginTransaction() {
	return this->exec("BEGIN TRANSACTION");
}

int GCM_API Database::commit() {
	return this->exec("COMMIT");
}

int GCM_API Database::rollback() {
	return this->exec("ROLLBACK");
}

GCM_API Database::~Database() {
	// Automatically close the database.
	if (this->opened) {
		this->close();
	}
}

void GCM_API Database::handleError() {
	GCM::util::Log::e("SQLite", "%s", sqlite3_errmsg(this->db));
}

void GCM_API Database::handleError(const char *sql) {
	GCM::util::Log::e("SQLite", "%s\nQuery: %s", sqlite3_errmsg(this->db), sql);
}

void GCM_API Database::handleDbNotOpen() {
	GCM::util::Log::e("SQLite", "%s", "Database is not opened.");
}

int64_t GCM_API Database::lastInsertId() {
	return sqlite3_last_insert_rowid(this->db);
}
