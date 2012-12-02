#ifndef _COMMAND_ADAPTER_H
# define _COMMAND_ADAPTER_H

#include <QAction>
#include <QTreeView>

#include <GC.h>
#include <ui/Command.h>

#include "models/CachesViewModel.h"
#include "models/CacheTreeItem.h"

#include <config/AppConfig.h>

class CommandAdapter: public QAction {
Q_OBJECT

private:
	GCM::GC<GCM::ui::Command> cmd;
	QTreeView *cachesView;
	bool selectedOnly;

public:
	CommandAdapter(GCM::GC<GCM::ui::Command> cmd, QTreeView *cachesView, bool selectedOnly = true):
		QAction(QString::fromUtf8(cmd->getName()->c_str()), NULL),
		cmd(cmd),
		cachesView(cachesView),
		selectedOnly(selectedOnly)
	{
		if (cmd.getObj() == GCM::ui::SEPARATOR.getObj()) {
			this->setSeparator(true);
		} else {
			this->setToolTip(QString::fromUtf8(cmd->getHelp()->c_str()));

			// TODO: Set icon
			connect(this, SIGNAL(triggered()), this, SLOT(onTrigger()));
		}
	}

public slots:
	void onTrigger() {
		GCM::GC<GCM::geolib::GeocacheList> list;

		if (this->selectedOnly) {
			list = new GCM::geolib::GeocacheList();
			QModelIndexList selected = this->cachesView->selectionModel()->selectedRows();
			QModelIndexList::iterator i = selected.begin();
			while (i != selected.end()) {
				QModelIndex index = *i;

				// Skip invalid selection
				if (!index.isValid()) {
					i++;
					continue;
				}

				// Bubble up to the cache
				while (index.parent().isValid()) {
					index = index.parent();
				}

				if (index.isValid()) {
					CacheTreeGeocache *item = (CacheTreeGeocache *)index.internalPointer();
					list->append(item->getCache());
				}

				i++;
			}
		} else {
			list = ((CachesViewModel *)this->cachesView->model())->getList();
		}

		this->cmd->setSelectedCaches(list);
		this->cmd->triggered();
	}
};

#endif