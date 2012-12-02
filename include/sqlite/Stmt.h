#ifndef _SQLITE_STMT_H
# define _SQLITE_STMT_H

#include <Object.h>
#include <sqlite3.h>
#include <stdint.h>
#include <GC.h>
#include <stl/String.h>
#include <stl/Modifiable.h>

namespace GCM {
	namespace sqlite {

class Database;

class GCM_API Stmt: public Object {
	friend class GCM::sqlite::Database;

	private:
		sqlite3_stmt *mStmt;
		GCM::String mSql;
		WeakGC<Database> mDb;
		bool finalized;
		Stmt(sqlite3_stmt *mStmt, GCM::String sql, GC<Database>db);
		int usage;
		double totalTime;

	public:
		int reset();
		int step();
		void finalize();

		int bind(const int id, const char *value);
		int bind(const int id, const char *value, size_t len);
		int bind(const int id, const GCM::String value);
		int bind(const int id, const int32_t value);
		int bind(const int id, const int64_t value);
		int bind(const int id, const uint32_t value);
		int bind(const int id, const uint64_t value);
		int bind(const int id, const double value);
		int bind(const int id, const void *value, const size_t len);
		int bindNull(const int id);

		int bindIfModified(const int id, const Modifiable<GCM::String> &value);
		int bindIfModified(const int id, const Modifiable<int32_t> &value);
		int bindIfModified(const int id, const Modifiable<int64_t> &value);
		int bindIfModified(const int id, const Modifiable<uint32_t> &value);
		int bindIfModified(const int id, const Modifiable<uint64_t> &value);
		int bindIfModified(const int id, const Modifiable<double> &value);
		int bindIfModified(const int id, const Modifiable<bool> &value);

		const char *columnString(int id);
		int columnInt(int id);
		int64_t columnInt64(int id);
		double columnDouble(int id);
		int columnBytes(int id);
		const void *columnBlob(int id);
		const std::string columnData(int id);

		virtual ~Stmt();
};

	}
}

#include <sqlite/Database.h>

#endif

