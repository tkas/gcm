#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include <QList>
#include <QAction>
#include <GC.h>
#include <geolib/Filter.h>
#include <util/File.h>
#include "MainWindow.h"

namespace Ui {
	class FilterDialog;
}

class FilterDialog : public QDialog {
	Q_OBJECT

public:
	FilterDialog(QWidget *parent = 0, GCM::GC<GCM::geolib::Filter> f = NULL);
	~FilterDialog();
	GCM::GC<GCM::geolib::Filter> getFilter();
	GCM::io::File getFilterFile();
	bool isFilterSaved();
	MainWindow::AppendMode getAppendMode();

protected:
	void changeEvent(QEvent *e);
	void loadIdentity();
	void loadFilter(GCM::GC<GCM::geolib::Filter> f);

	GCM::GC<GCM::geolib::Filter> buildFilter();
	GCM::GC<GCM::geolib::Filter> constructFilter();

	QList<QAction *> popupActions;
	QAction *actionAfAdd;
	QAction *actionAfSubtract;
	QAction *actionAfIntersect;

	QMenu *applyMenu;
	QAction *actionMenuAdd;
	QAction *actionMenuSub;
	QAction *actionMenuIntersect;

private:
	Ui::FilterDialog *ui;
	GCM::GC<GCM::geolib::Filter> filter;
	int lastAdvancedRow;
	bool filterSaved;
	GCM::io::File filterFile;
	MainWindow::AppendMode appendMode;

public slots:
	void onBtnAdvancedClick();
	void onBtnApplyClick();
	void onApplyAdd();
	void onApplySub();
	void onApplyIntersect();
	void onBtnCancelClick();
	void onAccepted();
	void onDiffFromChanged(int value);
	void onDiffToChanged(int value);
	void onTerrFromChanged(int value);
	void onTerrToChanged(int value);

	void onBtnCacheAllClicked();
	void onBtnCacheNoneClicked();
	void onBtnCacheInvertClicked();
	void onBtnWaypointAllClicked();
	void onBtnWaypointNoneClicked();
	void onBtnWaypointInvertClicked();

	void onBtnSaveClick();
	void onBtnLoadClick();

	void onAdvancedRowChanged(int row);
	void onBtnAddClick();
	void onBtnRemoveClick();
	void onAfModifyClick();
	void onAfAddClick();
	void onAfSubtractClick();
	void onAfIntersectClick();
	void showAfContextMenu(QPoint position);

	void anythingChanged();
};

#endif // FILTERDIALOG_H
