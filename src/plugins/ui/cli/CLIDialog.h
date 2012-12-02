#ifndef _GCM_CLI_DIALOG_H
# define _GCM_CLI_DIALOG_H

#include <ui.h>

class CLIDialog: public GCM::ui::Dialog {
	private:
		bool visible;

	public:
		CLIDialog();
		virtual void show();
		virtual void update();
		virtual ~CLIDialog();
};

#endif
