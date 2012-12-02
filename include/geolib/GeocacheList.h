/*
 * CacheList.h
 *
 *  Created on: Feb 21, 2012
 *      Author: Niximor
 */

#ifndef CACHELIST_H_
#define CACHELIST_H_

#include "Geocache.h"
#include <GC.h>
#include <stl/List.h>
#include <map>

namespace GCM {
	namespace geolib {

class GCM_API GeocacheList: public List<Geocache> {
	protected:
		struct IndexEntry {
			GC<Geocache> cache;
			List<Geocache>::iterator iterator;

			IndexEntry(GC<Geocache> cache, List<Geocache>::iterator i):
				cache(cache), iterator(i) {}
			IndexEntry():
				cache(NULL), iterator() {}
		};

		std::map< int, IndexEntry > cacheIndex;

	public:
		GC<Geocache> append(GC<Geocache> cache);
		GC<Geocache> getCache(String gcId);
		GC<Geocache> getCache(int gcId);
		bool contains(GC<Geocache> cache);
		bool contains(String gcId);
		bool contains(int gcId);

		void remove(GC<Geocache> cache);
		void remove(String gcId);
		void remove(int gcId);
		
		/**
		 * Calculate distances to every cache in the list (including
		 * additional waypoints from specified coordinates.
		 */
		void calcDistances(int latE6, int lonE6);
};

	}
}


#endif /* CACHELIST_H_ */
