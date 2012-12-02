/*
 * PluginSelector.h
 *
 *  Created on: Feb 21, 2012
 *      Author: Niximor
 */

#ifndef PLUGINSELECTOR_H_
#define PLUGINSELECTOR_H_

#include <gcm.h>

#include "PluginManager.h"

namespace GCM {
	namespace pluginmanager {

/**
 * Abstract class for implementing selectors for plugins.
 */
class GCM_API PluginSelector {
	public:
		/**
		 * Display the selector for given function and return name of
		 * selected plugin that implements this function.
		 * @param function Function that is requested
		 * @param plugins List of plugins that implements this function.
		 */
		virtual const char *selectPlugin(const char *function, PluginManager::PluginList &plugins) = 0;

		virtual ~PluginSelector() {}
};

	}
}

#endif /* PLUGINSELECTOR_H_ */
