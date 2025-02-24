#include "dmenu.h"
#include "OnePlayerStruct.h"
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPainter>
#include <QPropertyAnimation>
#include <QApplication>
#include <windows.h>

const static int TIME500 = 500;
const static int TIME300 = 300;
const static int TIME200 = 200;
const static int TIME150 = 150;
const static QString QSS_STYLE = " \
#menu_widget { \
    background-color: white; \
    border: 1px solid #5c5c66; \
    border-radius: 15px; \
} QPushButton { \
    border-radius: 10px; \
    background-color: #b6d1c8; \
    color: #5c5c66; \
}";

void show_top(WId winId)
{
#ifdef Q_OS_WIN32  // windows必须加这个，不然windows10 会不起作用，具体参看activateWindow 函数的文档
    HWND hForgroundWnd = GetForegroundWindow();
    DWORD dwForeID = ::GetWindowThreadProcessId(hForgroundWnd, NULL);
    DWORD dwCurID = ::GetCurrentThreadId();
    ::AttachThreadInput(dwCurID, dwForeID, TRUE);
    ::SetForegroundWindow((HWND)winId);
    ::AttachThreadInput(dwCurID, dwForeID, FALSE);
#endif
}

static DMenu* button_menu = nullptr;
DMenu* DMenu::getButtonMenu()
{
    if (nullptr == button_menu)
		button_menu = new DMenu(BUTTON_MENU_STR_LIST);
    return button_menu;
}

DMenu::DMenu(const QStringList& texts)
    : QWidget(nullptr, Qt::FramelessWindowHint | Qt::Drawer | Qt::WindowStaysOnTopHint)
    , widget_(new QWidget(this))
    , is_hidden_(true)
    , musicIndex_(0)
	, animateLabel_(new QLabel)
	, animationState_(0)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::StrongFocus);

	animateLabel_->setWindowFlags(Qt::FramelessWindowHint);
	animateLabel_->setAttribute(Qt::WA_TranslucentBackground);
	animateLabel_->setScaledContents(true);

	animationMove_ = new QPropertyAnimation(this, "pos");
	animationMove_->setEasingCurve(QEasingCurve::InOutQuad);
	animationMove_->setDuration(TIME300);

    animation_ = new QPropertyAnimation(animateLabel_, "geometry");
    animation_->setEasingCurve(QEasingCurve::InOutQuad);
	animation_->setDuration(TIME150);
	connect(animation_, &QPropertyAnimation::finished, this, [this]()
	{
		switch (animationState_)
		{
		case 0:  // show
			QWidget::show();
			break;
		case 1:  // hide
			break;
		default:
			break;
		}
		animateLabel_->hide();
		switch (animationState_)
		{
		case 0:  // show
			setFocus();
			break;
		}
	});

    move(QCursor::pos());
    widget_->move(0, 0);
    widget_->resize(130, 5 + 35 * texts.size());

    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(5, 5, 5, 5);
    lay->setSpacing(5);
    widget_->setLayout(lay);
    widget_->setObjectName("menu_widget");
    setStyleSheet(QSS_STYLE);

    for (const QString& text : texts)
    {
        QPushButton* btn = new QPushButton(text);
        connect(btn, &QPushButton::clicked, this, [this, text]()
        {
            emit sigBtnClicked(text);
            animateHide();
        });
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedHeight(30);
        lay->addWidget(btn);
    }

    // 界面会一直show，只是使用动画缩小为0*0来代替隐藏
//    resize(0, 0);
//    QWidget::show();

	animateLabel_->setPixmap(grab());
}

DMenu::~DMenu()
{
}

void DMenu::animateMove(int newx, int newy)
{
    animateMove(QPoint(newx, newy));
}

void DMenu::animateMove(QPoint pos)
{
	setFocus();
	animationMove_->stop();
	animationMove_->setStartValue(this->pos());
    animationMove_->setEndValue(pos);
    animationMove_->start();
}

void DMenu::animateShow()
{
	if (!isHidden())
		return;

	animateLabel_->show();
	animation_->stop();
    setFocus();
    is_hidden_ = false;
    int newx = QCursor::pos().x();
    int newy = QCursor::pos().y();
	move(newx, newy);

    //qDebug() << newx << newy << widget_->width() << widget_->height();
    animation_->setStartValue(QRect(newx, newy, 0, 0));
    animation_->setEndValue(QRect(newx, newy, widget_->width(), widget_->height()));
    animation_->start();
	animationState_ = 0;
}

void DMenu::animateHide()
{
	if (isHidden())
		return;

	animateLabel_->setPixmap(grab());
	animateLabel_->show();
	hide();
    animation_->stop();

    is_hidden_ = true;
    int newx = QCursor::pos().x();
    int newy = QCursor::pos().y();

    animation_->setStartValue(QRect(x(), y(), widget_->width(), widget_->height()));
    animation_->setEndValue(QRect(newx, newy, 0, 0));
    animation_->start();
	animationState_ = 1;
}

bool DMenu::setFocus()
{
    show_top(winId());  // 防止setFocus失效
    QWidget::setFocus();
    return hasFocus();
}

void DMenu::show(DSizeType musicIndex)
{
	musicIndex_ = musicIndex;
    if (isHidden())
        animateShow();
    else
        animateMove(QCursor::pos());
}

void DMenu::focusOutEvent(QFocusEvent* event)
{
    if (event->reason() == Qt::MouseFocusReason)
    {
//        qDebug() << "点击了菜单中的按钮";
		animateHide();
    }
    else if (event->reason() == Qt::TabFocusReason)
    {
//        qDebug() << "Widget lost focus due to tab key!";
    }
    else
    {
//        qDebug() << "Widget lost focus.";
        // 发信号，让外部来判断是否需要隐藏
        emit maybeNeedHide();
    }

    QWidget::focusOutEvent(event);
}
