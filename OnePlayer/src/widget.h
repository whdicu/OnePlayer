#ifndef WIDGET_H
#define WIDGET_H
#pragma execution_character_set("utf-8")
#include "HDBase/DVector.hpp"
#include "basemusicbutton.h"
#include "OnePlayerStruct.h"
#include "onlinehandler.h"
#include "PlayerBase.h"
#include <QGridLayout>
#include <QMediaPlayer>
#include <QLabel>
#include <QWidget>
#include "windows.h"

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
	void init();
	void uninit();
    void animateShow(bool fromCursor = true);
    void animateHide();
	bool isAnimateHide() { return isAnimateHide_; }
	
	// Widget构造函数失败时，为false
	bool initSuccess() { return initSuccess_; }

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
    void on_btn_music_name_clicked();

//	void slot_btn_open_dir_clicked();
//	void slot_btn_change_dir_clicked();
//	void slot_btn_open_dir_download_clicked();
//	void slot_btn_change_dir_download_clicked();
    void slotPlayerModeChanged(PLAYER_MODE playerMode);

    void slotKeyPressed(DWORD key);
    void slotPositionChanged(qint64 pos);
    void slotSearchEditClose();
    void slotMusicIndexChanged(DSizeType oldIndex, DSizeType newIndex);
	void slotMenuBtnClicked(const QString& text);
	void slotLocalMusicBtnClicked(DSizeType musicIndex);
	void slotNeteaseMusicBtnClicked(dint64 musicId);
	void slotBGModeChanged(BG_MODE bgMode);
	void slotNeteasePlayListClicked(const NeteasePlayListInfo& info);
     
signals:
	void sigChangeSystemIconToolTip(const QString& tip);

// 控件动画
private:
    // stacked_music_btn
    QTimer* stackedMusicBtnAnimationTimer_;
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

    // 初始化右侧控件
    void initMultiFuncWidget();

	// 画图片
	void drawImage(const QImage& image);

	// 刷新音乐信息控件，info里需要有image
    void refreshImageWidget(const MusicInfo& info);
	void setListener();
	void refreshMusicBtns();
	void refreshNeteaseMusicBtns(dint64 playListId);
	void refreshPlayListBtns();
	void appendNeteasePlayListBtns(const DVector<NeteasePlayListInfo>& infos);
	BaseMusicButton* addLocalMusicBtn(const QUrl& url);
	BaseMusicButton* addNeteaseMusicBtn(const NeteaseSongInfo& info);
	//    void add_online_music(const MusicInfo& music);
	//    void init_local();
	//    void init_mysite();
	//    void init_online();
	//    void init_netease();
    void setMusicBtnStyle(int index, void (BaseMusicButton::* setStyleFunc)());
	void setPlayMode(PLAY_MODE mode);
	void showMusicBtnAt(DSizeType index);


    Ui::Widget *ui;
    bool movingProgress_;
    PlayerBase* player_;
	bool initSuccess_;
    
    // 拖动窗口时记录按下的xy，播放动画时用来记录隐藏前的xy
    int pressX_;
    int pressY_;
    bool pressedCtrl_;
    bool thisIsMoveWindow_;

    bool isShowAnimation_;  // 是播放的主界面显示动画还是隐藏动画
    bool shutdownBtnClicked_;

	bool isAnimateHide_;
};
#endif // WIDGET_H
