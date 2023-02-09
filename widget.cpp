#include "widget.h"
#include "ui_widget.h"

#include <QAudioOutput>
#include <QCollator>
#include <QDebug>
#include <QKeyEvent>
#include <QFileDialog>
#include <QLocale>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QPixmap>
#include <QRandomGenerator64>
#include <QRegularExpression>
#include <QScrollBar>
#include <QShortcut>
#include <QTimer>
#include "settinghandler.h"

#define RADIUS 40
#define BORDER 20
#define MUSIC_HEIGHT 45
static QStringList TYPE_LIST = {"mp3", "flac", "wav", "ogg", "acc"};

Widget::Widget(const QString& filepath, QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget), hook_(Hook::getInstance()), pressed_ctrl_(false), moving_progress(false), player_(new QMediaPlayer(this)), audio_op_(new QAudioOutput(this))
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAcceptDrops(true);
    grabKeyboard();
    hook_->installHook();
    connect(hook_, &Hook::sendKeyType, this, &Widget::slot_key_pressed);

    // 初始化设置
    SettingHandler::init_setting();
    ui->find_widget->hide();
    ui->label_dir->setText(SettingHandler::get_music_dir());

    layout_ = new QGridLayout();
    layout_->setSpacing(7);
    layout_->setAlignment(Qt::AlignTop);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setContentsMargins(0, 0, 0, 0);
    ui->scrollArea->widget()->setLayout(layout_);//把布局放置到QScrollArea的内部QWidget中

    set_listener();

    audio_op_->setVolume(SettingHandler::get_volume());
    player_->setAudioOutput(audio_op_);
    if (filepath.isEmpty())  // 没有指定打开的歌曲则打开默认文件夹
    {
        if (SettingHandler::get_music_dir() != "")
        {
            QDir dir(SettingHandler::get_music_dir());
            dir.setFilter(QDir::Files);

            QStringList type_filter;
            std::for_each(TYPE_LIST.begin(), TYPE_LIST.end(), [&type_filter](const QString& t)
            {
                type_filter.push_back("*." + t);
            });

            dir.setNameFilters(type_filter);

            QStringList list = dir.entryList(QDir::Files);
            if (list.size() > 0)
            {
                // 先排个序
                auto coll = QCollator(QLocale(QLocale::Chinese));
                std::sort(list.begin(), list.end(), coll);

                ui->label_info->hide();
                for (const auto &url_str : list)
                {
                    add_music(QUrl::fromLocalFile(dir.absolutePath() + '/' + url_str));
                }

                // 如果上次播放的音乐也在这个文件夹中
                now_music_it_ = btn_list_.end();
                for (auto it = btn_list_.begin(); it != btn_list_.end(); ++it)
                {
                    if ((*it)->get_url() == SettingHandler::get_last_music())
                    {
                        now_music_it_ = it;
                        break;
                    }
                }

                qDebug() << "now_music_index_ =" << *now_music_it_;
                if (now_music_it_ == btn_list_.end())  // 如果上次播放的音乐不在这个文件夹中，则从头开始播放
                {
                    now_music_it_ = btn_list_.begin();
                    btn_list_.first()->setStyleSheet("background-color: #b6d1c8;");
                    player_->setSource(btn_list_.first()->get_url());
                }
                else
                {
                    (*now_music_it_)->setStyleSheet("background-color: #b6d1c8;");
                    player_->setSource((*now_music_it_)->get_url());
                }
            }
        }
    }
    else
    {
        ui->label_info->hide();
        QUrl url = QUrl::fromLocalFile(filepath);
        add_music(url);
        now_music_it_ = btn_list_.begin();
        btn_list_.first()->setStyleSheet("background-color: #b6d1c8;");
        player_->setSource(url);
        player_->play();
    }

    play_mode = SettingHandler::get_old_mode();
    switch (play_mode)
    {
    case ONE_AGAIN:
        player_->setLoops(-1);
        ui->btn_mode->setIcon(QIcon(":/svgs/one_again.svg"));
        break;
    case AGAIN:
        player_->setLoops(1);
        ui->btn_mode->setIcon(QIcon(":/svgs/again.svg"));
        break;
    case RANDOM:
        random_index_ = 0;
        random_index_list_.push_back(now_music_it_ - btn_list_.begin());
        player_->setLoops(1);
        ui->btn_mode->setIcon(QIcon(":/svgs/random.svg"));
        break;
    }
}

