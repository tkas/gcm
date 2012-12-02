#include <gcm.h>
#include <GC.h>
#include <database/Database.h>
#include <ui.h>
#include <pluginmanager/Plugin.h>
#include <stl/String.h>
#include <expat/expat.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <geolib/Geocache.h>
#include <geolib/GeocacheWaypoint.h>
#include <geolib/GenericWaypoint.h>
#include <geolib/Attribute.h>
#include <geolib/AttributeSet.h>
#include <geolib/Log.h>
#include <util/Log.h>
#include <util/File.h>
#include <import.h>

#define eq(a,b) (strcmp((const char *)a, (const char *)b) == 0)

using namespace GCM;
using namespace GCM::database;
using namespace GCM::pluginmanager;
using namespace GCM::geolib;
using namespace GCM::io;

typedef char xmlChar;

struct ParserState {
	enum StatesEnum {
		ST_INIT,
		ST_UNKNOWN,
		ST_END, // End of parsing, nothing should be accepted.

		ST_GPX, // gpx
		ST_GPX_WPT, // gpx/wpt
		ST_GPX_WPT_TIME, // gpx/wpt/time
		ST_GPX_WPT_NAME, // gpx/wpt/name
		ST_GPX_WPT_DESC, // gpx/wpt/desc
		ST_GPX_WPT_CMT, // gpx/wpt/cmt
		ST_GPX_WPT_SYM, // gpx/wpt/sym
		ST_GPX_WPT_TYPE, // gpx/wpt/type

		ST_GPX_WPT_CACHE, // gpx/wpt/groundspeak:cache
		ST_GPX_WPT_CACHE_NAME,
		ST_GPX_WPT_CACHE_PLACED_BY,
		ST_GPX_WPT_CACHE_OWNER,
		ST_GPX_WPT_CACHE_TYPE,
		ST_GPX_WPT_CACHE_CONTAINER,
		ST_GPX_WPT_CACHE_ATTRIBUTES,
		ST_GPX_WPT_CACHE_ATTRIBUTES_ATTRIBUTE,
		ST_GPX_WPT_CACHE_DIFFICULTY,
		ST_GPX_WPT_CACHE_TERRAIN,
		ST_GPX_WPT_CACHE_COUNTRY,
		ST_GPX_WPT_CACHE_STATE,
		ST_GPX_WPT_CACHE_SHORTDESC,
		ST_GPX_WPT_CACHE_LONGDESC,
		ST_GPX_WPT_CACHE_HINT,
		ST_GPX_WPT_CACHE_LOGS,
		ST_GPX_WPT_CACHE_NOTE,
		ST_GPX_WPT_CACHE_FP,

		ST_GPX_WPT_CACHE_LOGS_LOG,
		ST_GPX_WPT_CACHE_LOGS_LOG_DATE,
		ST_GPX_WPT_CACHE_LOGS_LOG_TYPE,
		ST_GPX_WPT_CACHE_LOGS_LOG_FINDER,
		ST_GPX_WPT_CACHE_LOGS_LOG_TEXT
	};

	enum GPXVersion {
		GPX_1_0,
		GPX_1_0_1,
		GPX_1_0_2
	};

	StatesEnum state;
	StatesEnum prevState;
	GPXVersion version;
	int unknownDepth;
	GC<Database> db;

	GC<Geocache> activeCache;
	GC<GeocacheWaypoint> activeWaypoint;
	GC<GenericWaypoint> genericWaypoint;
	GC<Log> activeLog;
	GC<Attribute> activeAttribute;

	XML_Parser parser;

	enum WptType {
		WPT_GEOCACHE,
		WPT_WAYPOINT,
		WPT_GENERIC_WAYPOINT
	};

	WptType currentWptType;
	String currData;

	ptrDialog progressDialog;

	ParserState(): state(ST_INIT), prevState(ST_INIT), unknownDepth(0) {}

	void setState(StatesEnum p_state) {
		this->prevState = this->state;
		this->state = p_state;
	}
};

/**
 * Decode time in format YYYY-MM-DDTHH:MM:SSZ
 */
time_t decodeXmlTime(String ps_time) {
	const char *p_time = ps_time->c_str();

	int year, month, day, hour, minute;
	float second;
	int readed = sscanf(
		p_time,
		"%d-%d-%dT%d:%d:%fZ",
		&year,
		&month,
		&day,
		&hour,
		&minute,
		&second
	);

	struct tm out;
	time_t tm = time(NULL);
	localtime_r(&tm, &out);

	switch (readed) {
		case 6:
			out.tm_sec = (int)second;
		case 5:
			out.tm_min = minute;
		case 4:
			out.tm_hour = hour;
		case 3:
			out.tm_mday = day;
		case 2:
			out.tm_mon = month - 1;
		case 1:
			out.tm_year = year - 1900;
	}
	out.tm_isdst = 0;

	return mktime(&out);
}

