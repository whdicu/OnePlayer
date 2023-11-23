#ifndef WIDGET_H
#define WIDGET_H
#pragma execution_character_set("utf-8")
#include "HDBase/DList.hpp"
#include "hook.h"
#include "basemusicbutton.h"
#include "OnePlayerStruct.h"
#include "onlinehandler.h"
#include "PlayerBase.h"
#include <QGridLayout>
#include <QMediaPlayer>
#include <QLabel>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class QPropertyAnimation;

class Widget : public QWidget
{
    Q_OBJECT

public:
	Widget(const QString& filepath = "", QWidget *parent = nullptr);
	~Widget();
    void animateShow();
    void animateHide();

private slots:
    void on_btn_shutdown_clicked();
    void on_btn_play_clicked();
    void on_btn_previoud_clicked();
    void on_btn_next_clicked();
    void on_btn_down_clicked();
    void on_btn_up_clicked();
    void on_btn_mode_clicked();
    void on_btn_more_clicked();
    void on_btn_min_clicked();
//    void on_btn_music_name_clicked();
//    void on_btn_left_clicked();
//    void on_btn_right_clicked();
//    void on_btn_search_clicked();

//	void slot_btn_open_dir_clicked();
//	void slot_btn_change_dir_clicked();
//	void slot_btn_open_dir_download_clicked();
//	void slot_btn_change_dir_download_clicked();
//    void slot_cmb_mode_currentIndexChanged(int index);

    void slotKeyPressed(DWORD key);
    void slotPositionChanged(qint64 pos);
    void slotSearchEditClose();
    void slotMusicIndexChanged(DSizeType oldIndex, DSizeType newIndex);
     
// 控件动画
private:
    // stacked_music_btn
	QPropertyAnimation* stackedMusicBtnAnimation_;
    void animationStackedMusicBtnSmall();
    void animationStackedMusicBtnBig();

	// stacked_local_btns
	QPropertyAnimation* stackedLocalBtnsAnimation_;
	void animationStackedLocalBtnsShort();
	void animationStackedLocalBtnsLong();

    QPropertyAnimation* animation_;
    QPropertyAnimation* stackedMusicBtnAnimationSub1_;
  
    
private:
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

//    void clear_button(QVBoxLayout* layout);
    void refreshImageWidget(const MusicInfo& info);
	//    void play_music(DSizeType musicIndex);
	void setListener();
	//    void set_setting_tab_listener();
	void refreshMusicBtns();
	//    void next_music();
	//    void previous_music();
	BaseMusicButton* addLocalMusicBtn(const QUrl& url);
	//    void add_online_music(const MusicInfo& music);
	//    void init_local();
	//    void init_mysite();
	//    void init_online();
	//    void init_netease();
    void setMusicBtnStyle(int index, void (BaseMusicButton::* setStyleFunc)());
	void setPlayMode(PLAY_MODE mode);


    Ui::Widget *ui;
    Hook* hook_;
    bool movingProgress_;
    PlayerBase* player_;

//    DSizeType now_music_index_;
//    DList<BaseMusicButton*> btn_list_;
//    DList<DSizeType> random_index_list_;  // 随机播放时保存音乐顺序下标的列表
//    DSizeType random_index_;  // 随机播放时的列表下表
    
    // 拖动窗口时记录按下的xy，播放动画时用来记录隐藏前的xy
    int pressX_;
    int pressY_;
    bool pressedCtrl_;
    bool thisIsMoveWindow_;

    bool isShowAnimation_;  // 时播放的主界面显示动画还是隐藏动画
    bool shutdownBtnClicked_;
};
#endif // WIDGET_H
