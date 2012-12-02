#include <geolib/GeocacheList.h>

using namespace GCM;
using namespace GCM::geolib;

GC<Geocache> GCM_API GeocacheList::append(GC<Geocache> cache) {
	List<Geocache>::append(cache);
	this->cacheIndex[Geocache::gcIdToInt(cache->getId())] = IndexEntry(cache, --this->end());
	return cache;
}

GC<Geocache> GCM_API GeocacheList::getCache(String gcId) {
	return this->getCache(Geocache::gcIdToInt(gcId));
}

GC<Geocache> GCM_API GeocacheList::getCache(int gcId) {
	std::map<int, IndexEntry >::iterator i = this->cacheIndex.find(gcId);
	if (i != this->cacheIndex.end()) {
		return i->second.cache;
	} else {
		return NULL;
	}
}

bool GeocacheList::contains(GC<Geocache> cache) {
	return this->contains(cache->getRowId());
}

bool GeocacheList::contains(String gcId) {
	return this->contains(Geocache::gcIdToInt(gcId));
}

bool GeocacheList::contains(int gcId) {
	std::map< int, IndexEntry >::iterator i = this->cacheIndex.find(gcId);
	return i != this->cacheIndex.end();
}

void GeocacheList::remove(GC<Geocache> cache) {
	this->remove(cache->getRowId());
}

void GeocacheList::remove(String gcId) {
	this->remove(Geocache::gcIdToInt(gcId));
}

void GeocacheList::remove(int gcId) {
	std::map< int, IndexEntry >::iterator i = this->cacheIndex.find(gcId);
	if (i != this->cacheIndex.end()) {
		this->erase(i->second.iterator);
		this->cacheIndex.erase(i);
	}
}

void GeocacheList::calcDistances(int latE6, int lonE6) {
	List<Geocache>::iterator ci = this->begin();
	while (ci != this->end()) {
		(*ci)->setDistanceFrom(latE6, lonE6);
	
		if ((*ci)->isWaypointsLoaded()) {
			GeocacheWaypointList::iterator wi = (*ci)->getWaypoints()->begin();
			while (wi != (*ci)->getWaypoints()->end()) {
				(*wi)->setDistanceFrom(latE6, lonE6);
			 	wi++;
			}
		}	
	
		ci++;
	}
}

