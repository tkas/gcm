#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "../main.h"

#include <GC.h>
#include <stdio.h>
#include <unistd.h>

#include <config/AppConfig.h>
#include <database/Database.h>
#include <pluginmanager/PluginManager.h>
#include <pluginmanager/UnsupportedFunctionException.h>
#include <pluginmanager/NoPluginSelectedException.h>
#include <util/Log.h>
#include <util/File.h>
#include <ui/BackgroundTask.h>
#include <import.h>
#include <export.h>

#include <ui/Command.h>

#include <geolib/GeocacheList.h>
#include <geolib/Filter.h>
#include <geolib/filter/MatchAll.h>
#include <geolib/ReferencePoint.h>

#include "AboutDialog.h"
#include "WaypointDialog.h"
#include "FilterDialog.h"
#include "CacheEditDialog.h"
#include "CacheDetailsDialog.h"
#include "ExportDialog.h"
#include "CreateDatabaseDialog.h"
#include "DatabaseSelectDialog.h"
#include "DatabaseRenameDialog.h"
#include "SetFoundDialog.h"
#include "SettingsDialog.h"
#include "SearchDialog.h"

#include "../models/CachesViewModel.h"
#include "../models/FilterItemModel.h"
#include "../models/DatabaseSelectModel.h"
#include "../widgets/CacheTreeView.h"

#include "../ListFilter.h"
#include "../CacheTreeDelegate.h"
#include "../CommandMenuAdapter.h"

#include "../tasks/LoaderTask.h"
#include "../tasks/ImportTask.h"
#include "../tasks/ExportTask.h"
#include "../tasks/SwitchDatabaseTask.h"
#include "../tasks/InitTask.h"
#include "../tasks/ErrorTask.h"

#include <QTreeView>
#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QIcon>
#include <QRegExp>
#include <QModelIndex>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QFrame>
#include <QHBoxLayout>
#include <QIODevice>
#include <QTextStream>
#include <QClipboard>
#include <QAction>
#include <QList>

#include <map>

using namespace GCM::ui;
using namespace GCM::config;
using namespace GCM::io;
using namespace GCM::pluginmanager;

class CopyTask: public BackgroundTask {
public:
	typedef enum {
		Copy,
		Move
	} Mode;

	CopyTask(GCM::GC<GCM::geolib::GeocacheList> caches,
		GCM::GC<GCM::database::DatabaseInfo> target,
		GCM::String fileName, Mode mode, ptrDialog dialog):
		caches(caches), target(target), fileName(fileName), mode(mode), dialog(dialog) {}

	void onPreExecute() {
		if (dialog) {
			dialog->show();
		}
	}

	void onPostExecute() {
		if (dialog) {
			dialog->dismiss();
		}
	}

	void doInBackground() {
		GCM::GC<GCM::database::Database> db = target->open(this->fileName->c_str());
		db->beginTransaction();

		GCM::GC<GCM::database::Database> olddb;

		if (dialog) {
			dialog->setMaxProgress(this->caches->size());
			dialog->setProgress(0);
		}

		GCM::geolib::GeocacheList::iterator i = this->caches->begin();
		while (i != this->caches->end()) {
			GCM::GC<GCM::geolib::Geocache> cache = *i;

			if (cache->getDbProvider()) {
				olddb = cache->getDbProvider();
				if (!cache->isAttributesLoaded()) {
					olddb->loadAttributes(cache);
				}

				if (!cache->isLogsLoaded()) {
					olddb->loadLogs(cache);
				}

				if (!cache->isLongLoaded()) {
					olddb->loadLongData(cache);
				}

				if (!cache->isWaypointsLoaded()) {
					olddb->loadWaypoints(cache);
				}
			}

			if (olddb) {
				olddb->beginTransaction();
				if (mode == Move) {
					olddb->removeCache(cache);

				}

				if (db->saveCache(cache) != GCM::database::Database::ST_FAILED) {
					olddb->commit();
				}
			} else {
				db->saveCache(cache);
			}

			if (dialog) {
				dialog->setProgress(dialog->getProgress() + 1);
			}

			i++;
		}

		db->commit();
	}

private:
	GCM::GC<GCM::geolib::GeocacheList> caches;
	GCM::GC<GCM::database::DatabaseInfo> target;
	GCM::String fileName;
	Mode mode;
	ptrDialog dialog;
};

class DatabaseSubmenu: public CommandMenu {
private:
	MainWindow *wnd;

public:
	class Copy: public Command {
	private:
		MainWindow *wnd;

	public:
		Copy(MainWindow *wnd): Command("Copy", "copy"), wnd(wnd) {};

		void triggered() {
			DatabaseSelectDialog dlg(wnd);
			if (dlg.exec() == QDialog::Accepted) {
				File dbFile = dlg.getDatabaseFile();

				PluginManager::PluginFunctionList &funcs = PluginManager::Instance()->listPlugins("Database");
				PluginManager::PluginFunctionList::iterator i = funcs.begin();
				while (i != funcs.end()) {
					GCM::database::fDatabase call = (GCM::database::fDatabase)i->second;
					GCM::GC< GCM::List<GCM::database::DatabaseInfo> > info = call();

					GCM::List<GCM::database::DatabaseInfo>::iterator ii = info->begin();
					while (ii != info->end()) {
						if ((*ii)->accept(dbFile->getAbsolutePath()->c_str())) {
							::ui->addTask(new CopyTask(
								this->getSelectedCaches(),
								*ii,
								dbFile->getAbsolutePath(),
								CopyTask::Copy,
								::ui->getDialogBuilder()->buildProgressDialog()
							));
							break;
						}
						ii++;
					}

					if (ii != info->end()) {
						break;
					}

					i++;
				}
			}
		}
	};

	class Move: public Command {
	private:
		MainWindow *wnd;

	public:
		Move(MainWindow *wnd): Command("Move", "move"), wnd(wnd) {};

		void triggered() {
			DatabaseSelectDialog dlg(wnd);
			if (dlg.exec() == QDialog::Accepted) {
				File dbFile = dlg.getDatabaseFile();

				PluginManager::PluginFunctionList &funcs = PluginManager::Instance()->listPlugins("Database");
				PluginManager::PluginFunctionList::iterator i = funcs.begin();
				while (i != funcs.end()) {
					GCM::database::fDatabase call = (GCM::database::fDatabase)i->second;
					GCM::GC< GCM::List<GCM::database::DatabaseInfo> > info = call();

					GCM::List<GCM::database::DatabaseInfo>::iterator ii = info->begin();
					while (ii != info->end()) {
						if ((*ii)->accept(dbFile->getAbsolutePath()->c_str())) {
							::ui->addTask(new CopyTask(
								this->getSelectedCaches(),
								*ii,
								dbFile->getAbsolutePath(),
								CopyTask::Move,
								::ui->getDialogBuilder()->buildProgressDialog()
							));
							break;
						}
						ii++;
					}

					if (ii != info->end()) {
						break;
					}

					i++;
				}
			}
		}
	};
	
  class Delete: public Command {
	private:
		MainWindow *wnd;

	public:
		Delete(MainWindow *wnd): Command("Delete", "delete"), wnd(wnd) {};

		void triggered() {
      ptrDialog dialog = ::ui->getDialogBuilder()->buildProgressDialog();

      dialog->setMaxProgress(this->getSelectedCaches()->size());
      dialog->setProgress(0);
      dialog->show();

      GCM::GC<GCM::database::Database> db;
      GCM::geolib::GeocacheList::iterator i = this->getSelectedCaches()->begin();

      while (i != this->getSelectedCaches()->end()) {
        GCM::GC<GCM::geolib::Geocache> cache = *i;

        if (cache->getDbProvider()) {
          db = cache->getDbProvider();
          db->removeCache(cache);
        }

        dialog->setProgress(dialog->getProgress() + 1);
        i++;
      }
      dialog->dismiss();
		}
	};

