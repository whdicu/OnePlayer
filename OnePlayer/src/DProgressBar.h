#pragma once

#include <QSlider>

class DProgressBar : public QSlider
{
	Q_OBJECT

public:
	DProgressBar(QWidget *parent);
	~DProgressBar();

signals:
	void sigMousePressed();
	void sigMouseReleased(int value);

private:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};