class MyImportInfo: public GCM::ImportInfo {
	protected:
		bool stopParsing;
		ParserState state;

	protected:
		void handleGpxNode(ParserState *state, const xmlChar **attrs);
		void handleWptNode(ParserState *state, const xmlChar **attrs);
		void handleWptName(ParserState *state, String val);
		void handleWptDesc(ParserState *state, String val);
		void handleWptSym(ParserState *state, String val);
		void handleWptType(ParserState *state, String val);
		void handleWptCmt(ParserState *state, String val);
		void handleWptTime(ParserState *state, String val);
		void handleWptCacheNode(ParserState *state, const xmlChar **attribs);
		void handleCacheOwnerNode(ParserState *state, const xmlChar **attribs);
		void handleCacheTypeNode(ParserState *state, const xmlChar **attribs);
		void handleCacheContainerNode(ParserState *state, const xmlChar **attribs);
		void handleLogNode(ParserState *state, const xmlChar **attribs);
		void handleLogTypeNode(ParserState *state, const xmlChar **attribs);
		void handleLogFinderNode(ParserState *state, const xmlChar **attribs);
		void handleLogTextNode(ParserState *state, const xmlChar **attribs);
		void handleAttributeNode(ParserState *state, const xmlChar **attribs);
		void handleSDNode(ParserState *state, const xmlChar **attrs);
		void handleLDNode(ParserState *state, const xmlChar **attrs);
		void storeActiveWaypoint(ParserState *state);

	public:
		MyImportInfo(String desc, String ext);
		bool accept(const char *fileName);
		void import(const char *fileName, GC<Database> db, ptrDialog progressDialog);
		void cancel();

		void startElementHandler(const xmlChar *name, const xmlChar **attrs);
		void endElementHandler(const xmlChar *name);
		void characterDataHandler(const xmlChar *ch, int len);
};

void startElementHandler(void *user_data, const xmlChar *name, const xmlChar **attrs) {
	MyImportInfo *info = (MyImportInfo *)user_data;
	info->startElementHandler(name, attrs);
}

void endElementHandler(void *user_data, const xmlChar *name) {
	MyImportInfo *info = (MyImportInfo *)user_data;
	info->endElementHandler(name);
}

void characterDataHandler(void *user_data, const xmlChar *ch, int len) {
	MyImportInfo *info = (MyImportInfo *)user_data;
	info->characterDataHandler(ch, len);
}

void MyImportInfo::handleGpxNode(ParserState *state, const xmlChar **attrs) {
	while (*attrs != NULL) {
		const char *name = (const char *)attrs[0];
		const char *value = (const char *)attrs[1];

		if (eq(name, "xsi:schemaLocation")) {
			if (strstr(value, "http://www.groundspeak.com/cache/1/0/cache.xsd") != NULL) {
				state->version = ParserState::GPX_1_0;
			} else if (strstr(value, "http://www.groundspeak.com/cache/1/0/1/cache.xsd") != NULL) {
				state->version = ParserState::GPX_1_0_1;
			} else if (strstr(value, "http://www.groundspeak.com/cache/1/0/2/cache.xsd") != NULL) {
				state->version = ParserState::GPX_1_0_2;
			}
		}

		attrs += 2;
	}
}

void MyImportInfo::handleWptNode(ParserState *state, const xmlChar **attrs) {
	state->currentWptType = ParserState::WPT_GENERIC_WAYPOINT;
	state->genericWaypoint = new GenericWaypoint();

	// Ger coordinates from the node.
	while (*attrs != NULL) {
		const char *name = (const char *)attrs[0];
		const char *value = (const char *)attrs[1];

		if (eq(name, "lat")) {
			state->genericWaypoint->setLatitudeE6((int)(atof(value) * 1E6));
		} else if (eq(name, "lon")) {
			state->genericWaypoint->setLongitudeE6((int)(atof(value) * 1E6));
		}

		attrs += 2;
	}
}

void MyImportInfo::handleWptName(ParserState *state, String val) {
	switch (state->currentWptType) {
		case ParserState::WPT_GEOCACHE:
			state->activeCache->setId(val);
			break;

		case ParserState::WPT_GENERIC_WAYPOINT:
			state->genericWaypoint->setId(val);
			break;

		case ParserState::WPT_WAYPOINT:
			state->activeWaypoint->setId(val);
			break;
	}
}

void MyImportInfo::handleWptDesc(ParserState *state, String val) {
	switch (state->currentWptType) {
		case ParserState::WPT_GENERIC_WAYPOINT:
			state->genericWaypoint->setName(val);
			break;

		case ParserState::WPT_WAYPOINT:
			state->activeWaypoint->setName(val);
			break;

		// Not used for geocache
		case ParserState::WPT_GEOCACHE:
			break;
	}
}

