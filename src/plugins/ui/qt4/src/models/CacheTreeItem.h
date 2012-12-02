#ifndef CACHETREEITEM_H
#define CACHETREEITEM_H

#include <QVariant>
#include <QList>
#include <QDateTime>
#include <geolib/GeocacheList.h>
#include <geolib/Geocache.h>
#include <geolib/GeocacheWaypoint.h>
#include <geolib/GeocacheWaypointList.h>
#include <Object.h>
#include <GC.h>
#include <util/Log.h>
#include <QIcon>
#include <vector>

class CacheTreeItem: public GCM::Object {
private:
	CacheTreeItem *parentItem;
	int rowNum;

public:
	CacheTreeItem(int row, CacheTreeItem *parent = NULL): parentItem(parent), rowNum(row) {
	}

	virtual ~CacheTreeItem();

	virtual void setRow(int row) { this->rowNum = row; }
	virtual int getType() const = 0;
	virtual CacheTreeItem *child(int row) = 0;
	virtual int childCount() = 0;
	virtual int columnCount() const = 0;
	virtual QVariant data(int column) const = 0;
	virtual int row() { return this->rowNum; }
	virtual QIcon icon(int column) = 0;
	virtual void sort(int column, Qt::SortOrder order) { (void)column; (void)order; }

	virtual CacheTreeItem *parent() {
		return this->parentItem;
	}
};

class CacheTreeWaypoint: public CacheTreeItem {
private:
	GCM::GC<GCM::geolib::GeocacheWaypoint> waypoint;

public:
	CacheTreeWaypoint(int row, GCM::GC<GCM::geolib::GeocacheWaypoint> wpt, CacheTreeItem *parent = NULL);

	virtual int getType() const;
	GCM::GC<GCM::geolib::GeocacheWaypoint> getWaypoint();

	virtual CacheTreeItem *child(int row);
	virtual int childCount();
	virtual int columnCount() const;
	virtual QVariant data(int column) const;
	virtual QIcon icon(int column);
};

class CacheTreeGeocache: public CacheTreeItem {
private:
	GCM::GC<GCM::geolib::Geocache> geocache;
	std::vector<CacheTreeWaypoint *> childs;

public:
	CacheTreeGeocache(int row, GCM::GC<GCM::geolib::Geocache> cache, CacheTreeItem *parent = NULL);
	virtual ~CacheTreeGeocache();

	virtual int getType() const;
	GCM::GC<GCM::geolib::Geocache> getCache();

	virtual CacheTreeItem *child(int row);
	virtual void initChilds();
	virtual int childCount();
	virtual int columnCount() const;
	virtual QVariant data(int column) const;
	virtual QIcon icon(int column);
};

class CacheTreeRoot: public CacheTreeItem {
private:
	std::vector<CacheTreeGeocache *> childs;

public:
	CacheTreeRoot(GCM::GC<GCM::geolib::GeocacheList> list, CacheTreeItem *parent = NULL);
	virtual ~CacheTreeRoot();
	virtual int getType() const;
	virtual CacheTreeItem *child(int row);
	virtual int childCount();
	virtual int columnCount() const;
	virtual QVariant data(int column) const;
	virtual QIcon icon(int column);
	virtual void sort(int column, Qt::SortOrder order);

	static bool AscGcIdPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
	static bool AscCacheNamePredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
	static bool AscCacheDistancePredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
	static bool AscCoordsPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
	static bool AscPlacedByPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
	static bool AscPlacedPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
	static bool AscContainerPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
	static bool AscDifficultyPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
	static bool AscTerrainPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
	static bool DescGcIdPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
	static bool DescCacheNamePredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
	static bool DescCacheDistancePredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
	static bool DescCoordsPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
	static bool DescPlacedByPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
	static bool DescPlacedPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
	static bool DescContainerPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
	static bool DescDifficultyPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
	static bool DescTerrainPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2);
};

#endif // CACHETREEITEM_H