void Widget::set_listener()
{
    // 音乐播放状态改变事件
    connect(player_, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state)
    {
        switch (state)
        {
        case QMediaPlayer::PlayingState:
            ui->btn_play->setIcon(QIcon(":/svgs/pause.svg"));
            // 每次播放音乐都
            break;
        case QMediaPlayer::PausedState:
            ui->btn_play->setIcon(QIcon(":/svgs/play.svg"));
            break;
        case QMediaPlayer::StoppedState:
            ui->btn_play->setIcon(QIcon(":/svgs/play.svg"));
            break;
        }
    });

    // 发生错误
    connect(player_, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error err, const QString& err_str)
    {
        qDebug() << err << "\n" << err_str << "\n";
        QMessageBox::critical(this, "发生了意想不到的事情", "详情：" + err_str + "\n文件：" + player_->source().fileName());
    });

    // 歌曲发生变化
    connect(player_, &QMediaPlayer::sourceChanged, this, [this](const QUrl &media)
    {
        QString file_type = media.fileName().section('.', 0, -1);
        QString file_name = media.fileName().section('.', 0, -2);

        ui->btn_music_name->setText(file_name);
        SettingHandler::set_last_music(media);
    });


    // 一些没用的事件
    connect(player_, &QMediaPlayer::tracksChanged, this, []()
    {
        qDebug() << 1;
    });
    connect(player_, &QMediaPlayer::videoOutputChanged, this, []()
    {
        qDebug() << 2;
    });
    connect(player_, &QMediaPlayer::seekableChanged, this, [](bool )
    {
        qDebug() << 3;
    });
    connect(player_, &QMediaPlayer::playbackStateChanged, this, []()
    {
        qDebug() << 4;
    });
    connect(player_, &QMediaPlayer::playbackRateChanged, this, []()
    {
        qDebug() << 5;
    });
    connect(player_, &QMediaPlayer::metaDataChanged, this, []()
    {
        qDebug() << 6;
    });
    connect(player_, &QMediaPlayer::activeTracksChanged, this, []()
    {
        qDebug() << 7;
    });
    connect(player_, &QMediaPlayer::audioOutputChanged, this, []()
    {
        qDebug() << 8;
    });
    connect(player_, &QMediaPlayer::bufferProgressChanged, this, []()
    {
        qDebug() << 9;
    });
    connect(player_, &QMediaPlayer::hasAudioChanged, this, []()
    {
        qDebug() << 10;
    });

    // 使本次播放进度变成上次关闭时的进度
    static bool first_play = true;
    connect(player_, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status)
    {
        if (first_play && status == QMediaPlayer::LoadedMedia)
        {
            first_play = false;
            player_->setPosition(SettingHandler::get_music_position());
//            qDebug() << "pos = " << SettingHandler::get_music_position() << player_->position() << player_->duration();
            player_->play();
        }
    });

    // 音乐时长改变
    connect(player_, &QMediaPlayer::durationChanged, this, [this](qint64 duration)
    {
        auto time_s = duration / 1000;
//        ui->label_end->setText(QString::number(time_s / 60).append(":%1").arg(time_s % 60, 2, 10, QLatin1Char('0')));
        // 设置进度条范围
        ui->progress->setMaximum(time_s);
    });

    // 关闭按钮
    connect(ui->btn_shutdown, &QPushButton::clicked, this, [this]()
    {
        close();
    });

    // 播放暂停按钮
    connect(ui->btn_play, &QPushButton::clicked, this, [this]()
    {
        auto state = player_->playbackState();
        if (QMediaPlayer::StoppedState == state || QMediaPlayer::PausedState == state)
        {
            player_->play();
        }
        else if (QMediaPlayer::PlayingState == state)
        {
            player_->pause();
        }
    });

    // 上一首
    connect(ui->btn_previoud, &QPushButton::clicked, this, [this]()
    {
        previous_music();
    });

    // 下一首
    connect(ui->btn_next, &QPushButton::clicked, this, [this]()
    {
        next_music();
    });

    // 音量减
    connect(ui->btn_down, &QPushButton::clicked, this, [this]()
    {
        if (SettingHandler::get_volume() > 0.05f)
            SettingHandler::set_volume(SettingHandler::get_volume() - 0.05f);
        else
            SettingHandler::set_volume(0.0f);
        audio_op_->setVolume(SettingHandler::get_volume());
    });

    // 音量加
    connect(ui->btn_up, &QPushButton::clicked, this, [this]()
    {
        if (SettingHandler::get_volume() < 0.95f)
            SettingHandler::set_volume(SettingHandler::get_volume() + 0.05f);
        else
            SettingHandler::set_volume(1.0f);
        audio_op_->setVolume(SettingHandler::get_volume());
    });

    // 模式切换按钮
    connect(ui->btn_mode, &QPushButton::clicked, this, [this]()
    {
        if (play_mode == AGAIN)
        {
            play_mode = ONE_AGAIN;
            player_->setLoops(-1);
            ui->btn_mode->setIcon(QIcon(":/svgs/one_again.svg"));
        }
        else if (play_mode == ONE_AGAIN)
        {
            random_index_list_.clear();
            random_index_list_.push_back(now_music_it_ - btn_list_.begin());
            random_index_ = 0;
            play_mode = RANDOM;
            player_->setLoops(1);
            ui->btn_mode->setIcon(QIcon(":/svgs/random.svg"));
        }
        else
        {
            play_mode = AGAIN;
            player_->setLoops(1);
            ui->btn_mode->setIcon(QIcon(":/svgs/again.svg"));
        }
        SettingHandler::set_old_mode(play_mode);
    });

    // 更多按钮
    connect(ui->btn_more, &QPushButton::clicked, this, [this]()
    {
        switch (ui->stacked_widget->currentIndex())
        {
        case 0:
            ui->btn_more->setIcon(QIcon(":/svgs/back.svg"));
            ui->stacked_widget->setCurrentIndex(1);
            break;
        case 1:
            ui->btn_more->setIcon(QIcon(":/svgs/more.svg"));
            ui->stacked_widget->setCurrentIndex(0);
            break;
        }
    });

    // 播放完毕
    connect(player_, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status)
    {
        if (QMediaPlayer::EndOfMedia == status)
            next_music();
    });

    // 音乐前进，改变进度条
    connect(player_, &QMediaPlayer::positionChanged, this, [this](qint64 pos)
    {
        if (! moving_progress)  // 如果没有手动拖动进度条，才根据音乐进度改变进度条
        {
            if (pos > 0)
                SettingHandler::set_music_position(pos);
//            qDebug() << pos << player_->position();
            auto time_s = pos / 1000;
            auto rest_time = (player_->duration() - pos) / 1000;
            ui->label_now->setText(QString::number(time_s / 60).append(":%1").arg(time_s % 60, 2, 10, QLatin1Char('0')));
            ui->label_rest->setText(QString::number(rest_time / 60).append(":%1").arg(rest_time % 60, 2, 10, QLatin1Char('0')));
            ui->progress->setValue(time_s);  // 进度条1格对应1秒
        }
    });

    // 按下进度条，停止根据音乐改变进度条
    connect(ui->progress, &QSlider::sliderPressed, this, [this]()
    {
        moving_progress = true;
    });

    connect(ui->progress, &QSlider::actionTriggered, this, [this](int action)
    {
        switch (action)
        {
        case 0:
            player_->setPosition(ui->progress->value() * 1000);
            moving_progress = false;
            break;
        case 3:
        case 4:
            moving_progress = true;
            break;
        }
    });

    // 松开进度条，改变音乐进度
    connect(ui->progress, &QSlider::sliderReleased, this, [this]()
    {
        player_->setPosition(ui->progress->value() * 1000);
        moving_progress = false;
    });

    // 歌曲名按钮点击事件
    connect(ui->btn_music_name, &QPushButton::clicked, this, [this]()
    {
        ui->scrollArea->ensureWidgetVisible(*now_music_it_);
    });

    // 更改初始目录按钮
    connect(ui->btn_change_dir, &QPushButton::clicked, this, [this]()
    {
        QString dir = QFileDialog::getExistingDirectory(this, "选择音乐目录", ui->label_dir->text());
        ui->label_dir->setText(dir);
        SettingHandler::set_music_dir(dir);
    });

    connect(ui->le_find, &QLineEdit::textChanged, this, [this]()
    {
        if (ui->le_find->text() == "")
            ui->label_count->setText("0/0");
        else
            find_music(ui->le_find->text());
    });

    connect(ui->btn_left, &QPushButton::clicked, this, [this]()
    {
        if (find_index > 0)
            --find_index;
        else
            find_index = find_index_list.size() - 1;

        ui->scrollArea->ensureWidgetVisible(btn_list_.at(find_index_list.at(find_index)));
        ui->label_count->setText(QString::number(find_index + 1) + "/" + QString::number(find_index_list.size()));
    });

    connect(ui->btn_right, &QPushButton::clicked, this, [this]()
    {
        if (find_index < find_index_list.size() - 1)
            ++find_index;
        else
            find_index = 0;

        ui->scrollArea->ensureWidgetVisible(btn_list_.at(find_index_list.at(find_index)));
        ui->label_count->setText(QString::number(find_index + 1) + "/" + QString::number(find_index_list.size()));
    });
}

