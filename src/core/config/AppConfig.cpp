#include <gcm.h>
#include <config/AppConfig.h>
#include <GC.h>
#include <database/Database.h>
#include <util/File.h>
#include <geolib/filter/MatchAll.h>

#include <string.h>
#include <errno.h>

#ifndef _WIN32
# include <unistd.h>
# include <sys/types.h>
# include <pwd.h>
# include <limits.h>
#else
# include <shlobj.h>
#endif

#include <expat/expat.h>


using namespace GCM::config;
using namespace GCM;
using namespace GCM::io;

AppConfig GCM_API *AppConfig::instance = NULL;

AppConfig GCM_API *AppConfig::getInstance() {
	if (AppConfig::instance == NULL) {
		AppConfig::instance = new AppConfig();
	}

	return AppConfig::instance;
}

void GCM_API AppConfig::finalize() {
	if (AppConfig::instance != NULL) {
		AppConfig *cfg = AppConfig::instance;
		
		GC<database::Database> db = cfg->getWorkingDatabase();
		if (db) {
			cfg->set("database_file", db->getConnectionString());
			cfg->set("database_format", db->getId());
		}
		
		// Save reference points
		cfg->saveReferencePoints();
	
		cfg->write();
		delete cfg;
	}
}

GCM_API AppConfig::AppConfig(): Configuration(), mDb(NULL), mDDFound(false),
	mActiveFilter(new GCM::geolib::filter::MatchAll()), mReferencePoints(new ArrayList<GCM::geolib::ReferencePoint>()) {
}

void GCM_API AppConfig::setWorkingDatabase(GCM::GC<GCM::database::Database> db) {
	this->mDb = db;
}

GCM::GC<GCM::database::Database> GCM_API AppConfig::getWorkingDatabase() {
	return this->mDb;
}

String GCM_API AppConfig::getDataDirectory() {
	if (!this->mDDFound) {
		// It is easy here... We have data directory specified.
		File dd("./data");
		if (this->keyExists("datadir")) {
			String dataDir = this->getString("datadir", NULL);

			// Fill up user's home directory.
			dataDir = dataDir->replace("$HOME", getenv("HOME"));
			dd = File(dataDir);
			this->mDataDirectory = dd->getAbsolutePath();
		// No data directory specified, suppose default in user's home.
		} else {
#ifndef _WIN32
			struct passwd pw;
			struct passwd *ppw;
			long size = sysconf(_SC_GETPW_R_SIZE_MAX);
			char *buffer = (char *)malloc(size);
			if (getpwuid_r(getuid(), &pw, buffer, size, &ppw) == 0) {
				dd->append(pw.pw_dir);
				dd->append(".gcm/");
			} else {
				GCM::util::Log::d("GCM", "Unable to obtain user's home directory.");
			}
			free(buffer);
#else
			wchar_t buffer[MAX_PATH];
			if (SHGetFolderPathW(NULL,
					CSIDL_APPDATA,
					NULL,
					0,
					buffer) == S_OK) {

				char mbuffer[MAX_PATH];
				size_t num;
				wcstombs_s(&num, mbuffer, MAX_PATH, buffer, MAX_PATH * sizeof(wchar_t));

				dd->append(mbuffer);
				dd->append("GCM/");
			}
#endif

			this->mDataDirectory = dd->getAbsolutePath();
		}

		// Try to create the home directory.
		if (!dd->exists()) {
			GCM::util::Log::d("GCM", "Data directory %s does not exists.", dd->toString()->c_str());
			dd->mkdirs();
		} else {
			GCM::util::Log::d("GCM", "Data directory %s exists.", dd->toString()->c_str());
		}

		this->mDDFound = true;
	}

	return this->mDataDirectory;
}

GCM::GC<GCM::geolib::Filter> GCM_API AppConfig::getActiveFilter() {
	return this->mActiveFilter;
}

void GCM_API AppConfig::setActiveFilter(GCM::GC<GCM::geolib::Filter> filter) {
	this->mActiveFilter = filter;
}

String GCM_API AppConfig::getWorkingDirectory() {
	return this->mWorkingDirectory;
}

void GCM_API AppConfig::setWorkingDirectory(String wd) {
	this->mWorkingDirectory = wd;
}

void GCM_API AppConfig::loadOtherSettings() {
	this->loadReferencePoints();
}

GCM::GC< GCM::ArrayList<GCM::geolib::ReferencePoint> > GCM_API AppConfig::getReferencePoints() {
	return this->mReferencePoints;
}

class RefPtState {
	public:
		enum State {
			ST_INIT,
			ST_END,
			ST_GPX,
			ST_GPX_WPT,
			ST_GPX_WPT_NAME,
			ST_IGNORE
		};
		
		State state;
		State oldState;
		int skipLevel;
		String currentString;
		
		GCM::GC<GCM::geolib::ReferencePoint> currentPoint;
		AppConfig *cfg;
		
		RefPtState(): state(ST_INIT), skipLevel(0) {}
};

