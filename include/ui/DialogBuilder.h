#ifndef _UI_DIALOGBUILDER_H
# define _UI_DIALOGBUILDER_H

#include <gcm.h>
#include <ui.h>

namespace GCM {
	namespace ui {

class GCM_API DialogBuilder: public Object {
	public:
		DialogBuilder();

		virtual ptrDialog buildStartupDialog() = 0;
		virtual ptrDialog buildProgressDialog() = 0;
		virtual ptrDialog buildAlertDialog() = 0;
		virtual ~DialogBuilder();
};

	}
}

#endif
