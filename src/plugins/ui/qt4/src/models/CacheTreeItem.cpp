#include "CacheTreeItem.h"
#include <QtAlgorithms>
#include <QtGeolib.h>

CacheTreeItem::~CacheTreeItem() {
}

CacheTreeWaypoint::CacheTreeWaypoint(int row, GCM::GC<GCM::geolib::GeocacheWaypoint> wpt, CacheTreeItem *parent):
	CacheTreeItem(row, parent), waypoint(wpt) {
}

int CacheTreeWaypoint::getType() const { return 2; }
GCM::GC<GCM::geolib::GeocacheWaypoint> CacheTreeWaypoint::getWaypoint() { return this->waypoint; }

CacheTreeItem *CacheTreeWaypoint::child(int row) {
	return NULL;
	(void)row;
}

int CacheTreeWaypoint::childCount() {
	return 0;
}

int CacheTreeWaypoint::columnCount() const {
	return 4;
}

QVariant CacheTreeWaypoint::data(int column) const {
	switch (column) {
	case 0:	return QString::fromUtf8(this->waypoint->getId()->c_str()).left(2);
	case 1: return QString::fromUtf8(this->waypoint->getName()->c_str());
	case 2: return QString::fromUtf8(this->waypoint->formatDistance()->c_str());
	case 3: return QString::fromUtf8(this->waypoint->formatCoords()->c_str());
	default: return "";
	}
}

QIcon CacheTreeWaypoint::icon(int column) {
	if (column == 0) {
		return QtGeolib::waypointIconSmall(this->waypoint);
	}
	return QIcon();
}

CacheTreeGeocache::CacheTreeGeocache(int row, GCM::GC<GCM::geolib::Geocache> cache, CacheTreeItem *parent): CacheTreeItem(row, parent), geocache(cache) {
	//GCM::util::Log::d("Qt4GUI", "Created geocache node %s", this->geocache->getName()->c_str());
	this->initChilds();
}

CacheTreeGeocache::~CacheTreeGeocache() {
	std::vector<CacheTreeWaypoint *>::iterator i = this->childs.begin();
	while (i != this->childs.end()) {
		delete *i;
		i++;
	}
}

int CacheTreeGeocache::getType() const { return 1; }
GCM::GC<GCM::geolib::Geocache> CacheTreeGeocache::getCache() { return this->geocache; }

CacheTreeItem *CacheTreeGeocache::child(int row) {
	return this->childs[row];
}

void CacheTreeGeocache::initChilds() {
	// Clear existing waypoints
	std::vector<CacheTreeWaypoint *>::iterator ci = this->childs.begin();
	while (ci != this->childs.end()) {
		delete *ci;
		ci++;
	}
	this->childs.clear();

	GCM::GC<GCM::geolib::GeocacheWaypointList> wpts = this->geocache->getWaypoints();
	GCM::geolib::GeocacheWaypointList::iterator i = wpts->begin();
	int num = 0;
	while (i != wpts->end()) {
		this->childs.push_back(new CacheTreeWaypoint(num++, *i, this));
		i++;
	}
}

int CacheTreeGeocache::childCount() {
	return this->childs.size();
}

int CacheTreeGeocache::columnCount() const {
	return 9;
}

QVariant CacheTreeGeocache::data(int column) const {
	switch (column) {
	case 0: return QString::fromUtf8(this->geocache->getId()->c_str());
	case 1: return QString::fromUtf8(this->geocache->getName()->c_str());
	case 2: return QString::fromUtf8(this->geocache->formatDistance()->c_str());
	case 3: return QString::fromUtf8(this->geocache->formatCoords()->c_str());
	case 4: return QString::fromUtf8(this->geocache->getPlacedBy()->c_str());
	case 5: return QDateTime::fromTime_t(this->geocache->getDtCreated()).date();
	case 6: return GCM::geolib::Geocache::sizeToGpxString(this->geocache->getSize())->c_str();
	case 7: return this->geocache->getDifficulty() / 10.0;
	case 8: return this->geocache->getTerrain() / 10.0;
	default: return "";
	}
}

QIcon CacheTreeGeocache::icon(int column) {
	if (column == 0) {
		return QtGeolib::cacheIconSmall(this->geocache);
	}
	return QIcon();
}

CacheTreeRoot::CacheTreeRoot(GCM::GC<GCM::geolib::GeocacheList> list, CacheTreeItem *parent): CacheTreeItem(0, parent) {
	if (!list) return;

	GCM::geolib::GeocacheList::iterator i = list->begin();

	int num = 0;
	this->childs.reserve(list->size());
	while (i != list->end()) {
		this->childs.push_back(new CacheTreeGeocache(num++, *i, this));
		i++;
	}
}

CacheTreeRoot::~CacheTreeRoot() {
	std::vector<CacheTreeGeocache *>::iterator i = this->childs.begin();
	while (i != this->childs.end()) {
		delete *i;
		i++;
	}
}

int CacheTreeRoot::getType() const { return 0; }

CacheTreeItem *CacheTreeRoot::child(int row) {
	return this->childs[row];
}

int CacheTreeRoot::childCount() {
	return this->childs.size();
}

int CacheTreeRoot::columnCount() const {
	return 9;
}

QVariant CacheTreeRoot::data(int column) const {
	switch (column) {
	case 0: return QVariant("GC Code");
	case 1: return QVariant("Name");
	case 2: return QVariant("Distance");
	case 3: return QVariant("Coordinates");
	case 4: return QVariant("Placed by");
	case 5: return QVariant("Placed");
	case 6: return QVariant("Container");
	case 7: return QVariant("Difficulty");
	case 8: return QVariant("Terrain");
	default: return QVariant();
	}
}

