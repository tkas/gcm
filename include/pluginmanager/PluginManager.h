/**
 * \file plugin_manager.h
 *
 * GCM Plugin Manager
 *
 * The basic core of the plugin system.
 *
 * @author Michal Kuchta
 * @version 1.0
 */

#ifndef _PLUGIN_MANAGER_H
#define _PLUGIN_MANAGER_H

#include <map>
#include <list>
#include <string>
#include <exception>

#ifdef major
# undef major
#endif

#ifdef minor
# undef minor
#endif

#include <gcm.h>
#include <Object.h>

namespace GCM {
	namespace pluginmanager {

class Plugin;
class PluginSelector;

/**
 * Plugin manager
 */
class GCM_API PluginManager: public Object {
	public:
		/**
		 * List of plugins
		 */
		typedef std::list<Plugin *> PluginList;

		/**
		 * List of function callbacks for given plugin.
		 */
		typedef std::map<Plugin *, void *> PluginFunctionList;

		/**
		 * List of functions with their registered callbacks.
		 */
		typedef std::map<std::string, PluginFunctionList> FunctionList;

		/**
		 * List of plugins that will be used primary for given
		 * function. Key is function name, value is plugin name.
		 */
		typedef std::map<std::string, std::string> PreferredPluginList;

		/**
		 * Function prototype for gcm_pluginInit function.
		 */
		typedef void (*fPluginInit)(Plugin *plugin);
		typedef void (*fPluginDone)(Plugin *plugin);

	private:
		static PluginManager *instance;

		/**
		 * Selector used when there are two or more implementations
		 * of given function available.
		 */
		PluginSelector *selector;

		/**
		 * List of registered functions.
		 */
		FunctionList functions;

		/**
		 * List of preferred plugins for certain functions.
		 */
		PreferredPluginList preferred;

		/**
		 * List of loaded plugins.
		 */
		PluginList plugins;

	public:
		static PluginManager *Instance();

		/**
		 * The function returns callback to function which was requested
		 * from the plugin.
		 * @param function Function name to be exported.
		 * @param plugin Preffered plugin to use. If not specified,
		 * plugin selector will be used to determine which plugin to
		 * use.
		 */
		void *getFunction(const char *function, const char *plugin = NULL);
		
		/**
		 * Returns list of plugins providing the specified function.
		 * @param function Function name
		 * @throws UnsupportedFunctionException when function is not provided by
		 *   any plugin.
		 * @return Plugin function list specifying list of functions and callbacks.
		 */
		PluginFunctionList &listPlugins(const char *function);

		/**
		 * Called by plugin initializer to register plugin functions.
		 * @param function Function name
		 * @param callbakc Pointer to function implementation.
		 * Must be cast to void * to support multiple function
		 * prototypes.
		 */
		void registerFunction(Plugin *plugin, const char *function, void *callback);

		/**
		 * Set preferred plugin for given function.
		 */
		void setPreferred(const char *function, const char *plugin);

		/**
		 * Function scans given directory (including all subdirectories),
		 * and lists plugins to load. For each plugin, it calls loadPlugin()
		 * method.
		 * This method is implemented in pm_posix.cpp or pm_win32.cpp acording
		 * to target platform.
		 */
		void loadPlugins(const char *directory);

		/**
		 * Method that loads one plugin into the system.
		 * This method is implemented in pm_posix.cpp or pm_win32.cpp acording
		 * to target platform.
		 */
		Plugin *loadPlugin(const char *path);

		virtual ~PluginManager();
		void callDestructors();
};

	} // namespace pluginmanager
} // namespace GCM

#include "Plugin.h"
#include "PluginSelector.h"

#endif // plugin_manager_h

