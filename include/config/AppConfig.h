#ifndef _APP_CONFIG_H
# define _APP_CONFIG_H

#include "Configuration.h"
#include <database/Database.h>
#include <stl/String.h>
#include <GC.h>
#include <geolib/Filter.h>
#include <geolib/ReferencePoint.h>

namespace GCM {
	namespace config {

/**
 * Singleton class that holds application configuration.
 */
class GCM_API AppConfig: public Configuration {
	private:
		static AppConfig *instance;
		AppConfig();

		GCM::GC<GCM::database::Database> mDb;

		String mWorkingDirectory;
		String mDataDirectory;
		bool mDDFound;
		GCM::GC<GCM::geolib::Filter> mActiveFilter;
		GCM::GC< ArrayList<GCM::geolib::ReferencePoint> > mReferencePoints;
		GCM::GC<GCM::geolib::ReferencePoint> mActiveRefPt;
		
		void loadReferencePoints();
		void saveReferencePoints();

	public:
		static AppConfig *getInstance();
		static void finalize();

		/**
		 * Load settings outside configuration file, such as reference points.
		 */
		void loadOtherSettings();

		void setWorkingDatabase(GCM::GC<GCM::database::Database> db);
		GCM::GC<GCM::database::Database> getWorkingDatabase();

		String getDataDirectory();
		GCM::GC<GCM::geolib::Filter> getActiveFilter();
		void setActiveFilter(GCM::GC<GCM::geolib::Filter> filter);

		String getWorkingDirectory();
		void setWorkingDirectory(String wd);
		
		GCM::GC< GCM::ArrayList<GCM::geolib::ReferencePoint> > getReferencePoints();
		GCM::GC<GCM::geolib::ReferencePoint> getActiveRefPt();
		void setActiveRefPt(GCM::GC<GCM::geolib::ReferencePoint> refPt);
};

	}
}

#endif
