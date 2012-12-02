#ifndef _GCM_GEOCACHE_H
#	define _GCM_GEOCACHE_H

#include <gcm.h>
#include <stl/String.h>
#include "GenericWaypoint.h"
#include "LogList.h"
#include "GeocacheWaypointList.h"
#include "AttributeSet.h"
#include <stl/Modifiable.h>

namespace GCM {
	namespace geolib {

class GCM_API Geocache: public GenericWaypoint {
	public:
		typedef enum {
			T_TRADITIONAL,
			T_MULTI,
			T_UNKNOWN,
			T_LETTERBOX,
			T_EARTH,
			T_WHERIGO,
			T_VIRTUAL,
			T_EVENT,
			T_MEGAEVENT,
			T_CITO,
			T_WEBCAM
		} Type;

		typedef enum {
			S_MICRO,
			S_SMALL,
			S_REGULAR,
			S_LARGE,
			S_OTHER,
			S_NOT_LISTED,
			S_VIRTUAL
		} Size;

		typedef enum {
			ST_AVAILABLE = 0,
			ST_DISABLED = 1,
			ST_ARCHIVED = 2
		} Status;

		typedef enum {
			LOCK_NONE = 0, // Nothing
			LOCK_COORDS = 1, // Initial coordinates
			LOCK_BASIC = 2, // Type, difficulty, terrain, size
			LOCK_NONBASIC = 4, // Non-basic cache info. Do not use for setting,
			// only for reading the data for update of the cache. It does not
			// make sense to lock only nonbasic info, and allow updating of
			// basic info.
			LOCK_INFO = 6, // Basic info - name, owner, country, state...
			LOCK_LISTING = 8, // Listing and hint
			LOCK_ALL = 0xFFFFFFFF // Lock all
		} LockFlags;

	protected:
		bool mapLoaded;			/**< Basic map data is loaded. */
		bool loaded;			/**< Basic cache info is loaded. */
		bool longLoaded;		/**< Long cache data is loaded. */
		bool waypointsLoaded;	/**< Are waypoints loaded? */
		bool attributesLoaded;	/**< Are attributes loaded? */
		bool tagsLoaded;		/**< Are tags loaded? */
		bool logsLoaded;		/**< Are logs loaded? */

		int rowid;				/**< Internal ID */

		ModifiableEnum<Type> cacheType;			/**< Geocache type */
		Modifiable<String> owner;			/**< Geocache owner name */
		Modifiable<int> ownerId;			/**< Geocache owner id */
		Modifiable<String> placedBy;		/**< Geocache placed by */
		Modifiable<int> difficulty;			/**< Geocache difficulty */
		Modifiable<int> terrain;			/**< Geocache terrain */
		ModifiableEnum<Size> size;				/**< Geocache size */
		ModifiableEnum<Status> status;			/**< Geocache status */
		Modifiable<String> country;			/**< Geocache country */
		Modifiable<String> state;			/**< Geocache state */
		Modifiable<int> favPoints;			/**< Number of favourite points */
		Modifiable<int> lockFlags;			/**< Lock flags */
		Modifiable<bool> correctedCoords;	/**< Geocache has corrected coordinates. */

		Modifiable<int64_t> dtLastUpdate;	/**< Date of last update */
		Modifiable<int64_t> dtFound;			/**< Date of last found */

		Modifiable<bool> own;

		int initialLatitudeE6;	/**< When cache has corrected coords, initial coordinates are stored here. */
		int initialLongitudeE6;	/**< When cache has corrected coords, initial coordinates are stored here. */

		Modifiable<String> shortDescription; /**< Short description */
		Modifiable<bool> shortIsHTML;		/**< Short description contains HTML */
		Modifiable<String> longDescription;	/**< Long description */
		Modifiable<bool> longIsHTML;		/**< Long description contains HTML */
		Modifiable<String> hint;			/**< Hint */

		// TODO: Tags
		GC<GeocacheWaypointList> waypoints; /**< List of waypoints for this cache */
		GC<LogList> logs;		/**< List of logs */
		GC<AttributeSet> attributes; /**< Set of attributes */

		int distance;			/**< Distance to the cache from current reference point. */

	private:
		static const char *ALPHABET;

	public:
		Geocache();
		Geocache(const GenericWaypoint &wpt);
		virtual ~Geocache();