void MyImportInfo::handleWptSym(ParserState *state, String val) {
	// Not used so far, as generic waypoint support is not done so far...
	(void)state;
	(void)val;
}

void MyImportInfo::handleWptType(ParserState *state, String val) {
	if (
		val->equals("Waypoint|Final Location") ||
		val->equals("Waypoint|Parking Area") ||
		val->equals("Waypoint|Question to Answer") ||
		val->equals("Waypoint|Stages of a Multicache") ||
		val->equals("Waypoint|Reference point") ||
		val->equals("Waypoint|Trailhead")
	) {
		state->activeWaypoint = new GeocacheWaypoint(*(state->genericWaypoint.getObj()));
		state->activeWaypoint->setWptType(GeocacheWaypoint::gpxStringToType(val));
		state->genericWaypoint = NULL;
		state->currentWptType = ParserState::WPT_WAYPOINT;
	}
}

void MyImportInfo::handleWptCmt(ParserState *state, String val) {
	switch (state->currentWptType) {
		case ParserState::WPT_GENERIC_WAYPOINT:
			state->genericWaypoint->setComment(val);
			break;

		case ParserState::WPT_WAYPOINT:
			state->activeWaypoint->setComment(val);
			break;

		// Not used for geocache, it should even not be present in the
		// geocache node.
		case ParserState::WPT_GEOCACHE:
			break;
	}
}

void MyImportInfo::handleWptTime(ParserState *state, String val) {
	switch (state->currentWptType) {
		case ParserState::WPT_GENERIC_WAYPOINT:
			state->genericWaypoint->setDtCreated(decodeXmlTime(val));
			break;

		case ParserState::WPT_WAYPOINT:
			state->activeWaypoint->setDtCreated(decodeXmlTime(val));
			break;

		case ParserState::WPT_GEOCACHE:
			state->activeCache->setDtCreated(decodeXmlTime(val));
			break;
	}
}

void MyImportInfo::handleWptCacheNode(ParserState *state, const xmlChar **attribs) {
	state->currentWptType = ParserState::WPT_GEOCACHE;
	state->activeCache = new Geocache(*(state->genericWaypoint.getObj()));
	state->activeCache->setLoaded(true);
	state->activeCache->setMapLoaded(true);
	state->genericWaypoint = NULL;

	if (attribs == NULL) return;

	// Set the id, status and so on
	while (*attribs != NULL) {
		const char *name = (const char *)attribs[0];
		const char *value = (const char *)attribs[1];

		if (eq(name, "available")) {
			if (eq(value, "True") || eq(value, "true")) {
				state->activeCache->setCacheStatus(Geocache::ST_AVAILABLE);
			} else {
				state->activeCache->setCacheStatus(Geocache::ST_DISABLED);
			}
		} else if (eq(name, "archived")) {
			if (eq(value, "True") || eq(value, "true")) {
				state->activeCache->setCacheStatus(Geocache::ST_ARCHIVED);
			}
		}

		attribs += 2;
	}
}

void MyImportInfo::handleCacheOwnerNode(ParserState *state, const xmlChar **attribs) {
	if (attribs == NULL) return;

	while (*attribs != NULL) {
		const char *name = (const char *)attribs[0];
		const char *value = (const char *)attribs[1];

		if (eq(name, "id")) {
			// GPX 1.0.2 and up has GCID representation of the owner ID.
			if (state->version >= ParserState::GPX_1_0_2) {
				state->activeCache->setOwnerId(Geocache::gcIdToInt(value));
			} else {
				state->activeCache->setOwnerId(atoi(value));
			}
			break; // Break is there, because we don't accept anything else than id
			// here. So in case of any other attribute after id, it is safe to skip
			// it and save few CPU cycles.
		}

		attribs += 2;
	}
}

void MyImportInfo::handleCacheTypeNode(ParserState *state, const xmlChar **attribs) {
	if (attribs == NULL) return;

	while (*attribs != NULL) {
		const char *name = (const char *)attribs[0];
		const char *value = (const char *)attribs[1];

		if (eq(name, "id")) {
			state->activeCache->setCacheType(Geocache::gsIdToType(atoi(value)));

			// Don't need anything else than ID, so we can break.
			break;
		}

		attribs += 2;
	}
}

void MyImportInfo::handleCacheContainerNode(ParserState *state, const xmlChar **attribs) {
	if (attribs == NULL) return;

	while (*attribs != NULL) {
		const char *name = (const char *)attribs[0];
		const char *value = (const char *)attribs[1];

		if (eq(name, "id")) {
			state->activeCache->setSize(Geocache::gsIdToSize(atoi(value)));

			// Don't need anything else than ID, so we can break.
			break;
		}

		attribs += 2;
	}
}

