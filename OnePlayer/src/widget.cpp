#include "widget.h"
#include "ui_widget.h"

#include "HDBase/DList.hpp"
#include "HDCore/HD2QT.hpp"
#include "musicbutton.h"
#include "neteasehandler.h"
#include "onlinemusicbutton.h"
#include "OnePlayerStruct.h"
#include "PlayerFFmpeg.h"
#include "PlayerQt.h"
#include <QAudioOutput>
#include <QCollator>
#include <QDebug>
#include <QDesktopServices>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QFileDialog>
#include <QLocale>
#include <QMediaMetaData>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QMutex>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QRegularExpression>
#include <QScrollBar>
#include <QShortcut>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include "settinghandler.h"
#include "dmenu.h"
#include <QPropertyAnimation>
#include <QProcess>


bool point_in_widget(QWidget* widget, QPoint pos)
{
    QPoint p = widget->mapToGlobal(QPoint(0, 0));
    return (pos.x() > p.x() && pos.x() < p.x() + widget->width() &&
        pos.y() > p.y() && pos.y() < p.y() + widget->height());
}

Widget::Widget(const QString& filepath, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , hook_(Hook::getInstance())
    , player_(new PlayerQt)
    , movingProgress_(false)
    //, now_music_index_(0)
    , pressedCtrl_(false)
    , thisIsMoveWindow_(false)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAcceptDrops(true);
    hook_->installHook();
    connect(hook_, &Hook::sendKeyType, this, &Widget::slot_key_pressed);

    PlayerBase* pp = new PlayerFFmpeg(this);


//    ui->stacked_widget->setCurrentIndex(2);
//
//    ui->find_widget->hide();
//	ui->setting_tab_widget->setMusicDir(SETTING_HANDLER->get_music_dir());
//	ui->setting_tab_widget->setDownloadDir(SETTING_HANDLER->get_download_dir());
//    //ui->label_dir->setText(SETTING_HANDLER->get_music_dir());
//    //ui->label_dir_download->setText(SETTING_HANDLER->get_download_dir());
//
//    // 圆角遮罩
//    QWidget* ww = new QWidget(ui->music);
//    ww->move(10, 10);
//    ww->resize(ui->stacked_info->width() + 20, ui->stacked_info->height() + 20);
//    ww->setStyleSheet("background-color: transparent; border: 10px solid white; border-radius: 30px;");
//
    setListener();
//	set_setting_tab_listener();
//
//	NeteaseHandler::getInstance();

    if (filepath.isEmpty())  // 没有指定打开的歌曲则打开默认文件夹
    {
        switch (SETTING_HANDLER->getStruct().playerMode)
        {
        case LOCAL:
        {

            refreshMusicBtns();
            //init_local();
            break;
        }
        case MYSITE:
            //init_mysite();
            break;
        case ONLINE:
            //init_online();
            break;
        case NETEASE:
            //init_netease();
            break;
        }
    }
    else
    {
//        SETTING_HANDLER->set_player_mode(LOCAL);
//        ui->stacked_widget->setCurrentIndex(0);
//        QUrl url = QUrl::fromLocalFile(filepath);
//        add_music(url);
//        now_music_index_ = 0;
//        play_music(0);
    }

//    play_mode = SETTING_HANDLER->get_old_mode();
//    switch (play_mode)
//    {
//    case ONE_AGAIN:
//#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//        player_->setLoops(-1);
//#else
//		player_->setPlaybackRate(QMediaPlaylist::CurrentItemInLoop);
//#endif
//        ui->btn_mode->setIcon(QIcon(":/svgs/one_again.svg"));
//        break;
//    case AGAIN:
//#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//		player_->setLoops(1);
//#else
//		player_->setPlaybackRate(QMediaPlaylist::CurrentItemOnce);
//#endif
//        ui->btn_mode->setIcon(QIcon(":/svgs/again.svg"));
//        break;
//    case RANDOM:
//        random_index_ = 0;
////        DSizeType music_index = QRandomGenerator64::global()->bounded(0, (int)btn_list_.size());
//        random_index_list_.pushBack(now_music_index_);
////        now_music_index_ = random_index_list_.at(0);
//        qDebug() << "000 =" << random_index_list_.at(0);
//#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//		player_->setLoops(1);
//#else
//		player_->setPlaybackRate(QMediaPlaylist::CurrentItemOnce);
//#endif
//        ui->btn_mode->setIcon(QIcon(":/svgs/random.svg"));
//        break;
//    }

    // 初始化界面
    ui->music_info_widget->drawImage(QImage(":/images/music.png"));
    //ui->music_info_widget->drawImage(QImage("C:/Users/WHDon/Desktop/aaa.jpg"));
    //ui->music_info_widget->drawImage(QImage("C:/Users/WHDon/Desktop/bbb.png"));
    //ui->music_info_widget->drawImage(QImage("C:/Users/WHDon/Desktop/ccc.png"));
}

void Widget::slotPositionChanged(qint64 pos)
{
    if (!movingProgress_)  // 如果没有手动拖动进度条，才根据音乐进度改变进度条
    {
        if (pos > 0)
        {
            SETTING_HANDLER->getStruct().musicPosition = pos;
        }

        auto time_s = pos / 1000;
        auto rest_time = (player_->getDuration() - pos) / 1000;
        ui->label_now->setText(QString::number(time_s / 60).append(":%1").arg(time_s % 60, 2, 10, QLatin1Char('0')));
        ui->label_rest->setText(QString::number(rest_time / 60).append(":%1").arg(rest_time % 60, 2, 10, QLatin1Char('0')));
        ui->progress->setValue(pos);

        //if (SETTING_HANDLER->get_player_mode() == ONLINE)
            //ui->lyrics_widget->set_duration(pos);
    }

    // 进度超过最大，强制播放下一首
    // 音乐播放到末尾切歌操作在 mediaStatusChanged 中，这里只是为了保险起见
    if (pos > player_->getDuration())
        on_btn_next_clicked();
}

