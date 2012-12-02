#include "ReferencePointModel.h"
#include <config/AppConfig.h>
#include <geolib/ReferencePoint.h>

using namespace GCM::config;
using namespace GCM::geolib;

ReferencePointModel::ReferencePointModel()
{
}

int ReferencePointModel::columnCount(const QModelIndex &index) const {
	return 1;
	(void)index;
}

QVariant ReferencePointModel::data(const QModelIndex &index, int role) const {
	if (index.isValid() && role == Qt::DisplayRole && index.row() < this->rowCount() && index.row() >= 0) {
		GCM::GC<ReferencePoint> refPt = AppConfig::getInstance()->getReferencePoints()->at(index.row());
		return QVariant(QString::fromUtf8(refPt->getName()->c_str()));
	} else {
		return QVariant();
	}
}

QModelIndex ReferencePointModel::index(int row, int column, const QModelIndex &parent) const {
	return createIndex(row, column, 0);
	(void)parent;
}

QModelIndex ReferencePointModel::parent(const QModelIndex &index) const {
	return QModelIndex();
	(void)index;
}

int ReferencePointModel::rowCount(const QModelIndex &parent) const {
	if (!parent.isValid()) {
		return AppConfig::getInstance()->getReferencePoints()->size();
	} else {
		return 0;
	}
}

void ReferencePointModel::refresh() {
	this->beginResetModel();
	this->endResetModel();
}
