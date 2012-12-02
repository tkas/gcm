#ifndef _GCM_CLI_DIALOGBUILDER_H
# define _GCM_CLI_DIALOGBUILDER_H

#include <ui.h>

class CLIDialogBuilder: public GCM::ui::DialogBuilder {
	public:
		virtual ptrDialog buildStartupDialog();
		virtual ptrDialog buildProgressDialog();
		virtual ptrDialog buildAlertDialog();
		virtual ~CLIDialogBuilder();
};

#endif