static void processWptAttribs(RefPtState *state, const char **attribs) {
	while (*attribs != NULL) {
		const char *name = attribs[0];
		const char *value = attribs[1];
		
		if (strcmp(name, "lat") == 0) {
			state->currentPoint->setLatitudeE6(atof(value) * 1E6);
		} else if (strcmp(name, "lon") == 0) {
			state->currentPoint->setLongitudeE6(atof(value) * 1E6);
		}
	
		attribs += 2;
	}
}

static void startRefPt(void *customData, const char *element, const char **attribs) {
	RefPtState *state = (RefPtState *)customData;

	RefPtState::State currState = state->state;

	if (state->state != RefPtState::ST_IGNORE) {	
		state->oldState = state->state;
		state->state = RefPtState::ST_IGNORE;
		state->skipLevel = 1;
	} else {
		state->skipLevel++;
	}
	
	switch (currState) {
		case RefPtState::ST_IGNORE:
			state->skipLevel++;
			break;
			
		case RefPtState::ST_INIT:
			if (strcmp(element, "gpx") == 0) { state->state = RefPtState::ST_GPX; }
			break;
			
		case RefPtState::ST_GPX:
			if (strcmp(element, "wpt") == 0) {
				state->state = RefPtState::ST_GPX_WPT;
				state->currentPoint = new GCM::geolib::ReferencePoint();
				state->currentPoint->setTemporary(false);
				processWptAttribs(state, attribs);
			}
			break;
			
		case RefPtState::ST_GPX_WPT:
			if (strcmp(element, "name") == 0) { state->state = RefPtState::ST_GPX_WPT_NAME; }
			break;
			
		default:
			break;
	}
	
	state->currentString = "";
}

static void endRefPt(void *customData, const char *element) {
	RefPtState *state = (RefPtState *)customData;
	
	switch (state->state) {
		case RefPtState::ST_IGNORE:
			if (--state->skipLevel == 0) state->state = state->oldState;
			break;
			
		case RefPtState::ST_GPX:
			state->state = RefPtState::ST_END;
			break;
			
		case RefPtState::ST_GPX_WPT:
			state->state = RefPtState::ST_GPX;
			
			// Got reference point
			state->cfg->getReferencePoints()->append(state->currentPoint);
			
			break;
			
		case RefPtState::ST_GPX_WPT_NAME:
			state->state = RefPtState::ST_GPX_WPT;
			state->currentPoint->setName(state->currentString);
			break;
			
		default:
			break;
	}
	
	state->currentString = "";
	
	(void)element;
}

static void textRefPt(void *customData, const char *string, int len) {
	RefPtState *state = (RefPtState *)customData;
	state->currentString->append(string, len);
}

void GCM_API AppConfig::loadReferencePoints() {
	// Reference points stored in GPX file
	File fp("reference.gpx");
	if (fp->exists()) {
		FILE *f = fopen(fp->toString()->c_str(), "r");
		if (f) {
			XML_Parser p = XML_ParserCreate("UTF-8");
			if (p) {
				XML_SetElementHandler(p, startRefPt, endRefPt);
				XML_SetCharacterDataHandler(p, textRefPt);
						
				RefPtState state;
				state.cfg = this;
			
				XML_SetUserData(p, &state);
			
				// 1MB for buffer here should be enough
				int len = 1*1024*1024;
			
				while (!feof(f)) {
					void *buffer = XML_GetBuffer(p, len);
					size_t readed = fread(buffer, sizeof(char), len, f);
					if (XML_ParseBuffer(p, readed, false) == XML_STATUS_ERROR) {
						GCM::util::Log::d("GCM", "Error parsing reference points: Parse error.");
						break;
					}
				}
			
				XML_ParseBuffer(p, 0, false);
				XML_ParserFree(p);
			} else {
				GCM::util::Log::d("GCM", "Unable to create XML parser. Reference points not loaded.");
			}
		
			fclose(f);
		}
	}	
}

void GCM_API AppConfig::saveReferencePoints() {
	FILE *f = fopen("reference.gpx", "w");
	if (f) {
		fprintf(f, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<gpx version=\"1.1\" creator=\"GCManager\">\n");
		
		ArrayList<GCM::geolib::ReferencePoint>::iterator i = this->mReferencePoints->begin();
		while (i != this->mReferencePoints->end()) {
			if (!(*i)->isTemporary()) {
				fprintf(f, "\t<wpt lat=\"%lf\" lon=\"%lf\">\n\t\t<name>%s</name>\n\t\t<sym>Reference Point</sym>\n\t\t<type>Reference Point</type>\n\t</wpt>\n",
					(*i)->getLatitudeE6() / 1E6,
					(*i)->getLongitudeE6() / 1E6,
					(*i)->getName()
						->replace("&", "&amp;")
						->replace("<", "&lt;")
						->replace(">", "&gt;")
						->c_str());
			}
			i++;
		}
	
		fprintf(f, "</gpx>\n");
		fclose(f);
	} else {
		GCM::util::Log::e("GCM", "Unable to save reference points to file: %s", strerror(errno));
	}
}

GCM::GC<GCM::geolib::ReferencePoint> AppConfig::getActiveRefPt() {
	return this->mActiveRefPt;
}

void AppConfig::setActiveRefPt(GCM::GC<GCM::geolib::ReferencePoint> refPt) {
	this->mActiveRefPt = refPt;
}
