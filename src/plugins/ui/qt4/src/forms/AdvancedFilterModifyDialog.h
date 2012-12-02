#ifndef ADVANCEDFILTERMODIFYDIALOG_H
#define ADVANCEDFILTERMODIFYDIALOG_H

#include <QDialog>
#include <geolib/filter/Advanced.h>

namespace Ui {
    class AdvancedFilterModifyDialog;
}

class AdvancedFilterModifyDialog : public QDialog {
    Q_OBJECT
public:
    AdvancedFilterModifyDialog(QWidget *parent = 0);
    ~AdvancedFilterModifyDialog();

	QString getFilterName();
	GCM::geolib::filter::AdvancedFilterItem::Mode getFilterMode();
	void setFilterName(QString name);
	void setFilterMode(GCM::geolib::filter::AdvancedFilterItem::Mode mode);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AdvancedFilterModifyDialog *ui;

public slots:
	virtual void accept();
};

#endif // ADVANCEDFILTERMODIFYDIALOG_H
