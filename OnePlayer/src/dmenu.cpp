#include "dmenu.h"
#include <QDebug>
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
        button_menu = new DMenu({ "下一首播放", "打开文件所在位置", "从列表中移除", "删除" });
    return button_menu;
}

DMenu::DMenu(const QStringList& texts)
    : QWidget(nullptr, Qt::FramelessWindowHint | Qt::Drawer | Qt::WindowStaysOnTopHint)
    , widget_(new QWidget(this))
    , is_hidden_(true)
    , now_btn_(nullptr)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::StrongFocus);
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
            emit btn_clicked(text);
            animateHide();
        });
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedHeight(30);
        lay->addWidget(btn);
    }

    // 界面会一直show，只是使用动画缩小为0*0来代替隐藏
//    resize(0, 0);
//    QWidget::show();
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
    static QPropertyAnimation* animation = nullptr;
    if (nullptr == animation)
    {
        animation = new QPropertyAnimation(this, "pos");
        animation->setDuration(TIME300);
        animation->setEasingCurve(QEasingCurve::InOutQuad);
    }

    animation->setStartValue(QPoint(x(), y()));
    animation->setEndValue(pos);
    animation->start();
}

void DMenu::animateShow()
{
    QWidget::show();
    setFocus();
    is_hidden_ = false;
    int newx = QCursor::pos().x();
    int newy = QCursor::pos().y();
    static QPropertyAnimation* animation = nullptr;
    if (nullptr == animation)
    {
        animation = new QPropertyAnimation(this, "geometry");
        animation->setDuration(TIME150);
        animation->setEasingCurve(QEasingCurve::InOutQuad);
    }
    qDebug() << newx << newy << widget_->width() << widget_->height();
    animation->setStartValue(QRect(newx, newy, 0, 0));
    animation->setEndValue(QRect(newx, newy, widget_->width(), widget_->height()));
    animation->start();
}

void DMenu::animateHide()
{
    is_hidden_ = true;
    int newx = QCursor::pos().x();
    int newy = QCursor::pos().y();
    static QPropertyAnimation* animation = nullptr;
    if (nullptr == animation)
    {
        animation = new QPropertyAnimation(this, "geometry");
        animation->setDuration(TIME150);
        animation->setEasingCurve(QEasingCurve::InOutQuad);
    }
    animation->setStartValue(QRect(x(), y(), widget_->width(), widget_->height()));
    animation->setEndValue(QRect(newx, newy, 0, 0));
    animation->start();
}

bool DMenu::setFocus()
{
    show_top(winId());  // 防止setFocus失效
    QWidget::setFocus();
    return hasFocus();
}

void DMenu::show(BaseMusicButton* btn)
{
    now_btn_ = btn;
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
