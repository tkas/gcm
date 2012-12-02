/*
 * PluginVersion.h
 *
 *  Created on: Feb 21, 2012
 *      Author: Niximor
 */

#ifndef PLUGINVERSION_H_
#define PLUGINVERSION_H_

#include <gcm.h>

#ifdef major
#	undef major
#endif

#ifdef minor
#	undef minor
#endif

namespace GCM {
	namespace pluginmanager {

/**
 * Plugin version information
 */
class GCM_API PluginVersion {
	public:
		PluginVersion(int p_major, int p_minor):
			major(p_major), minor(p_minor) {}

		int major;	/**< Major version number */
		int minor;	/**< Minor version number */
};

	} // namespace pluginmanager
} // namespace GCM

#endif /* PLUGINVERSION_H_ */
