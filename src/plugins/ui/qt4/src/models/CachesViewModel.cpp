#include "CachesViewModel.h"
#include <ui/BackgroundTask.h>

CachesViewModel::CachesViewModel(GCM::GC<GCM::geolib::GeocacheList> list, QObject *parent): QAbstractItemModel(parent), GCM::Object() {
	this->root = new CacheTreeRoot(list);
}

CachesViewModel::~CachesViewModel() {
	delete this->root;
}

void CachesViewModel::setList(GCM::GC<GCM::geolib::GeocacheList> list) {
	delete this->root;
	this->root = new CacheTreeRoot(list);
	this->list = list;
}

GCM::GC<GCM::geolib::GeocacheList> CachesViewModel::getList() {
	return this->list;
}

QVariant CachesViewModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid()) {
		return QVariant();
	}

	CacheTreeItem *item = static_cast<CacheTreeItem *>(index.internalPointer());
	GCM::GC<GCM::geolib::Geocache> cache;
	GCM::GC<GCM::geolib::GeocacheWaypoint> waypoint;

	if (item->getType() == 1) { cache = ((CacheTreeGeocache *)item)->getCache(); }
	if (item->getType() == 2) {
		waypoint = ((CacheTreeWaypoint *)item)->getWaypoint();
		cache = ((CacheTreeGeocache *)item->parent())->getCache();
	}

	switch (role) {
		// Display the data
		case Qt::DisplayRole: return item->data(index.column());

		// Draw background color
		case Qt::BackgroundRole:
			if (cache->isOwn()) {
				return QColor(227, 177, 226);
			} else if (cache->isFound()) {
				return QColor(223, 245, 220);
			// Cache is archived
			} else if (cache->isArchived()) {
				return QColor(252, 215, 215);
			// Cache with waypoints
			} else if (cache->getWaypoints()->size() > 0) {
				return QColor(252, 246, 194);
			}
			return QColor(Qt::white);

		// Alternating background color
		case Qt::UserRole:
			if (cache->isOwn()) {
				return QColor(212, 163, 211);
			} else if (cache->isFound()) {
				return QColor(194, 227, 191);
			// Cache is archived
			} else if (cache->isArchived()) {
				return QColor(237, 187, 187);
			// Cache with waypoints
			} else if (cache->getWaypoints()->size() > 0) {
				return QColor(235, 230, 197);
			}
			return QColor(242, 242, 242);

		// Draw foreground color
		case Qt::ForegroundRole:
			if (cache->isDisabled()) {
				return QColor(128, 128, 128);
			} else {
				return QColor(Qt::black);
			}

		// Item icon
		case Qt::DecorationRole:
			return item->icon(index.column());

		case Qt::FontRole:
			if (cache->isDisabled() || cache->isArchived()) {
				QFont f;
				f.setStrikeOut(true);
				return f;
			} else {
				return QVariant();
			}

		// Other...
		default: return QVariant();
	}
}

void CachesViewModel::postDataChanged(const QModelIndex row) {
	CacheTreeItem *item = (CacheTreeItem *)row.internalPointer();

	QModelIndex topLeft = createIndex(row.row(), 0, row.internalPointer());
	QModelIndex bottomRight = createIndex(row.row() + 1, item->columnCount() - 1, row.internalPointer());

	emit dataChanged(topLeft, bottomRight);
}

/*Qt::ItemFlags flags(const QModelIndex &index) const {
	if (!index.isValid()) return 0;
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}*/

QVariant CachesViewModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		return root->data(section);
	}

	return QVariant();
}

QModelIndex CachesViewModel::index(int row, int column, const QModelIndex &parent) const {
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	CacheTreeItem *parentItem;
	if (!parent.isValid()) {
		parentItem = this->root;
	} else {
		parentItem = static_cast<CacheTreeItem *>(parent.internalPointer());
	}

	CacheTreeItem *childItem = parentItem->child(row);
	if (childItem) {
		return createIndex(row, column, childItem);
	} else {
		return QModelIndex();
	}
}

QModelIndex CachesViewModel::parent(const QModelIndex &index) const {
	if (!index.isValid()) {
		return QModelIndex();
	}

	CacheTreeItem *childItem = static_cast<CacheTreeItem *>(index.internalPointer());
	CacheTreeItem *parentItem = childItem->parent();

	if (parentItem == this->root) {
		return QModelIndex();
	}

	return createIndex(parentItem->row(), 0, parentItem);
}

int CachesViewModel::rowCount(const QModelIndex &parent) const {
	if (parent.column() > 0) {
		return 0;
	}

	CacheTreeItem *parentItem;

	if (!parent.isValid()) {
		parentItem = this->root;
	} else {
		parentItem = static_cast<CacheTreeItem *>(parent.internalPointer());
	}

	return parentItem->childCount();
}

int CachesViewModel::columnCount(const QModelIndex &parent) const {
	if (parent.isValid()) {
		return static_cast<CacheTreeItem *>(parent.internalPointer())->columnCount();
	} else {
		return this->root->columnCount();
	}
}

void CachesViewModel::beginInsertRows(const QModelIndex &parent, int first, int last) {
	QAbstractItemModel::beginInsertRows(parent, first, last);
}

void CachesViewModel::endInsertRows() {
	QAbstractItemModel::endInsertRows();
}

void CachesViewModel::beginResetModel() {
	QAbstractItemModel::beginResetModel();
}

void CachesViewModel::endResetModel() {
	QAbstractItemModel::endResetModel();
}

class SortTask: public GCM::ui::BackgroundTask {
private:
	GCM::GC<CachesViewModel> model;
	int column;
	Qt::SortOrder order;

public:
	SortTask(GCM::GC<CachesViewModel> model, int column, Qt::SortOrder order): model(model), column(column), order(order) {
	}

	virtual void onPreExecute() {
		model->beginResetModel();
	}

	virtual void doInBackground() {
		model->root->sort(column, order);
	}

	virtual void onPostExecute() {
		model->endResetModel();
	}
};

void CachesViewModel::sort(int column, Qt::SortOrder order) {
	if (this->rowCount() > 0) {
		::ui->addTask(new SortTask(this, column, order));
	}
}
