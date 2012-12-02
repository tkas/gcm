#ifndef _GCM_CLI_PROGRESS_DIALOG_H
# define _GCM_CLI_PROGRESS_DIALOG_H

#include <ui.h>

class CLIProgressDialog: public GCM::ui::Dialog {
	private:
		bool visible;

	public:
		CLIProgressDialog();
		virtual void dismiss();
		virtual void show();
		virtual void update();
};

#endif
