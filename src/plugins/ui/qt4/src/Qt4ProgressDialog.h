#ifndef QT4PROGRESSDIALOG_H
#define QT4PROGRESSDIALOG_H

#include <ui.h>
#include "forms/ProgressDialog.h"
#include <QWidget>

class Qt4ProgressDialog : public GCM::ui::Dialog {
private:
	ProgressDialog *dialog;
	QWidget *wnd;

public:
	Qt4ProgressDialog(QWidget *wnd);
	virtual ~Qt4ProgressDialog();

	virtual void show();
	virtual void update();
	virtual void dismiss();

	virtual void setMaxProgress(int maxProgress);

	virtual void setCancellable(bool cancellable);

	void onCancel(void *);
};

#endif // QT4PROGRESSDIALOG_H
