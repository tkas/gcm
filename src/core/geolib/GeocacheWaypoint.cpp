#include <geolib/GeocacheWaypoint.h>
#include <geolib/Geocache.h>
#include <GC.h>
#include <stl/String.h>
#include <util/Log.h>
#include <database/Database.h>

using namespace GCM::geolib;
using namespace GCM;

GCM_API GeocacheWaypoint::GeocacheWaypoint(): GenericWaypoint(), wptType(GeocacheWaypoint::T_REFERENCE), userFlag(false), userComment(), corrected(false) {
}

GCM_API GeocacheWaypoint::GeocacheWaypoint(const GeocacheWaypoint &wpt):
	GenericWaypoint(wpt), cache(wpt.cache), wptType(wpt.wptType), userFlag(wpt.userFlag), userComment(wpt.userComment), corrected(false) {
}

GCM_API GeocacheWaypoint::GeocacheWaypoint(const GenericWaypoint &wpt):
	GenericWaypoint(wpt), cache(), wptType(GeocacheWaypoint::T_REFERENCE), userFlag(false), userComment(), corrected(false) {
}

WeakGC<Geocache> GCM_API GeocacheWaypoint::getCache() {
	return this->cache;
}

int GCM_API GeocacheWaypoint::getRefCacheId() {
	if (this->getCache()) {
		return this->getCache()->getRowId();
	} else {
		return Geocache::gcIdToInt(this->getId());
	}
}

void GCM_API GeocacheWaypoint::setCache(GC<Geocache> cache) {
	this->cache = cache;
}

int GCM_API GeocacheWaypoint::getRowId() {
	return this->rowId;
}

void GCM_API GeocacheWaypoint::setRowId(int rowid) {
	this->rowId = rowid;
}

GeocacheWaypoint::Type GCM_API GeocacheWaypoint::getWptType() {
	return this->wptType;
}

void GCM_API GeocacheWaypoint::setWptType(GeocacheWaypoint::Type type) {
	this->wptType = type;
}

bool GCM_API GeocacheWaypoint::getUserFlag() {
	return this->userFlag;
}

void GCM_API GeocacheWaypoint::setUserFlag(bool flag) {
	this->userFlag = flag;
}

String GCM_API GeocacheWaypoint::getUserComment() {
	return this->userComment;
}

time_t GCM_API GeocacheWaypoint::getDtLastUpdate() {
	return this->dtUpdate;
}

void GCM_API GeocacheWaypoint::setDtLastUpdate(time_t tm) {
	this->dtUpdate = tm;
}

void GCM_API GeocacheWaypoint::setUserComment(String comment) {
	this->userComment = comment;
}

void GCM_API GeocacheWaypoint::setUseAsCorrected(bool use) {
	this->corrected = use;
}

bool GCM_API GeocacheWaypoint::useAsCorrected() {
	return this->corrected;
}

GeocacheWaypoint::Type GCM_API GeocacheWaypoint::gpxStringToType(String val) {
	if (
		val->equals("Waypoint|Final Location") ||
		val->equals("Final Location")
	) {
		return T_FINAL;
	} else if (
		val->equals("Waypoint|Question to Answer") ||
		val->equals("Question to Answer")
	) {
		return T_QTA;
	} else if (
		val->equals("Waypoint|Reference Point") ||
		val->equals("Reference Point")
	) {
		return T_REFERENCE;
	} else if (
		val->equals("Waypoint|Trailhead") ||
		val->equals("Trailhead")
	) {
		return T_TRAILHEAD;
	} else if (
		val->equals("Waypoint|Parking Area") ||
		val->equals("Parking Area")
	) {
		return T_PARKING;
	} else if (
		val->equals("Waypoint|Stages of a Multicache") ||
		val->equals("Stages of a Multicache")
	) {
		return T_SOM;
	} else {
		GCM::util::Log::d("GeocacheWaypoint",
			"Unknown waypoint type %s. Fallback to reference point.", val->c_str()
		);
		return T_REFERENCE; // Fallback
	}
}

String GCM_API GeocacheWaypoint::typeToGpxString(GeocacheWaypoint::Type type) {
	switch (type) {
		case T_FINAL: return "Final Location";
		case T_QTA: return "Question to Answer";
		case T_REFERENCE: return "Reference Point";
		case T_PARKING: return "Parking Area";
		case T_TRAILHEAD: return "Trailhead";
		case T_SOM: return "Stages of a Multicache";
		default: return "Unknown waypoint type";
	}
}
