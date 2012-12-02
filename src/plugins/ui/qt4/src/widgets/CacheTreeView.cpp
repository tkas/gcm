#include "CacheTreeView.h"
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QStyleOptionViewItemV4>
#include <QHeaderView>
#include "../models/CachesViewModel.h"
#include <util/Log.h>

CacheTreeView::CacheTreeView(QWidget *parent): QTreeView(parent) {
	//connect(this->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChanged(QItemSelection,QItemSelection)), Qt::DirectConnection);
}

/*
void CacheTreeView::drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	QModelIndex current = this->currentIndex();
	const QModelIndex parent = index.parent();
	int y = option.rect.y();

	const QStyleOptionViewItemV4 *v4 = qstyleoption_cast<const QStyleOptionViewItemV4 *>(&option);
	QTextOption opt;
	opt.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	opt.setWrapMode(QTextOption::NoWrap);

	QBrush background;
	QColor foreground;

	if (current.row() == index.row()) {
		background = option.palette.brush(QPalette::Highlight);
		foreground = option.palette.color(QPalette::HighlightedText);
	} else {
		// Get background color
		if ((v4->features & QStyleOptionViewItemV4::Alternate) != 0) {
			background = index.data(Qt::UserRole).value<QColor>();
		} else {
			background = index.data(Qt::BackgroundRole).value<QColor>();
		}

		foreground = index.data(Qt::ForegroundRole).value<QColor>();
	}

	painter->fillRect(option.rect, background);

	int width, height = option.rect.height();

	// Draw columns
	for (int headerSection = 0; headerSection < this->model()->columnCount(); ++headerSection) {
		QModelIndex modelIndex = this->model()->index(index.row(), headerSection, parent);
		int position = columnViewportPosition(headerSection);
		width = this->header()->sectionSize(headerSection);

		if (!modelIndex.isValid()) continue;

		QStyleOptionViewItemV4 opt = option;

		// Draw branches
		if (headerSection == 0) {
			opt.rect.setRect(position, y, width, height);
		} else {
			opt.rect.setRect(position, y, width, height);
		}

		// Draw the column

		this->itemDelegate()->paint(painter, opt, modelIndex);
	}



	//QTreeView::drawRow(painter, opt, index);
}
*/

void CacheTreeView::drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const {
	QTreeView::drawBranches(painter, rect, index);
}

/*
void CacheTreeView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
	QTreeView::selectionChanged(selected, deselected);
	this->repaint();
	GCM::util::Log::d("CacheTreeView", "Selection changed...");
}
*/