void MyImportInfo::handleLogNode(ParserState *state, const xmlChar **attribs) {
	state->activeLog = new Log();
	state->activeLog->setCache(state->activeCache);

	if (attribs == NULL) return;

	while (*attribs != NULL) {
		const char *name = (const char *)attribs[0];
		const char *value = (const char *)attribs[1];

		if (eq(name, "id")) {
			if (state->version >= ParserState::GPX_1_0_2) {
				state->activeLog->setId(Geocache::gcIdToInt(value));
			} else {
				state->activeLog->setId(atoi(value));
			}
			break;
		}

		attribs += 2;
	}
}

void MyImportInfo::handleLogTypeNode(ParserState *state, const xmlChar **attribs) {
	if (attribs == NULL) return;

	while (*attribs != NULL) {
		const char *name = (const char *)attribs[0];
		const char *value = (const char *)attribs[1];

		if (eq(name, "id")) {
			state->activeLog->setType(Log::gsIdToType(atoi(value)));
			break;
		}

		attribs += 2;
	}
}

void MyImportInfo::handleLogFinderNode(ParserState *state, const xmlChar **attribs) {
	if (attribs == NULL) return;

	while (*attribs != NULL) {
		const char *name = (const char *)attribs[0];
		const char *value = (const char *)attribs[1];

		if (eq(name, "id")) {
			if (state->version >= ParserState::GPX_1_0_2) {
				state->activeLog->setFinderId(Geocache::gcIdToInt(value));
			} else {
				state->activeLog->setFinderId(atoi(value));
			}
		}

		attribs += 2;
	}
}

void MyImportInfo::handleLogTextNode(ParserState *state, const xmlChar **attribs) {
	if (attribs == NULL) return;

	while (*attribs != NULL) {
		const char *name = (const char *)attribs[0];
		const char *value = (const char *)attribs[1];

		if (eq(name, "encoded")) {
			if (eq(value, "True") || eq(value, "true")) {
				state->activeLog->setTextEncoded(true);
			} else {
				state->activeLog->setTextEncoded(false);
			}
			break;
		}

		attribs += 2;
	}
}

void MyImportInfo::handleAttributeNode(ParserState *state, const xmlChar **attribs) {
	if (attribs == NULL) return;

	int id = -1;
	bool positive = false;

	while (*attribs != NULL) {
		const char *name = (const char *)attribs[0];
		const char *value = (const char *)attribs[1];

		if (eq(name, "id")) {
			id = atoi(value);
		} else if (eq(name, "inc")) {
			if (eq(value, "1")) positive = true;
		}

		attribs += 2;
	}

	if (id > 0) {
		state->activeAttribute = new Attribute(id, positive);
	}
}

void MyImportInfo::handleSDNode(ParserState *state, const xmlChar **attrs) {
	while (*attrs != NULL) {
		const char *name = (const char *)attrs[0];
		const char *value = (const char *)attrs[1];

		if (eq(name, "html")) {
			if (eq(value, "True") || eq(value, "true") || eq(value, "1")) {
				state->activeCache->setSDIsHTML(true);
			} else {
				state->activeCache->setSDIsHTML(false);
			}
		}

		attrs += 2;
	}
}

void MyImportInfo::handleLDNode(ParserState *state, const xmlChar **attrs) {
	while (*attrs != NULL) {
		const char *name = (const char *)attrs[0];
		const char *value = (const char *)attrs[1];

		if (eq(name, "html")) {
			if (eq(value, "True") || eq(value, "true") || eq(value, "1")) {
				state->activeCache->setLDIsHTML(true);
			} else {
				state->activeCache->setLDIsHTML(false);
			}
		}

		attrs += 2;
	}
}

