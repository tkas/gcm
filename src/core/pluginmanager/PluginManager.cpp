/**
 * \file plugin_manager.cpp
 *
 * GCM Plugin Manager
 *
 * Implementation of basic platform-independent plugin manager functions.
 */

#include <gcm.h>
#include <string.h>
#include <stdio.h>

#include <pluginmanager/PluginManager.h>
#include <pluginmanager/UnsupportedFunctionException.h>
#include <pluginmanager/NoPluginSelectedException.h>

#include <util/Log.h>

using namespace GCM::pluginmanager;

/**
 * Instance of the PluginManager singleton.
 */
PluginManager GCM_API *PluginManager::instance;

PluginManager GCM_API *PluginManager::Instance() {
	if (!PluginManager::instance) {
		PluginManager::instance = new PluginManager();
	}

	return PluginManager::instance;
}

void GCM_API *PluginManager::getFunction(const char *function, const char *plugin) {
	this->enterCriticalSection();

	// Find if function is even registered.
	FunctionList::iterator i = this->functions.find(function);
	if (i == this->functions.end()) {
		UnsupportedFunctionException exc;
		this->leaveCriticalSection();
		throw exc;
	}

	// Here, we know the function exists.
	PluginFunctionList &func = i->second;

	// If we have preferred plugin, try to use it.
	if (!plugin) {
		PreferredPluginList::iterator p = preferred.find(function);
		if (p != preferred.end()) {
			plugin = p->second.c_str();

		// Here, we don't have plugin selected from preferred plugins,
		// so we need to display selector for this plugin.
		} else {
			// Here we need to select one plugin from (possibly) multiple solutions.
			PluginFunctionList::iterator f = func.begin();

			// We have only one plugin for that function. Don't need to call
			// the selector.
			if (func.size() == 1) {
				plugin = f->first->name->c_str();
			} else {
				// List for the selector
				PluginList list;
				while (f != func.end()) {
					list.push_back(f->first);
					f++;
				}

				if (this->selector) {
					plugin = this->selector->selectPlugin(function, list);
				} else {
					// Don't have plugin selector available, don't know how
					// to select the plugin.

					GCM::util::Log::d("PluginManager", "Don't have plugin selector available. Need selector to select from multiple plugins that supports %s function.", function);

					NoPluginSelectedException exc;
					this->leaveCriticalSection();
					throw exc;
				}
			}
		}
	}

	// If we have plugin specified, try to find if it supports the function.
	if (plugin) {
		PluginFunctionList::iterator f = func.begin();

		while (f != func.end()) {
			if (f->first->name == plugin) {
				break;
			}

			f++;
		}

		if (f != func.end()) {
			this->leaveCriticalSection();
			return f->second;
		} else {
			// This plugin does not support the function.
			UnsupportedFunctionException exc;
			this->leaveCriticalSection();
			throw exc;
		}
	} else {
		// No plugin was selected for this function.
		NoPluginSelectedException exc;
		this->leaveCriticalSection();
		throw exc;
	}
}

PluginManager::PluginFunctionList GCM_API &PluginManager::listPlugins(const char *function) {
	this->enterCriticalSection();
	FunctionList::iterator i = this->functions.find(function);
	if (i != this->functions.end()) {
		this->leaveCriticalSection();
		return i->second;
	}
	this->leaveCriticalSection();

	UnsupportedFunctionException exc;
	throw exc;
}

void GCM_API PluginManager::registerFunction(Plugin *plugin, const char *function, void *callback) {
	this->enterCriticalSection();

	FunctionList::iterator f = this->functions.find(function);

	// No function present currently...
	if (f == this->functions.end()) {
		this->functions[function] = PluginFunctionList();
		f = this->functions.find(function);
	}

	PluginFunctionList &l = f->second;
	l[plugin] = callback;

	GCM::util::Log::d("PluginManager", "Plugin %s registered for function %s.",
		plugin->name->c_str(),
		function
	);

	this->leaveCriticalSection();
}

void GCM_API PluginManager::setPreferred(const char *function, const char *plugin) {
	this->enterCriticalSection();

	this->preferred[function] = plugin;

	this->leaveCriticalSection();
}

GCM_API PluginManager::~PluginManager() {
	this->preferred.clear();

	// Clear the plugins
	PluginList::iterator i = plugins.begin();
	while (i != plugins.end()) {
		delete *i;
		i++;
	}
}

void GCM_API PluginManager::callDestructors() {
	PluginList::iterator i = plugins.begin();
	while (i != plugins.end()) {
		(*i)->callDestructor();
		i++;
	}
}