void Widget::setListener()
{
//    // 菜单失去焦点，判断是否需要隐藏
//    connect(DMenu::getButtonMenu(), &DMenu::maybeNeedHide, this, [this]()
//    {
//        QPoint pos = QCursor::pos();
//        // 遍历所有按钮，判断鼠标的绝对坐标是否在某个按钮上
//        bool need_hide = true;
//        for (BaseMusicButton* btn : btn_list_)
//        {
//            if (point_in_widget(btn, pos))
//            {
//                need_hide = false;
//                break;
//            }
//        }
//
//        if (need_hide)
//            DMenu::getButtonMenu()->animateHide();
//    });
//
//    // 菜单中点了某一项
//    connect(DMenu::getButtonMenu(), &DMenu::btn_clicked, this, [this](QString text)
//    {
//        if (text == "下一首播放")
//        {
//
//        }
//        else if (text == "打开文件所在位置")
//        {
//            QUrl url = DMenu::getButtonMenu()->getNowBtn()->get_url();
//            QString path = url.toLocalFile();
//
//            QProcess process;
//            path.replace("/", "\\");
//            process.startDetached("explorer.exe", {"/select,", path});
//        }
//        else if (text == "从列表中移除")
//        {
//
//        }
//        else if (text == "删除")
//        {
//
//        }
//    });

    // 开始放歌时，播放图片Widget的隐藏动画
    connect(player_, &PlayerBase::beginPlay, ui->music_info_widget, &MusicInfoWidget::animationHide);

//    // 音乐播放状态改变事件
//#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//    connect(player_, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state)
//#else
//	connect(player_, &QMediaPlayer::stateChanged, this, [this](QMediaPlayer::State state)
//#endif
//	{
//        switch (state)
//        {
//        case QMediaPlayer::PlayingState:
//            ui->btn_play->setIcon(QIcon(":/svgs/pause.svg"));
//            // 每次播放音乐都
//            break;
//        case QMediaPlayer::PausedState:
//            ui->btn_play->setIcon(QIcon(":/svgs/play.svg"));
//            break;
//        case QMediaPlayer::StoppedState:
//            ui->btn_play->setIcon(QIcon(":/svgs/play.svg"));
//            break;
//        }
//    });
//
//    // 发生错误
//#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//    connect(player_, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error err, const QString& err_str)
//	{
//		qDebug() << err << "\n" << err_str << "\n";
//		QMessageBox::critical(this, "发生了意想不到的事情", "详情：" + err_str + "\n文件：" + player_->source().fileName());
//	});
//#else
//	auto slotError = [this](QMediaPlayer::Error err)
//	{
//		qDebug() << err << "\n" << player_->errorString() << "\n";
//		QMessageBox::critical(this, "发生了意想不到的事情", "详情：" + player_->errorString() + "\n文件："/* + player_->source().fileName()*/);
//	};
//	connect(player_, SIGNAL(error(QMediaPlayer::Error error)), this, SLOT(slotError));
//#endif
	

    // 先sourceChanged，再metaDataChanged
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(player_, &PlayerBase::sourceChanged, this, [this](const QUrl& media)
    {
        QImage image = PlayerFFmpeg::getMusicImage(media.toString());
        ui->music_info_widget->drawImage(image);

        //switch (SETTING_HANDLER->get_player_mode())
        //{
        //case LOCAL:
        //case MYSITE:
        //{
        //    //            QString file_type = media.fileName().section('.', 0, -1);
        //    QString file_name = media.fileName().section('.', 0, -2);
        //    ui->btn_music_name->setText(file_name);
        //    //            ui->label_sound_name->setText(file_name);
        //}
        //break;
        //case ONLINE:
        //{
        //    OnlineMusicButton* btn = static_cast<OnlineMusicButton*> (btn_list_.at(now_music_index_));
        //    MusicInfo& info = btn->get_info();
        //    ui->btn_music_name->setText(info.name_);
        //}
        //break;
        //}
        //SETTING_HANDLER->set_last_music(media);
    });
#else
	// todo 不知道用哪个
	//connect(player_, &QMediaPlayer::mediaChanged, this, [this](const QMediaContent& media)
	//connect(player_, &QMediaPlayer::currentMediaChanged, this, [this](const QMediaContent& media)
#endif

    connect(player_, &PlayerQt::metaDataChanged, this, &Widget::slotMetaDataChanged);

    // 一些没用的事件
//	connect(player_, &QMediaPlayer::seekableChanged, this, [](bool)
//	{
//		//        qDebug() << 3;
//	});
//	connect(player_, &QMediaPlayer::playbackRateChanged, this, []()
//	{
//		//        qDebug() << 5;
//	});
//#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//    connect(player_, &QMediaPlayer::tracksChanged, this, []()
//    {
////        qDebug() << 1;
//    });
//    connect(player_, &QMediaPlayer::videoOutputChanged, this, []()
//    {
////        qDebug() << 2;
//    });
//    connect(player_, &QMediaPlayer::activeTracksChanged, this, []()
//    {
////        qDebug() << 7;
//    });
//    connect(player_, &QMediaPlayer::audioOutputChanged, this, []()
//    {
////        qDebug() << 8;
//    });
//    connect(player_, &QMediaPlayer::bufferProgressChanged, this, []()
//    {
////        qDebug() << 9;
//    });
//    connect(player_, &QMediaPlayer::hasAudioChanged, this, []()
//    {
////        qDebug() << 10;
//    });
//#endif
    // 使本次播放进度变成上次关闭时的进度
    //static bool first_play = true;
    connect(player_, &PlayerBase::mediaAtEnd, this, &Widget::on_btn_next_clicked);
    //connect(player_, &PlayerBase::mediaAtEnd, this, [this](QMediaPlayer::MediaStatus status)
    //{
    //    if (QMediaPlayer::EndOfMedia == status)
    //        on_btn_next_clicked();

        //if (first_play)
        //{
        //    if (status == QMediaPlayer::LoadedMedia)
        //    {
        //        first_play = false;
        //        // 延时播放音乐，防止进度不正确
        //        QTimer* timer = new QTimer(this);
        //        connect(timer, &QTimer::timeout, this, [this, timer]()
        //        {
        //            qint64 pos = SETTING_HANDLER->get_music_position();  // 放到player_->play()后面会导致音乐播放后新的进度写入，覆盖原有进度
        //            player_->setPosition(pos);
        //            player_->play();
        //            timer->deleteLater();
        //        });
        //        timer->start(10);
        //    }
        //}
    //});

    // 音乐时长改变
    connect(player_, &PlayerQt::durationChanged, ui->progress, &QSlider::setMaximum);

    // 音乐前进，改变进度条
    connect(player_, &PlayerQt::positionChanged, this, &Widget::slotPositionChanged);

    // 按下进度条，停止根据音乐改变进度条
    connect(ui->progress, &QSlider::sliderPressed, this, [this]()
    {
        movingProgress_ = true;
    });

////    connect(ui->progress, &QSlider::actionTriggered, this, [this](int action)
////    {
////        qDebug() << action;
////        switch (action)
////        {
////        case 0:
////            player_->setPosition(ui->progress->value());
////            movingProgress_ = false;
////            break;
////        case 3:
////        case 4:
////            movingProgress_ = true;
////            break;
////        }
////    });

    // 松开进度条，改变音乐进度
    connect(ui->progress, &QSlider::sliderReleased, this, [this]()
    {
        player_->setPosition(ui->progress->value());
        movingProgress_ = false;
    });

//    // 搜索框文字改变
//    connect(ui->le_find, &QLineEdit::textChanged, this, [this]()
//    {
//        if (ui->le_find->text() == "")
//            ui->label_count->setText("0/0");
//        else
//            find_music(ui->le_find->text());
//    });
}

