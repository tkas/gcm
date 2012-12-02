/**
 * \file
 * Implementation of the libgcm.
 *
 * @version 1.0
 * @author Michal Kuchta
 */

#include <gcm.h>
#include <stdio.h>
#include <stdlib.h>

#include <pluginmanager/PluginManager.h>
#include <pluginmanager/UnsupportedFunctionException.h>
#include <pluginmanager/NoPluginSelectedException.h>
#include <ui/UI.h>
#include <ui.h>
#include <util/Log.h>
#include <util/File.h>
#include <GC.h>
#include <Thread.h>
#include <database/Database.h>
#include <config/AppConfig.h>

#include <errno.h>
#include <string.h>

#ifndef _WIN32
# include <unistd.h>
# include <sys/types.h>
# include <pwd.h>
# include <limits.h>
#else
# include <windows.h>
# include <shlobj.h>
#endif

#include <time.h>

using namespace GCM::pluginmanager;
using namespace GCM::ui;
using namespace GCM::database;
using namespace GCM::config;
using namespace GCM::io;
using namespace GCM;

PluginManager *pluginManager;

#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _BSD_SOURCE || _SVID_SOURCE || _POSIX_SOURCE
#else
# ifndef _WIN32
struct tm *localtime_r(const time_t *timep, struct tm *result) {
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_lock(&mutex);

	struct tm *global = localtime(timep);
	memcpy(result, global, sizeof(struct tm));

	pthread_mutex_unlock(&mutex);

	return result;
}

struct tm *gmtime_r(const time_t *timep, struct tm *result) {
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_lock(&mutex);

	struct tm *global = gmtime(timep);
	memcpy(result, global, sizeof(struct tm));

	pthread_mutex_unlock(&mutex);

	return result;
}
# else
# endif
#endif

#ifdef _WIN32
# define PATH_MAX MAX_PATH

char GCM_API *getcwd(char *buffer, size_t size) {
	if (GetCurrentDirectory(size, buffer)) {
		return buffer;
	} else {
		return NULL;
	}
}

int GCM_API chdir(const char *buffer) {
	if (SetCurrentDirectory(buffer)) {
		return 0;
	} else {
		return -1;
	}
}

#endif

const char *gcm_version() {
	return GCM_VERSION;
}

class LoadingTask: public BackgroundTask {
	private:
		ptrDialog mProgressDialog;
		ptrUI mUI;

	public:
		LoadingTask(ptrDialog progressDialog, ptrUI ui): BackgroundTask(), mProgressDialog(progressDialog), mUI(ui) {}

		virtual void onPreExecute() {
			mProgressDialog->show();
		}

		virtual void onPostExecute() {
			mProgressDialog->dismiss();
		}

