#ifndef _GCM_CLI_IMPORTTHREAD_H
# define _GCM_CLI_IMPORTTHREAD_H

#include <Thread.h>
#include <ui.h>

class ImportTask: public GCM::ui::BackgroundTask {
	private:
		const char *mFilename;
		const char *mDBname;
		ptrDialog mProgressDialog;
		ptrUI mUI;

	public:
		ImportTask(const char *filename, const char*dbName, ptrUI ui);
		virtual void doInBackground();
		virtual void onPreExecute();
		virtual void onPostExecute();
};

#endif