void Widget::refreshMusicBtns()
{
    QLayoutItem* child;
    while (child = ui->music_layout->itemAt(0))
    {
        ui->music_layout->removeItem(child);
        if (child->widget())
            delete child->widget();
    }

    DList<QString> playList = SETTING_HANDLER->getNowPlayList();
    for (const QString& musicPath : playList)
    {
        addMusicBtn(musicPath);
    }
//    if (list.size() > 0)
//    {
//        // 先排个序
//        auto coll = QCollator(QLocale(QLocale::Chinese));
//        std::sort(list.begin(), list.end(), coll);
//
//        for (const auto &url_str : list)
//        {
//            if (MYSITE == SETTING_HANDLER->get_player_mode())
//            {
//                addMusicBtn(QString("http://%1:%2/%3").arg(IP).arg(PORT).arg(url_str));
//            }
//            else
//            {
//                addMusicBtn(QUrl::fromLocalFile(SETTING_HANDLER->get_music_dir() + '/' + url_str));
//            }
//        }
//
//        // 如果上次播放的音乐也在这个文件夹中
//        DSizeType newMusicIndex = btn_list_.size();
//        for (DSizeType i = 0; i < btn_list_.size(); ++i)
//        {
//            if (btn_list_.at(i)->get_url() == SETTING_HANDLER->get_last_music())
//            {
//                newMusicIndex = i;
//                break;
//            }
//        }
//
//        if (newMusicIndex == btn_list_.size())  // 如果上次播放的音乐不在这个文件夹中，则从头开始播放
//        {
//            qDebug() << play_mode;
//            if (play_mode == RANDOM)
//            {
//                random_index_list_.clear();
//                random_index_ = 0;
//#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//                newMusicIndex = QRandomGenerator64::global()->bounded(0, (int)btn_list_.size());
//#else
//				newMusicIndex = qrand() % btn_list_.size();
//#endif
//				random_index_list_.pushBack(newMusicIndex);
//            }
//            else
//                newMusicIndex = 0;
//        }
//        play_music(newMusicIndex);
//    }
}

//void Widget::next_music()
//{
//    DSizeType newMusicIndex = now_music_index_;
//    if (play_mode == RANDOM)  // 随机播放
//    {
//        ++random_index_;
//        if (random_index_ == random_index_list_.size())
//        {
//#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//            DSizeType music_index = QRandomGenerator64::global()->bounded(0, (int)btn_list_.size());
//#else
//			DSizeType music_index =  qrand() % btn_list_.size();
//#endif
//			random_index_list_.pushBack(music_index);
//        }
//        newMusicIndex = random_index_list_.at(random_index_);
//    }
//    else
//    {
//        ++newMusicIndex;
//        if (newMusicIndex == btn_list_.size())
//            newMusicIndex = 0;
//    }
//
//    play_music(newMusicIndex);
//}
//
//void Widget::previous_music()
//{
//    DSizeType newMusicIndex = now_music_index_;
//    if (play_mode == RANDOM)
//    {
//        if (random_index_ > 0)
//            --random_index_;
//        else
//        {
//#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//            DSizeType music_index = QRandomGenerator64::global()->bounded(0, (int)btn_list_.size());
//#else
//			DSizeType music_index = qrand() % btn_list_.size();
//#endif
//			random_index_list_.pushFront(music_index);
//        }
//
//        newMusicIndex = random_index_list_.at(random_index_);
//    }
//    else
//    {
//        if (newMusicIndex == 0)
//            newMusicIndex = btn_list_.size() - 1;
//        else
//            --newMusicIndex;
//
//    }
//    play_music(newMusicIndex);
//}

void Widget::addMusicBtn(const QUrl& url)
{
    MusicButton* btn = new MusicButton(url, this);
    btn->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(btn, &MusicButton::clicked, this, [this, btn]()
    {
        //DSizeType newMusicIndex = btn_list_.indexOf(btn);
        //play_music(newMusicIndex);

        //if (play_mode == RANDOM)
        //{
        //    random_index_ = random_index_list_.size();
        //    random_index_list_.pushBack(newMusicIndex);
        //}
    });
    connect(btn, &QPushButton::customContextMenuRequested, this, [btn](const QPoint& pos)
    {
        DMenu* menu = DMenu::getButtonMenu();
        menu->show(btn);
    });
    ui->music_layout->addWidget(btn);
}

