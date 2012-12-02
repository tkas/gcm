/**
 * This file is used to register geolib conversion functions for the Templater.
 */

#include <templater/Templater.h>
#include <templater/Variable.h>
#include <GC.h>
#include <geolib/Geocache.h>

using namespace GCM;
using namespace GCM::templater;
using namespace GCM::geolib;

class GCM_API GeolibTemplaterFuncs {
	public:
		GeolibTemplaterFuncs() {
			Templater::registerFormatFunc("formatLatitude", formatLatitude);
			Templater::registerFormatFunc("formatLongitude", formatLongitude);
			Templater::registerFormatFunc("starsToNum", starsToNum);
			Templater::registerFormatFunc("sizeToString", sizeToString);
			Templater::registerFormatFunc("gcIdToString", gcIdToString);
			Templater::registerFormatFunc("cacheTypeToString", cacheTypeToString);
			Templater::registerFormatFunc("waypointTypeToString", waypointTypeToString);
			Templater::registerFormatFunc("logTypeToString", logTypeToString);
		}

		static GC<Variable> formatLatitude(GC<Variable> var) {
			double latitude = var->toDouble();

			char ns;

			if (latitude > 0) {
				ns = 'N';
			} else {
				ns = 'S';
				latitude *= -1;
			}

			int deg = (int)latitude;
			double min = (latitude - deg) * 60;

			return new Variable(String::format("%c%02d\xC2\xB0 %06.3f'", ns, deg, min));
		}

		static GC<Variable> formatLongitude(GC<Variable> var) {
			double longitude = var->toDouble();

			char ew;

			if (longitude > 0) {
				ew = 'E';
			} else {
				ew = 'W';
				longitude *= -1;
			}

			int deg = (int)longitude;
			double min = (longitude - deg) * 60;

			return new Variable(String::format("%c%03d\xC2\xB0 %06.3f'", ew, deg, min));
		}

		static GC<Variable> starsToNum(GC<Variable> var) {
			double val = var->toDouble();
			if ((int)(val * 2) % 2 == 1) {
				return new Variable(String::format("%1.1f", val));
			} else {
				return new Variable(String::format("%d", (int)val));
			}
		}

		static GC<Variable> sizeToString(GC<Variable> var) {
			/*switch (size->toInt()) {
				case Geocache::S_MICRO: return new Variable(String("Micro"));
				case Geocache::S_SMALL: return new Variable(String("Small"));
				case Geocache::S_REGULAR: return new Variable(String("Regular"));
				case Geocache::S_LARGE: return new Variable(String("Large"));
				case Geocache::S_NOT_LISTED: return new Variable(String("Not Choosen"));
				case Geocache::S_OTHER: return new Variable(String("Other"));
				case Geocache::S_VIRTUAL: return new Variable(String("Virtual"));
				default: return new Variable();
			}*/
			return new Variable(Geocache::sizeToGpxString((Geocache::Size)var->toInt()));
		}

		static GC<Variable> gcIdToString(GC<Variable> var) {
			return new Variable(Geocache::intToGcId(var->toInt(), ""));
		}

		static GC<Variable> cacheTypeToString(GC<Variable> var) {
			return new Variable(Geocache::typeToGpxString((Geocache::Type)var->toInt()));
		}

		static GC<Variable> waypointTypeToString(GC<Variable> var) {
			return new Variable(GeocacheWaypoint::typeToGpxString((GeocacheWaypoint::Type)var->toInt()));
		}

		static GC<Variable> logTypeToString(GC<Variable> var) {
			return new Variable(Log::typeToGpxString((Log::Type)var->toInt()));
		}
} f;
