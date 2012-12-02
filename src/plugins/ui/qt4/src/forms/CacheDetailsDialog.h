#ifndef CACHEDETAILSDIALOG_H
#define CACHEDETAILSDIALOG_H

#include <QDialog>
#include <GC.h>
#include <geolib/Geocache.h>
#include <QUrl>

namespace Ui {
	class CacheDetailsDialog;
}

class CacheDetailsDialog : public QWidget {
	Q_OBJECT
public:
	CacheDetailsDialog(GCM::GC<GCM::geolib::Geocache> cache, QWidget *parent = 0);
	~CacheDetailsDialog();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::CacheDetailsDialog *ui;
	GCM::GC<GCM::geolib::Geocache> cache;

public slots:
	void onLinkClicked(QUrl url);
	void onCopy();
};

#endif // CACHEDETAILSDIALOG_H
