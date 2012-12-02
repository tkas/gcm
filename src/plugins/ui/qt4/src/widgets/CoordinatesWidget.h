#ifndef COORDINATESWIDGET_H
#define COORDINATESWIDGET_H

#include <QWidget>

namespace Ui {
	class CoordinatesWidget;
}

struct Coords {
	double dlat;
	double dlon;
	double mlat;
	double mlon;
	double slat;
	double slon;
};

class CoordinatesWidget : public QWidget {
	Q_OBJECT
public:
	CoordinatesWidget(QWidget *parent = 0);
	~CoordinatesWidget();

	Coords getCoords();
	void setCoords(Coords c);

protected:
	void changeEvent(QEvent *e);

	void setWgsDDCoords(Coords c);
	void setWgsDMCoords(Coords c);
	void setWgsDMSCoords(Coords c);
	Coords getWgsDMCoords();
	Coords getWgsDDCoords();
	Coords getWgsDMSCoords();

private:
	Ui::CoordinatesWidget *ui;

public slots:
	void wgsFormatChanged(int index);
	void wgsDMChanged();
	void wgsDDChanged();
	void wgsDMSChanged();
};

#endif // COORDINATESWIDGET_H
