#include "DColorDialog.h"
#include "DColorButton.h"
#include "HDCore/DGlobal.h"
#include "DStyle.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

static QFont FONT = QFont("Microsoft YaHei UI", 10);
static QColor TEXT_COLOR = QColor(92, 92, 102);
const static int DRAG_SPACE = 5;
const static int RADIUS = 40;


DColorDialog::DColorDialog(const DList<DPair<QColor, QString>>& defaultColors, QWidget* parent)
	: QDialog(parent)
	, htmlEdit_(new QLineEdit)
	, rulerWidget_(new QWidget)
	, redLayout_(nullptr)
	, greenLayout_(nullptr)
	, blueLayout_(nullptr)
	, redSpin_(nullptr)
	, greenSpin_(nullptr)
	, blueSpin_(nullptr)
	, mouseIsPressed_(false)
	, mouseState_(FreeState)
{
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setMouseTracking(true);

	QVBoxLayout* btnLayout = new QVBoxLayout;
	btnLayout->setMargin(0);
	for (const DPair<QColor, QString>& pair : defaultColors)
	{
		DColorButton* btn = new DColorButton(pair.first, pair.second);
		btn->setFont(FONT, TEXT_COLOR);
		connect(btn, &DColorButton::sigClicked, this, &DColorDialog::setCurrentColor);
		btnLayout->addWidget(btn);
	}
	QWidget* btnWidget = new QWidget;
	btnWidget->setFixedWidth(110);
	btnWidget->setLayout(btnLayout);

	colorWidget_ = new QWidget;
	colorWidget_->setFixedSize(110, 132);
	colorWidget_->setStyleSheet("border-radius: 20px; background-color: white;");

	QHBoxLayout* hLayout1 = new QHBoxLayout;
	hLayout1->setMargin(0);
	hLayout1->addWidget(colorWidget_);
	hLayout1->addLayout(makeRGBLayout());

	QLabel* htmlLabel = new QLabel("HTML:");
	htmlLabel->setFont(FONT);

	htmlEdit_->setFont(FONT);
	htmlEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	connect(htmlEdit_, &QLineEdit::textChanged, this, &DColorDialog::slotHTMLEditTextChanged);

	QHBoxLayout* htmlLayout = new QHBoxLayout;
	htmlLayout->setContentsMargins(20, 0, 20, 0);
	htmlLayout->addWidget(htmlLabel);
	htmlLayout->addWidget(htmlEdit_);

	rulerWidget_->setFixedHeight(40);
	rulerWidget_->setStyleSheet("color: #5c5c66; background-color: white; border-radius: 20px;");
	rulerWidget_->setLayout(htmlLayout);

	QPushButton* btnYes = new QPushButton(tr("行吧"));
	btnYes->setFont(FONT);
	btnYes->setFlat(true);
	btnYes->setCursor(Qt::PointingHandCursor);
	btnYes->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	btnYes->setMinimumHeight(40);
	btnYes->setStyleSheet("QPushButton { \
		color: #5c5c66; \
		background-color: white; \
		border-radius: 20px; } \
	QPushButton:hover { \
		background-color: rgba(255, 255, 255, 0.5); \
	}");
	connect(btnYes, &QPushButton::clicked, this, &QDialog::accept);

	QPushButton* btnNo = new QPushButton(tr("算了"));
	btnNo->setFont(FONT);
	btnNo->setFlat(true);
	btnNo->setCursor(Qt::PointingHandCursor);
	btnNo->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	btnNo->setMinimumHeight(40);
	btnNo->setStyleSheet("QPushButton { \
		color: #5c5c66; \
		background-color: white; \
		border-radius: 20px; } \
	QPushButton:hover { \
		background-color: rgba(255, 255, 255, 0.5); \
	}");
	connect(btnNo, &QPushButton::clicked, this, &QDialog::reject);

	QHBoxLayout* hLayout2 = new QHBoxLayout;
	hLayout2->setMargin(0);
	hLayout2->addWidget(btnYes);
	hLayout2->addWidget(btnNo);

	QVBoxLayout* rightLayout = new QVBoxLayout;
	rightLayout->addLayout(hLayout1);
	rightLayout->addWidget(rulerWidget_);
	rightLayout->addLayout(hLayout2);

	QHBoxLayout* mainLayout = new QHBoxLayout;
	mainLayout->setMargin(20);
	mainLayout->addWidget(btnWidget);
	mainLayout->addLayout(rightLayout);

	QWidget* outSideWidget = new QWidget;
	outSideWidget->setMouseTracking(true);
	outSideWidget->setObjectName("outSideWidget");
	outSideWidget->setStyleSheet(QString("#outSideWidget { \
		background-color: rgb(230, 230, 220); \
		border: 1px solid #5c5c66; \
		border-radius: %1px; \
	}").arg(RADIUS));
	outSideWidget->setLayout(mainLayout);

	QHBoxLayout* outSideLayout = new QHBoxLayout;
	outSideLayout->setMargin(0);
	outSideLayout->addWidget(outSideWidget);
	setLayout(outSideLayout);
}

DColorDialog::~DColorDialog()
{
}