//void Widget::add_online_music(const MusicInfo& music)
//{
//    OnlineMusicButton* btn = new OnlineMusicButton(music, this);
//    connect(btn, &BaseMusicButton::clicked, this, [this, btn]()
//    {
//        DSizeType newMusicIndex = btn_list_.indexOf(btn);
//        play_music(newMusicIndex);
//
//        if (play_mode == RANDOM)
//        {
//            random_index_ = random_index_list_.size();
//            random_index_list_.pushBack(newMusicIndex);
//        }
//    });
//    connect(btn, &OnlineMusicButton::download_clicked, this, [this, btn]()
//    {
//        if (SETTING_HANDLER->get_download_dir().isEmpty())
//        {
//            QMessageBox::warning(this, tr("警告你"), tr("请先选择下载歌曲保存目录"));
//            slot_btn_change_dir_download_clicked();
//            if (SETTING_HANDLER->get_download_dir().isEmpty())
//            {
//                QMessageBox::warning(this, tr("警告你"), tr("你选择的目录为空"));
//                return;
//            }
//        }
//
//        QDir dir(SETTING_HANDLER->get_download_dir());
//        if (!dir.exists())
//        {
//            dir.mkdir(SETTING_HANDLER->get_download_dir());
//        }
//        OnlineHandler::getInstance()->get_music_info(btn->get_info());
//        QByteArray data = OnlineHandler::getInstance()->get_music(btn->get_info().absolute_url_);
//
//        QString url = btn->get_info().absolute_url_.toString();
//        QString type = url.mid(url.lastIndexOf('.'));
//        QFile file(SETTING_HANDLER->get_download_dir() + "/" + btn->get_info().name_ + "-" + btn->get_info().singer_ + type);
//        file.open(QIODevice::WriteOnly);
//        file.write(data);
//        file.close();
//    });
//
//    ui->music_layout_online->addWidget(btn);
//    btn_list_.pushBack(btn);
//}
//
//void Widget::find_music(const QString& word)
//{
//    if (word == "")
//        return;
//
//    find_index = 0;
//    ui->label_count->setText("0/0");
//    find_index_list.clear();
//    for (DSizeType i = 0; i < btn_list_.size(); ++i)
//    {
//        QRegularExpression reg(".*" + word + ".*");
//        auto ret = reg.match(btn_list_.at(i)->get_filename());
//        if (ret.hasMatch())
//        {
//            find_index_list.pushBack(i);
//        }
//    }
//
//    if (find_index_list.size() > 0)
//    {
//        ui->scrollArea->ensureWidgetVisible(btn_list_.at(find_index_list.first()));
//        ui->label_count->setText(QString::number(find_index + 1) + "/" + QString::number(find_index_list.size()));
//    }
//}
//
//void Widget::init_local()
//{
//    clear_button(ui->music_layout);
//
//    ui->stacked_info->setCurrentIndex(0);
//
//    if (!SETTING_HANDLER->get_music_dir().isEmpty())
//    {
//        QDir dir(SETTING_HANDLER->get_music_dir());
//        dir.setFilter(QDir::Files);
//
//        QStringList type_filter;
//        std::for_each(TYPE_LIST.begin(), TYPE_LIST.end(), [&type_filter](const QString& t)
//        {
//            type_filter.push_back("*." + t);
//        });
//
//        dir.setNameFilters(type_filter);
//
//        QStringList list = dir.entryList(QDir::Files);
//
//        load_music_list(list);
//    }
//
//    ui->stacked_widget->setCurrentIndex(btn_list_.isEmpty() ? 2 : 0);
//    ui->stacked_music_btn->setCurrentIndex(0);
//    ui->btn_more->setIcon(QIcon(":/svgs/more.svg"));
//}
//
//void Widget::init_mysite()
//{
//    clear_button(ui->music_layout);
//
//    ui->stacked_info->setCurrentIndex(0);
//
//    QTcpSocket* socket = new QTcpSocket(this);
//    socket->connectToHost(IP, PORT);
//    connect(socket, &QTcpSocket::readyRead, this, [this, socket]()
//    {
//        QByteArray data = socket->readAll();
//        socket->close();
//        QString request_text = QString(data);
//
//        int at_index = request_text.indexOf('@');
//        if (-1 == at_index)
//        {
//            qDebug() << "接收的消息中没有找到@:\n" << request_text;
//            return;
//        }
//
//        int index1 = request_text.indexOf(',');
//        if (-1 == index1)
//        {
//            qDebug() << "接收的消息中没找到逗号:\n" << request_text;
//            return;
//        }
//
//        QString cmd = request_text.mid(at_index+1, index1-1);
//        if ("C1" == cmd)
//        {
////            int index1 = request_text.indexOf(',', index1+1);
//            QStringList file_list = request_text.mid(index1+1).split(',');
//            load_music_list(file_list);
//            ui->btn_more->setIcon(QIcon(":/svgs/more.svg"));
//            ui->stacked_widget->setCurrentIndex(btn_list_.isEmpty() ? 2 : 0);
//            ui->stacked_music_btn->setCurrentIndex(0);
//        }
//    });
//    socket->write("@C1,");
//}
//
//void Widget::init_online()
//{
//    clear_button(ui->music_layout_online);
//    ui->stacked_info->setCurrentIndex(1);
//
//    ui->btn_more->setIcon(QIcon(":/svgs/more.svg"));
//    ui->stacked_widget->setCurrentIndex(0);
//    ui->stacked_music_btn->setCurrentIndex(1);
//    draw_image(QImage(), true);
//}
//
//void Widget::init_netease()
//{
//    clear_button(ui->music_layout_online);
//    player_->stop();
//    //ui->stacked_info->setCurrentIndex(1);
//
//    ui->btn_more->setIcon(QIcon(":/svgs/more.svg"));
//    ui->stacked_widget->setCurrentIndex(3);
//    //ui->stacked_music_btn->setCurrentIndex(3);
//}

