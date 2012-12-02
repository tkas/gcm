/**
 * \brief Specifies format of functions for import plugins.
 */

#ifndef _GCM_IMPORT_H
# define _GCM_IMPORT_H

#include <gcm.h>
#include <GC.h>
#include <database/Database.h>
#include <ui.h>
#include <stl/List.h>
#include <stl/String.h>

namespace GCM {

class ImportInfo: public GCM::Object {
	public:
		ImportInfo(String fileDesc, String fileMask):
			fileDesc(fileDesc), fileMask(fileMask) {}
			
		GCM::String fileDesc;
		GCM::String fileMask;
		
		/**
		 * Function that determines whether the import accepts
		 * the specified file. Should return true, if import can handle this,
		 * false otherwise.
		 * @param filename File name to be imported.
		 */
		virtual bool accept(const char *filename) = 0;
		
		/**
		 * Do the actual import to specified database.
		 * @param filename File name to be imported.
		 * @param db Database where to import
		 * @param ptrDialog Progress dialog to use to inform about progress of import.
		 */
		virtual void import(const char *filename, GC<database::Database> db, ptrDialog progressDialog) = 0;

		/**
		 * Cancel importing...
		 */
		virtual void cancel() = 0;
};

typedef GC< List<ImportInfo> > (*fImportInfo)();

}

#endif
