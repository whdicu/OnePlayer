#pragma once

#include <QWidget>

class QLabel;
class QWidget;

class DColorButton : public QWidget
{
	Q_OBJECT

public:
	DColorButton(QWidget* parent = nullptr);
	DColorButton(const QColor& color, const QString& text, QWidget* parent = nullptr);
	~DColorButton();
	void setHasBorder(bool hasBorder);
	void setColor(const QColor& color);
	void setText(const QString& text);
	void setFont(const QFont& font, const QColor& textColor);

signals:
	void sigClicked(const QColor& color);

private:
	void enterEvent(QEvent* event) override;
	void leaveEvent(QEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	
	QWidget* widget_;
	QWidget* widgetColor_;
	QLabel* labelText_;
	QColor color_;
	bool hasBorder_;
	bool isClicked_;
};