	DatabaseSubmenu(MainWindow *wnd): CommandMenu("Database", "database"), wnd(wnd) {
		this->addCommand(new Copy(wnd));
		this->addCommand(new Move(wnd));
		this->addCommand(new Delete(wnd));
	}
};

MainWindow::MainWindow(QWidget *parent) :
		QMainWindow(parent),
		ui(new Ui::MainWindow),
		delegate(NULL),
		lastFilterIndex(0),
		lastDbIndex(0),
		startup(true)
{
	ui->setupUi(this);

	cbError = new GCM::Callback<MainWindow>(this, &MainWindow::onError);

	GCM::util::Log::events.bind("error", cbError.getObj());

	// Setup the caches view...
	this->model = new CachesViewModel(new GCM::geolib::GeocacheList());

	QItemSelectionModel *m = ui->cachesView->selectionModel();
	ui->cachesView->setSelectionMode( QAbstractItemView::ExtendedSelection );
	ui->cachesView->setModel(this->model.getObj());
	delete m;
	connect(ui->cachesView->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(onRowChanged(const QModelIndex &, const QModelIndex &)));

	ui->cachesView->setAlternatingRowColors(true);
	this->delegate = new CacheTreeDelegate(ui->cachesView);
	ui->cachesView->setItemDelegate(this->delegate);

	ui->cachesView->setContextMenuPolicy(Qt::CustomContextMenu);

	// TODO: Support moving of columns
	//ui->cachesView->header()->setMovable(false);

	connect(ui->cachesView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));
	connect(ui->cachesView->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(onRowChanged(const QModelIndex &, const QModelIndex &)));

	// Prepare context menu for treeview
	cacheDetails = new QAction(QIcon(":/icons/16x16/ic_details.png"), QString::fromUtf8("Cache details..."), NULL);
	connect(cacheDetails, SIGNAL(triggered()), this, SLOT(onShowDetailsClick()));

	modifyCache = new QAction(QString::fromUtf8("Edit cache..."), NULL);
	connect(modifyCache, SIGNAL(triggered()), this, SLOT(onModifyCacheClick()));

	removeCache = new QAction(QString::fromUtf8("Remove cache"), NULL);

	showOnline = new QAction(QIcon(":/icons/16x16/ic_show_online.png"), QString::fromUtf8("Show online"), NULL);
	connect(showOnline, SIGNAL(triggered()), this, SLOT(onShowOnlineClick()));

	addWaypoint = new QAction(QIcon(":/icons/16x16/ic_waypoint_add.png"), QString::fromUtf8("Add waypoint..."), NULL);
	connect(addWaypoint, SIGNAL(triggered()), this, SLOT(actionAddWaypoint()));

	modifyWaypoint = new QAction(QIcon(":/icons/16x16/ic_waypoint_modify.png"), QString::fromUtf8("Edit waypoint..."), NULL);
	connect(modifyWaypoint, SIGNAL(triggered()), this, SLOT(actionModifyWaypoint()));

	removeWaypoint = new QAction(QIcon(":/icons/16x16/ic_waypoint_remove.png"), QString::fromUtf8("Remove waypoint"), NULL);
	connect(removeWaypoint, SIGNAL(triggered()), this, SLOT(actionRemoveWaypoint()));

	setFoundAction = new QAction(QIcon(":/icons/16x16/ic_log_found.png"), QString::fromUtf8("Set found..."), NULL);
	connect(setFoundAction, SIGNAL(triggered()), this, SLOT(actionFoundIt()));

	QAction *separator = new QAction(NULL);
	separator->setSeparator(true);

	this->popupActions.append(cacheDetails);	// Details
	this->popupActions.append(modifyCache);		// Modify cache
	//this->popupActions.append(removeCache);		// Remove cache
	this->popupActions.append(showOnline);		// Show online
	this->popupActions.append(separator);		// ---
	this->popupActions.append(addWaypoint);		// Add waypoint
	this->popupActions.append(modifyWaypoint);	// Modify waypoint
	this->popupActions.append(removeWaypoint);	// Remove waypoint

	separator = new QAction(NULL);
	separator->setSeparator(true);
	this->popupActions.append(separator);		// ---
	this->popupActions.append(setFoundAction);	// Found it

	filterAction = this->ui->mainToolBar->addAction(QIcon(":/icons/16x16/ic_filter.png"), "Filter");
	connect(filterAction, SIGNAL(triggered()), this, SLOT(onFilterClick()));
	connect(this->ui->actionFilter, SIGNAL(triggered()), this, SLOT(onFilterClick()));

	importAction = this->ui->mainToolBar->addAction(QIcon(":/icons/16x16/ic_open.png"), "Import");
	connect(importAction, SIGNAL(triggered()), this, SLOT(onImportClick()));
	connect(this->ui->actionOpen_Import, SIGNAL(triggered()), this, SLOT(onImportClick()));

	exportAction = this->ui->mainToolBar->addAction(QIcon(":/icons/16x16/ic_save.png"), "Export");
	connect(exportAction, SIGNAL(triggered()), this, SLOT(onExportClick()));
	connect(this->ui->actionSave_Export, SIGNAL(triggered()), this, SLOT(onExportClick()));

	connect(this->ui->actionAbout, SIGNAL(triggered()), this, SLOT(onAboutClick()));
	connect(this->ui->actionQuit, SIGNAL(triggered()), this, SLOT(onQuitClick()));
	connect(this->ui->actionSaveList, SIGNAL(triggered()), this, SLOT(onSaveListClick()));
	connect(this->ui->actionOpenList, SIGNAL(triggered()), this, SLOT(onOpenListClick()));
	connect(this->ui->actionAddList, SIGNAL(triggered()), this, SLOT(onAddListClick()));
	connect(this->ui->actionSubtractList, SIGNAL(triggered()), this, SLOT(onSubtractListClick()));
	connect(this->ui->actionIntersectList, SIGNAL(triggered()), this, SLOT(onIntersectListClick()));
	connect(this->ui->actionCreateDatabase, SIGNAL(triggered()), this, SLOT(onDbCreateClick()));
	connect(this->ui->actionSelectDatabase, SIGNAL(triggered()), this, SLOT(onDbSelectClick()));
	connect(this->ui->actionRenameDatabase, SIGNAL(triggered()), this, SLOT(onDbRenameClick()));
	connect(this->ui->actionRemoveDatabase, SIGNAL(triggered()), this, SLOT(onDbRemoveClick()));
	connect(this->ui->actionSettings, SIGNAL(triggered()), this, SLOT(onSettingsClick()));
	connect(this->ui->actionReload_list, SIGNAL(triggered()), this, SLOT(onRefreshClick()));
	connect(this->ui->actionClear_list, SIGNAL(triggered()), this, SLOT(onClearClick()));
	connect(this->ui->actionSearch, SIGNAL(triggered()), this, SLOT(onSearchClick()));
	connect(this->ui->actionCopy, SIGNAL(triggered()), this, SLOT(onCopyClick()));
	connect(this->ui->actionRemoveFromList, SIGNAL(triggered()), this, SLOT(onRemoveFromListClick()));
	connect(this->ui->actionRemove_all_below, SIGNAL(triggered()), this, SLOT(onRemoveAllBelowClick()));

	separator = new QAction(NULL);
	separator->setSeparator(true);
	this->popupActions.append(separator);		// ---

	this->popupActions.append(this->ui->actionCopy); // Copy
	this->popupActions.append(this->ui->actionRemoveFromList); // Remove selected
	this->popupActions.append(this->ui->actionRemove_all_below); // Remove all below

	this->popupActions.append(new CommandMenuAdapter(new DatabaseSubmenu(this), this->ui->cachesView, true));

	connect(this->ui->filterGCID, SIGNAL(textChanged(QString)), this, SLOT(filterGCID(QString)));
	connect(this->ui->filterName, SIGNAL(textChanged(QString)), this, SLOT(filterName(QString)));

	this->filterModel = new FilterItemModel();
	this->ui->comboFilterSelect->setModel(this->filterModel);
	connect(this->ui->comboFilterSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(switchFilter(int)));

	this->databaseModel = new DatabaseSelectModel();
	this->ui->comboDatabase->setModel(this->databaseModel);
	connect(this->ui->comboDatabase, SIGNAL(currentIndexChanged(int)), this, SLOT(switchDatabase(int)));

	this->refPtModel = new ReferencePointModel();
	this->ui->comboReference->setModel(this->refPtModel);
	connect(this->ui->comboReference, SIGNAL(currentIndexChanged(int)), this, SLOT(onRefChanged(int)));

	connect(this->ui->cachesView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClick(QModelIndex)));

	QFrame *frame = new QFrame();

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	frame->setLayout(layout);

	QWidget *dbIcon = new QWidget();
	dbIcon->setStyleSheet("background-image:url(':/icons/16x16/ic_database.png');");
	dbIcon->setMaximumSize(16, 16);
	dbIcon->setMinimumSize(16, 16);
	dbIcon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	this->cachesCount = new QLabel();
	frame->layout()->addWidget(dbIcon);
	frame->layout()->addWidget(this->cachesCount);

	this->listCount = new QLabel();

	//this->ui->widget->layout()->addWidget(frame);
	this->ui->statusBar->addPermanentWidget(frame);
	this->ui->statusBar->addWidget(this->listCount);
}