void DColorDialog::setCurrentColor(const QColor& currentColor)
{
	selectedColor_ = currentColor;
	colorWidget_->setStyleSheet(QString("border-radius: 20px; background-color: %1;")
		.arg(DStyle::color2Str(selectedColor_)));
	htmlEdit_->setText(selectedColor_.name().toUpper());
	refreshColorSpin();
}

int DColorDialog::selectColor(const QColor& currentColor, QColor& selectedColor)
{
	setCurrentColor(currentColor);
	int ret = QDialog::exec();
	selectedColor = selectedColor_;
	return ret;
}

void DColorDialog::slotHTMLEditTextChanged(const QString& text)
{
	if (!QColor::isValidColor(text))
		return;

	selectedColor_ = QColor(text);
	colorWidget_->setStyleSheet(QString("border-radius: 20px; background-color: %1;")
		.arg(DStyle::color2Str(selectedColor_)));
	refreshColorSpin();
}

QBoxLayout* DColorDialog::makeRGBLayout()
{
	auto makeFunc = [](const QString& colorStr1, const QString& colorStr2, QSpinBox*& pSpin) -> QHBoxLayout*
	{
		QWidget* pcWidget = new QWidget;
		pcWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		pcWidget->setMinimumSize(30, 30);
		pcWidget->setStyleSheet(QString("border-radius: 15px; background-color: %1;").arg(colorStr1));

		QLabel* label = new QLabel;
		label->setAlignment(Qt::AlignCenter);
		label->setFont(FONT);
		label->setStyleSheet("color: #5c5c66;");
		label->setFixedWidth(18);
		label->setText(colorStr2);

		pSpin = new QSpinBox;
		pSpin->setFont(FONT);
		pSpin->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
		pSpin->setFixedWidth(40);
		pSpin->setStyleSheet("color: #5c5c66;");
		pSpin->setButtonSymbols(QSpinBox::NoButtons);
		pSpin->setMinimum(0);
		pSpin->setMaximum(255);

		QHBoxLayout* layout = new QHBoxLayout;
		layout->setMargin(5);
		layout->addWidget(pcWidget);
		layout->addWidget(label);
		layout->addWidget(pSpin);

		QWidget* widget = new QWidget;
		widget->setStyleSheet("border-radius: 20px; background-color: white;");
		widget->setLayout(layout);
		widget->setFixedHeight(40);

		QHBoxLayout* outLayout = new QHBoxLayout;
		outLayout->setMargin(0);
		outLayout->setSpacing(0);
		outLayout->addStretch();
		outLayout->addWidget(widget);

		return outLayout;
	};

	QVBoxLayout* rgbLayout = new QVBoxLayout;
	rgbLayout->setMargin(0);
	redLayout_ = makeFunc("#f66", "红", redSpin_);
	greenLayout_ = makeFunc("#6f6", "绿", greenSpin_);
	blueLayout_ = makeFunc("#66f", "蓝", blueSpin_);
	rgbLayout->addLayout(redLayout_);
	rgbLayout->addLayout(greenLayout_);
	rgbLayout->addLayout(blueLayout_);

	connect(redSpin_, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this](int value)
	{
		selectedColor_.setRed(value);
		colorWidget_->setStyleSheet(QString("border-radius: 20px; background-color: %1;")
			.arg(DStyle::color2Str(selectedColor_)));
		htmlEdit_->blockSignals(true);
		htmlEdit_->setText(selectedColor_.name().toUpper());
		htmlEdit_->blockSignals(false);
		refreshColorWidgetWidth();
	});
	connect(greenSpin_, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this](int value)
	{
		selectedColor_.setGreen(value);
		colorWidget_->setStyleSheet(QString("border-radius: 20px; background-color: %1;")
			.arg(DStyle::color2Str(selectedColor_)));
		htmlEdit_->blockSignals(true);
		htmlEdit_->setText(selectedColor_.name().toUpper());
		htmlEdit_->blockSignals(false);
		refreshColorWidgetWidth();
	});
	connect(blueSpin_, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this](int value)
	{
		selectedColor_.setBlue(value);
		colorWidget_->setStyleSheet(QString("border-radius: 20px; background-color: %1;")
			.arg(DStyle::color2Str(selectedColor_)));
		htmlEdit_->blockSignals(true);
		htmlEdit_->setText(selectedColor_.name().toUpper());
		htmlEdit_->blockSignals(false);
		refreshColorWidgetWidth();
	});

	return rgbLayout;
}

void DColorDialog::refreshColorSpin()
{
	redSpin_->blockSignals(true);
	greenSpin_->blockSignals(true);
	blueSpin_->blockSignals(true);
	redSpin_->setValue(selectedColor_.red());
	greenSpin_->setValue(selectedColor_.green());
	blueSpin_->setValue(selectedColor_.blue());
	redSpin_->blockSignals(false);
	greenSpin_->blockSignals(false);
	blueSpin_->blockSignals(false);
	refreshColorWidgetWidth();
}

