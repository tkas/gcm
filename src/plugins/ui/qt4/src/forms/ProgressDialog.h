#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include <QTimer>
#include <Events.h>

namespace Ui {
	class ProgressDialog;
}

class ProgressDialog : public QDialog {
	Q_OBJECT
public:
	ProgressDialog(QWidget *parent = 0);
	~ProgressDialog();

	void setMessage(const char *);
	void setProgress(int);
	void setMaxProgress(int);

	void setCancellable(bool);
	bool isCancellable();
	bool isCancelled();

	GCM::Events events;

protected:
	void changeEvent(QEvent *e);
	virtual void showEvent(QShowEvent *e);
	virtual void hideEvent(QHideEvent *e);
	virtual void accept();
	virtual void reject();

private:
	Ui::ProgressDialog *ui;
	QTimer *timer;
	QString message;
	int value;
	int maxVal;
	bool maxChanged;
	bool cancellable;
	bool iscancelled;

public slots:
	void onTimer();

protected slots:
	void onCancel();
};

#endif // PROGRESSDIALOG_H
