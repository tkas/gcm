#ifndef _THREAD_H
# define _THREAD_H

#include <gcm.h>
#include <Runnable.h>

namespace GCM {

class GCM_API Thread: public Runnable {
	private:
		pthread_t thread;
		static void *startHelper(void *param);
		bool restart;
		bool started;

#ifdef _WIN32
		// To support tryJoin
		bool ended;
#endif

	public:
		Thread();
		virtual ~Thread();

		virtual void start();
		virtual void join();
		virtual void cancel();
		virtual bool tryJoin();
};

}

#endif