		virtual void setLatitudeE6(int latE6);
		virtual void setLongitudeE6(int lonE6);

		virtual int getRowId();
		virtual void setRowId(int id);

		virtual ModifiableEnum<Type> getCacheType();
		virtual void setCacheType(Type type);

		virtual const Modifiable<String> getOwner();
		virtual void setOwner(String owner);

		virtual Modifiable<int> getOwnerId();
		virtual void setOwnerId(int ownerId);

		virtual const Modifiable<String> getPlacedBy();
		virtual void setPlacedBy(String placedBy);

		virtual Modifiable<int> getDifficulty();
		virtual void setDifficulty(int difficulty);

		virtual Modifiable<int> getTerrain();
		virtual void setTerrain(int terrain);

		virtual ModifiableEnum<Size> getSize();
		virtual void setSize(Size size);

		virtual ModifiableEnum<Status> getCacheStatus();
		virtual bool isAvailable();
		virtual bool isDisabled();
		virtual bool isArchived();
		virtual void setCacheStatus(Status status);

		virtual const Modifiable<String> getCountry();
		virtual void setCountry(String country);

		virtual const Modifiable<String> getState();
		virtual void setState(String state);

		virtual Modifiable<int> getFavPoints();
		virtual void setFavPoints(int favPoints);

		virtual Modifiable<int64_t> getDtLastUpdate();
		virtual bool isFound();
		virtual Modifiable<int64_t> getDtFound();
		virtual void setDtLastUpdate(int64_t time);
		virtual void setDtFound(int64_t time);

		virtual void setOwn(bool own);
		virtual Modifiable<bool> isOwn();

		virtual Modifiable<String> getShortDescription();
		virtual void setShortDescription(String text);

		virtual Modifiable<String> getLongDescription();
		virtual void setLongDescription(String text);

		virtual Modifiable<bool> getSDIsHTML();
		virtual void setSDIsHTML(bool isHTML);

		virtual Modifiable<bool> getLDIsHTML();
		virtual void setLDIsHTML(bool isHTML);

		virtual Modifiable<String> getHint();
		virtual void setHint(String hint);

		virtual GC<LogList> getLogs();
		virtual GC<GeocacheWaypointList> getWaypoints();
		virtual GC<AttributeSet> getAttributes();

		virtual bool isMapLoaded();
		virtual bool isLoaded();
		virtual bool isLongLoaded();
		virtual bool isWaypointsLoaded();
		virtual bool isAttributesLoaded();
		virtual bool isTagsLoaded();
		virtual bool isLogsLoaded();

		virtual void setMapLoaded(bool isLoaded);
		virtual void setLoaded(bool isLoaded);
		virtual void setLongLoaded(bool isLoaded);
		virtual void setWaypointsLoaded(bool isLoaded);
		virtual void setAttributesLoaded(bool isLoaded);
		virtual void setTagsLoaded(bool isLoaded);
		virtual void setLogsLoaded(bool isLoaded);

		virtual Modifiable<int> getLockFlags();
		virtual bool isCoordsLocked();
		virtual bool isBasicLocked();
		virtual bool isInfoLocked();
		virtual bool isListingLocked();
		virtual void setLockFlags(int flags);
		virtual void setCoordsLocked(bool locked);
		virtual void setBasicLocked(bool locked);
		virtual void setInfoLocked(bool locked);
		virtual void setListingLocked(bool locked);
		virtual void setLockAll(bool locked);

		virtual Modifiable<bool> hasCorrectedCoords();
		virtual void setHasCorrectedCoords(bool corrected);

		virtual int getInitialLatitudeE6();
		virtual int getInitialLongitudeE6();
		virtual double getInitialLatitude();
		virtual double getInitialLongitude();
		virtual void setInitialLatitudeE6(int latE6);
		virtual void setInitialLongitudeE6(int lonE6);

		virtual void tryMatchOwner();

		static String typeToGpxString(Type type);
		static Type gpxStringToType(String gpxString);
		static Type gsIdToType(int id);

		static Size gpxStringToSize(String gpxString);
		static String sizeToGpxString(Size size);
		static Size gsIdToSize(int id);

		static int gcIdToInt(const char *gcid);
		static int gcIdToInt(String gcid);
		static String intToGcId(int id, const char *prefix = "GC");
};

	} // namespace geolib
} // namespace GCM

#include "GeocacheWaypoint.h"

#endif