void Widget::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void Widget::dropEvent(QDropEvent *event)
{
    // 在设置页不接受拖入事件
    //if (ui->stacked_widget->currentIndex() == 1)
    //    return;

    QPoint stackedWidgetPoint = ui->stackedWidget->mapTo(this, QPoint(0, 0));
    QRect rect(stackedWidgetPoint, ui->stackedWidget->size());
    if (!rect.contains(event->pos()))
        return;


    //if (SETTING_HANDLER->get_player_mode() != LOCAL)
    //{
    //    SETTING_HANDLER->set_player_mode(LOCAL);
    //    clear_button(ui->music_layout);

    //    ui->stacked_info->setCurrentIndex(0);
    //    ui->stacked_music_btn->setCurrentIndex(0);
    //}

    //bool play = btn_list_.isEmpty();

    DList<QString> list;
    auto all = event->mimeData()->urls();
    for (const auto &url : all)
    {
        QString type = url.toLocalFile().section('.', -1);

        if (TYPE_LIST.indexOf(type) != -1)
            list.pushBack(url.toLocalFile());
    }
    SETTING_HANDLER->addPlayList("新播放列表", list);

    //// 重新设置滚动条的数值
    //ui->scrollArea->verticalScrollBar()->setMaximum(btn_list_.size());

    //// 说明本来没有歌曲在播放列表中
    //if (play && ! btn_list_.isEmpty())
    //{
    //    ui->stacked_widget->setCurrentIndex(0);
    //    play_music(0);
    //}
}

void Widget::animationStackedMusicBtnSmall()
{
    int nowX = ui->stacked_music_btn->x();
    int nowY = ui->stacked_music_btn->y();
    int nowWidth = ui->stacked_music_btn->width();
    int nowHeight = ui->stacked_music_btn->height();

    QPropertyAnimation* animation = new QPropertyAnimation(ui->stacked_music_btn, "geometry");
    animation->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    connect(animation, &QPropertyAnimation::finished, animation, &QPropertyAnimation::deleteLater);

    animation->setStartValue(QRect(nowX, nowY, nowWidth, nowHeight));
    animation->setEndValue(QRect(nowX, nowY, stackedMusicBtnWidth_, nowHeight));
    animation->start();
    
}

void Widget::animationStackedMusicBtnBig()
{
    int oldX = ui->stacked_music_btn->x();
    int oldY = ui->stacked_music_btn->y();
    stackedMusicBtnWidth_ = ui->stacked_music_btn->width();
    int oldHeight = ui->stacked_music_btn->height();

    QPropertyAnimation* animation = new QPropertyAnimation(ui->stacked_music_btn, "geometry");
    animation->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    connect(animation, &QPropertyAnimation::finished, animation, &QPropertyAnimation::deleteLater);

    animation->setStartValue(QRect(oldX, oldY, stackedMusicBtnWidth_, oldHeight));
    animation->setEndValue(QRect(oldX, oldY, ui->multi_func_widget->width(), oldHeight));
    animation->start();
}

void Widget::mousePressEvent(QMouseEvent *ev)
{
    auto pos = ev->pos();
    if (pos.y() < 20)
    {
        thisIsMoveWindow_ = true;
        pressX_ = pos.x();
        pressY_ = pos.y();
    }
}

void Widget::mouseMoveEvent(QMouseEvent *ev)
{
    if (thisIsMoveWindow_)
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        auto global_pos = ev->globalPosition();
#else
		auto global_pos = ev->globalPos();
#endif
        move(global_pos.x() - pressX_, global_pos.y() - pressY_);
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *)
{
    thisIsMoveWindow_ = false;
}

void Widget::keyPressEvent(QKeyEvent *event)
{
//    qDebug() << event->key();
    switch (event->key())
    {
    case 32:  // space
        /*if (QMediaPlayer::StoppedState == GET_PLAY_STATE || QMediaPlayer::PausedState == GET_PLAY_STATE)
        {
            player_->play();
        }
        else if (QMediaPlayer::PlayingState == GET_PLAY_STATE)
        {
            player_->pause();
        }*/
        break;
    case 16777249:  // ctrl
        pressedCtrl_ = true;
        break;
    case Qt::Key_Left:
        if (pressedCtrl_)
        {
            on_btn_previoud_clicked();
        }
        else
        {
            player_->setPosition(player_->getPosition() - 2000);
        }
        break;
    case Qt::Key_Right:
        if (pressedCtrl_)
        {
            on_btn_next_clicked();
        }
        else
        {
            player_->setPosition(player_->getPosition() + 2000);
        }
        break;
    case Qt::Key_Up:
        on_btn_up_clicked();
        break;
    case Qt::Key_Down:
        on_btn_down_clicked();
        break;
    case Qt::Key_F:
        if (pressedCtrl_)  // 按了ctrl + f弹出搜索框
        {
            //if (ui->multi_btn_widget->isAnimateHide())
                //ui->multi_btn_widget->hide();

            if (ui->find_widget->isHidden())
            {
                ui->find_widget->show();
                ui->find_widget->setEditFocus();
            }
            else
            {
                ui->find_widget->hide();
                ui->find_widget->setEditText("");
            }
        }
        break;
    case 16777220:
    case 16777221:  // 回车
        //if (ui->le_search->hasFocus())  // // 查找播放列表的输入框按下回车
        //{
        //    on_btn_search_clicked();
        //}
        break;
    }

    QWidget::keyPressEvent(event);
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Control:
        pressedCtrl_ = false;
        break;
    }
}

