/**
 * \file
 * GCM Plugin Manager
 *
 * Plugin loading support for POSIX compilant systems.
 */

#include <windows.h>
#include <gcm.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <pluginmanager/PluginManager.h>
#include <util/path.h>
#include <util/Log.h>

using namespace GCM::pluginmanager;
using namespace GCM::util;

class Win32Plugin: public Plugin {
	private:
		HMODULE handle;

	public:
		virtual ~Win32Plugin() {
			FreeLibrary(this->handle);
		}

		virtual void callDestructor() {
			PluginManager::fPluginDone destruct = (PluginManager::fPluginDone)GetProcAddress(
				this->handle, "gcm_pluginDone");

			if (destruct) {
				destruct(this);
			}
		}

		Win32Plugin(PluginManager &pm, HMODULE pHandle): Plugin(pm), handle(pHandle) {}
};

void GCM_API PluginManager::loadPlugins(const char *directory) {
	path p(directory);

	std::string dir(p.c_str());
	dir += "\\*";

	WIN32_FIND_DATA file;
	HANDLE dp = FindFirstFile(dir.c_str(), &file);
	if (dp == INVALID_HANDLE_VALUE) {
		GCM::util::Log::e("PluginManager", "Unable to open %s - error %ld", directory, GetLastError());
		return;
	}

	do {
		if ((file.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) || file.cFileName[0] == '.') continue;

		p.add("/");
		p.add(file.cFileName);

		if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			this->loadPlugins(p.c_str());
		} else {
			size_t nl = strlen(file.cFileName);
			if (strcmp(".dll", file.cFileName + nl - 4) == 0) {
				Plugin *plugin = this->loadPlugin(p.c_str());
				this->enterCriticalSection();
				this->plugins.push_back(plugin);
				this->leaveCriticalSection();
			}
		}
	} while (FindNextFile(dp, &file) != 0);

	FindClose(dp);
}

Plugin GCM_API *PluginManager::loadPlugin(const char *p_path) {
	// Load given plugin.
	HMODULE handle = LoadLibrary(p_path);

	if (!handle) {
		GCM::util::Log::e("PluginManager", "Unable to load plugin %s.", p_path);
		return NULL;
	}

	fPluginInit gcm_pluginInit = (fPluginInit)GetProcAddress(handle, "gcm_pluginInit");

	if (!gcm_pluginInit) {
		GCM::util::Log::e("PluginManager", "Plugin %s does not contain gcm_pluginInit function.", p_path);
		return NULL;
	} else {
		Plugin *plugin = new Win32Plugin(*this, handle);
		path p(p_path);
		plugin->name = p.basename(".dll");
		gcm_pluginInit(plugin);
		return plugin;
	}
}
