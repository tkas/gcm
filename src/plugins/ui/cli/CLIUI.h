#ifndef _GCM_CLIUI_H
# define _GCM_CLIUI_H

#include <gcm.h>
#include <ui/UI.h>
#include <stl/String.h>
#include <util/File.h>

class CLIUI: public GCM::ui::UI {
	private:
		typedef enum {
			OP_NOP,
			OP_IMPORT,
			OP_EXPORT,
		} OP;

		ptrDialogBuilder dialogBuilder;
		OP op;
		const char     *importDB;
		GCM::StringList importFile;
		GCM::io::File exportFile;
		const char *exportFmt;
		const char *exportDB;
		const char *exportFilter;

	public:
		CLIUI(int argc, char **argv);
		virtual void parseLine(int argc, char **argv);
		virtual void start();
		virtual ptrDialogBuilder getDialogBuilder();
		virtual ~CLIUI();
};

#endif