void MyImportInfo::startElementHandler(const xmlChar *name, const xmlChar **attrs) {
	ParserState *state = &this->state;

	//GCM::util::Log::d("Import|PQGPX", "Start element %s", name);

	//ParserState::StatesEnum initState = state->state;
	ParserState::StatesEnum nextState = ParserState::ST_UNKNOWN;

	if (state->state == ParserState::ST_UNKNOWN) {
		//GCM::util::Log::d("PQGPX", "Start of element %s, unknown, skip.", name);
		++state->unknownDepth;
		return;
	} else {
		// Set the state to unknown by default.
		nextState = ParserState::ST_UNKNOWN;
	}

	switch (state->state) {
		case ParserState::ST_INIT:
			if (eq(name, "gpx")) {
				handleGpxNode(state, attrs);
				nextState = ParserState::ST_GPX;
			}
			break;

		case ParserState::ST_GPX:
			if (eq(name, "wpt")) {
				handleWptNode(state, attrs);
				nextState = ParserState::ST_GPX_WPT;
			}
			break;

		case ParserState::ST_GPX_WPT:
			if (eq(name, "time")) {
				nextState = ParserState::ST_GPX_WPT_TIME;
			} else if (eq(name, "name")) {
				nextState = ParserState::ST_GPX_WPT_NAME;
			} else if (eq(name, "desc")) {
				nextState = ParserState::ST_GPX_WPT_DESC;
			} else if (eq(name, "sym")) {
				nextState = ParserState::ST_GPX_WPT_SYM;
			} else if (eq(name, "type")) {
				nextState = ParserState::ST_GPX_WPT_TYPE;
			} else if (eq(name, "cmt")) {
				nextState = ParserState::ST_GPX_WPT_CMT;
			} else if (eq(name, "groundspeak:cache")) {
				handleWptCacheNode(state, attrs);
				nextState = ParserState::ST_GPX_WPT_CACHE;
			}

		case ParserState::ST_GPX_WPT_CACHE:
			if (eq(name, "groundspeak:name")) {
				nextState = ParserState::ST_GPX_WPT_CACHE_NAME;
			} else if (eq(name, "groundspeak:placed_by")) {
				nextState = ParserState::ST_GPX_WPT_CACHE_PLACED_BY;
			} else if (eq(name, "groundspeak:owner")) {
				handleCacheOwnerNode(state, attrs);
				nextState = ParserState::ST_GPX_WPT_CACHE_OWNER;
			} else if (eq(name, "groundspeak:type")) {
				if (state->version >= ParserState::GPX_1_0_2) {
					handleCacheTypeNode(state, attrs);
				}
				nextState = ParserState::ST_GPX_WPT_CACHE_TYPE;
			} else if (eq(name, "groundspeak:container")) {
				if (state->version >= ParserState::GPX_1_0_2) {
					handleCacheContainerNode(state, attrs);
				}
				nextState = ParserState::ST_GPX_WPT_CACHE_CONTAINER;
			} else if (eq(name, "groundspeak:attributes")) {
				nextState = ParserState::ST_GPX_WPT_CACHE_ATTRIBUTES;
			} else if (eq(name, "groundspeak:difficulty")) {
				nextState = ParserState::ST_GPX_WPT_CACHE_DIFFICULTY;
			} else if (eq(name, "groundspeak:terrain")) {
				nextState = ParserState::ST_GPX_WPT_CACHE_TERRAIN;
			} else if (eq(name, "groundspeak:country")) {
				nextState = ParserState::ST_GPX_WPT_CACHE_COUNTRY;
			} else if (eq(name, "groundspeak:state")) {
				nextState = ParserState::ST_GPX_WPT_CACHE_STATE;
			} else if (eq(name, "groundspeak:short_description")) {
				handleSDNode(state, attrs);
				nextState = ParserState::ST_GPX_WPT_CACHE_SHORTDESC;
			} else if (eq(name, "groundspeak:long_description")) {
				handleLDNode(state, attrs);
				nextState = ParserState::ST_GPX_WPT_CACHE_LONGDESC;
			} else if (eq(name, "groundspeak:encoded_hints")) {
				nextState = ParserState::ST_GPX_WPT_CACHE_HINT;
			} else if (eq(name, "groundspeak:logs")) {
				nextState = ParserState::ST_GPX_WPT_CACHE_LOGS;
			} else if (eq(name, "groundspeak:personal_note")) {
				nextState = ParserState::ST_GPX_WPT_CACHE_NOTE;
			} else if (eq(name, "groundspeak:favorite_points")) {
				nextState = ParserState::ST_GPX_WPT_CACHE_FP;
			}
			break;

		case ParserState::ST_GPX_WPT_CACHE_LOGS:
			if (eq(name, "groundspeak:log")) {
				handleLogNode(state, attrs);
				nextState = ParserState::ST_GPX_WPT_CACHE_LOGS_LOG;
			}
			break;

		case ParserState::ST_GPX_WPT_CACHE_LOGS_LOG:
			if (eq(name, "groundspeak:date")) {
				nextState = ParserState::ST_GPX_WPT_CACHE_LOGS_LOG_DATE;
			} else if (eq(name, "groundspeak:type")) {
				handleLogTypeNode(state, attrs);
				nextState = ParserState::ST_GPX_WPT_CACHE_LOGS_LOG_TYPE;
			} else if (eq(name, "groundspeak:finder")) {
				handleLogFinderNode(state, attrs);
				nextState = ParserState::ST_GPX_WPT_CACHE_LOGS_LOG_FINDER;
			} else if (eq(name, "groundspeak:text")) {
				handleLogTextNode(state, attrs);
				nextState = ParserState::ST_GPX_WPT_CACHE_LOGS_LOG_TEXT;
			}
			break;

		case ParserState::ST_GPX_WPT_CACHE_ATTRIBUTES:
			if (eq(name, "groundspeak:attribute")) {
				handleAttributeNode(state, attrs);
				nextState = ParserState::ST_GPX_WPT_CACHE_ATTRIBUTES_ATTRIBUTE;
			}
			break;

		// Not needed, it is already handled by presetting the state to unknown
		// before the switch, which also solves issues with unknown chids in known
		// nodes.
		default:
			break;
	}

	if (nextState == ParserState::ST_UNKNOWN) {
		state->unknownDepth = 1;
		state->prevState = state->state;
	}

	if (nextState != state->state) {
		state->currData = "";
	}

	state->state = nextState;

	//GCM::util::Log::d("PQGPX", "Start of element %s, state %d -> %d.", name, initState, state->state);

	if (stopParsing) XML_StopParser(state->parser, false);
}

