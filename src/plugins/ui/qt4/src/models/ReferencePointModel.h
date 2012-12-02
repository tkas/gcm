#ifndef REFERENCEPOINTMODEL_H
#define REFERENCEPOINTMODEL_H

#include <QAbstractItemModel>

class ReferencePointModel: public QAbstractItemModel
{
public:
	ReferencePointModel();
	virtual int columnCount(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	void refresh();
};

#endif // REFERENCEPOINTMODEL_H
