#ifndef ATTRIBUTECHECKED_H
#define ATTRIBUTECHECKED_H

#include <QFrame>
#include <QMouseEvent>
#include <QPaintEvent>

class AttributeChecked : public QFrame {
	Q_OBJECT
public:
	AttributeChecked(int attributeId, QWidget *parent = 0);
	~AttributeChecked();
	Qt::CheckState checkState();
	void setCheckState(Qt::CheckState state);

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);

private:
	int attributeId;
	QPixmap iconInactive;
	QPixmap iconYes;
	QPixmap iconNo;
	Qt::CheckState state;

signals:
	void stateChanged(int);

};

#endif // ATTRIBUTECHECKED_H
