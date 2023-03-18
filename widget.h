#ifndef WIDGET_H
#define WIDGET_H

#include "HDBase/DList.hpp"
#include "hook.h"
#include <QGridLayout>
#include <QMediaPlayer>
#include "musicbutton.h"
#include <QLabel>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

enum PLAY_MODE
{
    AGAIN,
    ONE_AGAIN,
    RANDOM
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(const QString& filepath="", QWidget *parent=nullptr);
    ~Widget();

private slots:
    void slot_key_pressed(DWORD key);
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
    void on_btn_open_dir_clicked();
    void on_btn_change_dir_clicked();
    void on_btn_left_clicked();
    void on_btn_right_clicked();

private:
    void set_listener();
    void next_music();
    void previous_music();
    void add_music(const QUrl& url);
    void find_music(const QString& word);
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

    Ui::Widget *ui;
    Hook* hook_;
    PLAY_MODE play_mode;
    bool moving_progress;
    QMediaPlayer* player_;
    QAudioOutput* audio_op_;
    DSizeType now_music_index_;
    DList<MusicButton*> btn_list_;
    DSizeType find_index;
    DList<DSizeType> find_index_list;
    DList<DSizeType> random_index_list_;  // 随机播放时保存音乐顺序下标的列表
    DSizeType random_index_;  // 随机播放时的列表下表

    int press_x;
    int press_y;
    bool pressed_ctrl_;
    bool this_is_move_window;

};
#endif // WIDGET_H
