#include <gcm.h>
#include <geolib/Geocache.h>
#include <stl/String.h>
#include <string.h>
#include <math.h>
#include <database/Database.h>
#include <config/AppConfig.h>

using namespace GCM::geolib;
using namespace GCM::config;
using namespace GCM;

const char *Geocache::ALPHABET = "0123456789ABCDEFGHJKMNPQRTVWXYZ";

GCM_API Geocache::Geocache(): GenericWaypoint(), mapLoaded(false), loaded(false),
	longLoaded(false), waypointsLoaded(false), attributesLoaded(false),
	tagsLoaded(false), logsLoaded(false), rowid(0),
	cacheType(Geocache::T_TRADITIONAL), owner(), ownerId(0), placedBy(),
	difficulty(0), terrain(0), size(Geocache::S_OTHER),
	status(Geocache::ST_AVAILABLE), country(), state(), favPoints(0), lockFlags(0),
	correctedCoords(false), dtLastUpdate(0), dtFound(0), own(false), initialLatitudeE6(0),
	initialLongitudeE6(0), shortDescription(), shortIsHTML(false),
	longDescription(), longIsHTML(false), hint(),
	waypoints(new GeocacheWaypointList()), logs(new LogList()), attributes(new AttributeSet()) {
}

GCM_API Geocache::Geocache(const GenericWaypoint &wpt): GenericWaypoint(wpt),
	mapLoaded(false), loaded(false), longLoaded(false), waypointsLoaded(false),
	attributesLoaded(false), tagsLoaded(false), logsLoaded(false), rowid(0),
	cacheType(Geocache::T_TRADITIONAL), owner(), ownerId(0), placedBy(),
	difficulty(0), terrain(0), size(Geocache::S_OTHER),
	status(Geocache::ST_AVAILABLE), country(), state(), favPoints(0), lockFlags(0),
	correctedCoords(false), dtLastUpdate(0), dtFound(0), own(false), initialLatitudeE6(0),
	initialLongitudeE6(0), shortDescription(), shortIsHTML(false),
	longDescription(), longIsHTML(false), hint(),
	waypoints(new GeocacheWaypointList()), logs(new LogList()), attributes(new AttributeSet()) {

	// Calculate row ID from the GC id.
	this->setRowId(Geocache::gcIdToInt(this->getId()->c_str()));
	this->setLatitudeE6(this->getLatitudeE6());
	this->setLongitudeE6(this->getLongitudeE6());
}

GCM_API Geocache::~Geocache() {
}

void GCM_API Geocache::setLatitudeE6(int latE6) {
	if (!this->hasCorrectedCoords()) {
		this->initialLatitudeE6 = latE6;
	}

	GenericWaypoint::setLatitudeE6(latE6);
}

void GCM_API Geocache::setLongitudeE6(int lonE6) {
	if (!this->hasCorrectedCoords()) {
		this->initialLongitudeE6 = lonE6;
	}

	GenericWaypoint::setLongitudeE6(lonE6);
}

int GCM_API Geocache::getRowId() {
	return this->rowid;
}

void GCM_API Geocache::setRowId(int rowid) {
	this->rowid = rowid;
}

ModifiableEnum<Geocache::Type> GCM_API Geocache::getCacheType() {
	return this->cacheType;
}

void GCM_API Geocache::setCacheType(Geocache::Type type) {
	this->cacheType = type;
}

const Modifiable<String> GCM_API Geocache::getOwner() {
	return this->owner;
}

void GCM_API Geocache::setOwner(String owner) {
	this->owner = owner;
}

Modifiable<int> GCM_API Geocache::getOwnerId() {
	return this->ownerId;
}

void GCM_API Geocache::setOwnerId(int ownerId) {
	this->ownerId = ownerId;
}

const Modifiable<String> GCM_API Geocache::getPlacedBy() {
	return this->placedBy;
}

void GCM_API Geocache::setPlacedBy(String placedBy) {
	this->placedBy = placedBy;
}

Modifiable<int> GCM_API Geocache::getDifficulty() {
	return this->difficulty;
}

