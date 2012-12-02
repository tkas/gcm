#include "FilterItemModel.h"

FilterItemModel::~FilterItemModel() {
	std::vector<FilterItem *>::iterator i = items.begin();
	while (i != items.end()) {
		delete *i;
		i++;
	}
	items.clear();
}

int FilterItemModel::columnCount(const QModelIndex &parent) const {
	return 1;
	(void)parent;
}

QVariant FilterItemModel::data(const QModelIndex &index, int role) const {
	if (role == Qt::DisplayRole && index.isValid()) {
		return QVariant(((FilterItem *)index.internalPointer())->name);
	}
	return QVariant();
}

QModelIndex FilterItemModel::index(int row, int column, const QModelIndex &parent) const {
	if (!parent.isValid() && row >= 0 && row <= (int)items.size()) {
		return createIndex(row, column, items[row]);
	} else {
		return QModelIndex();
	}
}

QModelIndex FilterItemModel::parent(const QModelIndex &index) const {
	return QModelIndex();
	(void)index;
}

int FilterItemModel::rowCount(const QModelIndex &parent) const {
	if (!parent.isValid()) {
		return this->items.size();
	} else {
		return 0;
	}
}

void FilterItemModel::addItem(FilterItem *f) {
	this->beginResetModel();
	this->items.push_back(f);
	this->endResetModel();
}

FilterItem *FilterItemModel::item(int row) {
	if (row >= 0 && row < (int)this->items.size()) {
		return this->items[row];
	} else {
		return NULL;
	}
}
