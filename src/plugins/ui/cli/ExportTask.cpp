#include "ExportTask.h"
#include <pluginmanager/PluginManager.h>
#include <export.h>
#include <config/AppConfig.h>
#include <util/Log.h>
#include <stl/String.h>
#include <string.h>

using namespace GCM::pluginmanager;
using namespace GCM::config;
using namespace GCM::io;
using namespace GCM;

ExportTask::ExportTask(File mFile, const char *mFmt, const char *mDB, const char *mFilter, ptrUI ui):
	BackgroundTask(), mFilename(mFile), mFmtname(mFmt), mDBname(mDB), mFiltername(mFilter), mProgressDialog(NULL), mUI(ui) {};

void ExportTask::onPreExecute() {
	mProgressDialog = mUI->getDialogBuilder()->buildProgressDialog();
}

void ExportTask::onPostExecute() {
	mProgressDialog->dismiss();
}

void ExportTask::doInBackground() {
	PluginManager *pm = PluginManager::Instance();
	
	PluginManager::PluginFunctionList &funcs = pm->listPlugins("Export");
	PluginManager::PluginFunctionList::iterator i = funcs.begin();
	
	while (i != funcs.end()) {
		fExportInfo infoCall = (fExportInfo)i->second;
		GC< List<ExportInfo> > info = infoCall();
		
		List<ExportInfo>::iterator ii = info->begin();
		while (ii != info->end()) {

		  GCM::util::Log::d("Export", "Requested export to %s file with %s format (DB: %s, Filter: %s).",
                        mFilename->toString()->c_str(), mFmtname, mDBname, mFiltername);

      if(strcmp(mFmtname, (*ii)->exportId->c_str()) == 0){
        printf("\n");
        mProgressDialog->setMessage("Building list for export...");
	      mProgressDialog->show();

        //set database to use
        GC<database::Database> db = AppConfig::getInstance()->getWorkingDatabase();
        if (mDBname != NULL) { //user requested other then default DB
          File fFile(mDBname);
          if(fFile->exists()) {
            GCM::util::Log::d("Export", "DB %s exists, using instead of default.", fFile->getName()->c_str());
            
            PluginManager::PluginFunctionList &funcs = PluginManager::Instance()->listPlugins("Database");
            PluginManager::PluginFunctionList::iterator i = funcs.begin();
            while (i != funcs.end()) {
              GCM::database::fDatabase call = (GCM::database::fDatabase)i->second;
              GCM::GC< GCM::List<GCM::database::DatabaseInfo> > info = call();

              GCM::List<GCM::database::DatabaseInfo>::iterator ii = info->begin();
              while (ii != info->end()) {
                if ((*ii)->accept(fFile->getAbsolutePath()->c_str())) {
                  db = (*ii)->open(fFile->toString()->c_str());
                  break;
                }
                ii++;
              }

              if (ii != info->end())
                break;
              i++;
            }
            if (i == funcs.end())
              GCM::util::Log::e("Export", "Unable to open selected database %s - no plugin was found.", fFile->getName()->c_str());
          }
        }

        GCM::GC<GCM::geolib::Filter> filter = AppConfig::getInstance()->getActiveFilter();
        if (mFiltername != NULL) {//user requested other then default Filter
          File fFile(AppConfig::getInstance()->getDataDirectory());
          fFile->append("filters");
          fFile->append(mFiltername);
          if(fFile->exists()) {
            GCM::util::Log::d("Export", "Filter %s exists, using instead of default.", fFile->getName()->c_str());
            filter = GCM::geolib::Filter::loadFromFile(fFile->toString());
          }
        }
        
        GCM::GC<GCM::geolib::GeocacheList> list = db->listCaches(filter, mProgressDialog);

        mProgressDialog->setProgress(0);
        mProgressDialog->setMessage(String::format("Exporting %s...", mFilename->getName()->c_str()));
  			(*ii)->exportList(list, mFilename,  mProgressDialog);
        break;
			}
			ii++;
		}

		// Exported.
		if (ii != info->end()) {
			break;
		}
		i++;
	}
	
	if (i == funcs.end()) {
		GCM::util::Log::e("Export", "Unable to found export plugin that handles this file type.");
	}
}