//void Widget::clear_button(QVBoxLayout* layout)
//{
//    QLayoutItem* child;
//    while (true)
//    {
//        child = layout->itemAt(0);
//        if (nullptr == child)
//            break;
//
//        layout->removeItem(child);
//        if (child->widget())
//            delete child->widget();
//    }
//    btn_list_.clear();
//}

void Widget::refreshImageWidget(const QString& title, const QString& singers, const QString& album_title)
{
//    static QMutex mutex;
//    static QPropertyAnimation* animationHide = nullptr;
//    static QPropertyAnimation* animationShow = nullptr;
//    static QEventLoop loop;
//    if (nullptr == animationHide)
//    {
//        animationHide = new QPropertyAnimation(opacityEffect, "opacity");
//        animationHide->setEasingCurve(QEasingCurve::InOutQuad);
//        animationHide->setStartValue(1.0);
//        animationHide->setEndValue(0.0);
//        animationHide->setDuration(TIME350);
//
//        animationShow = new QPropertyAnimation(opacityEffect, "opacity");
//        animationShow->setEasingCurve(QEasingCurve::InOutQuad);
//        animationShow->setStartValue(0.0);
//        animationShow->setEndValue(1.0);
//        animationShow->setDuration(TIME350);
//
//        connect(animationHide, &QPropertyAnimation::finished, &loop, &QEventLoop::quit);
////        connect(animationShow, &QPropertyAnimation::finished, this, []()
////        {
//////            mutex.unlock();
////        });
//    }
//
////    mutex.lock();
//    if (loop.isRunning())
//        loop.quit();
//    animationHide->start();
//    loop.exec();

    //ui->music_info_widget->drawImage(image);

    // 设置歌曲名
    if (!title.isEmpty())
    {
        ui->music_info_widget->setMusicName(title);
        ui->btn_music_name->setText(title);
    }
    else
    {
        // H:/音乐/Apologize.mp3
        QString nowMusicPath = SETTING_HANDLER->nowMusicPath().replace('\\', '/');
        int index1 = nowMusicPath.lastIndexOf('/') + 1;
        int index2 = nowMusicPath.lastIndexOf('.');
        QString filename = SETTING_HANDLER->nowMusicPath().mid(index1, index2 - index1);
        ui->music_info_widget->setMusicName(filename);
        ui->btn_music_name->setText(filename);
    }

    // 设置歌手名
    if (!singers.isEmpty())
        ui->music_info_widget->setSingerName(singers);
    else
        ui->music_info_widget->setSingerName("未知歌手");

    // 设置专辑名
    if (!album_title.isEmpty())
        ui->music_info_widget->setAlbumName(album_title);
    else
        ui->music_info_widget->setAlbumName("未知专辑");

    // 播放切换动画
    //animationShow->start();
}

void Widget::animateShow()
{
    QWidget::show();
    int startx = QCursor::pos().x();
    int starty = QCursor::pos().y();
    int endx = startx - width() / 2;
    if (endx < 20)
        endx = 20;
    int endy = starty - height() / 2;
    if (endy < 20)
        endy = 20;
    static QPropertyAnimation* animation = nullptr;
    if (nullptr == animation)
    {
        animation = new QPropertyAnimation(this, "geometry");
        animation->setDuration(MAIN_WIDGET_ANIMATION_TIME);
        animation->setEasingCurve(QEasingCurve::InOutQuad);
    }
    animation->setStartValue(QRect(startx, starty, 0, 0));
    animation->setEndValue(QRect(endx, endy, width(), height()));
    animation->start();
}

void Widget::animateHide(bool closeAfterFinshed)
{
    int oldx = x();
    int oldy = y();
    int newx = QCursor::pos().x();
    int newy = QCursor::pos().y();
    int w = width();
    int h = height();

    QPropertyAnimation* animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(MAIN_WIDGET_ANIMATION_TIME);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    if (closeAfterFinshed)
        connect(animation, &QPropertyAnimation::finished, this, &QWidget::close);
    else
        connect(animation, &QPropertyAnimation::finished, this, [this, animation, w, h, oldx, oldy]()
        {
            resize(w, h);
            move(oldx, oldy);
            show();
            animation->deleteLater();
            setWindowState(Qt::WindowMinimized);
        });

    animation->setStartValue(QRect(oldx, oldy, w, h));
    animation->setEndValue(QRect(newx, newy, 0, 0));
    animation->start();
}

Widget::~Widget()
{
    delete ui;
	NeteaseHandler::getInstance()->deleteThis();
    hook_->unInstallHook();
}

void Widget::slot_key_pressed(DWORD key)
{
    /*auto state = GET_PLAY_STATE;
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
    }*/
}

// 关闭
void Widget::on_btn_shutdown_clicked()
{
    SETTING_HANDLER->save();
    animateHide(true);
}

// 播放/暂停
void Widget::on_btn_play_clicked()
{
    if (player_->playOrPause())
    {
        ui->btn_play->setIcon(QIcon(":/svgs/pause.svg"));
    }
    else
    {
        ui->btn_play->setIcon(QIcon(":/svgs/play.svg"));
    }
}

// 上一首
void Widget::on_btn_previoud_clicked()
{
    ui->btn_play->setIcon(QIcon(":/svgs/pause.svg"));
    player_->playPrevious();
}

// 下一首
void Widget::on_btn_next_clicked()
{
    ui->btn_play->setIcon(QIcon(":/svgs/pause.svg"));
    player_->playNext();
}

// 音量减
void Widget::on_btn_down_clicked()
{
    if (SETTING_HANDLER->getStruct().volume > 0.05f)
        SETTING_HANDLER->getStruct().volume -= 0.05f;
    else
        SETTING_HANDLER->getStruct().volume = 0.0f;
    player_->setVolume(SETTING_HANDLER->getStruct().volume);
}

// 音量加
void Widget::on_btn_up_clicked()
{
    if (SETTING_HANDLER->getStruct().volume < 0.95f)
        SETTING_HANDLER->getStruct().volume += 0.05f;
    else
        SETTING_HANDLER->getStruct().volume = 1.0f;
    player_->setVolume(SETTING_HANDLER->getStruct().volume);
}