QIcon CacheTreeRoot::icon(int column) {
	return QIcon();
	(void)column;
}

void CacheTreeRoot::sort(int column, Qt::SortOrder order) {
	if (order == Qt::AscendingOrder) {
		switch (column) {
		case 0: qSort(this->childs.begin(), this->childs.end(), AscGcIdPredicate); break;
		case 1: qSort(this->childs.begin(), this->childs.end(), AscCacheNamePredicate); break;
		case 2: qSort(this->childs.begin(), this->childs.end(), AscCacheDistancePredicate); break;
		case 3: qSort(this->childs.begin(), this->childs.end(), AscCoordsPredicate); break;
		case 4: qSort(this->childs.begin(), this->childs.end(), AscPlacedByPredicate); break;
		case 5: qSort(this->childs.begin(), this->childs.end(), AscPlacedPredicate); break;
		case 6: qSort(this->childs.begin(), this->childs.end(), AscContainerPredicate); break;
		case 7: qSort(this->childs.begin(), this->childs.end(), AscDifficultyPredicate); break;
		case 8: qSort(this->childs.begin(), this->childs.end(), AscTerrainPredicate); break;
		}
	} else {
		switch (column) {
		case 0: qSort(this->childs.begin(), this->childs.end(), DescGcIdPredicate); break;
		case 1: qSort(this->childs.begin(), this->childs.end(), DescCacheNamePredicate); break;
		case 2: qSort(this->childs.begin(), this->childs.end(), DescCacheDistancePredicate); break;
		case 3: qSort(this->childs.begin(), this->childs.end(), DescCoordsPredicate); break;
		case 4: qSort(this->childs.begin(), this->childs.end(), DescPlacedByPredicate); break;
		case 5: qSort(this->childs.begin(), this->childs.end(), DescPlacedPredicate); break;
		case 6: qSort(this->childs.begin(), this->childs.end(), DescContainerPredicate); break;
		case 7: qSort(this->childs.begin(), this->childs.end(), DescDifficultyPredicate); break;
		case 8: qSort(this->childs.begin(), this->childs.end(), DescTerrainPredicate); break;
		}
	}

	std::vector<CacheTreeGeocache *>::iterator i = this->childs.begin();
	int num = 0;
	while (i != this->childs.end()) {
		(*i)->setRow(num++);
		i++;
	}

}

bool CacheTreeRoot::AscGcIdPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getRowId() < ((CacheTreeGeocache *)item2)->getCache()->getRowId();
}

bool CacheTreeRoot::AscCacheNamePredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getName()->compare(((CacheTreeGeocache *)item2)->getCache()->getName()->c_str()) < 0;
}

bool CacheTreeRoot::AscCacheDistancePredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getDistance().distance < ((CacheTreeGeocache *)item2)->getCache()->getDistance().distance;
}

bool CacheTreeRoot::AscCoordsPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getLatitudeE6() < ((CacheTreeGeocache *)item2)->getCache()->getLatitudeE6();
}

bool CacheTreeRoot::AscPlacedByPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getPlacedBy()->compare(((CacheTreeGeocache *)item2)->getCache()->getPlacedBy()->c_str()) < 0;
}

bool CacheTreeRoot::AscPlacedPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getDtCreated() < ((CacheTreeGeocache *)item2)->getCache()->getDtCreated();
}

bool CacheTreeRoot::AscContainerPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getSize() < ((CacheTreeGeocache *)item2)->getCache()->getSize();
}

bool CacheTreeRoot::AscDifficultyPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getDifficulty() < ((CacheTreeGeocache *)item2)->getCache()->getDifficulty();
}

bool CacheTreeRoot::AscTerrainPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getTerrain() < ((CacheTreeGeocache *)item2)->getCache()->getTerrain();
}

bool CacheTreeRoot::DescGcIdPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getRowId() > ((CacheTreeGeocache *)item2)->getCache()->getRowId();
}

bool CacheTreeRoot::DescCacheNamePredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getName()->compare(((CacheTreeGeocache *)item2)->getCache()->getName()->c_str()) > 0;
}

bool CacheTreeRoot::DescCacheDistancePredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getDistance().distance > ((CacheTreeGeocache *)item2)->getCache()->getDistance().distance;
}

bool CacheTreeRoot::DescCoordsPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getLatitudeE6() > ((CacheTreeGeocache *)item2)->getCache()->getLatitudeE6();
}

bool CacheTreeRoot::DescPlacedByPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getPlacedBy()->compare(((CacheTreeGeocache *)item2)->getCache()->getPlacedBy()->c_str()) > 0;
}

bool CacheTreeRoot::DescPlacedPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getDtCreated() > ((CacheTreeGeocache *)item2)->getCache()->getDtCreated();
}

bool CacheTreeRoot::DescContainerPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getSize() > ((CacheTreeGeocache *)item2)->getCache()->getSize();
}

bool CacheTreeRoot::DescDifficultyPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getDifficulty() > ((CacheTreeGeocache *)item2)->getCache()->getDifficulty();
}

bool CacheTreeRoot::DescTerrainPredicate(const CacheTreeGeocache *item1, const CacheTreeGeocache *item2) {
	return ((CacheTreeGeocache *)item1)->getCache()->getTerrain() > ((CacheTreeGeocache *)item2)->getCache()->getTerrain();
}
