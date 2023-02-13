#ifndef WIDGET_H
#define WIDGET_H

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
    void on_btn_change_dir_clicked();

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
    QList<MusicButton*>::iterator now_music_it_;
    QList<MusicButton*> btn_list_;
    int find_index;
    QList<int> find_index_list;
    QList<int> random_index_list_;  // 随机播放时保存音乐顺序下标的列表
    int random_index_;  // 随机播放时的列表下表

    int press_x;
    int press_y;
    bool pressed_ctrl_;
    bool this_is_move_window;

};
#endif // WIDGET_H
