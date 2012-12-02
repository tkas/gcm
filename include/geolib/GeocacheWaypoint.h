/*
 * GeocacheWaypoint.h
 *
 *  Created on: Feb 21, 2012
 *      Author: Niximor
 */

#ifndef GEOCACHEWAYPOINT_H_
#define GEOCACHEWAYPOINT_H_

#include "Geocache.h"
#include "GenericWaypoint.h"
#include <GC.h>
#include <stl/String.h>

namespace GCM {
	namespace geolib {

class GCM_API GeocacheWaypoint: public GenericWaypoint {
	public:
		typedef enum {
			T_FINAL,
			T_PARKING,
			T_REFERENCE,
			T_QTA,
			T_SOM,
			T_TRAILHEAD
		} Type;
		
	protected:
		WeakGC<Geocache> cache;
		int rowId;
		Type wptType;
		bool userFlag;
		String userComment;
		time_t dtUpdate;
		bool corrected;

	public:
		GeocacheWaypoint();
		GeocacheWaypoint(const GeocacheWaypoint &wpt);
		GeocacheWaypoint(const GenericWaypoint &wpt);

		virtual WeakGC<Geocache> getCache();
		virtual void setCache(GC<Geocache> cache);
		virtual int getRefCacheId();

		virtual int getRowId();
		virtual void setRowId(int rowid);

		virtual Type getWptType();
		virtual void setWptType(Type type);

		virtual bool getUserFlag();
		virtual void setUserFlag(bool flag);

		virtual String getUserComment();
		virtual void setUserComment(String comment);

		virtual time_t getDtLastUpdate();
		virtual void setDtLastUpdate(time_t dtUpdate);
		
		virtual void setUseAsCorrected(bool use);
		virtual bool useAsCorrected();

		static Type gpxStringToType(String str);
		static String typeToGpxString(Type type);
};

	} // namespace geolib
} // namespace GCM


#endif /* GEOCACHEWAYPOINT_H_ */
