#ifndef _ERROR_TASK_H
# define _ERROR_TASK_H

#include <ui/BackgroundTask.h>
#include <QMessageBox>

class ErrorTask: public GCM::ui::BackgroundTask {
private:
	QString msg;

public:
	ErrorTask(QString txt): GCM::ui::BackgroundTask(), msg(txt) {}

	void onPreExecute() {
	}

	void onPostExecute() {
		QMessageBox::critical(NULL, "Critical error", this->msg);
	}

	void doInBackground() {
	}
};

#endif
