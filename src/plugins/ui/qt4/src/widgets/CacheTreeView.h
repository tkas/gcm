#ifndef CACHETREEVIEW_H
#define CACHETREEVIEW_H

#include <QtGui/QTreeView>


class CacheTreeView : public QTreeView {
public:
	CacheTreeView(QWidget *parent = NULL);
	//virtual void drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const;
	virtual void drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const;
	//virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
};

#endif // CACHETREEVIEW_H
