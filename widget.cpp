#include "widget.h"
#include "ui_widget.h"

#include "musicbutton.h"
#include "onlinemusicbutton.h"
#include <QAudioOutput>
#include <QCollator>
#include <QDebug>
#include <QDesktopServices>
#include <QKeyEvent>
#include <QFileDialog>
#include <QLocale>
#include <QMediaMetaData>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QRandomGenerator64>
#include <QRegularExpression>
#include <QScrollBar>
#include <QShortcut>
#include <QTcpSocket>
#include <QTimer>
#include "settinghandler.h"

static QStringList TYPE_LIST = {"mp3", "flac", "wav", "ogg", "acc", "m4a"};
const static QString IP = "47.113.231.74";
const static int PORT = 9002;

QRgb getMainColor(const QImage& image)
{
    unsigned long long r = 0, g = 0, b = 0, a = 0;
    unsigned long long cnt = 0;
    // 下半部分图像抽样取点
    for (int i = 0; i < image.width(); i += 4)
    {
        for (int j = image.height() / 2; j < image.height(); j += 4)
        {
            QRgb pix = image.pixel(i, j);
            r += qRed(pix);
            g += qGreen(pix);
            b += qBlue(pix);
            a += qAlpha(pix);
            ++cnt;
        }
    }
    return qRgba(r / cnt, g / cnt, b / cnt, a / cnt);
}

QString getTextColor(const QImage& image)
{
    QRgb c3 = getMainColor(image);
    if (qRed(c3) + qGreen(c3) + qBlue(c3) < 384)
        return "#b6d1c8";
    else
        return "#5c5c66";
}

