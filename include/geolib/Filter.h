#ifndef _GCM_FILTER_H
#	define _GCM_FILTER_H

#include <gcm.h>
#include <stl/String.h>
#include <GC.h>

#ifdef LIBGCM
# include <libxml/tree.h>
#else
struct _xmlNode;
#endif

namespace GCM {
	namespace geolib {

class Geocache;
class GeocacheWaypoint;
class GenericWaypoint;

/**
 * Filter settings
 */
class GCM_API Filter: public Object {
	public:
		/**
		 * Base ID of filter to be used by custom user filters.
		 */
		static const int USER;

		virtual int getId() const = 0;
		virtual String getName() const = 0;
		virtual bool matchCache(GC<Geocache> cache) = 0;
		virtual bool matchWaypoint(GC<GeocacheWaypoint> cache) = 0;
		virtual bool matchGenericWaypoint(GC<GenericWaypoint> cache) = 0;

		/**
		 * For filters that does not require any settings, toXml can be
		 * empty. Overwrite this to save filter settings.
		 */
		virtual void toXml(_xmlNode *root) { (void)root; }

		/**
		 * For filters that does not require any settings, fromXml
		 * can be empy. Overwrite this to load filter settings.
		 * @return true if filter has been successfully loaded.
		 */
		virtual bool fromXml(_xmlNode *root) { return true; (void)root; }

		static void saveToFile(GCM::GC<Filter> f, String fileName);
		static GC<Filter> loadFromFile(String fileName);
		static GC<Filter> loadFromXml(_xmlNode *node);
};

	} // namespace geolib
} // namespace GCM

#include <geolib/Geocache.h>
#include <geolib/GeocacheWaypoint.h>
#include <geolib/GenericWaypoint.h>

#endif