MainWindow::~MainWindow()
{
	delete ui;
	delete filterModel;
	delete databaseModel;
	delete refPtModel;

	if (delegate != NULL) {
		delete delegate;
	}

	// Delete the actions
	QList<QAction *>::iterator i = this->popupActions.begin();
	while (i != this->popupActions.end()) {
		delete (*i);
		i++;
	}

	GCM::util::Log::events.unbind("error", cbError.getObj());
}

void MainWindow::init() {
	this->ui->comboDatabase->clear();

	this->lastFilterIndex = -1;

	// Clear the model by recreating it.
	FilterItemModel *model = (FilterItemModel *)this->ui->comboFilterSelect->model();
	this->filterModel = new FilterItemModel();
	this->ui->comboFilterSelect->setModel(this->filterModel);
	delete model;

	this->databaseModel->updateFiles();

	//this->ui->comboReference->clear();
	const char *refpt = AppConfig::getInstance()->getString("reference_point", NULL);
	if (refpt) {
		bool found = false;
		for (int index = 0; index < this->ui->comboReference->model()->rowCount(); index++) {
			if (AppConfig::getInstance()->getReferencePoints()->at(index)->getName()->equals(refpt)) {
				found = true;
				this->ui->comboReference->setCurrentIndex(index);
				break;
			}
		}

		if (!found) {
			if (this->ui->comboReference->model()->rowCount() > 0) {
				this->ui->comboReference->setCurrentIndex(0);
			} else {
				this->ui->comboReference->setCurrentIndex(-1);
			}
		}
	} else if (this->ui->comboReference->model()->rowCount() > 0) {
		this->ui->comboReference->setCurrentIndex(0);
	} else {
		this->ui->comboReference->setCurrentIndex(-1);
	}
}

void MainWindow::showEvent(QShowEvent *event) {
	if (!startup) {
		QMainWindow::showEvent(event);
		return;
	}

	startup = false;

	AppConfig *cfg = AppConfig::getInstance();
	// Do not resize last section to saved size, because it fills rest of the window.
	for (int i = 0; i < ui->cachesView->header()->count() - 1; i++) {
		QString itemName;
		itemName.sprintf("cachesview_width_%d", i);
		int size = cfg->getInt(itemName.toLatin1().data(), 0);
		if (size > 0) {
			ui->cachesView->header()->resizeSection(i, size);
		}
	}

	if (cfg->getInt("window_maximized", 0)) {
		this->setWindowState(Qt::WindowMaximized);
	} else {
		this->setGeometry(
			this->x(),
			this->y(),
			cfg->getInt("window_width", this->width()),
			cfg->getInt("window_height", this->height())
		);
	}

	this->restoreState(QByteArray::fromBase64(cfg->getString("window_state", "")));

	int sortCol;
	if ((sortCol = cfg->getInt("sort_column", -1)) >= 0) {
		this->ui->cachesView->sortByColumn(sortCol, (Qt::SortOrder)cfg->getInt("sort_direction", Qt::AscendingOrder));
	}

	this->updateStatusCounts();

	::ui->addTask(new LoaderTask(this));
	QMainWindow::showEvent(event);
}

