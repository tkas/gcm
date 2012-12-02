#ifndef _GCM_RUNNABLE_H
# define _GCM_RUNNABLE_H

#include <gcm.h>
#include <Object.h>

namespace GCM {

class GCM_API Runnable: public Object {
	public:
		Runnable(): Object() {}
		virtual void run() = 0;
		virtual ~Runnable();
};

}

#endif