void GCM_API Geocache::setDifficulty(int difficulty) {
	this->difficulty = difficulty;
}

Modifiable<int> GCM_API Geocache::getTerrain() {
	return this->terrain;
}

void GCM_API Geocache::setTerrain(int terrain) {
	this->terrain = terrain;
}

ModifiableEnum<Geocache::Size> GCM_API Geocache::getSize() {
	return this->size;
}

void GCM_API Geocache::setSize(Geocache::Size size) {
	this->size = size;
}

ModifiableEnum<Geocache::Status> GCM_API Geocache::getCacheStatus() {
	return this->status;
}

bool GCM_API Geocache::isAvailable() {
	return this->status == ST_AVAILABLE;
}

bool GCM_API Geocache::isDisabled() {
	return this->status == ST_DISABLED;
}

bool GCM_API Geocache::isArchived() {
	return this->status == ST_ARCHIVED;
}

void GCM_API Geocache::setCacheStatus(Status status) {
	this->status = status;
}

const Modifiable<String> GCM_API Geocache::getCountry() {
	return this->country;
}

void GCM_API Geocache::setCountry(String country) {
	this->country = country;
}

const Modifiable<String> GCM_API Geocache::getState() {
	return this->state;
}

void GCM_API Geocache::setState(String state) {
	this->state = state;
}

Modifiable<int> GCM_API Geocache::getFavPoints() {
	return this->favPoints;
}

void GCM_API Geocache::setFavPoints(int favPoints) {
	this->favPoints = favPoints;
}

Modifiable<int64_t> GCM_API Geocache::getDtLastUpdate() {
	return this->dtLastUpdate;
}

bool GCM_API Geocache::isFound() {
	return this->dtFound > (int64_t)0;
}

Modifiable<int64_t> GCM_API Geocache::getDtFound() {
	return this->dtFound;
}

Modifiable<bool> GCM_API Geocache::isOwn() {
	return this->own;
}

void GCM_API Geocache::setOwn(bool own) {
	this->own = own;
}

void GCM_API Geocache::setDtLastUpdate(int64_t time) {
	this->dtLastUpdate = time;
}

void GCM_API Geocache::setDtFound(int64_t time) {
	this->dtFound = time;
}

Modifiable<String> GCM_API Geocache::getShortDescription() {
	return this->shortDescription;
}

void GCM_API Geocache::setShortDescription(String text) {
	this->shortDescription = text;
}

Modifiable<String> GCM_API Geocache::getLongDescription() {
	return this->longDescription;
}

void GCM_API Geocache::setLongDescription(String text) {
	this->longDescription = text;
}

Modifiable<bool> GCM_API Geocache::getSDIsHTML() {
	return this->shortIsHTML;
}

void GCM_API Geocache::setSDIsHTML(bool isHTML) {
	this->shortIsHTML = isHTML;
}

Modifiable<bool> GCM_API Geocache::getLDIsHTML() {
	return this->longIsHTML;
}

void GCM_API Geocache::setLDIsHTML(bool isHTML) {
	this->longIsHTML = isHTML;
}

Modifiable<String> GCM_API Geocache::getHint() {
	return this->hint;
}

void GCM_API Geocache::setHint(String hint) {
	this->hint = hint;
}

GC<LogList> GCM_API Geocache::getLogs() {
	return this->logs;
}

GC<AttributeSet> GCM_API Geocache::getAttributes() {
	return this->attributes;
}

bool GCM_API Geocache::isMapLoaded() {
	return this->mapLoaded;
}

bool GCM_API Geocache::isLoaded() {
	return this->loaded;
}

bool GCM_API Geocache::isLongLoaded() {
	return this->longLoaded;
}

bool GCM_API Geocache::isWaypointsLoaded() {
	return this->waypointsLoaded;
}

bool GCM_API Geocache::isAttributesLoaded() {
	return this->attributesLoaded;
}

bool GCM_API Geocache::isTagsLoaded() {
	return this->tagsLoaded;
}

bool GCM_API Geocache::isLogsLoaded() {
	return this->logsLoaded;
}

