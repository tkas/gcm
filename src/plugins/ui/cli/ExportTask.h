#ifndef _GCM_CLI_EXPORTTHREAD_H
# define _GCM_CLI_EXPORTTHREAD_H

#include <Thread.h>
#include <ui.h>
#include <util/File.h>

class ExportTask: public GCM::ui::BackgroundTask {
	private:
		GCM::io::File mFilename;
		const char *mFmtname;
		const char *mDBname;
		const char *mFiltername;
		ptrDialog mProgressDialog;
		ptrUI mUI;

	public:
		ExportTask(GCM::io::File mFile,const char* mFmt, const char *mDB, const char *mFilter, ptrUI ui);
		virtual void doInBackground();
		virtual void onPreExecute();
		virtual void onPostExecute();
};

#endif
