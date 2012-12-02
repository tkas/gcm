#ifndef _GCM_EXPORT_H
# define _GCM_EXPORT_H

#include <geolib/GeocacheList.h>
#include <util/File.h>
#include <GC.h>
#include <Object.h>
#include <stl/List.h>
#include <util/File.h>
#include <ui.h>

namespace GCM {

class ExportInfo: public Object {
	public:
		ExportInfo(String exportId,	String formatDescription,
			String fileExtension):
			exportId(exportId), formatDescription(formatDescription),
			fileExtension(fileExtension) {}
		
		/**
		 * Identifier used for command line export format specification.
		 * Should contain only valid identifier letters, without any special
		 * characters.
		 */
		String exportId;
		
		/**
		 * Human readable description of the export format
		 * (for example "PocketQuery GPX"). Will be used in the GUI to display
		 * choice of export format.
		 */
		String formatDescription;
		
		/**
		 * Extension of the exported file.
		 */
		String fileExtension;
		
		/**
		 * Callback function that does the export.
		 */
		virtual bool exportList(GC<geolib::GeocacheList> list, io::File outputFile, ptrDialog progressDialog) = 0;
};

typedef GC< List<ExportInfo> > (*fExportInfo)();

}

#endif