void GCM_API Geocache::setMapLoaded(bool isLoaded) {
	this->mapLoaded = isLoaded;
}

void GCM_API Geocache::setLoaded(bool isLoaded) {
	this->loaded = isLoaded;
}

void GCM_API Geocache::setLongLoaded(bool isLoaded) {
	this->longLoaded = isLoaded;

	// Clear data if unloaded.
	if (!isLoaded) {
		this->setHint("");
		this->setLongDescription("");
		this->setShortDescription("");
	}
}

void GCM_API Geocache::setWaypointsLoaded(bool isLoaded) {
	this->waypointsLoaded = isLoaded;

	if (!isLoaded && this->waypoints) {
		this->waypoints->clear();
	}
}

void GCM_API Geocache::setAttributesLoaded(bool isLoaded) {
	this->attributesLoaded = isLoaded;

	if (!isLoaded && this->attributes) {
		this->attributes->clear();
	}
}

void GCM_API Geocache::setTagsLoaded(bool isLoaded) {
	this->tagsLoaded = isLoaded;
}

void GCM_API Geocache::setLogsLoaded(bool isLoaded) {
	this->logsLoaded = isLoaded;

	if (!isLoaded && this->logs) {
		this->logs->clear();
	}
}

Modifiable<int> GCM_API Geocache::getLockFlags() {
	return this->lockFlags;
}

bool GCM_API Geocache::isCoordsLocked() {
	return (this->lockFlags & LOCK_COORDS) != 0;
}

bool GCM_API Geocache::isBasicLocked() {
	return (this->lockFlags & LOCK_BASIC) != 0;
}

bool GCM_API Geocache::isInfoLocked() {
	return (this->lockFlags & LOCK_INFO) != 0;
}

bool GCM_API Geocache::isListingLocked() {
	return (this->lockFlags & LOCK_LISTING) != 0;
}

void GCM_API Geocache::setLockFlags(int flags) {
	this->lockFlags = flags;
}

void GCM_API Geocache::setCoordsLocked(bool locked) {
	if (locked) {
		this->lockFlags.get() |= LOCK_COORDS;
	} else {
		this->lockFlags.get() &= ~LOCK_COORDS;
	}
}

void GCM_API Geocache::setBasicLocked(bool locked) {
	if (locked) {
		this->lockFlags.get() |= LOCK_BASIC;
	} else {
		this->lockFlags.get() &= ~LOCK_BASIC;
	}
}

void GCM_API Geocache::setInfoLocked(bool locked) {
	if (locked) {
		this->lockFlags.get() |= LOCK_INFO;
	} else {
		this->lockFlags.get() &= ~LOCK_INFO;
	}
}

void GCM_API Geocache::setListingLocked(bool locked) {
	if (locked) {
		this->lockFlags.get() |= LOCK_LISTING;
	} else {
		this->lockFlags.get() &= ~LOCK_LISTING;
	}
}

void GCM_API Geocache::setLockAll(bool locked) {
	if (locked) {
		this->lockFlags.get() |= LOCK_ALL;
	} else {
		this->lockFlags.get() &= ~LOCK_ALL;
	}
}

Modifiable<bool> GCM_API Geocache::hasCorrectedCoords() {
	return this->correctedCoords;
}

void GCM_API Geocache::setHasCorrectedCoords(bool corrected) {
	this->correctedCoords = corrected;

	if (!corrected) {
		this->setLatitudeE6(this->getInitialLatitudeE6());
		this->setLongitudeE6(this->getInitialLongitudeE6());
	}
}

int GCM_API Geocache::getInitialLatitudeE6() {
	return this->initialLatitudeE6;
}

int GCM_API Geocache::getInitialLongitudeE6() {
	return this->initialLongitudeE6;
}

double GCM_API Geocache::getInitialLatitude() {
	return this->getInitialLatitudeE6() / 1E6;
}

double GCM_API Geocache::getInitialLongitude() {
	return this->getInitialLongitudeE6() / 1E6;
}

void GCM_API Geocache::setInitialLatitudeE6(int latE6) {
	this->initialLatitudeE6 = latE6;

	if (!this->hasCorrectedCoords()) {
		this->setLatitudeE6(latE6);
	}
}

