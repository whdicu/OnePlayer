#include "DProgressBar.h"
#include <QMouseEvent>

DProgressBar::DProgressBar(QWidget *parent)
	: QSlider(parent)
{
}

DProgressBar::~DProgressBar()
{
}

void DProgressBar::mousePressEvent(QMouseEvent *event)
{
	emit sigMousePressed();
}

void DProgressBar::mouseMoveEvent(QMouseEvent *event)
{
	int mouseX = event->x();
	int progressPercentage = mouseX * maximum() / width();
	setValue(progressPercentage);
}

void DProgressBar::mouseReleaseEvent(QMouseEvent *event)
{
	int mouseX = event->x();
	int progressPercentage = mouseX * maximum() / width();
	setValue(progressPercentage);
	emit sigMouseReleased(progressPercentage);
}
