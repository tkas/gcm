#include "ImportTask.h"
#include <pluginmanager/PluginManager.h>
#include <import.h>
#include <config/AppConfig.h>
#include <util/Log.h>
#include <util/File.h>
#include <stl/String.h>

using namespace GCM::pluginmanager;
using namespace GCM::config;
using namespace GCM::io;
using namespace GCM;

ImportTask::ImportTask(const char *filename, const char*dbName, ptrUI ui):
	BackgroundTask(), mFilename(filename), mDBname(dbName), mProgressDialog(NULL), mUI(ui) {};

void ImportTask::onPreExecute() {
	mProgressDialog = mUI->getDialogBuilder()->buildProgressDialog();
	mProgressDialog->setMessage(String::format("Importing %s...", mFilename));
	mProgressDialog->show();
}

void ImportTask::onPostExecute() {
	mProgressDialog->dismiss();
}

void ImportTask::doInBackground() {
	PluginManager *pm = PluginManager::Instance();
	
	PluginManager::PluginFunctionList &funcs = pm->listPlugins("Import");
	PluginManager::PluginFunctionList::iterator i = funcs.begin();
	
	while (i != funcs.end()) {
		fImportInfo infoCall = (fImportInfo)i->second;
		GC< List<ImportInfo> > info = infoCall();
		
		List<ImportInfo>::iterator ii = info->begin();
		while (ii != info->end()) {
			if ((*ii)->accept(mFilename)) {
        //choose proper DB to import to
        GC<database::Database> db = AppConfig::getInstance()->getWorkingDatabase();
        if (mDBname != NULL) { //user requested other then default DB
          File fFile(mDBname);
          if(fFile->exists()) {
            GCM::util::Log::d("Import", "DB %s exists, using instead of default.", fFile->getName()->c_str());
            
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
              GCM::util::Log::e("Import", "Unable to open selected database %s - no plugin was found.", fFile->getName()->c_str());
          }
        }

				(*ii)->import(mFilename, db, mProgressDialog);
				break;
			}
			ii++;
		}
		
		// Imported.
		if (ii != info->end()) {
			break;
		}
		
		i++;
	}
	
	if (i == funcs.end()) {
		GCM::util::Log::e("Import", "Unable to found import plugin that handles this file type.");
	}
}

