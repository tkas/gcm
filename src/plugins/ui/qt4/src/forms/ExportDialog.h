#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <export.h>
#include <QModelIndex>
#include <QStyledItemDelegate>
#include <QSize>

namespace Ui {
	class ExportDialog;
}

class ExportDelegate: public QStyledItemDelegate {
public:
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
		QSize size = QStyledItemDelegate::sizeHint(option, index);
		return QSize(size.width(), size.height() * 1.5);
	}
};

class ExportDialog : public QDialog {
	Q_OBJECT
public:
	ExportDialog(GCM::GC< GCM::List<GCM::ExportInfo> > allExports, QWidget *parent = 0);
	~ExportDialog();
	GCM::GC<GCM::ExportInfo> selected();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::ExportDialog *ui;
	GCM::GC< GCM::List<GCM::ExportInfo> > allExports;
	ExportDelegate *delegate;

public slots:
	void onExportDoubleClick(QModelIndex);
	void onCurrentRowChanged();
};

#endif // EXPORTDIALOG_H