Widget::Widget(const QString& filepath, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , hook_(Hook::getInstance())
    , moving_progress(false)
    , player_(new QMediaPlayer(this))
    , audio_op_(new QAudioOutput(this))
    , now_music_index_(0)
    , pressed_ctrl_(false)
    , this_is_move_window(false)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAcceptDrops(true);
    grabKeyboard();
    hook_->installHook();
    connect(hook_, &Hook::sendKeyType, this, &Widget::slot_key_pressed);
    ui->stacked_widget->setCurrentIndex(2);

    ui->find_widget->hide();
    ui->label_dir->setText(SETTING_HANDLER->get_music_dir());

    // 圆角遮罩
    QWidget* ww = new QWidget(ui->music);
    ww->move(10, 10);
    ww->resize(ui->stacked_info->width() + 20, ui->stacked_info->height() + 20);
    ww->setStyleSheet("background-color: transparent; border: 10px solid white; border-radius: 30px;");

    set_listener();

    audio_op_->setVolume(SETTING_HANDLER->get_volume());
    player_->setAudioOutput(audio_op_);
    if (filepath.isEmpty())  // 没有指定打开的歌曲则打开默认文件夹
    {
        switch (SETTING_HANDLER->get_player_mode())
        {
        case LOCAL:
            init_local();
            break;
        case MYSITE:
            init_mysite();
            break;
        case ONLINE:
            init_online();
            break;
        }
    }
    else
    {
        ui->stacked_widget->setCurrentIndex(0);
        QUrl url = QUrl::fromLocalFile(filepath);
        add_music(url);
        now_music_index_ = 0;
        play_music(btn_list_.first());
    }

    play_mode = SETTING_HANDLER->get_old_mode();
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
        random_index_list_.pushBack(now_music_index_);
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

    // 先sourceChanged，再metaDataChanged
    connect(player_, &QMediaPlayer::sourceChanged, this, [this](const QUrl &media)
    {
        switch (SETTING_HANDLER->get_player_mode())
        {
        case LOCAL:
        case MYSITE:
        {
//            QString file_type = media.fileName().section('.', 0, -1);
            QString file_name = media.fileName().section('.', 0, -2);
            ui->btn_music_name->setText(file_name);
            ui->label_sound_name->setText(file_name);
        }
            break;
        case ONLINE:
        {
            OnlineMusicButton* btn = static_cast<OnlineMusicButton*> (btn_list_.at(now_music_index_));
            MusicInfo& info = btn->get_info();
            ui->btn_music_name->setText(info.name_);
        }
            break;
        }
        SETTING_HANDLER->set_last_music(media);
    });
    connect(player_, &QMediaPlayer::metaDataChanged, this, [this]()
    {
        QMediaMetaData meta_data = player_->metaData();
        if (meta_data.isEmpty())
            return;

        QString title = meta_data.value(QMediaMetaData::Title).toString();
        QStringList author_list = meta_data.value(QMediaMetaData::Author).toStringList();  // 去重
        QImage thumbnail_image = meta_data.value(QMediaMetaData::ThumbnailImage).value<QImage>();  // 缩略图
        QString album_title = meta_data.value(QMediaMetaData::AlbumTitle).toString();  // 专辑标题
        QStringList album_artist = meta_data.value(QMediaMetaData::AlbumArtist).toStringList();  // 去重 专辑艺术家
//        QString genre = meta_data.value(QMediaMetaData::Genre).toString();  // 流派
//        QStringList contributing_artist = meta_data.value(QMediaMetaData::ContributingArtist).toStringList();  // 去重 贡献艺术家


        // 绘制圆角图片
        static QImage default_image(":/images/music.png");
        if (thumbnail_image.isNull())
            thumbnail_image = default_image;
        QPixmap pixmap = QPixmap::fromImage(thumbnail_image);

        QPixmap resultPixmap(ui->label_image->size());
        resultPixmap.fill(Qt::transparent);
        QPainter painter(&resultPixmap);
        painter.setRenderHints(QPainter::Antialiasing);
        painter.setRenderHints(QPainter::SmoothPixmapTransform);
        QPainterPath path;  // 绘制路径
        //绘制圆角矩形，其中最后两个参数值的范围为（0-99），就是圆角的px值
        path.addRoundedRect(0, 0, ui->label_image->width(), ui->label_image->height(), 20, 20);
        painter.setClipPath(path);
        painter.drawPixmap(0, 0, ui->label_image->width(), ui->label_image->height()
                           , pixmap.scaled(resultPixmap.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

        ui->label_image->setPixmap(resultPixmap);
        ui->widget_music_mask->setPixmap(pixmap);
        ui->page_music_info->setStyleSheet(QString("QLabel{color: %1;}").arg(getTextColor(thumbnail_image)));

        // 设置歌曲名
        if (!title.isEmpty())
        {
            ui->label_sound_name->setText(title);
            ui->btn_music_name->setText(title);
        }

        QSet<QString> singer_set(author_list.begin(), author_list.end());
        QString temp = "";
        for (const QString& singer : singer_set)
        {
            temp.append(singer).append(' ');
        }
        QString singers = temp.trimmed();
        if (!singers.isEmpty())
            ui->label_singer->setText(singers);
        else
            ui->label_singer->setText("未知歌手");


        if (!album_title.isEmpty())
            ui->label_album->setText(album_title);
        else
            ui->label_album->setText("未知专辑");

//        qDebug() << "Comment" << meta_data.value(QMediaMetaData::Comment);
//        qDebug() << "Description" << meta_data.value(QMediaMetaData::Description);
//        qDebug() << "Date" << meta_data.value(QMediaMetaData::Date);
//        qDebug() << "Language" << meta_data.value(QMediaMetaData::Language);
//        qDebug() << "Publisher" << meta_data.value(QMediaMetaData::Publisher);
//        qDebug() << "Copyright" << meta_data.value(QMediaMetaData::Copyright);
//        qDebug() << "Duration" << meta_data.value(QMediaMetaData::Duration);
//        qDebug() << "MediaType" << meta_data.value(QMediaMetaData::MediaType);
//        qDebug() << "FileFormat" << meta_data.value(QMediaMetaData::FileFormat);
//        qDebug() << "AudioBitRate" << meta_data.value(QMediaMetaData::AudioBitRate);
//        qDebug() << "AudioCodec" << meta_data.value(QMediaMetaData::AudioCodec);
//        qDebug() << "TrackNumber" << meta_data.value(QMediaMetaData::TrackNumber);
//        qDebug() << "Composer" << meta_data.value(QMediaMetaData::Composer);
//        qDebug() << "LeadPerformer" << meta_data.value(QMediaMetaData::LeadPerformer);
//        qDebug() << "CoverArtImage" << meta_data.value(QMediaMetaData::CoverArtImage);
//        qDebug() << "Orientation" << meta_data.value(QMediaMetaData::Orientation);
//        qDebug() << "Resolution" << meta_data.value(QMediaMetaData::Resolution);
    });

    // 一些没用的事件
    connect(player_, &QMediaPlayer::tracksChanged, this, []()
    {
//        qDebug() << 1;
    });
    connect(player_, &QMediaPlayer::videoOutputChanged, this, []()
    {
//        qDebug() << 2;
    });
    connect(player_, &QMediaPlayer::seekableChanged, this, [](bool )
    {
//        qDebug() << 3;
    });
    connect(player_, &QMediaPlayer::playbackRateChanged, this, []()
    {
//        qDebug() << 5;
    });
    connect(player_, &QMediaPlayer::activeTracksChanged, this, []()
    {
//        qDebug() << 7;
    });
    connect(player_, &QMediaPlayer::audioOutputChanged, this, []()
    {
//        qDebug() << 8;
    });
    connect(player_, &QMediaPlayer::bufferProgressChanged, this, []()
    {
//        qDebug() << 9;
    });
    connect(player_, &QMediaPlayer::hasAudioChanged, this, []()
    {
//        qDebug() << 10;
    });

    // 使本次播放进度变成上次关闭时的进度
    static bool first_play = true;
    connect(player_, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status)
    {
        if (QMediaPlayer::EndOfMedia == status)
            next_music();

        if (first_play)
        {
            if (status == QMediaPlayer::LoadedMedia)
            {
                first_play = false;
                // 延时播放音乐，防止进度不正确
                QTimer* timer = new QTimer(this);
                connect(timer, &QTimer::timeout, this, [this, timer]()
                {
                    qint64 pos = SETTING_HANDLER->get_music_position();  // 放到player_->play()后面会导致音乐播放后新的进度写入，覆盖原有进度
                    player_->setPosition(pos);
                    player_->play();
                    timer->deleteLater();
                });
                timer->start(10);
            }
        }
    });

    // 音乐时长改变
    connect(player_, &QMediaPlayer::durationChanged, ui->progress, &QSlider::setMaximum);

    // 音乐前进，改变进度条
    connect(player_, &QMediaPlayer::positionChanged, this, [this](qint64 pos)
    {
        if (! moving_progress)  // 如果没有手动拖动进度条，才根据音乐进度改变进度条
        {
            if (pos > 0)
                SETTING_HANDLER->set_music_position(pos);

            auto time_s = pos / 1000;
            auto rest_time = (player_->duration() - pos) / 1000;
            ui->label_now->setText(QString::number(time_s / 60).append(":%1").arg(time_s % 60, 2, 10, QLatin1Char('0')));
            ui->label_rest->setText(QString::number(rest_time / 60).append(":%1").arg(rest_time % 60, 2, 10, QLatin1Char('0')));
            ui->progress->setValue(pos);  // 进度条1格对应1秒
        }

        // 进度超过最大，强制播放下一首
        if (pos > player_->duration())
            next_music();
    });

    // 按下进度条，停止根据音乐改变进度条
    connect(ui->progress, &QSlider::sliderPressed, this, [this]()
    {
        moving_progress = true;
    });

//    connect(ui->progress, &QSlider::actionTriggered, this, [this](int action)
//    {
//        qDebug() << action;
//        switch (action)
//        {
//        case 0:
//            player_->setPosition(ui->progress->value());
//            moving_progress = false;
//            break;
//        case 3:
//        case 4:
//            moving_progress = true;
//            break;
//        }
//    });

    // 松开进度条，改变音乐进度
    connect(ui->progress, &QSlider::sliderReleased, this, [this]()
    {
        player_->setPosition(ui->progress->value());
        moving_progress = false;
    });

    // 搜索框文字改变
    connect(ui->le_find, &QLineEdit::textChanged, this, [this]()
    {
        if (ui->le_find->text() == "")
            ui->label_count->setText("0/0");
        else
            find_music(ui->le_find->text());
    });
}

void Widget::load_music_list(QStringList &list)
{
    if (list.size() > 0)
    {
        // 先排个序
        auto coll = QCollator(QLocale(QLocale::Chinese));
        std::sort(list.begin(), list.end(), coll);

        for (const auto &url_str : list)
        {
            if (SETTING_HANDLER->get_player_mode())
            {
                add_music(url_str);
            }
            else
            {
                add_music(QUrl::fromLocalFile(SETTING_HANDLER->get_music_dir() + '/' + url_str));
            }
        }

        // 如果上次播放的音乐也在这个文件夹中
        now_music_index_ = btn_list_.size();
        for (DSizeType i = 0; i < btn_list_.size(); ++i)
        {
            if (btn_list_.at(i)->get_url() == SETTING_HANDLER->get_last_music())
            {
                now_music_index_ = i;
                break;
            }
        }

        if (now_music_index_ == btn_list_.size())  // 如果上次播放的音乐不在这个文件夹中，则从头开始播放
        {
            now_music_index_ = 0;
            play_music(btn_list_.first());
        }
        else
        {
            play_music(btn_list_.at(now_music_index_));
        }
    }
}

void Widget::next_music()
{
    // 重新设置旧的歌曲按钮的颜色
    btn_list_.at(now_music_index_)->setNormalStyle();

    if (play_mode == AGAIN)  // 循环播放
    {
        ++now_music_index_;
        if (now_music_index_ == btn_list_.size())
            now_music_index_ = 0;
    }
    else if (play_mode == RANDOM)  // 随机播放
    {
        ++random_index_;
        if (random_index_ == random_index_list_.size())
        {
            DSizeType music_index = QRandomGenerator64::global()->bounded(0, (int)btn_list_.size());
            random_index_list_.pushBack(music_index);
        }
            now_music_index_ = random_index_list_.at(random_index_);
    }

    play_music(btn_list_.at(now_music_index_));
}

void Widget::previous_music()
{
    // 重新设置旧的歌曲按钮的颜色
    btn_list_.at(now_music_index_)->setNormalStyle();

    if (play_mode == RANDOM)
    {
        if (random_index_ > 0)
            --random_index_;
        else
        {
            DSizeType music_index = QRandomGenerator64::global()->bounded(0, (int)btn_list_.size());
            random_index_list_.pushFront(music_index);
        }

        now_music_index_ = random_index_list_.at(random_index_);
    }
    else
    {
        if (now_music_index_ == 0)
            now_music_index_ = btn_list_.size() - 1;
        else
            --now_music_index_;

    }
    play_music(btn_list_.at(now_music_index_));
}

void Widget::add_music(const QUrl& url)
{
    MusicButton* btn = new MusicButton(url, this);
    connect(btn, &MusicButton::clicked, this, [this, btn]()
    {
        // 重新设置旧的歌曲按钮的颜色
        btn_list_.at(now_music_index_)->setNormalStyle();
        now_music_index_ = btn_list_.indexOf(btn);
        play_music(btn);

        if (play_mode == RANDOM)
        {
            random_index_ = random_index_list_.size();
            random_index_list_.pushBack(now_music_index_);
            now_music_index_ = random_index_list_.at(random_index_);
        }
    });
    ui->music_layout->addWidget(btn);
    btn_list_.pushBack(btn);
}

void Widget::add_online_music(const MusicInfo& music)
{
    OnlineMusicButton* btn = new OnlineMusicButton(music, this);
    connect(btn, &BaseMusicButton::clicked, this, [this, btn]()
    {
        now_music_index_ = btn_list_.indexOf(btn);
        play_music(btn);

        if (play_mode == RANDOM)
        {
            random_index_ = random_index_list_.size();
            random_index_list_.pushBack(now_music_index_);
            now_music_index_ = random_index_list_.at(random_index_);
        }
    });
    connect(btn, &OnlineMusicButton::download_clicked, this, [this, btn]()
    {

    });

    ui->music_layout_online->addWidget(btn);
    btn_list_.pushBack(btn);
}

void Widget::find_music(const QString& word)
{
    if (word == "")
        return;

    find_index = 0;
    ui->label_count->setText("0/0");
    find_index_list.clear();
    for (DSizeType i = 0; i < btn_list_.size(); ++i)
    {
        QRegularExpression reg(".*" + word + ".*");
        auto ret = reg.match(btn_list_.at(i)->get_filename());
        if (ret.hasMatch())
        {
            find_index_list.pushBack(i);
        }
    }

    if (find_index_list.size() > 0)
    {
        ui->scrollArea->ensureWidgetVisible(btn_list_.at(find_index_list.first()));
        ui->label_count->setText(QString::number(find_index + 1) + "/" + QString::number(find_index_list.size()));
    }
}

void Widget::init_local()
{
    for (BaseMusicButton* btn : btn_list_)
    {
        btn->deleteLater();
    }
    btn_list_.clear();

    if (!SETTING_HANDLER->get_music_dir().isEmpty())
    {
        QDir dir(SETTING_HANDLER->get_music_dir());
        dir.setFilter(QDir::Files);

        QStringList type_filter;
        std::for_each(TYPE_LIST.begin(), TYPE_LIST.end(), [&type_filter](const QString& t)
        {
            type_filter.push_back("*." + t);
        });

        dir.setNameFilters(type_filter);

        QStringList list = dir.entryList(QDir::Files);

        load_music_list(list);
    }

    ui->stacked_widget->setCurrentIndex(btn_list_.isEmpty() ? 2 : 0);
    ui->stacked_music_btn->setCurrentIndex(0);
    ui->btn_more->setIcon(QIcon(":/svgs/more.svg"));
}

void Widget::init_mysite()
{
    for (BaseMusicButton* btn : btn_list_)
    {
        btn->deleteLater();
    }
    btn_list_.clear();

    QTcpSocket* socket = new QTcpSocket(this);
    socket->connectToHost(IP, PORT);
    connect(socket, &QTcpSocket::readyRead, this, [this, socket]()
    {
        QByteArray data = socket->readAll();
        socket->close();
        QString request_text = QString(data);

        int at_index = request_text.indexOf('@');
        if (-1 == at_index)
        {
            qDebug() << "接收的消息中没有找到@:\n" << request_text;
            return;
        }

        int index1 = request_text.indexOf(',');
        if (-1 == index1)
        {
            qDebug() << "接收的消息中没找到逗号:\n" << request_text;
            return;
        }

        QString cmd = request_text.mid(at_index+1, index1-1);
        if ("C1" == cmd)
        {
//            int index1 = request_text.indexOf(',', index1+1);
            QStringList file_list = request_text.mid(index1+1).split(',');
            load_music_list(file_list);
            ui->btn_more->setIcon(QIcon(":/svgs/more.svg"));
            ui->stacked_widget->setCurrentIndex(btn_list_.isEmpty() ? 2 : 0);
            ui->stacked_music_btn->setCurrentIndex(0);
        }
    });
    socket->write("@C1,");
}

void Widget::init_online()
{
    for (BaseMusicButton* btn : btn_list_)
    {
        btn->deleteLater();
    }
    btn_list_.clear();

    ui->btn_more->setIcon(QIcon(":/svgs/more.svg"));
    ui->stacked_widget->setCurrentIndex(0);
    ui->stacked_music_btn->setCurrentIndex(1);
}

void Widget::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void Widget::dropEvent(QDropEvent *event)
{
    // 在设置页不接受拖入事件
    if (ui->stacked_widget->currentIndex() == 1)
        return;

    bool play = btn_list_.isEmpty();

    auto all = event->mimeData()->urls();
    for (const auto &url : all)
    {
        QString type = url.toLocalFile().section('.', -1);

        if (TYPE_LIST.indexOf(type) != -1)
            add_music(url);
    }

    // 重新设置滚动条的数值
    ui->scrollArea->verticalScrollBar()->setMaximum(btn_list_.size());

    // 说明本来没有歌曲在播放列表中
    if (play && ! btn_list_.isEmpty())
    {
        ui->stacked_widget->setCurrentIndex(0);
        now_music_index_ = 0;
        play_music(btn_list_.at(now_music_index_));
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
        if (SETTING_HANDLER->get_volume() < 0.95f)
            SETTING_HANDLER->set_volume(SETTING_HANDLER->get_volume() + 0.05f);
        else
            SETTING_HANDLER->set_volume(1.0f);
        audio_op_->setVolume(SETTING_HANDLER->get_volume());
        break;
    case Qt::Key_Down:
        if (SETTING_HANDLER->get_volume() > 0.05f)
            SETTING_HANDLER->set_volume(SETTING_HANDLER->get_volume() - 0.05f);
        else
            SETTING_HANDLER->set_volume(0.0f);
        audio_op_->setVolume(SETTING_HANDLER->get_volume());
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
    hook_->unInstallHook();
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

// 关闭
void Widget::on_btn_shutdown_clicked()
{
    close();
}

// 播放/暂停
void Widget::on_btn_play_clicked()
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
}

// 上一首
void Widget::on_btn_previoud_clicked()
{
    previous_music();
}

// 下一首
void Widget::on_btn_next_clicked()
{
    next_music();
}

// 音量减
void Widget::on_btn_down_clicked()
{
    if (SETTING_HANDLER->get_volume() > 0.05f)
        SETTING_HANDLER->set_volume(SETTING_HANDLER->get_volume() - 0.05f);
    else
        SETTING_HANDLER->set_volume(0.0f);
    audio_op_->setVolume(SETTING_HANDLER->get_volume());
}

// 音量加
void Widget::on_btn_up_clicked()
{
    if (SETTING_HANDLER->get_volume() < 0.95f)
        SETTING_HANDLER->set_volume(SETTING_HANDLER->get_volume() + 0.05f);
    else
        SETTING_HANDLER->set_volume(1.0f);
    audio_op_->setVolume(SETTING_HANDLER->get_volume());
}

// 模式切换按钮
void Widget::on_btn_mode_clicked()
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
        random_index_list_.pushBack(now_music_index_);
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
    SETTING_HANDLER->set_old_mode(play_mode);
}

// 更多按钮
void Widget::on_btn_more_clicked()
{
    switch (ui->stacked_widget->currentIndex())
    {
    case 0:
    case 2:
        ui->btn_more->setIcon(QIcon(":/svgs/back.svg"));
        ui->stacked_widget->setCurrentIndex(1);
        break;
    case 1:
        ui->btn_more->setIcon(QIcon(":/svgs/more.svg"));
        if (ONLINE == SETTING_HANDLER->get_player_mode())
        {
            ui->stacked_widget->setCurrentIndex(0);
        }
        else
        {
            ui->stacked_widget->setCurrentIndex(btn_list_.isEmpty() ? 2 : 0);
        }

        break;
    }
}

// 最小化
void Widget::on_btn_min_clicked()
{
    setWindowState(Qt::WindowMinimized);
}

// 歌曲名按钮
void Widget::on_btn_music_name_clicked()
{
    switch (SETTING_HANDLER->get_player_mode())
    {
    case LOCAL:
    case MYSITE:
        ui->scrollArea->ensureWidgetVisible(btn_list_.at(now_music_index_));
        break;
    case ONLINE:
        ui->scrollArea_online->ensureWidgetVisible(btn_list_.at(now_music_index_));
        break;
    }
}

void Widget::on_btn_open_dir_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(ui->label_dir->text()));
}

