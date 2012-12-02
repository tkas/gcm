#include "CacheTreeDelegate.h"
#include "widgets/CacheTreeView.h"
#include <QPainter>
#include <QStyle>

#include <util/Log.h>

CacheTreeDelegate::CacheTreeDelegate(QWidget *parent): QAbstractItemDelegate(parent), dlg(new QStyledItemDelegate(parent)), widget(parent) {
}

CacheTreeDelegate::~CacheTreeDelegate() {
	delete dlg;
}

void CacheTreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	const QStyleOptionViewItemV4 *v4 = qstyleoption_cast<const QStyleOptionViewItemV4 *>(&option);
	QTextOption opt;
	opt.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	opt.setWrapMode(QTextOption::NoWrap);

	QBrush background;
	QColor foreground;
	QRect itemRect = option.rect;

	if (option.state & QStyle::State_Selected) {
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

	// Draw background
	painter->fillRect(option.rect, background);

	QStyle *style = widget->style();

	if (index.column() == 0) {
		QRect branchesRect = option.rect;
		// Extend rect to fill the branches too.
		branchesRect.setLeft(0);
		branchesRect.setRight(option.rect.left());

		painter->fillRect(branchesRect, background);

		// Draw the branches...
		((CacheTreeView *)this->parent())->drawBranches(painter, branchesRect, index);
	}

	if (index.column() == 0) {
		// Draw the cache icon.
		QVariant vicon = index.data(Qt::DecorationRole);
		if (vicon.canConvert<QIcon>()) {
			QIcon icon = vicon.value<QIcon>();
			QPixmap pixmap = icon.pixmap(QSize(16, 16), QIcon::Normal, QIcon::On);
			painter->drawPixmap(
				itemRect.left() + 2,
				itemRect.top() + (itemRect.bottom() - itemRect.top()) / 2 - 8,
				16,
				16,
				pixmap
			);
			itemRect.setLeft(itemRect.left() + 22);
		}
	}

	if ((option.state & QStyle::State_HasFocus) != 0) {
		QStyleOptionFocusRect fr;
		fr.QStyleOption::operator=(option);
		fr.initFrom(widget);
		fr.rect = itemRect;
		fr.state |= QStyle::State_KeyboardFocusChange;
		fr.state |= QStyle::State_Item;
		fr.backgroundColor = background.color();

		style->drawPrimitive(QStyle::PE_FrameFocusRect, &fr, painter, widget);
	}

	QVariant font = index.data(Qt::FontRole);
	QFont currFont = painter->font();
	if (font.canConvert<QFont>()) {
		QFont f = font.value<QFont>();
		currFont.setStrikeOut(f.strikeOut());
	} else {
		currFont.setStrikeOut(false);
	}

	painter->setFont(currFont);

	// Draw foreground
	painter->setPen(foreground);
	QString txt = this->dlg->displayText(index.data(Qt::DisplayRole), v4->locale);

	const int textMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1;
	QRect textRect = itemRect.adjusted(textMargin, 0, -textMargin, 0); // remove width padding

	painter->drawText(textRect, txt, opt);
}

QSize CacheTreeDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
	QSize out = this->dlg->sizeHint(option, index);
	if (out.height() < 22) {
		out.setHeight(22);
	}
	return out;
}
