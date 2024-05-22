#ifndef _GCM_GEOLIB_LOG_H
# define _GCM_GEOLIB_LOG_H

#include <gcm.h>
#include <GC.h>
#include <stl/String.h>

namespace GCM {
	namespace geolib {

class Geocache;

class GCM_API Log: public Object {
	public:
		typedef enum {
			T_FOUND_IT,
			T_UNKNOWN,
			T_WEBCAM_PHOTO_TAKEN,
			T_DNF,
			T_WRITE_NOTE,
			T_NEEDS_ARCHIVED,
			T_NEEDS_MAINTENANCE,
			T_WILL_ATTEND,
			T_ATTENDED,
			T_REVIEWER_NOTE,
			T_OWNER_MAINTENANCE,
			T_UPDATE_COORDINATES,
			T_DISABLE,
			T_ENABLE,
			T_ARCHIVE,
			T_UNARCHIVE,
			T_ANNOUNCEMENT,
			T_PUBLISH,
			T_RETRACT,
			T_SUBMIT
		} Type;

	private:
		int id;
		time_t time;
		Type type;
		int finderId;
		String finder;
		String text;
		bool textEncoded;
		WeakGC<Geocache> cache;

	public:
		GC<Geocache> getCache();
		void setCache(GC<Geocache>);

		int getId();
		void setId(int id);
		time_t getTime();
		void setTime(time_t time);
		Type getType();
		void setType(Type type);
		int getFinderId();
		void setFinderId(int finderId);
		String getFinder();
		void setFinder(String finder);
		String getText();
		void setText(String text);
		bool isTextEncoded();
		void setTextEncoded(bool encoded);

		static String typeToGpxString(Type type);
		static Type gpxStringToType(String gpxString);
		static Type gsIdToType(int id);
};

	}
}

#endif

