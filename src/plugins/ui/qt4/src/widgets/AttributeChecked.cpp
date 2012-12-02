#include "AttributeChecked.h"
#include "ui_AttributeChecked.h"

#include <util/Log.h>
#include <QPainter>

AttributeChecked::AttributeChecked(int attributeId, QWidget *parent) :
	QFrame(parent),
	attributeId(attributeId)
{
	QString fnInactive, fnYes, fnNo;
	fnInactive.sprintf(":/attributes/attr_%du.png", attributeId);
	fnYes.sprintf(":/attributes/attr_%dy.png", attributeId);
	fnNo.sprintf(":/attributes/attr_%dn.png", attributeId);

	this->iconInactive = QPixmap(fnInactive);
	this->iconYes = QPixmap(fnYes);
	this->iconNo = QPixmap(fnNo);

	this->setMaximumSize(30, 30);
	this->setMinimumSize(30, 30);
	this->setCheckState(Qt::PartiallyChecked);
}

AttributeChecked::~AttributeChecked()
{
}

Qt::CheckState AttributeChecked::checkState() {
	return this->state;
}

void AttributeChecked::setCheckState(Qt::CheckState state) {
	this->state = state;
	emit update();
	emit stateChanged(this->state);
}

void AttributeChecked::mousePressEvent(QMouseEvent *event) {
	QFrame::mousePressEvent(event);

	if (event->button() == Qt::LeftButton) {
		switch (this->checkState()) {
		case Qt::Checked: this->setCheckState(Qt::Unchecked); break;
		case Qt::Unchecked: this->setCheckState(Qt::PartiallyChecked); break;
		case Qt::PartiallyChecked: this->setCheckState(Qt::Checked); break;
		}
	}
}

void AttributeChecked::paintEvent(QPaintEvent *event) {
	QPainter painter(this);

	switch (this->checkState()) {
	case Qt::Checked: painter.drawPixmap(0, 0, this->width(), this->height(), this->iconYes); break;
	case Qt::Unchecked: painter.drawPixmap(0, 0, this->width(), this->height(), this->iconNo); break;
	case Qt::PartiallyChecked: painter.drawPixmap(0, 0, this->width(), this->height(), this->iconInactive); break;
	}

	QFrame::paintEvent(event);
}