void MainWindow::setCaches(GCM::GC<GCM::geolib::GeocacheList> list, AppendMode mode) {
	this->ui->cachesView->setSortingEnabled(false);

	GCM::geolib::GeocacheList::iterator i, curr;

	switch (mode) {
	case Replace:
		if (!list) {
			this->list = new GCM::geolib::GeocacheList();
		} else {
			this->list = list;
		}
		break;

	case Add:
		if (list) {
			i = list->begin();
			while (i != list->end()) {
				if (!this->list->contains(*i)) {
					this->list->append(*i);
				}
				i++;
			}
		}
		break;

	case Subtract:
		if (!list) {
			i = this->list->begin();
			while (i != this->list->end()) {
				curr = i;
				i++;
				this->list->remove(*curr);
			}
		} else {
			i = list->begin();
			while (i != list->end()) {
				if (this->list->contains(*i)) {
					this->list->remove(*i);
				}
				i++;
			}
		}
		break;

	case Intersect:
		if (!list) {
			i = this->list->begin();
			while (i != this->list->end()) {
				curr = i;
				i++;
				this->list->remove(*curr);
			}
		} else {
			i = this->list->begin();
			while (i != this->list->end()) {
				curr = i;
				++i;
				if (!list->contains(*curr)) {
					this->list->remove(*curr);
				}
			}
		}
		break;
	}

	// This is not needed anymore, as it is done in the database driver itself.
	/*int row = this->ui->comboReference->currentIndex();
	if (row >= 0 && row < this->ui->comboReference->model()->rowCount()) {
		GCM::GC<GCM::geolib::ReferencePoint> rp = AppConfig::getInstance()->getReferencePoints()->at(row);
		this->list->calcDistances(rp->getLatitudeE6(), rp->getLongitudeE6());
	}*/

	CachesViewModel *model = new CachesViewModel(this->list);
	this->model = model;

	QItemSelectionModel *m = ui->cachesView->selectionModel();
	ui->cachesView->setModel(this->model.getObj());
	delete m;
	connect(ui->cachesView->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(onRowChanged(const QModelIndex &, const QModelIndex &)));

	this->ui->cachesView->setSortingEnabled(true);
	this->updateStatusCounts();
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void MainWindow::closeEvent(QCloseEvent *event) {
	// Save column widths
	AppConfig *cfg = AppConfig::getInstance();

	for (int i = 0; i < ui->cachesView->header()->count(); i++) {
		QString itemName;
		itemName.sprintf("cachesview_width_%d", i);

		cfg->set(itemName.toLatin1().data(), (long int)ui->cachesView->header()->sectionSize(i));
	}

	if (this->windowState() & Qt::WindowMaximized) {
		cfg->set("window_maximized", 1);
	} else {
		cfg->set("window_maximized", 0);
		cfg->set("window_width", this->width());
		cfg->set("window_height", this->height());
	}

	cfg->set("window_state", this->saveState().toBase64().data());

	cfg->set("sort_column", this->ui->cachesView->header()->sortIndicatorSection());
	cfg->set("sort_direction", this->ui->cachesView->header()->sortIndicatorOrder());

	QMainWindow::closeEvent(event);
	::ui->quitMainLoop();
}

void MainWindow::showContextMenu(const QPoint &position) {
	QModelIndexList list = this->ui->cachesView->selectionModel()->selectedRows();
	if (list.length() > 0) {
		// If selected is waypoint, enable modify waypoint...
		QModelIndexList::iterator i = list.begin();
		while (i != list.end()) {
			CacheTreeItem *item = (CacheTreeItem *)i->internalPointer();

			if (item->getType() == 2) {
				modifyWaypoint->setEnabled(true);
				removeWaypoint->setEnabled(true);
			} else {
				modifyWaypoint->setEnabled(false);
				removeWaypoint->setEnabled(false);
			}

			i++;
		}

		QMenu::exec(this->popupActions, this->ui->cachesView->mapToGlobal(position));
	}
}

void MainWindow::actionAddWaypoint() {
	QModelIndexList list = this->ui->cachesView->selectionModel()->selectedRows();
	if (list.length() > 0) {
		QModelIndexList::iterator i = list.begin();
		while (i != list.end()) {
			CacheTreeItem *item = (CacheTreeItem *)i->internalPointer();

			if (item->getType() == 2) item = item->parent();
			CacheTreeGeocache *cacheItem = (CacheTreeGeocache *)item;
			GCM::GC<GCM::geolib::Geocache> cache = cacheItem->getCache();

			WaypointDialog dlg(cache, this);
			if (dlg.exec() == QDialog::Accepted) {
				// Create the waypoint in the model.

				// Update the cache in the tree.
				if (item->getType() == 2) {
					// Selected item is waypoint
					((CachesViewModel *)this->ui->cachesView->model())->beginResetModel();
					cacheItem->initChilds();
					((CachesViewModel *)this->ui->cachesView->model())->endResetModel();
					//this->ui->cachesView->update(i->parent());
					//((CachesViewModel *)this->ui->cachesView->model())->postDataChanged(i->parent());
				} else {
					// Selected item is cache
					((CachesViewModel *)this->ui->cachesView->model())->beginResetModel();
					cacheItem->initChilds();
					((CachesViewModel *)this->ui->cachesView->model())->endResetModel();
					//this->ui->cachesView->update(*i);
					//((CachesViewModel *)this->ui->cachesView->model())->postDataChanged(*i);
				}
			}

			i++;
		}
	}
}

void MainWindow::actionModifyWaypoint() {
	QModelIndexList list = this->ui->cachesView->selectionModel()->selectedRows();
	if (list.length() > 0) {
		QModelIndexList::iterator i = list.begin();
		while (i != list.end()) {
			CacheTreeItem *item = (CacheTreeItem *)i->internalPointer();
			if (item->getType() == 2) {
				CacheTreeWaypoint *waypointItem = (CacheTreeWaypoint *)item;
				GCM::GC<GCM::geolib::GeocacheWaypoint> waypoint = waypointItem->getWaypoint();

				WaypointDialog dlg(waypoint, this);
				if (dlg.exec() == QDialog::Accepted) {
					// Update the tree...
					((CachesViewModel *)this->ui->cachesView->model())->postDataChanged(*i);
					this->ui->cachesView->update(*i);
				}
			} else {
				GCM::util::Log::e("Qt4GUI", "Cannot modify cache as waypoint!");
			}
			i++;
		}
	}
}

void MainWindow::actionRemoveWaypoint() {
	QModelIndexList list = this->ui->cachesView->selectionModel()->selectedRows();
	if (list.length() > 0) {
		QMessageBox msg(this);
		msg.setWindowTitle("Confirm");

		QString sMsg;
		sMsg.sprintf("Do you really want to remove %d waypoints?", list.length());
		msg.setText(sMsg);
		msg.setIcon(QMessageBox::Question);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

		if (msg.exec() == QMessageBox::Yes) {
			QModelIndexList::iterator i = list.begin();

			while (i != list.end()) {
				CacheTreeItem *item = (CacheTreeItem *)i->internalPointer();
				if (item->getType() == 2) {
					CacheTreeWaypoint *waypointItem = (CacheTreeWaypoint *)item;
					GCM::GC<GCM::geolib::GeocacheWaypoint> waypoint = waypointItem->getWaypoint();

					try {
						waypoint->getDbProvider()->removeCacheWaypoint(waypoint);
					} catch (GCM::NullPointerException &) {
					}

					CacheTreeGeocache *cacheItem = (CacheTreeGeocache *)waypointItem->parent();

					QModelIndex parent = i->parent();

					((CachesViewModel *)this->ui->cachesView->model())->beginResetModel();
					cacheItem->initChilds();
					((CachesViewModel *)this->ui->cachesView->model())->endResetModel();

					this->ui->cachesView->expand(parent);
				} else {
					GCM::util::Log::e("Qt4GUI", "You must select a waypoint!");
				}

				i++;
			}
		}
	}
}

void MainWindow::onFilterClick() {
	FilterDialog dlg(this, AppConfig::getInstance()->getActiveFilter());

	int status = dlg.exec();

	// Filters could be changed.
	if (status == QDialog::Accepted) {
		GCM::GC<GCM::geolib::Filter> filterToApply = dlg.getFilter();

		if (dlg.getAppendMode() == MainWindow::Replace) {
			AppConfig::getInstance()->setActiveFilter(filterToApply);

			if (!dlg.isFilterSaved()) {
				GCM::geolib::Filter::saveToFile(dlg.getFilter(), "filters/last.xml");
				AppConfig::getInstance()->set("active_filter", "filters/last.xml");
			} else if (dlg.getFilterFile()) {
				GCM::String filterFile = dlg.getFilterFile()->toString();

				GCM::io::File curFilterDir(".");
				if (filterFile->startsWith(curFilterDir->toString())) {
					filterFile->substr(curFilterDir->toString()->length());
				}

				// TODO: Remove working directory from filter, if present.

				AppConfig::getInstance()->set("active_filter", filterFile->c_str());
			} else {
				// Nothing has been changed...
			}
		}

		GCM::util::Log::d("MainWindow", "About to load caches with append mode %d.", dlg.getAppendMode());

		::ui->addTask(new InitTask(this));
		::ui->addTask(new LoaderTask(this, filterToApply, dlg.getAppendMode()));
	}
}

void MainWindow::filterGCID(QString gcid) {
	// Filter by GCID

	QString search = gcid.toUpper();

	QAbstractItemModel *model = this->ui->cachesView->model();
	int rowCount = model->rowCount();
	for (int i = 0; i < rowCount; i++) {
		QModelIndex row = model->index(i, 0);
		QModelIndex parent = row.parent();

		if (!row.data(Qt::DisplayRole).toString().contains(search)) {
			this->ui->cachesView->setRowHidden(i, parent, true);
		} else {
			this->ui->cachesView->setRowHidden(i, parent, false);
		}
	}
}

void MainWindow::filterName(QString name) {
	QStringList strings = name.toLower().split(QRegExp("\\s+"));

	QAbstractItemModel *model = this->ui->cachesView->model();
	int rowCount = model->rowCount();
	for (int i = 0; i < rowCount; i++) {
		QModelIndex row = model->index(i, 1);
		QModelIndex parent = row.parent();
		QString lowerName = row.data(Qt::DisplayRole).toString().toLower();

		QStringList::iterator iter = strings.begin();
		bool result = true;
		while (iter != strings.end()) {
			if (!lowerName.contains(*iter)) {
				result = false;
				break;
			}
			iter++;
		}

		if (!result) {
			this->ui->cachesView->setRowHidden(i, parent, true);
		} else {
			this->ui->cachesView->setRowHidden(i, parent, false);
		}
	}
}

void MainWindow::addFilter(GCM::io::File filterFile) {
	GCM::util::Log::d("MainWindow", "Add filter %s", filterFile->toString()->c_str());

	FilterItem *item = new FilterItem();
	item->file = filterFile;
	item->name = QString::fromUtf8(filterFile->getBasename(".xml")->c_str());
	((FilterItemModel *)this->ui->comboFilterSelect->model())->addItem(item);
}

void MainWindow::switchFilter(int index) {
	GCM::util::Log::d("MainWindow", "Current filter index changed to %d", index);

	if (this->lastFilterIndex != index && index >= 0 && index < this->ui->comboFilterSelect->model()->rowCount()) {
		File f = ((FilterItemModel *)this->ui->comboFilterSelect->model())->item(index)->file;
		if (f->exists()) {
			GCM::GC<GCM::geolib::Filter> filter = GCM::geolib::Filter::loadFromFile(f->toString());
			AppConfig::getInstance()->setActiveFilter(filter);
		} else {
			//QMessageBox::critical(this, "Error", "Filter file does not exists.");
		}

		FilterItem *item = (FilterItem *)this->ui->comboFilterSelect->model()->index(index, 0, QModelIndex()).internalPointer();
		AppConfig::getInstance()->set("active_filter", item->file->toString()->c_str());

		if (this->lastFilterIndex >= 0) {
			::ui->addTask(new LoaderTask(this));
		}

		this->lastFilterIndex = index;
	}
}

bool doImport(GCM::GC<GCM::ImportInfo> plugin, GCM::String fileName, ptrDialog dialog) {
	// Import using selected plugin
	if (plugin->accept(fileName->c_str())) {
		GCM::util::Log::d("Qt4GUI|Import", "Importing %s using plugin %s.", fileName->c_str(), plugin->fileDesc->c_str());

		::ui->addTask(new ImportTask(plugin, dialog, fileName->c_str(), AppConfig::getInstance()->getWorkingDatabase()));

		return true;
	} else {
		return false;
	}
}

void MainWindow::onImportClick() {
	try {
		QStringList filters;
		GCM::ArrayList<GCM::ImportInfo> imports;

		PluginManager::PluginFunctionList list = PluginManager::Instance()->listPlugins("Import");
		PluginManager::PluginFunctionList::iterator i = list.begin();
		while (i != list.end()) {
			void *func = i->second;
			GCM::fImportInfo infoCall = (GCM::fImportInfo)func;
			GCM::GC< GCM::List<GCM::ImportInfo> > info = infoCall();
			GCM::List<GCM::ImportInfo>::iterator ii = info->begin();
			while (ii != info->end()) {
				filters.append(QString::fromUtf8((*ii)->fileDesc->c_str()) + " (" + QString::fromUtf8((*ii)->fileMask->c_str()) + ")");
				imports.append(*ii);
				ii++;
			}
			i++;
		}

		QString nm;
		QStringList f = QFileDialog::getOpenFileNames(this, "Open files for import", QString::fromUtf8(AppConfig::getInstance()->getString("import_dir", "")), filters.join(";;"), &nm);
		QStringList::iterator file = f.begin();

		ptrDialog dialog = ::ui->getDialogBuilder()->buildProgressDialog();

		GCM::GC<GCM::ImportInfo> iinfo;
		for (int i = 0; i < filters.length(); i++) {
			if (filters[i] == nm) {
				iinfo = imports[i];
				break;
			}
		}

		bool first = true;
		while (file != f.end()) {
			GCM::String fileName = file->toUtf8().data();

			// Save last directory.
			if (first) {
				dialog->show();

				GCM::io::File fImport(fileName);
				AppConfig::getInstance()->set("import_dir", fImport->getDirname());
				first = false;
			}

			// Here, decide which plugin to use.
			if (!iinfo) {
				// Test all plugins
				GCM::ArrayList<GCM::ImportInfo>::iterator pi = imports.begin();
				while (pi != imports.end()) {
					if (doImport(*pi, fileName, dialog)) break;
					pi++;
				}

				if (pi == imports.end()) {
					QMessageBox msg(this);
					msg.setWindowTitle("Import error");
					msg.setIcon(QMessageBox::Critical);

					QString message;
					message.sprintf("File %s contains invalid data.", fileName->c_str());
					msg.setText(message);
					msg.exec();
				}
			} else {
				if (!doImport(iinfo, fileName, dialog)) {
					QMessageBox msg(this);
					msg.setWindowTitle("Import error");

					QString message;
					message.sprintf("File %s contains invalid data.", fileName->c_str());
					msg.setText(message);
					msg.exec();
				}
			}
			file++;
		}

		// Reload only if something has been imported.
		if (f.begin() != f.end()) {
			::ui->addTask(new LoaderTask(this));
		}

	} catch (GCM::pluginmanager::UnsupportedFunctionException &e) {
		QMessageBox msg(this);
		msg.setWindowTitle("Error");
		msg.setText("No import plugin available.");
		msg.setIcon(QMessageBox::Critical);
		msg.exec();

		(void)e;
	}
}

void MainWindow::onShowOnlineClick() {
	QModelIndexList list = this->ui->cachesView->selectionModel()->selectedRows();
	if (list.length() > 0) {
		QModelIndexList::iterator i = list.begin();
		while (i != list.end()) {
			CacheTreeItem *item = (CacheTreeItem *)i->internalPointer();

			if (item->getType() == 2) item = item->parent();
			CacheTreeGeocache *cacheItem = (CacheTreeGeocache *)item;
			GCM::GC<GCM::geolib::Geocache> cache = cacheItem->getCache();
			GCM::util::Log::d("Qt4GUI", "Selected cache %s", cache->getName()->c_str());

			QString url = "http://coord.info/";
			url.append(cache->getId()->c_str());
			QDesktopServices::openUrl(QUrl(url, QUrl::StrictMode));

			i++;
		}
	}
}

void MainWindow::onModifyCacheClick() {
	QModelIndexList list = this->ui->cachesView->selectionModel()->selectedRows();
	if (list.length() > 0) {
		QModelIndexList::iterator i = list.begin();
		while (i != list.end()) {
			CacheTreeItem *item = (CacheTreeItem *)i->internalPointer();

			if (item->getType() == 2) item = item->parent();
			CacheTreeGeocache *cacheItem = (CacheTreeGeocache *)item;
			GCM::GC<GCM::geolib::Geocache> cache = cacheItem->getCache();
			GCM::util::Log::d("Qt4GUI", "Selected cache %s", cache->getName()->c_str());

			CacheEditDialog dlg(cache, this);
			if (dlg.exec() == QDialog::Accepted) {
				((CachesViewModel *)this->ui->cachesView->model())->postDataChanged(*i);
			}

			i++;
		}
	}
}

void MainWindow::onShowDetailsClick() {
	QModelIndexList list = this->ui->cachesView->selectionModel()->selectedRows();
	if (list.length() > 0) {
		QModelIndexList::iterator i = list.begin();
		while (i != list.end()) {
			CacheTreeItem *item = (CacheTreeItem *)i->internalPointer();

			if (item->getType() == 2) item = item->parent();
			CacheTreeGeocache *cacheItem = (CacheTreeGeocache *)item;
			GCM::GC<GCM::geolib::Geocache> cache = cacheItem->getCache();
			GCM::util::Log::d("Qt4GUI", "Selected cache %s", cache->getName()->c_str());

			CacheDetailsDialog *dlg = new CacheDetailsDialog(cache, NULL);
			dlg->show();

			i++;
		}
	}
}

void MainWindow::onExportClick() {
	// Get export plugin
	try {
		GCM::GC< GCM::List<GCM::ExportInfo> > allExports = new GCM::List<GCM::ExportInfo>();

		PluginManager::PluginFunctionList plugins = PluginManager::Instance()->listPlugins("Export");
		PluginManager::PluginFunctionList::iterator i = plugins.begin();
		while (i != plugins.end()) {
			GCM::GC< GCM::List<GCM::ExportInfo> > pluginExports = ((GCM::fExportInfo)(i->second))();
			allExports->insert(
				allExports->end(),
				pluginExports->begin(),
				pluginExports->end());

			i++;
		}

		ExportDialog dlg(allExports, this);
		if (dlg.exec() == QDialog::Accepted) {
			// Export should be done.
			GCM::GC<GCM::ExportInfo> selected = dlg.selected();
			if (selected) {
				QString filter;
				filter.sprintf("%s (*%s)", selected->formatDescription->c_str(), selected->fileExtension->c_str());
				QString file = QFileDialog::getSaveFileName(this, "Select export destination", AppConfig::getInstance()->getString("export_dir", ""), filter);
				if (file != "") {
					if (!file.endsWith(selected->fileExtension->c_str())) file.append(selected->fileExtension->c_str());

					File out(file.toUtf8().data());
					AppConfig::getInstance()->set("export_dir", out->getDirname());
					::ui->addTask(new ExportTask(this->ui->cachesView, selected, out));
				}
			}
		}
	} catch (GCM::pluginmanager::NoPluginSelectedException &e) {
		QMessageBox msg;
		msg.setWindowTitle("Export");
		msg.setText("No export plugin was found.");
		(void)e;
	} catch (GCM::pluginmanager::UnsupportedFunctionException &e) {
		QMessageBox msg;
		msg.setWindowTitle("Export");
		msg.setText("No export plugin was found.");
		(void)e;
	}
}

void MainWindow::onDoubleClick(QModelIndex index) {
	if (index.isValid()) {
		CacheTreeItem *item = (CacheTreeItem *)index.internalPointer();

		if (item->getType() == 2) item = item->parent();
		CacheTreeGeocache *cacheItem = (CacheTreeGeocache *)item;
		GCM::GC<GCM::geolib::Geocache> cache = cacheItem->getCache();
		GCM::util::Log::d("Qt4GUI", "Selected cache %s", cache->getName()->c_str());

		CacheDetailsDialog *dlg = new CacheDetailsDialog(cache, NULL);
		dlg->show();
	}
}

void MainWindow::updateStats() {
	const GCM::database::Database::Stats &st = AppConfig::getInstance()->getWorkingDatabase()->getStats();
	QString txt;
	GCM::String dbName = AppConfig::getInstance()->getWorkingDatabase()->getConnectionString();
	if (dbName->endsWith(".db3")) {
		dbName = dbName->substring(0, -4);
	}
	txt.sprintf("%s: %d caches, %d waypoints", dbName->c_str(), st.numCaches, st.numWaypoints);
	this->cachesCount->setText(txt);
}

void MainWindow::onAboutClick() {
	AboutDialog dlg(this);
	dlg.move(this->rect().center() - dlg.rect().center());
	dlg.exec();
}

void MainWindow::onQuitClick() {
	emit this->close();
}

void MainWindow::onSaveListClick() {
	QString file = QFileDialog::getSaveFileName(this, "Save list", QString(), "GeoGet list (*.ggt)");
	if (file != "") {
		if (!file.endsWith(".ggt")) file.append(".ggt");

		QFile f(file);
		if (f.open(QIODevice::WriteOnly)) {
			QTextStream s(&f);

			int rowCount = this->ui->cachesView->model()->rowCount(QModelIndex());

			for (int i = 0; i < rowCount; i++) {
				CacheTreeItem *item = (CacheTreeItem *)this->ui->cachesView->model()->index(i, 0, QModelIndex()).internalPointer();
				if (!this->ui->cachesView->isRowHidden(i, QModelIndex()) && item->getType() == 1) {
					CacheTreeGeocache *cacheItem = (CacheTreeGeocache *)item;
					GCM::GC<GCM::geolib::Geocache> cache = cacheItem->getCache();
					s << cache->getId()->c_str();
					s << endl;
				}
			}

			f.flush();
			f.close();
			QString txt;
			txt.sprintf("List saved to file %s", file.toUtf8().data());
			emit this->ui->statusBar->showMessage(txt, 5);
		}
	}
}

void MainWindow::onOpenListClick() {
	QString file = QFileDialog::getOpenFileName(this, "Open list", QString(), "GeoGet list (*.ggt)");
	GCM::GC<ListFilter> filter = new ListFilter();

	if (file != "") {
		QFile f(file);
		if (f.exists() && f.open(QIODevice::ReadOnly)) {
			QTextStream s(&f);
			while (!s.atEnd()) {
				QString gcid;
				s >> gcid;
				filter->append(gcid.toUtf8().data());
			}
		}

		::ui->addTask(new LoaderTask(this, filter.getObj()));
	}
}

void MainWindow::onAddListClick() {
	QString file = QFileDialog::getOpenFileName(this, "Open list", QString(), "GeoGet list (*.ggt)");
	GCM::GC<ListFilter> filter = new ListFilter();

	if (file != "") {
		QFile f(file);
		if (f.exists() && f.open(QIODevice::ReadOnly)) {
			QTextStream s(&f);
			while (!s.atEnd()) {
				QString gcid;
				s >> gcid;
				filter->append(gcid.toUtf8().data());
			}
		}
	}

	::ui->addTask(new LoaderTask(this, filter.getObj(), Add));
}

void MainWindow::onSubtractListClick() {
	QString file = QFileDialog::getOpenFileName(this, "Open list", QString(), "GeoGet list (*.ggt)");
	GCM::GC<ListFilter> filter = new ListFilter();

	if (file != "") {
		QFile f(file);
		if (f.exists() && f.open(QIODevice::ReadOnly)) {
			QTextStream s(&f);
			while (!s.atEnd()) {
				QString gcid;
				s >> gcid;
				filter->append(gcid.toUtf8().data());
			}
		}
	}

	::ui->addTask(new LoaderTask(this, filter.getObj(), Subtract));
}

void MainWindow::onIntersectListClick() {
	QString file = QFileDialog::getOpenFileName(this, "Open list", QString(), "GeoGet list (*.ggt)");
	GCM::GC<ListFilter> filter = new ListFilter();

	if (file != "") {
		QFile f(file);
		if (f.exists() && f.open(QIODevice::ReadOnly)) {
			QTextStream s(&f);
			while (!s.atEnd()) {
				QString gcid;
				s >> gcid;
				filter->append(gcid.toUtf8().data());
			}
		}
	}

	::ui->addTask(new LoaderTask(this, filter.getObj(), Intersect));
}

void MainWindow::onDbCreateClick() {
	CreateDatabaseDialog dlg(this);
	if (dlg.exec() == QDialog::Accepted) {
		GCM::GC<GCM::database::DatabaseInfo> info = dlg.getDatabaseInfo();
		if (info) {
			// Clear old cache list before new one is loaded to save memory.
			this->setCaches(NULL);

			::ui->addTask(new SwitchDatabaseTask(this, info, File(dlg.getDatabaseName().append(".db3").toUtf8().data())));
		}
	}
}

void MainWindow::onDbSelectClick(bool force) {
	DatabaseSelectDialog dlg(this);
	dlg.setForce(force);
	if (dlg.exec() == QDialog::Accepted) {
		File dbFile = dlg.getDatabaseFile();

		PluginManager::PluginFunctionList &funcs = PluginManager::Instance()->listPlugins("Database");
		PluginManager::PluginFunctionList::iterator i = funcs.begin();
		while (i != funcs.end()) {
			GCM::database::fDatabase call = (GCM::database::fDatabase)i->second;
			GCM::GC< GCM::List<GCM::database::DatabaseInfo> > info = call();

			GCM::List<GCM::database::DatabaseInfo>::iterator ii = info->begin();
			while (ii != info->end()) {
				if ((*ii)->accept(dbFile->getAbsolutePath()->c_str())) {
					// Clear old cache list before new one is loaded to save memory.
					this->setCaches(NULL);

					::ui->addTask(new SwitchDatabaseTask(this, *ii, dbFile));
					break;
				}
				ii++;
			}

			if (ii != info->end()) {
				break;
			}

			i++;
		}

		if (i == funcs.end()) {
			QMessageBox::critical(this, "Unable to open database", "Unable to open selected database - no plugin was found.", QMessageBox::Ok);
		}
	} else {
		this->updateCurrentDatabase();
	}
}

void MainWindow::onDbRenameClick() {
	DatabaseRenameDialog dlg(this);

	File f(AppConfig::getInstance()->getWorkingDatabase()->getConnectionString());
	dlg.setDatabaseName(QString::fromUtf8(f->getBasename(".db3")->c_str()));

	if (dlg.exec() == QDialog::Accepted) {
		// Rename current database to new name
		File newName(f->getDirname());
		newName->append(dlg.getDatabaseName().append(".db3").toUtf8().data());

		PluginManager::PluginFunctionList &funcs = PluginManager::Instance()->listPlugins("Database");
		PluginManager::PluginFunctionList::iterator i = funcs.begin();
		while (i != funcs.end()) {
			GCM::database::fDatabase call = (GCM::database::fDatabase)i->second;
			GCM::GC< GCM::List<GCM::database::DatabaseInfo> > info = call();

			GCM::List<GCM::database::DatabaseInfo>::iterator ii = info->begin();
			while (ii != info->end()) {
				if ((*ii)->id->equals(AppConfig::getInstance()->getWorkingDatabase()->getId())) {
					// Close current database
					this->setCaches(NULL);
					AppConfig::getInstance()->setWorkingDatabase(NULL);

					// Rename database
					if (::rename(f->toString()->c_str(), newName->toString()->c_str()) == 0) {
						// Reopen the database with new name
						::ui->addTask(new SwitchDatabaseTask(this, *ii, newName));
					} else {
						QMessageBox::critical(this, "Database rename failed", "Unable to rename database.", QMessageBox::Ok);
						::ui->addTask(new SwitchDatabaseTask(this, *ii, f));
					}
				}
				ii++;
			}
			i++;
		}
	}
}

void MainWindow::onDbRemoveClick() {
	if (QMessageBox::question(this, "Remove database?", "Do you really want to remove the database?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
		File f(AppConfig::getInstance()->getWorkingDatabase()->getConnectionString());

		GCM:: String id = AppConfig::getInstance()->getWorkingDatabase()->getId();

		this->setCaches(NULL);
		AppConfig::getInstance()->setWorkingDatabase(NULL);

		if (::unlink(f->toString()->c_str()) == 0) {
			this->onDbSelectClick(true);
		} else {
			QMessageBox::critical(this, "Database removal failed", "Unable to remove database.", QMessageBox::Ok);

			PluginManager::PluginFunctionList &funcs = PluginManager::Instance()->listPlugins("Database");
			PluginManager::PluginFunctionList::iterator i = funcs.begin();
			while (i != funcs.end()) {
				GCM::database::fDatabase call = (GCM::database::fDatabase)i->second;
				GCM::GC< GCM::List<GCM::database::DatabaseInfo> > info = call();

				GCM::List<GCM::database::DatabaseInfo>::iterator ii = info->begin();
				while (ii != info->end()) {
					if ((*ii)->id->equals(id)) {
						::ui->addTask(new SwitchDatabaseTask(this, *ii, f));
					}
					ii++;
				}
				i++;
			}
		}
	}
}

void MainWindow::onDbCopyClick() {

}

void MainWindow::actionFoundIt() {
	QModelIndexList list = this->ui->cachesView->selectionModel()->selectedRows();
	if (list.length() > 0) {
		QModelIndexList::iterator i = list.begin();
		while (i != list.end()) {
			QModelIndex index = *i;
			CacheTreeItem *item = (CacheTreeItem *)i->internalPointer();

			if (item->getType() == 2) {
				item = item->parent();
				index = index.parent();
			}
			CacheTreeGeocache *cacheItem = (CacheTreeGeocache *)item;
			GCM::GC<GCM::geolib::Geocache> cache = cacheItem->getCache();

			SetFoundDialog dlg(cache, this);
			if (dlg.exec() == QDialog::Accepted) {
				((CachesViewModel *)this->ui->cachesView->model())->postDataChanged(index);
			}

			i++;
		}
	}
}

void MainWindow::onSettingsClick() {
	SettingsDialog dlg(this);

	GCM::GC<GCM::geolib::ReferencePoint> refPt;

	// Save old reference point
	QModelIndex index = this->refPtModel->index(this->ui->comboReference->currentIndex(), 0, QModelIndex());
	if (index.isValid()) {
		refPt = AppConfig::getInstance()->getReferencePoints()->at(index.row());
	}

	if (dlg.exec() == QDialog::Accepted) {
		// Reload reference points
		this->refPtModel->refresh();

		// Try to restore old reference point
		if (refPt) {
			bool found = false;
			for (int index = 0; index < this->refPtModel->rowCount(); index++) {
				if (refPt.getObj() == AppConfig::getInstance()->getReferencePoints()->at(index).getObj()) {
					this->ui->comboReference->setCurrentIndex(index);

					found = true;
					break;
				}
			}

			if (!found) {
				if (this->refPtModel->rowCount() > 0) {
					this->ui->comboReference->setCurrentIndex(0);
				} else {
					this->ui->comboReference->setCurrentIndex(-1);
				}
			}
		} else {
			if (this->refPtModel->rowCount() > 0) {
				this->ui->comboReference->setCurrentIndex(0);
			} else {
				this->ui->comboReference->setCurrentIndex(-1);
			}
		}
	}
}

void MainWindow::updateCurrentDatabase() {
	if (AppConfig::getInstance()->getWorkingDatabase()) {
		GCM::String dbName = AppConfig::getInstance()->getWorkingDatabase()->getConnectionString();

		for (int index = 0; index < this->ui->comboDatabase->model()->rowCount(); index++) {
			DbSelectItemBundle *item = (DbSelectItemBundle *)this->ui->comboDatabase->model()->index(index, 0, QModelIndex()).internalPointer();
			if (item && item->file->getPath() == dbName) {
				this->lastDbIndex = index;
				this->ui->comboDatabase->setCurrentIndex(index);
				break;
			}
		}

		/*int index = this->ui->comboDatabase->findText(dbName->c_str());
		if (index >= 0) {
			this->ui->comboDatabase->setCurrentIndex(index);
		}*/
	}
}

void MainWindow::updateCurrentFilter() {
	File activeFilter = File(AppConfig::getInstance()->getString("active_filter", ""));
	GCM::String afStr = activeFilter->getAbsolutePath();

	for (int index = 0; index < this->ui->comboFilterSelect->model()->rowCount(); index++) {
		FilterItem *item = (FilterItem *)this->ui->comboFilterSelect->model()->index(index, 0, QModelIndex()).internalPointer();
		if (item && item->file->getAbsolutePath()->equals(afStr)) {
			this->ui->comboFilterSelect->setCurrentIndex(index);
			break;
		}
	}
}

void MainWindow::onRefChanged(int row) {
	if (row >= 0 && row < this->ui->comboReference->model()->rowCount()) {
		GCM::GC<GCM::geolib::ReferencePoint> rp = AppConfig::getInstance()->getReferencePoints()->at(row);

		if (this->list) {
			this->model->beginResetModel();
			this->list->calcDistances(rp->getLatitudeE6(), rp->getLongitudeE6());
			this->model->endResetModel();
		}

		if (!rp->isTemporary()) {
			AppConfig::getInstance()->set("reference_point", rp->getName()->c_str());
		}
		AppConfig::getInstance()->setActiveRefPt(rp);
	}
}

void MainWindow::switchDatabase(int index) {
	if (index < 0 || index >= this->databaseModel->rowCount()) return;
	if (index == this->lastDbIndex) return;

	GCM::io::File dbFile = ((DbSelectItemBundle *)this->databaseModel->index(index, 0, QModelIndex()).internalPointer())->file;

	PluginManager::PluginFunctionList &funcs = PluginManager::Instance()->listPlugins("Database");
	PluginManager::PluginFunctionList::iterator i = funcs.begin();
	while (i != funcs.end()) {
		GCM::database::fDatabase call = (GCM::database::fDatabase)i->second;
		GCM::GC< GCM::List<GCM::database::DatabaseInfo> > info = call();

		GCM::List<GCM::database::DatabaseInfo>::iterator ii = info->begin();
		while (ii != info->end()) {
			if ((*ii)->accept(dbFile->getAbsolutePath()->c_str())) {
				// Clear old cache list before new one is loaded to save memory.
				this->setCaches(NULL);

				::ui->addTask(new SwitchDatabaseTask(this, *ii, dbFile));
				break;
			}
			ii++;
		}

		if (ii != info->end()) {
			break;
		}

		i++;
	}

	if (i == funcs.end()) {
		QMessageBox::critical(this, "Unable to open database", "Unable to open selected database - no plugin was found.", QMessageBox::Ok);
	}

	this->lastDbIndex = index;
}

void MainWindow::onRefreshClick() {
	::ui->addTask(new LoaderTask(this, AppConfig::getInstance()->getActiveFilter()));
}

void MainWindow::onClearClick() {
	this->setCaches(new GCM::geolib::GeocacheList());
}

void MainWindow::onSearchClick() {
	SearchDialog dlg(this);
	if (dlg.exec() == QDialog::Accepted) {
		QString search = dlg.getSearchString();
		QStringList sl = search.split(' ');

		bool scrolled = false;

		// Search for GCID in database
		GCM::GC<GCM::geolib::Geocache> cache = AppConfig::getInstance()->getWorkingDatabase()->getByGCID(search.toUpper().toUtf8().data());
		if (cache) {
			GCM::GC<GCM::geolib::GeocacheList> list = new GCM::geolib::GeocacheList();
			list->append(cache);
			this->setCaches(list, Add);
		}

		// Search for name in database
		GCM::GC<GCM::geolib::GeocacheList> listByName = AppConfig::getInstance()->getWorkingDatabase()->getByName(search.toUtf8().data());
		this->setCaches(listByName, Add);

		// Scan through current list, find the cache and scroll the list to it.
		for (int i = 0; i < this->model->rowCount(); i++) {
			QModelIndex index = this->model->index(i, 0, QModelIndex());
			CacheTreeGeocache *cacheItem = (CacheTreeGeocache *)index.internalPointer();
			if (cacheItem->getCache()->getId()->equals(search.toUpper().toUtf8().data())) {
				this->ui->cachesView->setRowHidden(i, index.parent(), false);

				if (!scrolled) {
					this->ui->cachesView->scrollTo(index);
					scrolled = true;
				}
			} else {
				bool match = true;
				QStringList::iterator si = sl.begin();
				GCM::String cname = cacheItem->getCache()->getName();
				while (si != sl.end()) {
					if (cname->find(si->toUtf8().data()) == std::string::npos) {
						match = false;
						break;
					}

					si++;
				}

				if (match) {
					this->ui->cachesView->setRowHidden(i, index.parent(), false);

					if (!scrolled) {
						this->ui->cachesView->scrollTo(index);
						scrolled = true;
					}
				}
			}
		} // for
	} // dlg.exec()
}

void MainWindow::onCopyClick() {
	QModelIndex index = this->ui->cachesView->selectionModel()->currentIndex();
	if (index.isValid()) {
		QClipboard *cpb = QApplication::clipboard();
		cpb->setText(index.data(Qt::DisplayRole).toString(), QClipboard::Clipboard);
	}
}

void MainWindow::onRemoveFromListClick() {
	QModelIndexList sel = this->ui->cachesView->selectionModel()->selectedRows();
	QModelIndexList::iterator i = sel.begin();
	while (i != sel.end()) {
		this->ui->cachesView->setRowHidden(i->row(), i->parent(), true);
		i++;
	}

	this->updateStatusCounts();
}

void MainWindow::onRemoveAllBelowClick() {
	QModelIndexList sel = this->ui->cachesView->selectionModel()->selectedRows();
	if (sel.begin() != sel.end()) {
		int rows = this->ui->cachesView->model()->rowCount(QModelIndex());
		for (int i = sel.begin()->row() + 1; i < rows; i++) {
			this->ui->cachesView->setRowHidden(i, QModelIndex(), true);
		}
	}

	this->updateStatusCounts();
}

void MainWindow::updateStatusCounts() {
	QString txt;
	txt.sprintf("In view: %d total, currently on row %d", this->ui->cachesView->model()->rowCount(QModelIndex()), this->ui->cachesView->selectionModel()->currentIndex().row() + 1);
	this->listCount->setText(txt);
}

void MainWindow::onRowChanged(const QModelIndex &current, const QModelIndex &) {
	this->updateStatusCounts();

	if (current.isValid()) {
		this->ui->actionCopy->setEnabled(true);
		this->ui->actionRemove_all_below->setEnabled(true);
		this->ui->actionRemoveFromList->setEnabled(true);
	} else {
		this->ui->actionCopy->setEnabled(false);
		this->ui->actionRemove_all_below->setEnabled(false);
		this->ui->actionRemoveFromList->setEnabled(false);
	}
}

void MainWindow::onError(void *params) {
	GCM::util::Log::EventParams *p = (GCM::util::Log::EventParams *)params;
	QString txt;
	txt.sprintf("%s: %s", p->section->c_str(), p->message->c_str());
	::ui->addTask(new ErrorTask(txt));
}