// 更改初始目录按钮
void Widget::on_btn_change_dir_clicked()
{
    QString str_dir = QFileDialog::getExistingDirectory(this, "选择音乐目录", ui->label_dir->text());
    if (str_dir.isEmpty())
        return;

    QLayoutItem* child;
    while (true)
    {
        child = ui->music_layout->itemAt(0);
        if (nullptr == child)
            break;

        ui->music_layout->removeItem(child);
        if (child->widget())
            delete child->widget();
    }
    btn_list_.clear();

    ui->label_dir->setText(str_dir);
    SETTING_HANDLER->set_music_dir(str_dir);

    QDir dir(SETTING_HANDLER->get_music_dir());
    dir.setFilter(QDir::Files);

    QStringList type_filter;
    foreach(const QString& t, TYPE_LIST)
    {
        type_filter.push_back("*." + t);
    }

    dir.setNameFilters(type_filter);

    QStringList list = dir.entryList(QDir::Files);
    if (list.size() > 0)
    {
        // 先排个序
        auto coll = QCollator(QLocale(QLocale::Chinese));
        std::sort(list.begin(), list.end(), coll);

        ui->stacked_widget->setCurrentIndex(0);
        for (const auto &url_str : list)
        {
            add_music(QUrl::fromLocalFile(dir.absolutePath() + '/' + url_str));
        }

        now_music_index_ = 0;
        play_music(btn_list_.first());
    }
    else
    {
        ui->stacked_widget->setCurrentIndex(2);
        player_->stop();
        play_music();
    }
    ui->btn_more->setIcon(QIcon(":/svgs/more.svg"));
}

