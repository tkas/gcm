#include "DatabaseSelectModel.h"
#include <util/File.h>

using namespace GCM::io;

class Db3NameFilter: public FileFilter {
	bool accept(File dir, File file) const {
		if (file->getName()->endsWith(".db3")) return true;
		else return false;
		(void)dir;
	}
};

DatabaseSelectModel::DatabaseSelectModel()
{
	this->updateFiles();
}

int DatabaseSelectModel::columnCount(const QModelIndex &index) const {
	return 1;
	(void)index;
}

QVariant DatabaseSelectModel::data(const QModelIndex &index, int role) const {
	if (role == Qt::DisplayRole && index.isValid()) {
		DbSelectItemBundle *item = (DbSelectItemBundle *)index.internalPointer();
		return QVariant(item->name);
	} else {
		return QVariant();
	}
}

QModelIndex DatabaseSelectModel::index(int row, int column, const QModelIndex &parent) const {
	if (column == 0 && row >= 0 && row < this->items.count() && !parent.isValid()) {
		return createIndex(row, column, (DbSelectItemBundle *)&(this->items[row]));
	} else {
		return QModelIndex();
	}
}

QModelIndex DatabaseSelectModel::parent(const QModelIndex &index) const {
	return QModelIndex();
	(void)index;
}

int DatabaseSelectModel::rowCount(const QModelIndex &parent) const {
	if (!parent.isValid()) {
		return this->items.count();
	} else {
		return 0;
	}
}

void DatabaseSelectModel::loadFiles() {
	this->items.clear();
	File curDir("");
	FileList list = curDir->listFiles(new Db3NameFilter);
	FileList::iterator i = list->begin();
	while (i != list->end()) {
		this->items.append(DbSelectItemBundle(QString::fromUtf8((*i)->getBasename(".db3")->c_str()), *i));
		i++;
	}
}

void DatabaseSelectModel::updateFiles() {
	this->beginResetModel();
	this->loadFiles();
	this->endResetModel();
}
