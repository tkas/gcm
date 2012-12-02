#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QActionGroup>
#include <geolib/GeocacheList.h>
#include <util/File.h>
#include <QAbstractItemModel>
#include <QLabel>
#include "../models/DatabaseSelectModel.h"
#include "../models/ReferencePointModel.h"

namespace Ui {
	class MainWindow;
}

class CacheTreeDelegate;
class CachesViewModel;

class MainWindow : public QMainWindow {
	Q_OBJECT

	friend class LoaderTask;
	friend class SwitchDatabaseTask;
	friend class DatabaseSelectDialog;

public:
	enum AppendMode {
		Replace,
		Add,
		Subtract,
		Intersect
	};

private:
	Ui::MainWindow *ui;
	GCM::GC<CachesViewModel> model;
	GCM::GC<GCM::geolib::GeocacheList> list;
	CacheTreeDelegate *delegate;
	QList<QAction *> popupActions;
	QAbstractItemModel *filterModel;
	DatabaseSelectModel *databaseModel;
	ReferencePointModel *refPtModel;

	int lastFilterIndex;
	int lastDbIndex;
	bool startup;

	// Actions...

	QAction *cacheDetails;
	QAction *modifyCache;
	QAction *removeCache;
	QAction *showOnline;

	QAction *addWaypoint;
	QAction *modifyWaypoint;
	QAction *removeWaypoint;

	QAction *filterAction;
	QAction *importAction;
	QAction *exportAction;

	QAction *actionDatabaseMenu;
	QMenu *menuDatabase;
	QAction *actionCopyTo;
	QAction *actionMoveTo;

	QAction *setFoundAction;

	QLabel *cachesCount;
	QLabel *listCount;

	GCM::GC< GCM::Callback<MainWindow> > cbError;

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	void setCaches(GCM::GC<GCM::geolib::GeocacheList> list, AppendMode mode = Replace);
	void addFilter(GCM::io::File filterFile);
	void addDatabase(GCM::io::File databaseFile);
	void init();
	void updateStats();
	void updateCurrentFilter();
	void updateCurrentDatabase();
	void updateStatusCounts();

protected:
	void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent *event);
	void showEvent(QShowEvent *event);

public slots:
	void showContextMenu(const QPoint &position);
	void onDoubleClick(QModelIndex);

	void onShowDetailsClick();
	void onModifyCacheClick();
	void onShowOnlineClick();
	void actionAddWaypoint();
	void actionModifyWaypoint();
	void actionRemoveWaypoint();
	void actionFoundIt();

	void onFilterClick();
	void onImportClick();
	void onExportClick();

	void filterGCID(QString);
	void filterName(QString);

	void switchFilter(int);
	void switchDatabase(int);
	void onRefChanged(int);

	void onAboutClick();
	void onQuitClick();
	void onSettingsClick();

	void onSaveListClick();
	void onOpenListClick();
	void onAddListClick();
	void onSubtractListClick();
	void onIntersectListClick();
	void onCopyClick();

	void onRefreshClick();
	void onClearClick();
	void onSearchClick();

	void onDbCreateClick();
	void onDbSelectClick(bool force = false);
	void onDbRenameClick();
	void onDbRemoveClick();
	void onDbCopyClick();

	void onRemoveFromListClick();
	void onRemoveAllBelowClick();

	void onRowChanged(const QModelIndex &, const QModelIndex &);
	void onError(void *);
};

#endif // MAINWINDOW_H