void Widget::on_btn_local_clicked()
{
    SETTING_HANDLER->set_player_mode(LOCAL);
    init_local();
}

void Widget::on_btn_mysite_clicked()
{
    SETTING_HANDLER->set_player_mode(MYSITE);
    init_mysite();
}

void Widget::on_btn_online_clicked()
{
    SETTING_HANDLER->set_player_mode(ONLINE);
    init_online();
}

// 查找框内上一个按钮
void Widget::on_btn_left_clicked()
{
    if (find_index > 0)
        --find_index;
    else
        find_index = find_index_list.size() - 1;

    ui->scrollArea->ensureWidgetVisible(btn_list_.at(find_index_list.at(find_index)));
    ui->label_count->setText(QString::number(find_index + 1) + "/" + QString::number(find_index_list.size()));
}

// 查找框内下一个按钮
void Widget::on_btn_right_clicked()
{
    if (find_index < find_index_list.size() - 1)
        ++find_index;
    else
        find_index = 0;

    ui->scrollArea->ensureWidgetVisible(btn_list_.at(find_index_list.at(find_index)));
    ui->label_count->setText(QString::number(find_index + 1) + "/" + QString::number(find_index_list.size()));
}

void Widget::on_btn_search_clicked()
{
    QString word = ui->le_search->text();
    if (word.isEmpty())
        return;

    QList<MusicInfo> list = OnlineHandler::getInstance()->search_online_music(word);
    for (const MusicInfo& info : list)
    {
        add_online_music(info);
    }
}