void MyImportInfo::storeActiveWaypoint(ParserState *state) {
	switch (state->currentWptType) {
		case ParserState::WPT_GEOCACHE:
			// Try to set cache is own by owner / placed by acording to settings.
			state->activeCache->tryMatchOwner();
			state->db->saveCache(state->activeCache);
			break;

		case ParserState::WPT_WAYPOINT:
			state->db->saveCacheWaypoint(state->activeWaypoint);
			break;

		case ParserState::WPT_GENERIC_WAYPOINT:
			state->db->saveGenericWaypoint(state->genericWaypoint);
			break;
	}

	/*state->progressDialog->setProgress(
		state->progressDialog->getProgress() + 1
	);*/
}

void MyImportInfo::endElementHandler(const xmlChar *name) {
	ParserState *state = &this->state;
	ParserState::StatesEnum initState = state->state;

	//GCM::util::Log::d("Import|PQGPX", "Start element %s", name);

	if (state->state == ParserState::ST_UNKNOWN) {
		--state->unknownDepth;

		if (state->unknownDepth == 0) {
			//GCM::util::Log::d("PQGPX", "End of element %s, return from unknown state to %d.", name, state->prevState);
			state->setState(state->prevState);
		}
		return;
	}

	switch (state->state) {
		case ParserState::ST_INIT:
		case ParserState::ST_UNKNOWN:
		case ParserState::ST_END:
			// This cannot happen here...
			break;

		case ParserState::ST_GPX:
			state->state = ParserState::ST_END;
			break;

		// End of waypoint, save it to the database.
		case ParserState::ST_GPX_WPT:
			storeActiveWaypoint(state);
			state->state = ParserState::ST_GPX;
			state->progressDialog->setProgress(XML_GetCurrentByteIndex(state->parser));
			break;

		// End of something inside the <wpt> tag
		case ParserState::ST_GPX_WPT_NAME:
			handleWptName(state, state->currData);
			state->state = ParserState::ST_GPX_WPT;
			break;

		case ParserState::ST_GPX_WPT_DESC:
			handleWptDesc(state, state->currData);
			state->state = ParserState::ST_GPX_WPT;
			break;

		case ParserState::ST_GPX_WPT_SYM:
			handleWptSym(state, state->currData);
			state->state = ParserState::ST_GPX_WPT;
			break;

		case ParserState::ST_GPX_WPT_TYPE:
			handleWptType(state, state->currData);
			state->state = ParserState::ST_GPX_WPT;
			break;

		case ParserState::ST_GPX_WPT_CMT:
			handleWptCmt(state, state->currData);
			state->state = ParserState::ST_GPX_WPT;
			break;

		case ParserState::ST_GPX_WPT_CACHE:
			state->state = ParserState::ST_GPX_WPT;
			break;

		case ParserState::ST_GPX_WPT_TIME:
			handleWptTime(state, state->currData);
			state->state = ParserState::ST_GPX_WPT;
			break;

		// End of something in <groundspeak:cache> tag
		case ParserState::ST_GPX_WPT_CACHE_NAME:
			state->activeCache->setName(state->currData);
			state->state = ParserState::ST_GPX_WPT_CACHE;
			break;

		case ParserState::ST_GPX_WPT_CACHE_PLACED_BY:
			state->activeCache->setPlacedBy(state->currData);
			state->state = ParserState::ST_GPX_WPT_CACHE;
			break;

		case ParserState::ST_GPX_WPT_CACHE_OWNER:
			state->activeCache->setOwner(state->currData);
			state->state = ParserState::ST_GPX_WPT_CACHE;
			break;

		case ParserState::ST_GPX_WPT_CACHE_TYPE:
			state->activeCache->setCacheType(
				Geocache::gpxStringToType(state->currData));
			state->state = ParserState::ST_GPX_WPT_CACHE;
			break;

		case ParserState::ST_GPX_WPT_CACHE_CONTAINER:
			state->activeCache->setSize(
				Geocache::gpxStringToSize(state->currData));
			state->state = ParserState::ST_GPX_WPT_CACHE;
			break;

		case ParserState::ST_GPX_WPT_CACHE_ATTRIBUTES:
			state->activeCache->setAttributesLoaded(true);
			state->state = ParserState::ST_GPX_WPT_CACHE;
			break;

		case ParserState::ST_GPX_WPT_CACHE_ATTRIBUTES_ATTRIBUTE:
			// Push the attribute to the cache.
			state->activeCache->getAttributes()->append(state->activeAttribute);
			state->state = ParserState::ST_GPX_WPT_CACHE_ATTRIBUTES;
			break;

		case ParserState::ST_GPX_WPT_CACHE_DIFFICULTY:
			state->activeCache->setDifficulty(atof(state->currData->c_str()) * 10);
			state->state = ParserState::ST_GPX_WPT_CACHE;
			break;

		case ParserState::ST_GPX_WPT_CACHE_TERRAIN:
			state->activeCache->setTerrain(atof(state->currData->c_str()) * 10);
			state->state = ParserState::ST_GPX_WPT_CACHE;

		case ParserState::ST_GPX_WPT_CACHE_COUNTRY:
			state->activeCache->setCountry(state->currData);
			state->state = ParserState::ST_GPX_WPT_CACHE;
			break;

		case ParserState::ST_GPX_WPT_CACHE_STATE:
			state->activeCache->setState(state->currData);
			state->state = ParserState::ST_GPX_WPT_CACHE;
			break;

		case ParserState::ST_GPX_WPT_CACHE_SHORTDESC:
			state->activeCache->setShortDescription(state->currData);
			state->state = ParserState::ST_GPX_WPT_CACHE;
			break;

		case ParserState::ST_GPX_WPT_CACHE_LONGDESC:
			state->activeCache->setLongDescription(state->currData);
			state->state = ParserState::ST_GPX_WPT_CACHE;
			state->activeCache->setLongLoaded(true);
			break;

		case ParserState::ST_GPX_WPT_CACHE_HINT:
			state->activeCache->setHint(state->currData);
			state->state = ParserState::ST_GPX_WPT_CACHE;
			break;

		case ParserState::ST_GPX_WPT_CACHE_LOGS:
			state->activeCache->setLogsLoaded(true);
			state->state = ParserState::ST_GPX_WPT_CACHE;
			break;

		case ParserState::ST_GPX_WPT_CACHE_NOTE:
			// TODO: Process personal cache note
			state->state = ParserState::ST_GPX_WPT_CACHE;
			break;

		case ParserState::ST_GPX_WPT_CACHE_FP:
			state->activeCache->setFavPoints(atoi(state->currData->c_str()));
			state->state = ParserState::ST_GPX_WPT_CACHE;
			break;

		case ParserState::ST_GPX_WPT_CACHE_LOGS_LOG:
			state->state = ParserState::ST_GPX_WPT_CACHE_LOGS;
			/*GCM::util::Log::d("PQGPX", "Got log %s by %s.",
				Log::typeToGpxString(state->activeLog->getType())->c_str(),
				state->activeLog->getFinder()->c_str());*/
			state->activeCache->getLogs()->append(state->activeLog);
			break;

		case ParserState::ST_GPX_WPT_CACHE_LOGS_LOG_DATE:
			state->state = ParserState::ST_GPX_WPT_CACHE_LOGS_LOG;
			state->activeLog->setTime(decodeXmlTime(state->currData));
			break;

		case ParserState::ST_GPX_WPT_CACHE_LOGS_LOG_TYPE:
			state->state = ParserState::ST_GPX_WPT_CACHE_LOGS_LOG;

			// GPX 1.0.2 has the id type attribute. Don't need to process
			// the string.
			if (state->version <= ParserState::GPX_1_0_1) {
				state->activeLog->setType(Log::gpxStringToType(state->currData));
			}
			break;

		case ParserState::ST_GPX_WPT_CACHE_LOGS_LOG_FINDER:
			state->state = ParserState::ST_GPX_WPT_CACHE_LOGS_LOG;
			state->activeLog->setFinder(state->currData);
			break;

		case ParserState::ST_GPX_WPT_CACHE_LOGS_LOG_TEXT:
			state->state = ParserState::ST_GPX_WPT_CACHE_LOGS_LOG;
			state->activeLog->setText(state->currData);
			break;
	}

	if (initState != state->state) {
		state->currData = "";
	}

	//GCM::util::Log::d("PQGPX", "End of element %s, state %d -> %d.", name, initState, state->state);

	if (stopParsing) XML_StopParser(state->parser, false);

	(void)name;
}