void Widget::next_music()
{
    // 重新设置旧的歌曲按钮的颜色
    (*now_music_it_)->setStyleSheet("QPushButton {background-color: rgba(182, 209, 200, 0.25);} QPushButton:hover {background-color: rgba(182, 209, 200, 0.5);}");

    if (play_mode == AGAIN)  // 循环播放
    {
        ++now_music_it_;
        if (now_music_it_ == btn_list_.end())
            now_music_it_ = btn_list_.begin();
    }
    else if (play_mode == RANDOM)  // 随机播放
    {
        ++random_index_;
        if (random_index_ == random_index_list_.size())
        {
            int music_index = QRandomGenerator64::global()->bounded(0, btn_list_.size());
            random_index_list_.push_back(music_index);
        }
            now_music_it_ = btn_list_.begin() + random_index_list_.at(random_index_);
    }

    // 设置新的歌曲按钮的颜色
    (*now_music_it_)->setStyleSheet("background-color: rgb(182, 209, 200);");
    player_->setSource((*now_music_it_)->get_url());
    player_->play();
}

void Widget::previous_music()
{
    // 重新设置旧的歌曲按钮的颜色
    (*now_music_it_)->setStyleSheet("QPushButton {background-color: rgba(182, 209, 200, 0.25);} QPushButton:hover {background-color: rgba(182, 209, 200, 0.5);}");

    if (play_mode == RANDOM)
    {
        if (random_index_ > 0)
            --random_index_;
        else
        {
            int music_index = QRandomGenerator64::global()->bounded(0, btn_list_.size());
            random_index_list_.push_front(music_index);
        }

        now_music_it_ = btn_list_.begin() + random_index_list_.at(random_index_);
    }
    else
    {
        if (now_music_it_ == btn_list_.begin())
            now_music_it_ = btn_list_.end() - 1;
        else
            --now_music_it_;

    }
    // 设置新的歌曲按钮的颜色
    (*now_music_it_)->setStyleSheet("background-color: rgb(182, 209, 200);");
    player_->setSource((*now_music_it_)->get_url());
    player_->play();
}

