#pragma once
#include <QWidget>

class QPropertyAnimation;

class Toast : public QWidget
{
	Q_OBJECT

public:
	static void makeToast(const QString& str, int time = SHORT_TOAST);
	const static int SHORT_TOAST = 1000;
	const static int LONG_TOAST = 2000;

private:
	Toast(const QString& str, int time = SHORT_TOAST);
	~Toast();
	void animationShow();
	void animationHide();

	QPropertyAnimation* animation_;
	bool isHide_;
};