void MyImportInfo::characterDataHandler(const xmlChar *ch, int len) {
	ParserState *state = &this->state;
	if (state->state == ParserState::ST_UNKNOWN) return;

	state->currData->append((const char *)ch, len);

	if (stopParsing) XML_StopParser(state->parser, false);
}

MyImportInfo::MyImportInfo(String desc, String ext): GCM::ImportInfo(desc, ext), stopParsing(false) {}

bool MyImportInfo::accept(const char *fileName) {
	FILE *f = fopen(fileName, "r");
	if (!f) return false;

	unsigned char buffer[256];
	fgets((char *)buffer, 256, f);
	fclose(f);
	unsigned char *bufferStart = buffer;

	// Skip the byte-order-mark if present.
	if (buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF) {
		bufferStart += 3;
	} else if (buffer[0] == 0xBF && buffer[1] == 0xBB && buffer[2] == 0xEF) {
		bufferStart += 3;
	} else if (buffer[0] == 0xFE && buffer[1] == 0xFF) {
		bufferStart += 2;
	} else if (buffer[0] == 0xFF && buffer[1] == 0xFE) {
		bufferStart += 2;
	}

	// The detection here could be better...
	if (strncmp((const char *)bufferStart, "<?xml", 5) == 0) {
		return true;
	} else {
		return false;
	}
}

