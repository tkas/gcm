#ifndef _GCM_UI_COMMAND
# define _GCM_UI_COMMAND

#include <gcm.h>
#include <Object.h>
#include <stl/String.h>
#include <stl/List.h>

#include <geolib/GeocacheList.h>

namespace GCM {
	namespace ui {

/**
 * One command that holds sort of action.
 */
class GCM_API Command: public Object {
private:
	GCM::String name;	/**< Holds human readable name of the menu. It is used as label in GUI */
	GCM::String id;		/**< Holds identifier of the menu command. It is used as command shortcut for CLI and as icon identifier for the GUI. */
	GCM::String help;	/**< Help text that is displayed when user requests. It is used as tooltip in GUI and in help printout in the CLI. */
	GCM::GC<GCM::geolib::GeocacheList> selectedCaches; /**< List of caches that the action should use. */

public:
	Command(GCM::String name, GCM::String id);
	Command(GCM::String name, GCM::String id, GCM::String help);

	virtual bool isSubmenu();

	virtual void setName(GCM::String name);
	virtual void setId(GCM::String id);
	virtual void setHelp(GCM::String help);
	virtual void setSelectedCaches(GCM::GC<GCM::geolib::GeocacheList> selection);

	virtual GCM::String getName();
	virtual GCM::String getId();
	virtual GCM::String getHelp();
	virtual GCM::GC<GCM::geolib::GeocacheList> getSelectedCaches();

	virtual void triggered() = 0;
};

/**
 * Holds multiple commands, it is interpreted as menu
 */
class GCM_API CommandMenu: public Command {
private:
	GCM::GC< GCM::List<Command> > subcommands;

public:
	CommandMenu(String name, String id);
	CommandMenu(String name, String id, String help);

	virtual bool isSubmenu();

	virtual GCM::GC<Command> addCommand(GC<Command> cmd);
	virtual GCM::GC< List<Command> > getCommands();
	virtual void triggered();
};

class GCM_API Separator: public Command {
public:
	Separator(): Command("---", "") {}
	virtual void triggered() {}
};

static GCM::GC<Command> SEPARATOR;

	}
};

#endif