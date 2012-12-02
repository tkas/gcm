/**
 * There are basic prototypes for UI plugin functions.
 */

#ifndef _UI_H
#define _UI_H

#include <GC.h>

namespace GCM {
	namespace ui {

class UI;
class Dialog;
class DialogBuilder;

	}
}

typedef GCM::GC<GCM::ui::UI> ptrUI;
typedef GCM::GC<GCM::ui::Dialog> ptrDialog;
typedef GCM::GC<GCM::ui::DialogBuilder> ptrDialogBuilder;

#include <ui/BackgroundTask.h>
#include <ui/UI.h>
#include <ui/Dialog.h>
#include <ui/DialogBuilder.h>

typedef ptrUI(*fStartUI)(int argc, char **argv);

#endif
