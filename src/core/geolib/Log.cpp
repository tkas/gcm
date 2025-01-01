#include <geolib/Log.h>
#include <geolib/Geocache.h>
#include <util/Log.h>

using namespace GCM::geolib;
using namespace GCM;

GC<Geocache> GCM_API Log::getCache() {
	return this->cache;
}

void GCM_API Log::setCache(GC<Geocache> cache) {
	this->cache = cache;
}

int GCM_API Log::getId() {
	return this->id;
}

void GCM_API Log::setId(int id) {
	this->id = id;
}

time_t GCM_API Log::getTime() {
	return this->time;
}

void GCM_API Log::setTime(time_t time) {
	this->time = time;
}

Log::Type GCM_API Log::getType() {
	return this->type;
}

void GCM_API Log::setType(Log::Type type) {
	this->type = type;
}

int GCM_API Log::getFinderId() {
	return this->finderId;
}

void GCM_API Log::setFinderId(int finderId) {
	this->finderId = finderId;
}

String GCM_API Log::getFinder() {
	return this->finder;
}

void GCM_API Log::setFinder(String finder) {
	this->finder = finder;
}

String GCM_API Log::getText() {
	return this->text;
}

void GCM_API Log::setText(String text) {
	this->text = text;
}

bool GCM_API Log::isTextEncoded() {
	return this->textEncoded;
}

void GCM_API Log::setTextEncoded(bool encoded) {
	this->textEncoded = encoded;
}

String GCM_API Log::typeToGpxString(Type type) {
	switch (type) {
		case T_ANNOUNCEMENT: return "Announcement";
		case T_ARCHIVE: return "Archived";
		case T_ATTENDED: return "Attended";
		case T_DISABLE: return "Temporarily Disable Listing";
		case T_DNF: return "Didn't find it";
		case T_ENABLE: return "Enable Listing";
		case T_FOUND_IT: return "Found it";
		case T_NEEDS_ARCHIVED: return "Needs Archived";
		case T_NEEDS_MAINTENANCE: return "Needs Maintenance";
		case T_OWNER_MAINTENANCE: return "Owner Maintenance";
		case T_PUBLISH: return "Publish Listing";
		case T_RETRACT: return "Retract Listing";
		case T_REVIEWER_NOTE: return "Post Reviewer Note";
		case T_UNARCHIVE: return "Unarchive";
		case T_UPDATE_COORDINATES: return "Update Coordinates";
		case T_WEBCAM_PHOTO_TAKEN: return "Webcam Photo Taken";
		case T_WILL_ATTEND: return "Will Attend";
		case T_WRITE_NOTE: return "Write note";
		case T_SUBMIT: return "Submit for Review";
		case T_UNKNOWN: return "Unknown";
		default: return "Unknown log type.";
	}
}

Log::Type GCM_API Log::gpxStringToType(String gpxString) {
	if (gpxString->equals("Found it")) {
		return T_FOUND_IT;
	} else if (gpxString->equals("Unknown")) {
		return T_UNKNOWN;
	} else if (gpxString->equals("Write note")) {
		return T_WRITE_NOTE;
	} else if (gpxString->equals("Didn't find it")) {
		return T_DNF;
	} else if (gpxString->equals("Temporarily Disable Listing")) {
		return T_DISABLE;
	} else if (gpxString->equals("Owner Maintenance")) {
		return T_OWNER_MAINTENANCE;
	} else if (gpxString->equals("Needs Maintenance")) {
		return T_NEEDS_MAINTENANCE;
	} else if (gpxString->equals("Enable Listing")) {
		return T_ENABLE;
	} else if (gpxString->equals("Attended")) {
		return T_ATTENDED;
	} else if (gpxString->equals("Archive/Unarchive")) {
		return T_ARCHIVE;
	} else if (gpxString->equals("Update Coordinates")) {
		return T_UPDATE_COORDINATES;
	} else if (gpxString->equals("Post Reviewer Note")) {
		return T_REVIEWER_NOTE;
	} else if (gpxString->equals("Needs Archived")) {
		return T_NEEDS_ARCHIVED;
	} else if (gpxString->equals("Will Attend")) {
		return T_WILL_ATTEND;
	} else if (gpxString->equals("Archive")) {
		return T_ARCHIVE;
	} else if (gpxString->equals("Archived")) {
		return T_ARCHIVE;
	} else if (gpxString->equals("Publish Listing")) {
		return T_PUBLISH;
	} else if (gpxString->equals("Announcement")) {
		return T_ANNOUNCEMENT;
	} else if (gpxString->equals("Webcam Photo Taken")) {
		return T_WEBCAM_PHOTO_TAKEN;
	} else if (gpxString->equals("Unarchive")) {
		return T_UNARCHIVE;
	} else if (gpxString->equals("Write Note")) {
		return T_WRITE_NOTE;
	} else if (gpxString->equals("Retract Listing")) {
		return T_RETRACT;
	} else if (gpxString->equals("Submit for Review")) {
		return T_SUBMIT;
	} else {
		GCM::util::Log::e("Log", "Unknown log type '%s'.\n\n", gpxString->c_str());
		return T_WRITE_NOTE;
	}
}

Log::Type GCM_API Log::gsIdToType(int id) {
	switch (id) {
		case 1: return T_UNARCHIVE;
		case 2: return T_FOUND_IT;
		case 3: return T_DNF;
		case 4: return T_WRITE_NOTE;
		case 5: return T_ARCHIVE;
		case 6: return T_ARCHIVE;
		case 7: return T_NEEDS_ARCHIVED;
		case 8: return T_WRITE_NOTE; // Mark destroyed benchmark
		case 9: return T_WILL_ATTEND;
		case 10: return T_ATTENDED;
		case 11: return T_WEBCAM_PHOTO_TAKEN;
		case 12: return T_UNARCHIVE;
		case 18: return T_REVIEWER_NOTE;
		case 22: return T_DISABLE;
		case 23: return T_ENABLE;
		case 24: return T_PUBLISH;
		case 25: return T_RETRACT;
		case 45: return T_NEEDS_MAINTENANCE;
		case 46: return T_OWNER_MAINTENANCE;
		case 47: return T_UPDATE_COORDINATES;
		case 54: return T_REVIEWER_NOTE;
		case 68: return T_REVIEWER_NOTE;
		case 74: return T_ANNOUNCEMENT;
		default: return T_WRITE_NOTE;
	}
}