// 模式切换按钮
void Widget::on_btn_mode_clicked()
{
    switch (SETTING_HANDLER->getStruct().playMode)
    {
    case AGAIN:
    {
        SETTING_HANDLER->getStruct().playMode = ONE_AGAIN;
//#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//        player_->setLoops(-1);
//#else
//        player_->setPlaybackRate(QMediaPlaylist::CurrentItemInLoop);
//#endif
        ui->btn_mode->setIcon(QIcon(":/svgs/one_again.svg"));
        break;
    }
    case ONE_AGAIN:
    {
//        random_index_list_.clear();
//        random_index_list_.pushBack(now_music_index_);
//        random_index_ = 0;
        SETTING_HANDLER->getStruct().playMode = RANDOM;
        SETTING_HANDLER->clearRandomPlayList();
//#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//        player_->setLoops(1);
//#else
//        player_->setPlaybackRate(QMediaPlaylist::CurrentItemOnce);
//#endif
        ui->btn_mode->setIcon(QIcon(":/svgs/random.svg"));
        break;
    }
    case RANDOM:
    {
        SETTING_HANDLER->getStruct().playMode = AGAIN;
//#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//        player_->setLoops(1);
//#else
//        player_->setPlaybackRate(QMediaPlaylist::CurrentItemOnce);
//#endif
        ui->btn_mode->setIcon(QIcon(":/svgs/again.svg"));
        break;
    }
    }
}

// 更多按钮
void Widget::on_btn_more_clicked()
{
    // todo (实在不行的话)右侧按钮条可以用代码创建，播放完hide动画就delete，要显示了再重新创建
    if (ui->multi_btn_widget->isAnimateHide())
    {
        animationStackedMusicBtnSmall();
        ui->multi_btn_widget->animationShow();
    }
    else
    {
        ui->multi_btn_widget->animationHide();
        animationStackedMusicBtnBig();
    }

    //switch (ui->stacked_widget->currentIndex())
    //{
    //case 0:
    //case 2:
    //    ui->btn_more->setIcon(QIcon(":/svgs/back.svg"));
    //    ui->stacked_widget->setCurrentIndex(1);
    //    break;
    //case 1:
    //    ui->btn_more->setIcon(QIcon(":/svgs/more.svg"));
    //    if (ONLINE == SETTING_HANDLER->get_player_mode())
    //    {
    //        ui->stacked_widget->setCurrentIndex(0);
    //    }
    //    else
    //    {
    //        ui->stacked_widget->setCurrentIndex(btn_list_.isEmpty() ? 2 : 0);
    //    }

    //    break;
    //}
}

// 最小化
void Widget::on_btn_min_clicked()
{
    //setWindowState(Qt::WindowMinimized);
    animateHide();
}

//// 歌曲名按钮
//void Widget::on_btn_music_name_clicked()
//{
//    switch (SETTING_HANDLER->get_player_mode())
//    {
//    case LOCAL:
//    case MYSITE:
//        ui->scrollArea->ensureWidgetVisible(btn_list_.at(now_music_index_));
//        break;
//    case ONLINE:
//        ui->scrollArea_online->ensureWidgetVisible(btn_list_.at(now_music_index_));
//        break;
//    }
//}
//
//void Widget::slot_btn_open_dir_clicked()
//{
//    QDesktopServices::openUrl(QUrl::fromLocalFile(SETTING_HANDLER->get_music_dir()));
//}
//
//// 更改初始目录按钮
//void Widget::slot_btn_change_dir_clicked()
//{
//    QString str_dir = QFileDialog::getExistingDirectory(this, "选择音乐目录", SETTING_HANDLER->get_music_dir());
//    if (str_dir.isEmpty())
//        return;
//
//    clear_button(ui->music_layout);
//
//	ui->setting_tab_widget->setMusicDir(str_dir);
//    //ui->label_dir->setText(str_dir);
//    SETTING_HANDLER->set_music_dir(str_dir);
//
//    QDir dir(SETTING_HANDLER->get_music_dir());
//    dir.setFilter(QDir::Files);
//
//    QStringList type_filter;
//    foreach(const QString& t, TYPE_LIST)
//    {
//        type_filter.push_back("*." + t);
//    }
//
//    dir.setNameFilters(type_filter);
//
//    QStringList list = dir.entryList(QDir::Files);
//    if (list.size() > 0)
//    {
//        // 先排个序
//        auto coll = QCollator(QLocale(QLocale::Chinese));
//        std::sort(list.begin(), list.end(), coll);
//
//        ui->stacked_widget->setCurrentIndex(0);
//        for (const auto &url_str : list)
//        {
//            add_music(QUrl::fromLocalFile(dir.absolutePath() + '/' + url_str));
//        }
//
//        play_music(0);
//    }
//    else
//    {
//        ui->stacked_widget->setCurrentIndex(2);
//        player_->stop();
////        play_music();
//    }
//    ui->btn_more->setIcon(QIcon(":/svgs/more.svg"));
//}
//
//void Widget::slot_btn_open_dir_download_clicked()
//{
//    QDesktopServices::openUrl(QUrl::fromLocalFile(SETTING_HANDLER->get_download_dir()));
//}
//
//void Widget::slot_btn_change_dir_download_clicked()
//{
//    QString str_dir = QFileDialog::getExistingDirectory(this, "选择下载歌曲保存目录", SETTING_HANDLER->get_download_dir());
//    if (str_dir.isEmpty())
//        return;
//
//	ui->setting_tab_widget->setDownloadDir(str_dir);
//    //ui->label_dir_download->setText(str_dir);
//    SETTING_HANDLER->set_download_dir(str_dir);
//}
//
//void Widget::slot_cmb_mode_currentIndexChanged(int index)
//{
//    PLAYER_MODE newMode = static_cast<PLAYER_MODE>(index);
//    switch (newMode)
//    {
//    case LOCAL:
//        init_local();
//        break;
//    case MYSITE:
//        init_mysite();
//        break;
//    case ONLINE:
//        init_online();
//        break;
//    case NETEASE:
//        init_netease();
//        break;
//    }
//    SETTING_HANDLER->set_player_mode(newMode);
//}
//
//// 查找框内上一个按钮
//void Widget::on_btn_left_clicked()
//{
//    if (find_index > 0)
//        --find_index;
//    else
//        find_index = find_index_list.size() - 1;
//
//    ui->scrollArea->ensureWidgetVisible(btn_list_.at(find_index_list.at(find_index)));
//    ui->label_count->setText(QString::number(find_index + 1) + "/" + QString::number(find_index_list.size()));
//}
//
//// 查找框内下一个按钮
//void Widget::on_btn_right_clicked()
//{
//    if (find_index < find_index_list.size() - 1)
//        ++find_index;
//    else
//        find_index = 0;
//
//    ui->scrollArea->ensureWidgetVisible(btn_list_.at(find_index_list.at(find_index)));
//    ui->label_count->setText(QString::number(find_index + 1) + "/" + QString::number(find_index_list.size()));
//}
//
//void Widget::on_btn_search_clicked()
//{
//    QString word = ui->le_search->text();
//    if (word.isEmpty())
//        return;
//
//    clear_button(ui->music_layout_online);
//
//    DList<MusicInfo> list = OnlineHandler::getInstance()->search_online_music(word);
//    for (const MusicInfo& info : list)
//    {
//        add_online_music(info);
//    }
//}