void GCM_API Geocache::setInitialLongitudeE6(int lonE6) {
	this->initialLongitudeE6 = lonE6;

	if (!this->hasCorrectedCoords()) {
		this->setLongitudeE6(lonE6);
	}
}

GC<GeocacheWaypointList> GCM_API Geocache::getWaypoints() {
	return this->waypoints;
}

void GCM_API Geocache::tryMatchOwner() {
	// Do not match cache that is already mine.
	if (this->isOwn()) return;

	AppConfig *cfg = AppConfig::getInstance();
	const char *nick = cfg->getString("username", NULL);
	if (nick != NULL) {
		if (this->getOwner()->equals(nick)) {
			this->setOwn(true);

			// No need to continue, already found.
			return;
		}
	}

	int x = 0;
	char othernick[15];

	// 10k of nicks should be enough...
	while (x < 10000) {
		sprintf(othernick, "othernick_%d", x);

		nick = cfg->getString(othernick, NULL);
		if (nick != NULL) {
			if (this->getPlacedBy()->find(nick) != std::string::npos) {
				this->setOwn(true);
				break;
			}
		} else {
			// No next nick in row
			break;
		}

		x++;
	}
}

String GCM_API Geocache::typeToGpxString(Geocache::Type type) {
	switch (type) {
		case T_TRADITIONAL: return "Traditional Cache";
		case T_MULTI: return "Multi-cache";
		case T_UNKNOWN: return "Unknown Cache";
		case T_LETTERBOX: return "Letterbox Hybrid";
		case T_EARTH: return "Earthcache";
		case T_WHERIGO: return "Wherigo Cache";
		case T_VIRTUAL: return "Virtual Cache";
		case T_EVENT: return "Event Cache";
		case T_MEGAEVENT: return "Mega-Event Cache";
		case T_CITO: return "Cache In Trash Out Event";
		case T_WEBCAM: return "Webcam Cache";
	}

	return "Unknown cache type";
}

Geocache::Type GCM_API Geocache::gpxStringToType(String gpxString) {
	if (gpxString->equals("Traditional Cache")) return T_TRADITIONAL;
	if (gpxString->equals("Multi-cache")) return T_MULTI;
	if (gpxString->equals("Unknown Cache")) return T_UNKNOWN;
	if (gpxString->equals("Mystery Cache")) return T_UNKNOWN; // GPX 1.0.2
	if (gpxString->equals("Letterbox Hybrid")) return T_LETTERBOX;
	if (gpxString->equals("Earthcache")) return T_EARTH;
	if (gpxString->equals("Wherigo Cache")) return T_WHERIGO;
	if (gpxString->equals("Virtual Cache")) return T_VIRTUAL;
	if (gpxString->equals("Event Cache")) return T_EVENT;
	if (gpxString->equals("Mega-Event Cache")) return T_MEGAEVENT;
	if (gpxString->equals("Cache In Trash Out Event")) return T_CITO;
	if (gpxString->equals("Webcam Cache")) return T_WEBCAM;

	return T_TRADITIONAL; // Fallback
}

Geocache::Size GCM_API Geocache::gpxStringToSize(String gpxString) {
	if (gpxString->equals("Micro")) return S_MICRO;
	if (gpxString->equals("Small")) return S_SMALL;
	if (gpxString->equals("Regular")) return S_REGULAR;
	if (gpxString->equals("Large")) return S_LARGE;
	if (gpxString->equals("Not chosen")) return S_NOT_LISTED;
	if (gpxString->equals("Other")) return S_OTHER;
	if (gpxString->equals("Virtual")) return S_VIRTUAL;

	return S_OTHER; // Fallback
}