void MyImportInfo::import(const char *fileName, GC<Database> db, ptrDialog progressDialog) {
	stopParsing = false;

	XML_Parser parser = XML_ParserCreate("UTF-8");
	if (!parser) {
		GCM::util::Log::e("Import|PQGPX", "Unable to initialize the parser.");
		return;
	}

	//xmlSAXVersion(&importHandler, 2);

	// Reset ParserState
	this->state = ParserState();
	state.db = db;
	state.progressDialog = progressDialog;
	state.parser = parser;

	state.db->actionHint(GCM::database::Database::AH_IMPORT);

	FILE *f = fopen(fileName, "rb");
	File fn(fileName);
	if (f) {
		// Get file size
		fseek(f, 0L, SEEK_END);
		size_t fileSize = ftell(f);

		fseek(f, 0L, SEEK_SET);

		progressDialog->setMaxProgress(fileSize);
		progressDialog->setMessage(String::format("Importing %s...", fn->getBasename()->c_str()));

		state.db->beginTransaction();
		XML_SetUserData(parser, this);
		XML_SetStartElementHandler(parser, ::startElementHandler);
		XML_SetEndElementHandler(parser, ::endElementHandler);
		XML_SetCharacterDataHandler(parser, ::characterDataHandler);

		// 4MB buffer to read data
		static const int BUFF_SIZE = 4*1024*1024;

		bool abort = false;

		while (!abort && !feof(f)) {
			void *buffer = XML_GetBuffer(parser, BUFF_SIZE);
			int readed = fread(buffer, sizeof(char), BUFF_SIZE, f);
			if (XML_ParseBuffer(parser, readed, false) == XML_STATUS_ERROR) {
				abort = true;
				if (XML_GetErrorCode(parser) != XML_ERROR_ABORTED) {
					GCM::util::Log::e("Import|PQGPX", "Error when parsing XML: %s", XML_ErrorString(XML_GetErrorCode(parser)));
				} else {
					GCM::util::Log::d("Import|PQGPX", "Import aborted.");
				}
			}
		}
		fclose(f);

		if (!abort && XML_ParseBuffer(parser, 0, false) == XML_STATUS_ERROR) {
			if (XML_GetErrorCode(parser) != XML_ERROR_ABORTED) {
				GCM::util::Log::e("Import|PQGPX", "Error when parsing XML: %s", XML_ErrorString(XML_GetErrorCode(parser)));
			} else {
				GCM::util::Log::d("Import|PQGPX", "Import aborted.");
			}
		}

		state.db->commit();
	} else {
		GCM::util::Log::e("Import|PQGPX", "Unable to open imported file %s: %s", fileName, strerror(errno));
	}

	state.db->actionHint(GCM::database::Database::AH_NONE);

	XML_ParserFree(parser);
}

void MyImportInfo::cancel() {
	stopParsing = true;
}

GC< List<ImportInfo> > info() {
	GC< List<ImportInfo> > out = new List<ImportInfo>();
	out->append(new MyImportInfo("PocketQuery GPX", "*.gpx"));
	return out;
}

PLUGIN_EXPORT void gcm_pluginInit(Plugin *plugin) {
	plugin->author = "Niximor";
	plugin->version.major = 1;
	plugin->version.minor = 0;
	plugin->manager.registerFunction(plugin, "Import", callback(info));
};

PLUGIN_EXPORT void gcm_pluginDone(Plugin *plugin) {
	(void)plugin;
}
