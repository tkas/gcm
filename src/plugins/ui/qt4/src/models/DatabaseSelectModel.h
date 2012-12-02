#ifndef DATABASESELECTMODEL_H
#define DATABASESELECTMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <GC.h>
#include <util/File.h>

struct DbSelectItemBundle {
	QString name;
	GCM::io::File file;
	DbSelectItemBundle(QString name, GCM::io::File file): name(name), file(file) {}
};

class DatabaseSelectModel: public QAbstractItemModel
{
public:
	DatabaseSelectModel();
	virtual int columnCount(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	void updateFiles();

protected:
	void loadFiles();

private:
	QList<DbSelectItemBundle> items;
};

#endif // DATABASESELECTMODEL_H