void Widget::play_music(BaseMusicButton* btn)
{
    if (nullptr == btn)
    {
        player_->setSource(QUrl());
        return;
    }

    if (SETTING_HANDLER->get_player_mode() == ONLINE)
    {
        OnlineHandler::getInstance()->get_music_info(static_cast<OnlineMusicButton*>(btn)->get_info());
    }

    btn->setPlayingStyle();
    player_->setSource(btn->get_url());
    qDebug() << btn->get_url();
    player_->play();
//    switch (SETTING_HANDLER->get_player_mode())
//    {
//    case LOCAL:
//    case MYSITE:
//        player_->setSource(url);
//        break;
//    case ONLINE:
////        QString href = QString("http://%1:%2/%3").arg(IP).arg(PORT).arg(url.fileName());
////        qDebug() << href;
////        QString href = "https://cg-sycdn.kuwo.cn/a71666ddcb12ecafa60ef1256a455121/6419a985/resource/n1/25/27/278719973.mp3";
////        qDebug() << href;
////        player_->setSource(href);



////        QTcpSocket* socket = new QTcpSocket(this);
////        socket->connectToHost(IP, PORT);
////        socket->write(QString("@C2,%1").arg(url.fileName()).toUtf8());
////        socket->waitForReadyRead();
////        player_->setSourceDevice(socket);
////            QByteArray data = socket->readAll();
////            qDebug() << data.size();
////            QString request_text = QString(data);

////            int at_index = request_text.indexOf('@');
////            if (-1 == at_index)
////            {
////                qDebug() << "接收的消息中没有找到@:\n" << request_text;
////                return;
////            }

////            int index1 = request_text.indexOf(',');
////            if (-1 == index1)
////            {
////                qDebug() << "接收的消息中没找到逗号:\n" << request_text;
////                return;
////            }

////            QString cmd = request_text.mid(at_index+1, index1-1);
////            if ("C1" == cmd)
////            {
////    //            int index1 = request_text.indexOf(',', index1+1);
////                QStringList file_list = request_text.mid(index1+1).split(',');
////                load_music_list(file_list);
////                socket->close();
////            }
//        break;
//    }
}
