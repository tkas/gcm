/*
 * Plugin.h
 *
 *  Created on: Feb 21, 2012
 *      Author: Niximor
 */

#ifndef PLUGIN_H_
#define PLUGIN_H_

#include <gcm.h>
#include "PluginVersion.h"
#include "PluginManager.h"
#include <stl/String.h>

namespace GCM {
	namespace pluginmanager {

class PluginManager;

/**
 * One plugin information.
 */
class GCM_API Plugin {
	public:
		Plugin(PluginManager &p_manager):
			name(""), author(""), version(1, 0), manager(p_manager) {}

		void *handle;				/**< Handle to the plugin library */
		String id;					/**< Plugin ID */
		String name;				/**< Name of the plugin */
		String author;				/**< Plugin author */
		PluginVersion version;		/**< Plugin version */
		PluginManager &manager;		/**< Plugin manager instance */

		/**
		 * Comparator operator for char type.
		 */
		bool operator==(const char *) const;
		bool operator<(const Plugin &) const;

		virtual ~Plugin();
		virtual void callDestructor() = 0;
};

	} // namespace pluginmanager
} // namespace GCM


#endif /* PLUGIN_H_ */