void DColorDialog::refreshColorWidgetWidth()
{
	int r = selectedColor_.red();
	int g = selectedColor_.green();
	int b = selectedColor_.blue();

	//int maxColorValue = std::max({r, g, b});
	int maxColorValue = 255;

	int newRulerWidth = rulerWidget_->width() - 110 - 6;
	//if (newRulerWidth != oldRulerWidth_)  // rgb数值改变时，宽度并没有改变
	{
		int maxColorWidth = newRulerWidth - 110;
		int redWidgetWidth = maxColorWidth * r / maxColorValue + 110;
		int greenWidgetWidth = maxColorWidth * g / maxColorValue + 110;
		int blueWidgetWidth = maxColorWidth * b / maxColorValue + 110;

		//qWarning() << maxWidth << redWidth << greenWidth << blueWidth;
		//qWarning() << (newRulerWidth - 110 - 6 - 80 - redWidth) << (redWidth + 80);

		redLayout_->setStretch(0, newRulerWidth - redWidgetWidth);
		redLayout_->setStretch(1, redWidgetWidth);
		greenLayout_->setStretch(0, newRulerWidth - greenWidgetWidth);
		greenLayout_->setStretch(1, greenWidgetWidth);
		blueLayout_->setStretch(0, newRulerWidth - blueWidgetWidth);
		blueLayout_->setStretch(1, blueWidgetWidth);
	}
}

void DColorDialog::resizeEvent(QResizeEvent* event)
{
	refreshColorWidgetWidth();
}

void DColorDialog::mousePressEvent(QMouseEvent* event)
{
	pressPos_ = event->pos();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	pressGlobalPos_ = event->globalPosition().toPoint();
#else
	pressGlobalPos_ = event->globalPos();
#endif
	pressRect_ = geometry();
	mouseIsPressed_ = true;
}

void DColorDialog::mouseMoveEvent(QMouseEvent* event)
{
	QPoint point = event->pos();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	auto global_pos = event->globalPosition().toPoint();
#else
	auto global_pos = event->globalPos();
#endif

	if (mouseIsPressed_)
	{
		switch (mouseState_)
		{
		case MoveState:
		{
			move(global_pos - pressPos_);
			break;
		}
		default:
		{
			QRect newRect = pressRect_;
			if (dragLeft & mouseState_)
			{
				newRect.setLeft(global_pos.x() - pressPos_.x());
				if (newRect.width() < minimumWidth())
					newRect.setLeft(newRect.right() - minimumWidth());
			}
			else if (dragRight & mouseState_)
			{
				newRect.setRight(global_pos.x() - pressPos_.x() + newRect.width());
				if (newRect.width() < minimumWidth())
					newRect.setRight(newRect.left() + minimumWidth());
			}
			if (dragTop & mouseState_)
			{
				newRect.setTop(global_pos.y() - pressPos_.y());
				if (newRect.height() < minimumHeight())
					newRect.setTop(newRect.bottom() - minimumHeight());
			}
			else if (dragBottom & mouseState_)
			{
				newRect.setBottom(global_pos.y() - pressPos_.y() + newRect.height());
				if (newRect.height() < minimumHeight())
					newRect.setBottom(newRect.top() + minimumHeight());
			}
			setGeometry(newRect);
			break;
		}
		}
	}
	else
	{
		mouseState_ = FreeState;
		if (point.y() < DRAG_SPACE)
			mouseState_ = dragTop;
		else if ((height() - point.y()) < DRAG_SPACE)
			mouseState_ = dragBottom;
		else if (point.x() < DRAG_SPACE)
			mouseState_ = dragLeft;
		else if ((width() - point.x()) < DRAG_SPACE)
			mouseState_ = dragRight;
		else if (point.x() < 40
			&& point.y() < 40
			&& dPowSum(40 - point.x(), 40 - point.y()) > 35 * 35)
			mouseState_ = dragTopLeft;
		else if (point.x() < 40
			&& (height() - point.y()) < 40
			&& dPowSum(40 - point.x(), 40 - (height() - point.y())) > 35 * 35)
			mouseState_ = dragBottomLeft;
		else if ((width() - point.x()) < 40
			&& point.y() < 40
			&& dPowSum(40 - (width() - point.x()), 40 - point.y()) > 35 * 35)
			mouseState_ = dragTopRight;
		else if ((width() - point.x()) < 40
			&& (height() - point.y()) < 40
			&& dPowSum(40 - (width() - point.x()), 40 - (height() - point.y())) > 35 * 35)
			mouseState_ = dragBottomRight;
		else if (point.y() < 20)
			mouseState_ = MoveState;

		switch (mouseState_)
		{
		case dragLeft:
		case dragRight:
			setCursor(Qt::SizeHorCursor);
			break;
		case dragTop:
		case dragBottom:
			setCursor(Qt::SizeVerCursor);
			break;
		case dragTopLeft:
		case dragBottomRight:
			setCursor(Qt::SizeFDiagCursor);
			break;
		case dragTopRight:
		case dragBottomLeft:
			setCursor(Qt::SizeBDiagCursor);
			break;
		default:
			unsetCursor();
		}
	}
}

void DColorDialog::mouseReleaseEvent(QMouseEvent* event)
{
	mouseIsPressed_ = false;
}
