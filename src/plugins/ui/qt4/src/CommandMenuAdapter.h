#ifndef _COMMAND_MENU_ADAPTER_H
# define _COMMAND_MENU_ADAPTER_H

#include <GC.h>
#include <ui/Command.h>

#include <QAction>
#include <QMenu>
#include <QTreeView>

#include "CommandAdapter.h"

class CommandMenuAdapter: public QAction {
Q_OBJECT

private:
	GCM::GC<GCM::ui::CommandMenu> menu;
	QMenu *qmenu;
	QTreeView *cachesView;
	bool selectedOnly;

public:
	CommandMenuAdapter(GCM::GC<GCM::ui::CommandMenu> menu, QTreeView *cachesView, bool selectedOnly = true):
		QAction(QString::fromUtf8(menu->getName()->c_str()), NULL),
		menu(menu),
		cachesView(cachesView),
		selectedOnly(selectedOnly)
	{
		this->qmenu = new QMenu();

		// Set actions to the menu
		GCM::List<GCM::ui::Command>::iterator i = menu->getCommands()->begin();
		while (i != menu->getCommands()->end()) {
			if ((*i)->isSubmenu()) {
				this->qmenu->addAction(new CommandMenuAdapter(
					(GCM::ui::CommandMenu *)i->getObj(), cachesView, selectedOnly
				));
			} else {
				this->qmenu->addAction(new CommandAdapter(
					*i, cachesView, selectedOnly
				));
			}
			i++;
		}

		this->setMenu(this->qmenu);
		this->setToolTip(QString::fromUtf8(menu->getHelp()->c_str()));
		// TODO: Set icon
	}

	~CommandMenuAdapter() {
		delete this->qmenu;
	}
};

#endif