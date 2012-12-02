#ifndef FILTERITEMMODEL_H
#define FILTERITEMMODEL_H

#include <QString>
#include <QAbstractItemModel>

#include <util/File.h>

#include <vector>

struct FilterItem {
	QString name;
	GCM::io::File file;
};

class FilterItemModel: public QAbstractItemModel {
private:
	std::vector<FilterItem *> items;

public:
	virtual ~FilterItemModel();
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual void addItem(FilterItem *f);
	virtual FilterItem *item(int row);
};

#endif // FILTERITEMMODEL_H
