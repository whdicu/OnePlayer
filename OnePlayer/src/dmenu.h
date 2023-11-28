#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QFocusEvent>
#include "basemusicbutton.h"

class QPropertyAnimation;

class DMenu : public QWidget
{
    Q_OBJECT

public:
    static DMenu* getButtonMenu();
    BaseMusicButton* getNowBtn() {return now_btn_;}

    void animateMove(int newx, int newy);
    void animateMove(QPoint pos);
    void animateShow();
    void animateHide();
    bool isHidden() { return is_hidden_; }
    bool setFocus();
    void show(BaseMusicButton* btn);

signals:
    void sigBtnClicked(const QString& text);
    void maybeNeedHide();

private:
    DMenu(const QStringList& texts);
    ~DMenu();

    void focusOutEvent(QFocusEvent*);

    QWidget* widget_;
    bool is_hidden_;
    BaseMusicButton* now_btn_;
    QPropertyAnimation* animation_;
};