		virtual void doInBackground() {
			mProgressDialog->setMessage("Loading settings...");
			AppConfig *cfg = AppConfig::getInstance();

			// The settings are searched in the following order:
			// 1) gcm.cfg in current working directory.
			// 2) ~/.gcm/gcm.cfg (or equivalent on windows)
			// 3) /etc/gcm.cfg (or current application directory/gcm.cfg)
			// First existing file wins.

			// Search for boot.ini in current working directory.
			File configFile("gcm.cfg");
			if (!configFile->exists()) {
				// No config in current directory, try to search in user's home.

#ifndef _WIN32
				struct passwd pw;
				struct passwd *ppw;
				long size = sysconf(_SC_GETPW_R_SIZE_MAX);
				char *buffer = (char *)malloc(size);
				if (getpwuid_r(getuid(), &pw, buffer, size, &ppw) == 0) {
					configFile->append(pw.pw_dir);
					configFile->append(".gcm/gcm.cfg");
				} else {
					GCM::util::Log::d("GCM", "Unable to obtain user's home directory.");
				}
				free(buffer);
#else
				wchar_t buffer[MAX_PATH];
				if (SHGetFolderPathW(NULL,
						CSIDL_APPDATA,
						NULL,
						0,
						buffer) == S_OK) {

					char mbuffer[MAX_PATH];
					size_t num;
					wcstombs_s(&num, mbuffer, MAX_PATH, buffer, MAX_PATH * sizeof(wchar_t));

					configFile->append(mbuffer);
					configFile->append("GCM/gcm.cfg");
				}
#endif

				if (!configFile->exists()) {
#ifdef _WIN32
					// Search for boot.ini in application executable directory.
					char result[MAX_PATH];
					if (GetModuleFileName(NULL, result, MAX_PATH) > 0) {
						File exec(result);
						configFile->append(exec->getParent());
						configFile->append("gcm.cfg");
					}
#else
					// Search for boot.ini in /etc.
					configFile->append("/etc/gcm.cfg");
#endif
				}
			}

			// Configuration has been found.
			if (configFile->exists()) {
				String cfgFile = configFile->toString();
				GCM::util::Log::d("GCM", "Loading configuration file %s", cfgFile->c_str());
				cfg->load(cfgFile->c_str());
			}

			// Try to search configuration in user's home directory.

			// Open the data directory.
			String datadir = cfg->getDataDirectory();
			GCM::util::Log::d("GCM", "Data directory is %s.", datadir->c_str());

			char buffer[PATH_MAX];
			if (getcwd(buffer, PATH_MAX)) {
				cfg->setWorkingDirectory(buffer);
			} else {
				GCM::util::Log::e("GCM", "Unable to obtain current working directory (%s).", strerror(errno));
				cfg->setWorkingDirectory("/");
			}

			if (chdir(datadir->c_str()) != 0) {
				GCM::util::Log::e("GCM", "Unable to switch to data directory: %s", strerror(errno));
			} else {
				GCM::util::Log::d("GCM", "Successfully changed into data directory.");
			}

			File test(".");
			GCM::io::FileList testlist = test->listFiles();
			GCM::io::FileList::iterator ti = testlist->begin();
			while (ti !=testlist->end()) {
				GCM::util::Log::d("GCM", "Datadir contains %s", (*ti)->toString()->c_str());
				ti++;
			}

			// Try to load configuration specified in the data directory.
			configFile = File("gcm.cfg");
			if (configFile->exists()) {
				String cfgFile = configFile->toString();
				GCM::util::Log::d("GCM", "Loading configuration file %s", cfgFile->c_str());
				cfg->load(cfgFile->c_str());
			} else {
				// Set file name to use when quiting to save the configuration.
				cfg->setFileName(configFile->getAbsolutePath()->c_str());
			}
			
			cfg->loadOtherSettings();

			mProgressDialog->setMessage("Opening database...");

			try {
				PluginManager::PluginFunctionList &plugins = pluginManager->listPlugins("Database");
				PluginManager::PluginFunctionList::iterator i = plugins.begin();
				
				const char *dbid = cfg->getString("database_format", "gcm");
				const char *dbname = cfg->getString("database_file", "database.db3");
				
				while (i != plugins.end()) {
					fDatabase dbcall = (fDatabase)i->second;
					GC< List<DatabaseInfo> > info = dbcall();
					
					List<DatabaseInfo>::iterator ii = info->begin();
					while (ii != info->end()) {
						try {
							if ((*ii)->id->equals(dbid)) {
								GC<Database> db = (*ii)->open(dbname);
								AppConfig::getInstance()->setWorkingDatabase(db);
								break;
							}
						} catch (GCM::database::DatabaseLoadFailed &) {
						}
						
						ii++;
					}
					
					if (ii != info->end()) {
						break;
					}
					
					i++;
				}
				
				if (i == plugins.end()) {
					GCM::util::Log::e("GCM", "Unable to find database plugin to open database.");
				}
				
				
			} catch (GCM::pluginmanager::NoPluginSelectedException e) {
				GCM::util::Log::e("GCM", "Unable to find any database plugin. Aborting.");
				this->mUI->quitMainLoop();
			}
		}
};

int GCM_API gcmcore_run(int argc, char *argv[]) {
	pluginManager = PluginManager::Instance();

	// Load plugins
	pluginManager->loadPlugins(PLUGINS_DIR);

	// Give control to the UI itself.
	try {
		fStartUI startUI = (fStartUI)pluginManager->getFunction("UI");
		ptrUI ui = startUI(argc, argv);

		// Start the initial loading.
		ui->addTask(new LoadingTask(
			ui->getDialogBuilder()->buildStartupDialog(),
			ui
		));

		// Start the main UI
		ui->start();

		GCM::util::Log::d("GCM", "Begining shutdown.");
	} catch (UnsupportedFunctionException &e) {
		GCM::util::Log::e("GCM", "Unable to initialize interface. No plugin was found.");
	} catch (NoPluginSelectedException &e) {
		GCM::util::Log::e("GCM", "Unable to initialize interface. No plugin was found.");
	}

	// Close the plugin manager
	pluginManager->callDestructors();

	AppConfig::finalize();

	// Free up plugins address space after finalizing of configuration. That
	// prevents memory auto-collection after unloading the plugin, and does not
	// cause SIGSEGV when trying to free database.
	delete pluginManager;

	return 0;
}