void Widget::slotMetaDataChanged(const MusicMetaData& metaData)
{
    refreshImageWidget(metaData.title, metaData.singers, metaData.albumTitle);
}

//void Widget::play_music(DSizeType musicIndex)
//{
//    // 重新设置旧的歌曲按钮的颜色
//    btn_list_.at(now_music_index_)->setNormalStyle();
//
//    if (musicIndex >= btn_list_.size())
//    {
////        player_->setSource(QUrl());
//        qWarning() << "play music index error, index =" << musicIndex << ", btn_list_.size() = " << btn_list_.size();
//        return;
//    }
//
//    now_music_index_ = musicIndex;
//    BaseMusicButton* btn = btn_list_.at(musicIndex);
//
//    if (SETTING_HANDLER->get_player_mode() == ONLINE)
//    {
//        OnlineMusicButton* online_btn = static_cast<OnlineMusicButton*>(btn);
//
//        MusicInfo& info = online_btn->get_info();
//        OnlineHandler::getInstance()->get_music_info(info);
//        QImage image = OnlineHandler::getInstance()->get_image(info.image_url_);
//
//        ui->label_sound_name_online->setText(QString("%1  %2").arg(info.name_).arg(info.singer_));
//        ui->lyrics_widget->set_lyrics(info.lyrics_);
//    }
//
//    qDebug() << "播放->" << btn->get_url();
//
//    btn->setPlayingStyle();
//#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//	player_->setSource(btn->get_url());
//#else
//	player_->setMedia(btn->get_url());
//#endif
//    
////    qDebug() << btn->get_url();
//    player_->play();
////    switch (SETTING_HANDLER->get_player_mode())
////    {
////    case LOCAL:
////    case MYSITE:
////        player_->setSource(url);
////        break;
////    case ONLINE:
//////        QString href = QString("http://%1:%2/%3").arg(IP).arg(PORT).arg(url.fileName());
//////        qDebug() << href;
//////        QString href = "https://cg-sycdn.kuwo.cn/a71666ddcb12ecafa60ef1256a455121/6419a985/resource/n1/25/27/278719973.mp3";
//////        qDebug() << href;
//////        player_->setSource(href);
//
//
//
//////        QTcpSocket* socket = new QTcpSocket(this);
//////        socket->connectToHost(IP, PORT);
//////        socket->write(QString("@C2,%1").arg(url.fileName()).toUtf8());
//////        socket->waitForReadyRead();
//////        player_->setSourceDevice(socket);
//////            QByteArray data = socket->readAll();
//////            qDebug() << data.size();
//////            QString request_text = QString(data);
//
//////            int at_index = request_text.indexOf('@');
//////            if (-1 == at_index)
//////            {
//////                qDebug() << "接收的消息中没有找到@:\n" << request_text;
//////                return;
//////            }
//
//////            int index1 = request_text.indexOf(',');
//////            if (-1 == index1)
//////            {
//////                qDebug() << "接收的消息中没找到逗号:\n" << request_text;
//////                return;
//////            }
//
//////            QString cmd = request_text.mid(at_index+1, index1-1);
//////            if ("C1" == cmd)
//////            {
//////    //            int index1 = request_text.indexOf(',', index1+1);
//////                QStringList file_list = request_text.mid(index1+1).split(',');
//////                load_music_list(file_list);
//////                socket->close();
//////            }
////        break;
////    }
//}
//
//void Widget::set_setting_tab_listener()
//{
//	connect(ui->setting_tab_widget, &SettingTabWidget::sig_btn_open_dir_clicked, this, &Widget::slot_btn_open_dir_clicked);
//	connect(ui->setting_tab_widget, &SettingTabWidget::sig_btn_change_dir_clicked, this, &Widget::slot_btn_change_dir_clicked);
//	connect(ui->setting_tab_widget, &SettingTabWidget::sig_btn_open_dir_download_clicked, this, &Widget::slot_btn_open_dir_download_clicked);
//	connect(ui->setting_tab_widget, &SettingTabWidget::sig_btn_change_dir_download_clicked, this, &Widget::slot_btn_change_dir_download_clicked);
//	connect(ui->setting_tab_widget, &SettingTabWidget::sig_cmb_mode_currentIndexChanged, this, &Widget::slot_cmb_mode_currentIndexChanged);
//}