void Widget::add_music(const QUrl& url)
{
    MusicButton* btn = new MusicButton(url, this);
    btn->setText(url.fileName());
    btn->setMinimumSize(QSize(60, MUSIC_HEIGHT));
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, this, [this, btn]()
    {
        // 重新设置旧的歌曲按钮的颜色
        (*now_music_it_)->setStyleSheet("QPushButton {background-color: rgba(182, 209, 200, 0.25);} QPushButton:hover {background-color: rgba(182, 209, 200, 0.5);}");
        int music_index = btn_list_.indexOf(btn);
        now_music_it_ = btn_list_.begin() + music_index;
        // 设置新的歌曲按钮的颜色
        btn->setStyleSheet("background-color: rgb(182, 209, 200);");
        player_->setSource(btn->get_url());
        player_->play();

        if (play_mode == RANDOM)
        {
            random_index_list_.clear();
            random_index_list_.push_back(music_index);
            random_index_ = 0;
        }
    });
    layout_->addWidget(btn);  //把按钮添加到布局控件中
    btn_list_.push_back(btn);
}

void Widget::find_music(const QString& word)
{
    if (word == "")
        return;

    find_index = 0;
    ui->label_count->setText("0/0");
    find_index_list.clear();
    for (int i = 0; i < btn_list_.size(); ++i)
    {
        QRegularExpression reg(".*" + word + ".*");
        auto ret = reg.match(btn_list_.at(i)->get_filename());
        if (ret.hasMatch())
        {
            find_index_list.push_back(i);
        }
    }

    if (find_index_list.size() > 0)
    {
        ui->scrollArea->ensureWidgetVisible(btn_list_.at(find_index_list.first()));
        ui->label_count->setText(QString::number(find_index + 1) + "/" + QString::number(find_index_list.size()));
    }
}

