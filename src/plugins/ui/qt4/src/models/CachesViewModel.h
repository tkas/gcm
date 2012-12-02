#ifndef CACHESVIEWMODEL_H
#define CACHESVIEWMODEL_H

#include <QAbstractItemModel>
#include "CacheTreeItem.h"
#include <geolib/GeocacheList.h>
#include <QColor>
#include <QIcon>
#include <QFont>
#include "main.h"

class SortTask;

class CachesViewModel: public QAbstractItemModel, public GCM::Object {
	Q_OBJECT

friend class SortTask;

private:
	CacheTreeItem *root;
	GCM::GC<GCM::geolib::GeocacheList> list;

public:
	CachesViewModel(GCM::GC<GCM::geolib::GeocacheList> list, QObject *parent = NULL);
	~CachesViewModel();
	void setList(GCM::GC<GCM::geolib::GeocacheList> list);
	GCM::GC<GCM::geolib::GeocacheList> getList();
	QVariant data(const QModelIndex &index, int role) const;
	void postDataChanged(const QModelIndex row);
	//Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	void beginInsertRows(const QModelIndex &parent, int first, int last);
	void endInsertRows();
	void beginResetModel();
	void endResetModel();
	void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
};

#endif // CACHESVIEWMODEL_H
