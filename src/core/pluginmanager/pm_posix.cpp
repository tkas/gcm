/**
 * \file
 * GCM Plugin Manager
 *
 * Plugin loading support for POSIX compilant systems.
 */

#include <gcm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <dlfcn.h>

#include <pluginmanager/PluginManager.h>
#include <util/path.h>
#include <util/Log.h>

using namespace GCM::pluginmanager;
using namespace GCM::util;

class PosixPlugin: public Plugin {
	private:
		void *handle;

	public:
		virtual ~PosixPlugin() {
			dlclose(this->handle);
		}

		virtual void callDestructor() {
			PluginManager::fPluginDone destruct = (PluginManager::fPluginDone)dlsym(
				this->handle, "gcm_pluginDone");
			if (destruct) {
				destruct(this);
			}
		}

		PosixPlugin(PluginManager &pm, void *pHandle): Plugin(pm), handle(pHandle) {}
};

void PluginManager::loadPlugins(const char *directory) {
	DIR *dp;
	struct dirent *dirp;

	if ((dp = opendir(directory)) == NULL) {
		GCM::util::Log::e("PluginManager", "Unable to open %s: %s", directory, strerror(errno));
		return;
	}

	path p(directory);

	while ((dirp = readdir(dp)) != NULL) {
		unsigned char d_type = dirp->d_type;

		// Skip hidden files/directories
		if (dirp->d_name[0] == '.') continue;

		// Set current path to p.
		p.add("/");
		p.add(dirp->d_name);

		size_t nl = strlen(dirp->d_name);

		// If entry is directory, recursive descend to this dir.
		if (d_type == DT_DIR) {
			this->loadPlugins(p.c_str());
		} else if (d_type == DT_REG) {
			if (strcmp(".so", dirp->d_name + nl - 3) == 0) {
				Plugin *plugin = this->loadPlugin(p.c_str());
				this->enterCriticalSection();
				this->plugins.push_back(plugin);
				this->leaveCriticalSection();
			}
		}
	}
	closedir(dp);
}

Plugin *PluginManager::loadPlugin(const char *p_path) {
	// Load given plugin.
	void *handle = dlopen(p_path, RTLD_NOW | RTLD_LOCAL);

	if (!handle) {
		GCM::util::Log::e("PluginManager", "Unable to load plugin %s: %s", p_path, dlerror());
		return NULL;
	}

	fPluginInit gcm_pluginInit = (fPluginInit)dlsym(handle, "gcm_pluginInit");

	if (!gcm_pluginInit) {
		GCM::util::Log::e("PluginManager", "Plugin %s does not contain gcm_pluginInit function: %s.", p_path, dlerror());
		dlclose(handle);
		return NULL;
	} else {
		PosixPlugin *plugin = new PosixPlugin(*this, handle);
		path p(p_path);
		plugin->name = p.basename(".so");
		gcm_pluginInit(plugin);
		return plugin;
	}
}