void Widget::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void Widget::dropEvent(QDropEvent *event)
{
    // 在音乐页才接受拖入事件
    if (ui->stacked_widget->currentIndex() != 0)
        return;

    bool play = btn_list_.isEmpty();

    auto all = event->mimeData()->urls();
    for (const auto &url : all)
    {
        QString type = url.toLocalFile().section('.', -1);
        qDebug() << type;
        if (TYPE_LIST.indexOf(type) != -1)
            add_music(url);
    }

    // 重新设置滚动条的数值
    ui->scrollArea->verticalScrollBar()->setMaximum(btn_list_.size());

    // 说明本来没有歌曲在播放列表中
    if (play && ! btn_list_.isEmpty())
    {
        ui->label_info->hide();
        now_music_it_ = btn_list_.begin();
        btn_list_.first()->setStyleSheet("background-color: #b6d1c8;");
        player_->setSource((*now_music_it_)->get_url());
        player_->play();
    }
}

void Widget::mousePressEvent(QMouseEvent *ev)
{
    auto pos = ev->pos();
    if (pos.y() < 20)
    {
        this_is_move_window = true;
        press_x = pos.x();
        press_y = pos.y();
    }
}

void Widget::mouseMoveEvent(QMouseEvent *ev)
{
    if (this_is_move_window)
    {
        auto global_pos = ev->globalPosition();
        move(global_pos.x() - press_x, global_pos.y() - press_y);
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *)
{
    this_is_move_window = false;
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    qDebug() << event->key();
    switch (event->key())
    {
    case 32:  // space
        if (QMediaPlayer::StoppedState == player_->playbackState() || QMediaPlayer::PausedState == player_->playbackState())
        {
            player_->play();
        }
        else if (QMediaPlayer::PlayingState == player_->playbackState())
        {
            player_->pause();
        }
        break;
    case 16777249:  // ctrl
        pressed_ctrl_ = true;
        break;
    case Qt::Key_Left:
        if (pressed_ctrl_)
        {
            previous_music();
        }
        else
        {
            player_->setPosition(player_->position() - 2000);
        }
        break;
    case Qt::Key_Right:
        if (pressed_ctrl_)
        {
            next_music();
        }
        else
        {
            player_->setPosition(player_->position() + 2000);
        }
        break;
    case Qt::Key_Up:
        if (SettingHandler::get_volume() < 0.95f)
            SettingHandler::set_volume(SettingHandler::get_volume() + 0.05f);
        else
            SettingHandler::set_volume(1.0f);
        audio_op_->setVolume(SettingHandler::get_volume());
        break;
    case Qt::Key_Down:
        if (SettingHandler::get_volume() > 0.05f)
            SettingHandler::set_volume(SettingHandler::get_volume() - 0.05f);
        else
            SettingHandler::set_volume(0.0f);
        audio_op_->setVolume(SettingHandler::get_volume());
        break;
    case Qt::Key_F:
        if (pressed_ctrl_)  // 按了ctrl + f弹出搜索框
        {
            if (ui->find_widget->isHidden())
            {
                ui->find_widget->show();
                releaseKeyboard();
                ui->le_find->setFocus();
            }
            else
            {
                ui->find_widget->hide();
                ui->le_find->setText("");
            }
        }
        break;
//    case 16777220:
//    case 16777221:  // 回车
//        if (ui->le_find->hasFocus())  // // 查找播放列表的输入框按下回车
//        {
//            find_music(ui->le_find->text());
//        }
//        break;
    }

    QWidget::keyPressEvent(event);
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Control:
        pressed_ctrl_ = false;
        break;
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::slot_key_pressed(DWORD key)
{
    auto state = player_->playbackState();
    switch (key)
    {
    case 179ul:
        if (QMediaPlayer::StoppedState == state || QMediaPlayer::PausedState == state)
            player_->play();
        else if (QMediaPlayer::PlayingState == state)
            player_->pause();
        break;
    case 176ul:
        next_music();
        break;
    case 177ul:
        previous_music();
        break;
    case 178ul:
        player_->stop();
        break;
    }
}

