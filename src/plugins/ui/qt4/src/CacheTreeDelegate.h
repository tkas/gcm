#ifndef CACHETREEDELEGATE_H
#define CACHETREEDELEGATE_H

#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>
#include <QTextOption>

class CacheTreeDelegate: public QAbstractItemDelegate {
private:
	QStyledItemDelegate *dlg;
	QWidget *widget;

public:
	CacheTreeDelegate(QWidget *parent = NULL);
	~CacheTreeDelegate();
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // CACHETREEDELEGATE_H