Geocache::Type GCM_API Geocache::gsIdToType(int id) {
	switch (id) {
		case 2: return T_TRADITIONAL;
		case 3: return T_MULTI;
		case 4: return T_VIRTUAL;
		case 5: return T_LETTERBOX;
		case 6: return T_EVENT;
		case 8: return T_UNKNOWN;
		case 9: return T_TRADITIONAL; // Project APE not supported... No need for only one still existing cache.
		case 11: return T_WEBCAM;
		case 12: return T_UNKNOWN; // Locationless... probably nobody will use this.
		case 13: return T_CITO;
		case 137: return T_EARTH;
		case 453: return T_MEGAEVENT;
		case 605: return T_EVENT; // Geocache Courses - WTF is this?
		case 1304: return T_EVENT; // No GPX Adventure Exhibit around...
		case 1868: return T_WHERIGO;
		case 3653: return T_EVENT; // Lost and found are not found in the GPX so far...
		case 3773: return T_TRADITIONAL; // HQ cache... only one and add whole new type, icon and so on?
		case 3774: return T_EVENT; // Lost and found celebration, again, fallback to events.
		case 4738: return T_EVENT; // Groundspeak block party, again, something so rare it is not worth implementing.

		// Fallback to traditional cache type.
		default: return T_TRADITIONAL;
	}
}

Geocache::Size GCM_API Geocache::gsIdToSize(int id) {
	switch (id) {
		case 1: return S_NOT_LISTED;
		case 2: return S_MICRO;
		case 3: return S_REGULAR;
		case 4: return S_LARGE;
		case 5: return S_VIRTUAL;
		case 6: return S_OTHER;
		case 8: return S_SMALL;

		// Fallback to other (unknown) cache type.
		default: return S_OTHER;
	}
}

String GCM_API Geocache::sizeToGpxString(Geocache::Size size) {
	switch (size) {
		case S_MICRO: return "Micro";
		case S_SMALL: return "Small";
		case S_REGULAR: return "Regular";
		case S_LARGE: return "Large";
		case S_NOT_LISTED: return "Not chosen";
		case S_OTHER: return "Other";
		case S_VIRTUAL: return "Virtual";
	}

	// Should never happen.
	return "Unknown cache size";
}

/**
 * Converts numerical ID back to the GCID form.
 * It inserts prefix to the begining of that number.
 * @param id Numerical ID that should be converted.
 * @param prefix Prefix that should be inserted before the ID. It defaults to "GC".
 */
String GCM_API Geocache::intToGcId(int id, const char *prefix) {
	if (id <= 65535) {
		return String::format("%s%X", prefix, id);
	} else {
		String out("");
		static size_t _BASE = strlen(ALPHABET);
		static int MAGIC = 411120;

		int num = id + MAGIC; // Magic number to skip 0-FFFF.
		while (num > 0) {
			out->append(1, ALPHABET[num % _BASE]);
			num = num / _BASE;
		}
		String result(prefix);
		result->append(out->rbegin(), out->rend());
		return result;
	}
}

/**
 * Convert the GCID to numerical form. It strips out the prefix (first two chars)
 * from the code, so you don't need to bother.
 * @param gcid GCID including prefix.
 * @return Numerical version of the specified GC code.
 */
int GCM_API Geocache::gcIdToInt(const char *gcid) {
	// The Python code for this is actually much shorter and prettier than this.

	size_t mlen = strlen(gcid);

	// Skip first two chars as prefix.
	if (mlen < 3) return 0;
	const char *num = gcid + 2;
	mlen -= 2;

	// Is it 0 - FFFF?
	if (mlen <= 4 && ((num[0] >= '0' && num[0] <= '9') || (toupper(num[0]) >= 'A' && toupper(num[0]) <= 'F'))) {
		return strtol(num, NULL, 16);
	} else {
		int sum = 0;
		static size_t _BASE = strlen(ALPHABET);
		static int MAGIC = 411120;

		for (size_t x = 0; x < mlen; x++) {
			int power = mlen - x;
			for (size_t i = 0; i < _BASE; i++) {
				if (ALPHABET[i] == toupper(num[x])) {
					sum += i * pow((double)_BASE, (double)power - 1);
					break;
				}
			}
		}

		return sum - MAGIC;
	}
}

int GCM_API Geocache::gcIdToInt(String gcid) {
	return Geocache::gcIdToInt(gcid->c_str());
}
