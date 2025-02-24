#pragma once
#pragma execution_character_set("utf-8")
#include <QDialog>
#include <QColor>
#include "HDBase/DList.hpp"
#include "HDBase/DPair.hpp"

class QBoxLayout;
class QHBoxLayout;
class QLineEdit;
class QSpinBox;

enum MouseState
{
	FreeState = 0,
	MoveState = 2,
	dragLeft = 0x10,
	dragTop = 0x20,
	dragRight = 0x40,
	dragBottom = 0x80,
	dragTopLeft = dragTop | dragLeft,
	dragTopRight = dragTop | dragRight,
	dragBottomLeft = dragBottom | dragLeft,
	dragBottomRight = dragBottom | dragRight,
};


class DColorDialog : public QDialog
{
	Q_OBJECT

public:
	DColorDialog(const DList<DPair<QColor, QString>>& defaultColors, QWidget* parent = nullptr);
	~DColorDialog();
	void setCurrentColor(const QColor& currentColor);
	int selectColor(const QColor& currentColor, QColor& selectedColor);

private slots:
	void slotHTMLEditTextChanged(const QString& text);

private:
	QBoxLayout* makeRGBLayout();
	void refreshColorSpin();
	void refreshColorWidgetWidth();

	void resizeEvent(QResizeEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

	QColor selectedColor_;
	QWidget* colorWidget_;
	QLineEdit* htmlEdit_;
	QWidget* rulerWidget_;
	QHBoxLayout* redLayout_;
	QHBoxLayout* greenLayout_;
	QHBoxLayout* blueLayout_;
	QSpinBox* redSpin_;
	QSpinBox* greenSpin_;
	QSpinBox* blueSpin_;
	QPoint pressPos_;  // 拖动窗口时记录按下的点
	QPoint pressGlobalPos_;  // 拖动窗口时记录按下的全局坐标点
	QRect pressRect_;  // 鼠标按下时记录当前窗口的rect
	bool mouseIsPressed_;
	MouseState mouseState_;
};
