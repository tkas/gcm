#include <geolib/LogList.h>
#include <ctype.h>
#include <config/AppConfig.h>
#include <stl/String.h>

using namespace GCM::geolib;
using namespace GCM::config;
using namespace GCM;

GC<Log> LogList::append(GC<Log> log) {
	// Append to set
	List<Log>::append(log);
	
	// Try to match user name of log
	if (log->getCache() && !log->getCache()->isFound()) {
		AppConfig *cfg = AppConfig::getInstance();
		const char *nick;
		if ((nick = cfg->getString("username", NULL)) != NULL) {
			if ((log->getType() == Log::T_FOUND_IT || log->getType() == Log::T_ATTENDED || log->getType() == Log::T_WEBCAM_PHOTO_TAKEN) && log->getFinder()->equals(nick)) {
				int logtm = 0;
			
				// Try to find time in log 
				const char *text = log->getText()->c_str();
				size_t len = log->getText()->length();
				for (size_t i = 0; i < len; i++) {
					if (text[i] == ':' && i >= 1 && i < len - 1) {
						// Try to find forward and backward numbers.
						
						// Forward first
						if (isdigit(text[i+1]) && (i == len - 2 || isdigit(text[i+2]))) {
							int min = atoi(text + i + 1);
							if (min >= 0 && min <= 59) {
								// Found forward correct minute, search backwards.
								size_t t = i - 1;
								while (t > 0 && isdigit(text[t])) {
									t--;
								}
								
								int hour = atoi(text + t);
								
								if (hour >= 0 && hour <= 23) {
									// Found correct time, set it and stop searching.
									logtm = hour * 3600 + min * 60;
 									break;
								}
							}
						}
					}
				}
		
				log->getCache()->setDtFound(log->getTime() + logtm);
			}
		}
	}
	
	return log;
}
