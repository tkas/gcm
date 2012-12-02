#ifndef _SQLITE_DATABASE_H
# define _SQLITE_DATABASE_H

#include <Object.h>
#include <GC.h>
#include <sqlite3.h>

#include "Stmt.h"

namespace GCM {
	namespace sqlite {

class GCM_API Database: public Object {
	private:
		sqlite3 *db;
		bool opened;

		void close();
		void handleError();
		void handleError(const char *sql);
		void handleDbNotOpen();

	friend class Stmt;

	public:
		Database();

		int open(const char *filename, int flags);
		GC<Stmt> prepare(const char *statement);
		int exec(const char *sql);
		int beginTransaction();
		int commit();
		int rollback();
		int64_t lastInsertId();

		virtual ~Database();
};

	}
}


#endif
