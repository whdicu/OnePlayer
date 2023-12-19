#include "widget.h"
#include "ui_widget.h"

#include "HDBase/DList.hpp"
#include "HDCore/HD2QT.hpp"
#include "hook.h"
#include "ImageHandler.h"
#include "LocalMusicButton.h"
#include "neteasehandler.h"
#include "onlinemusicbutton.h"
#include "PlayerFFmpeg.h"
#include "PlayerQt.h"
#include "OneMessageBox.h"
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


bool pointInWidget(QWidget* widget, QPoint pos)
{
    QPoint p = widget->mapToGlobal(QPoint(0, 0));
    return (pos.x() > p.x() && pos.x() < p.x() + widget->width() &&
        pos.y() > p.y() && pos.y() < p.y() + widget->height());
}

Widget::Widget(const QString& filepath, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , player_(new PlayerQt(this))
	, initSuccess_(true)
    , movingProgress_(false)
    , pressedCtrl_(false)
    , thisIsMoveWindow_(false)
    , isShowAnimation_(true)
    , shutdownBtnClicked_(false)
    , isAnimateHide_(true)
    , stackedMusicBtnAnimationTimer_(nullptr)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

	// 硬件加速
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_PaintOnScreen);

    // 捕获所有按键，让上下左右键也能捕获到
    grabKeyboard();

    setAcceptDrops(true);
	Hook::getInstance()->installHook();
    connect(Hook::getInstance(), &Hook::sendKeyType, this, &Widget::slotKeyPressed, Qt::QueuedConnection);
	
    // 动画创建
    animation_ = new QPropertyAnimation(this, "geometry");
    animation_->setDuration(MAIN_WIDGET_ANIMATION_TIME);
    connect(animation_, &QPropertyAnimation::finished, this, [this]()
    {
        if (isShowAnimation_)
            return;

        if (shutdownBtnClicked_)
            close();
        else
		{
            //resize(MAIN_WIDGET_WIDTH, MAIN_WIDGET_HEIGHT);
            move(pressX_, pressY_);
            hide();
			//show();
			//setWindowState(Qt::WindowMinimized);
        }
    });

    stackedMusicBtnAnimation_ = new QPropertyAnimation(ui->stacked_music_btn, "geometry");
    stackedMusicBtnAnimation_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME);
    stackedMusicBtnAnimation_->setEasingCurve(QEasingCurve::OutCubic);

	stackedMusicBtnAnimationSub1_ = new QPropertyAnimation(ui->find_widget, "geometry");
	stackedMusicBtnAnimationSub1_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME);
	stackedMusicBtnAnimationSub1_->setEasingCurve(QEasingCurve::OutCubic);

	stackedLocalBtnsAnimation_ = new QPropertyAnimation(ui->stacked_local_btns, "geometry");
	stackedLocalBtnsAnimation_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME);
	stackedLocalBtnsAnimation_->setEasingCurve(QEasingCurve::OutCubic);

//    ui->stacked_widget->setCurrentIndex(2);
//
//    ui->find_widget->hide();
//	ui->setting_tab_widget->setMusicDir(SETTING_HANDLER->get_music_dir());
//	ui->setting_tab_widget->setDownloadDir(SETTING_HANDLER->get_download_dir());
//    //ui->label_dir->setText(SETTING_HANDLER->get_music_dir());
//    //ui->label_dir_download->setText(SETTING_HANDLER->get_download_dir());

    setListener();
//	set_setting_tab_listener();

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
		QString type = filepath.section('.', -1);
		if (!TYPE_LIST.contains(type))
		{
			QMessageBox::warning(this, tr("警告"), tr("不支持的类型:\n") + type);
			initSuccess_ = false;
			return;
		}
//        SETTING_HANDLER->set_player_mode(LOCAL);
//        ui->stacked_widget->setCurrentIndex(0);
//        QUrl url = QUrl::fromLocalFile(filepath);
//        add_music(url);
//        now_music_index_ = 0;
//        play_music(0);
    }

	setPlayMode(SETTING_HANDLER->getStruct().playMode);
}

void Widget::slotKeyPressed(DWORD key)
{
    switch (key)
    {
    case 179ul:
        on_btn_play_clicked();
        break;
    case 176ul:
        on_btn_next_clicked();
        break;
    case 177ul:
        on_btn_previoud_clicked();
        break;
    case 178ul:
        //player_->stop();
        break;
    }
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

void Widget::slotSearchEditClose()
{
    ui->find_widget->animationHide();
    animationStackedLocalBtnsLong();
    ui->find_widget->setEditText("");
}

void Widget::slotMusicIndexChanged(DSizeType oldIndex, DSizeType newIndex)
{
    setMusicBtnStyle(oldIndex, &BaseMusicButton::setNormalStyle);
    setMusicBtnStyle(newIndex, &BaseMusicButton::setPlayingStyle);
}

void Widget::slotMenuBtnClicked(const QString& text)
{
	DSizeType musicIndex = DMenu::getButtonMenu()->getSelectedMusicIndex();
	int i = BUTTON_MENU_STR_LIST.indexOf(text);
	switch (i)
	{
	case 0:  // 下一首播放
	{
		switch (SETTING_HANDLER->getStruct().playMode)
		{
		case RANDOM:
			SETTING_HANDLER->insertToRandomPlayList(musicIndex);
			break;
		default:
			SETTING_HANDLER->setNextIndexTemp(musicIndex);
			break;
		}
		break;
	}
	case 1:  // 打开文件所在位置
	{
		QUrl url = SETTING_HANDLER->currentPlayList().at(musicIndex);
		QString path = url.toLocalFile();
        path.replace("/", "\\");

		//QProcess process;
		//process.startDetached("explorer.exe", { "/select,", path });
        QProcess::startDetached("explorer.exe", { "/select,", path });
		break;
	}
	case 2:  // 从列表中移除
	{
		break;
	}
	case 3:  // 删除
	{
		break;
	}
	}
}

void Widget::slotLocalMusicBtnClicked(DSizeType musicIndex)
{
	if (SETTING_HANDLER->getMusicIndex() == musicIndex)
		return;

	switch (SETTING_HANDLER->getStruct().playMode)
	{
	case RANDOM:
		SETTING_HANDLER->insertToRandomPlayList(musicIndex);
		SETTING_HANDLER->plusRandomIndex();
		break;
	default:
		SETTING_HANDLER->setMusicIndex(musicIndex);
		break;
	}

	player_->playCurrentIndex();
}

void Widget::setListener()
{
    // 菜单失去焦点，判断是否需要隐藏
    connect(DMenu::getButtonMenu(), &DMenu::maybeNeedHide, this, [this]()
    {
        QPoint pos = QCursor::pos();
        // 遍历所有按钮，判断鼠标的绝对坐标是否在某个按钮上
        bool needHide = true;

        QLayoutItem* child;
        for (int i = 0; i < ui->music_layout->count(); ++i)
        {
            if (child = ui->music_layout->itemAt(i))
            {
                QWidget* btn = child->widget();
                if (btn)
                {
                    if (pointInWidget(btn, pos))
                    {
                        needHide = false;
                        break;
                    }
                }
            }
        }

        if (needHide)
            DMenu::getButtonMenu()->animateHide();
    });

    // 菜单中点了某一项
	connect(DMenu::getButtonMenu(), &DMenu::sigBtnClicked, this, &Widget::slotMenuBtnClicked);
    
    // 开始放歌时，播放图片Widget的隐藏动画
    connect(player_, &PlayerBase::beginPlay, this, [this]()
    {
        ui->btn_play->setIcon(QIcon(":/svgs/pause.svg"));
        ui->music_info_widget->animationHide();
    });

    // 先sourceChanged，再metaDataChanged
    connect(player_, &PlayerBase::sourceChanged, this, [this](const QUrl& media)
    {
        MusicInfo musicInfo = player_->getMusicInfo();
		refreshImageWidget(musicInfo);

		emit sigChangeSystemIconToolTip(QString("OnePlayer\n正在播放：%1\n歌手：%2\n专辑：%3")
			.arg(musicInfo.title).arg(musicInfo.singers).arg(musicInfo.album));
    });

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
    connect(ui->progress, &DProgressBar::sigMousePressed, this, [this]()
    {
        movingProgress_ = true;
    });

//    connect(ui->progress, &QSlider::actionTriggered, this, [this](int action)
//    {
//        qDebug() << action;
//        switch (action)
//        {
//        case 0:
//            player_->setPosition(ui->progress->value());
//            movingProgress_ = false;
//            break;
//        case 3:
//        case 4:
//            movingProgress_ = true;
//            break;
//        }
//    });

    // 松开进度条，改变音乐进度
    connect(ui->progress, &DProgressBar::sigMouseReleased, this, [this](int val)
    {
        player_->setPosition(val);
        movingProgress_ = false;
    });


    // 搜索框文字改变
    connect(ui->find_widget, &SearchEdit::focusOnBtnAt, this, [this](DSizeType index)
    {
		showMusicBtnAt(index);
    });

    // 点击搜索框内关闭按钮
    connect(ui->find_widget, &SearchEdit::sigBtnCloseClicked, this, &Widget::slotSearchEditClose);

    // 音乐下标改变
    connect(SETTING_HANDLER, &SettingHandler::sigMusicIndexChanged, this, &Widget::slotMusicIndexChanged);

    // 右侧按钮Widget
    connect(ui->multi_btn_widget, &MultiBtnWidget::sigBtnSettingClicked, this, [this]()
    {
        ui->btn_more->setIcon(QIcon(":/svgs/back.svg"));
        ui->stacked_widget->setCurrentIndex(1);
    });
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

    DList<QUrl> playList = SETTING_HANDLER->currentPlayList();
    DSizeType index = 0;
    for (const QUrl& musicUrl : playList)
    {
        BaseMusicButton* btn = addLocalMusicBtn(musicUrl);
        btn->setMusicIndex(index);
        ++index;
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

BaseMusicButton* Widget::addLocalMusicBtn(const QUrl& url)
{
	//QString filename = url.fileName();
	//QString str = filename.mid(0, filename.indexOf('.'));
    LocalMusicButton* btn = new LocalMusicButton(url.fileName(), this);
    btn->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(btn, &LocalMusicButton::clicked, this, &Widget::slotLocalMusicBtnClicked);
    connect(btn, &QPushButton::customContextMenuRequested, this, [btn](const QPoint& pos)
    {
        DMenu* menu = DMenu::getButtonMenu();
        menu->show(btn->getMusicIndex());
    });
    ui->music_layout->addWidget(btn);
    return btn;
}

void Widget::setMusicBtnStyle(int index, void (BaseMusicButton::* setStyleFunc)())
{
    QLayoutItem* child = ui->music_layout->itemAt(index);
    if (nullptr == child)
    {
        qWarning() << "child at" << index << "is nullptr!" << __FUNCTION__ << __LINE__;
        return;
    }

    BaseMusicButton* oldBtn = (BaseMusicButton*)child->widget();
    if (nullptr == oldBtn)
    {
        qWarning() << "oldBtn is nullptr!" << __FUNCTION__ << __LINE__;
        return;
    }

    (oldBtn->*setStyleFunc)();
}

void Widget::setPlayMode(PLAY_MODE mode)
{
	SETTING_HANDLER->getStruct().playMode = mode;
	switch (mode)
	{
	case AGAIN:
	{
		//#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		//        player_->setLoops(1);
		//#else
		//        player_->setPlaybackRate(QMediaPlaylist::CurrentItemOnce);
		//#endif
		ui->btn_mode->setIcon(QIcon(":/svgs/again.svg"));
		break;
	}
	case ONE_AGAIN:
	{
		//#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		//        player_->setLoops(-1);
		//#else
		//        player_->setPlaybackRate(QMediaPlaylist::CurrentItemInLoop);
		//#endif
		ui->btn_mode->setIcon(QIcon(":/svgs/one_again.svg"));
		break;
	}
	case RANDOM:
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
	}
}

void Widget::showMusicBtnAt(DSizeType index)
{
	QLayoutItem* child = ui->music_layout->itemAt(index);
	if (child && child->widget())
	{
		int margin = (ui->scrollArea->height() - child->widget()->height()) / 2;
		ui->scrollArea->ensureWidgetVisible(child->widget(), 0, margin);
	}
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

    QPoint stackedWidgetPoint = ui->stacked_local_btns->mapTo(this, QPoint(0, 0));
    QRect rect(stackedWidgetPoint, ui->stacked_local_btns->size());
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

	DList<QUrl> list;
	QList<QUrl> all = event->mimeData()->urls();
    for (const QUrl& url : all)
    {
        QString type = url.toLocalFile().section('.', -1);

        //if (TYPE_LIST.indexOf(type) != -1)
        if (TYPE_LIST.contains(type))
            list.pushBack(url);
    }

	int ret = OneMessageBox::information(nullptr, tr("提示"),
		tr("把这%1首歌添加到歌单%2?").arg(list.size()).arg(SETTING_HANDLER->getStruct().playListName), ALL_BTN);

	if (QDialog::Accepted == ret)
	{
		if (SETTING_HANDLER->notExistPlayList())
			SETTING_HANDLER->addPlayList("新播放列表", list);
		else
			SETTING_HANDLER->addList2CurrentPlayList(list);
	}

	refreshMusicBtns();


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
	stackedMusicBtnAnimation_->stop();
	stackedMusicBtnAnimationSub1_->stop();
	stackedLocalBtnsAnimation_->stop();

    int nowX = ui->stacked_music_btn->x();
    int nowY = ui->stacked_music_btn->y();
    int nowWidth = ui->stacked_music_btn->width();
    int nowHeight = ui->stacked_music_btn->height();

    stackedMusicBtnAnimation_->setStartValue(QRect(nowX, nowY, nowWidth, nowHeight));
    stackedMusicBtnAnimation_->setEndValue(QRect(nowX, nowY, STACKED_MUSIC_BTN_WIDTH, nowHeight));
    stackedMusicBtnAnimation_->start();

	stackedMusicBtnAnimationSub1_->setStartValue(QRect(0, ui->find_widget->y(), ui->find_widget->width(), ui->find_widget->height()));
	stackedMusicBtnAnimationSub1_->setEndValue(QRect(0, ui->find_widget->y(), STACKED_MUSIC_BTN_WIDTH, ui->find_widget->height()));
	stackedMusicBtnAnimationSub1_->start();
	
	stackedLocalBtnsAnimation_->setStartValue(QRect(0, ui->stacked_local_btns->y(), ui->stacked_local_btns->width(), ui->stacked_local_btns->height()));
	stackedLocalBtnsAnimation_->setEndValue(QRect(0, ui->stacked_local_btns->y(), STACKED_MUSIC_BTN_WIDTH, ui->stacked_local_btns->height()));
	stackedLocalBtnsAnimation_->start();
}

void Widget::animationStackedMusicBtnBig()
{
	stackedMusicBtnAnimation_->stop();
	stackedMusicBtnAnimationSub1_->stop();
	stackedLocalBtnsAnimation_->stop();

    int oldX = ui->stacked_music_btn->x();
    int oldY = ui->stacked_music_btn->y();
    int oldWidth = ui->stacked_music_btn->width();
    int oldHeight = ui->stacked_music_btn->height();

    stackedMusicBtnAnimation_->setStartValue(QRect(oldX, oldY, oldWidth, oldHeight));
    stackedMusicBtnAnimation_->setEndValue(QRect(oldX, oldY, ui->multi_func_widget->width(), oldHeight));
    stackedMusicBtnAnimation_->start();

	stackedMusicBtnAnimationSub1_->setStartValue(QRect(0, ui->find_widget->y(), ui->find_widget->width(), ui->find_widget->height()));
	stackedMusicBtnAnimationSub1_->setEndValue(QRect(0, ui->find_widget->y(), ui->multi_func_widget->width(), ui->find_widget->height()));
	stackedMusicBtnAnimationSub1_->start();

	stackedLocalBtnsAnimation_->setStartValue(QRect(0, ui->stacked_local_btns->y(), ui->stacked_local_btns->width(), ui->stacked_local_btns->height()));
	stackedLocalBtnsAnimation_->setEndValue(QRect(0, ui->stacked_local_btns->y(), ui->multi_func_widget->width(), ui->stacked_local_btns->height()));
	stackedLocalBtnsAnimation_->start();
}

void Widget::animationStackedLocalBtnsShort()
{
	stackedLocalBtnsAnimation_->stop();

	int oldX = ui->stacked_local_btns->x();
	int oldY = ui->stacked_local_btns->y();
	int oldWidth = ui->stacked_local_btns->width();
	int oldHeight = ui->stacked_local_btns->height();

	stackedLocalBtnsAnimation_->setStartValue(QRect(oldX, oldY, oldWidth, oldHeight));
	stackedLocalBtnsAnimation_->setEndValue(QRect(oldX, ui->stacked_music_btn->height() - STACKED_MUSIC_BTN_HEIGHT, oldWidth, STACKED_MUSIC_BTN_HEIGHT));
	stackedLocalBtnsAnimation_->start();
}

void Widget::animationStackedLocalBtnsLong()
{
	stackedLocalBtnsAnimation_->stop();

	int oldX = ui->stacked_local_btns->x();
	int oldY = ui->stacked_local_btns->y();
	int oldWidth = ui->stacked_local_btns->width();
	int oldHeight = ui->stacked_local_btns->height();

	stackedLocalBtnsAnimation_->setStartValue(QRect(oldX, oldY, oldWidth, oldHeight));
	stackedLocalBtnsAnimation_->setEndValue(QRect(oldX, 0, oldWidth, ui->stacked_music_btn->height()));
	stackedLocalBtnsAnimation_->start();
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
        on_btn_play_clicked();
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
            if (ui->find_widget->isAnimateHide())
            {
                ui->find_widget->animationShow();
				animationStackedLocalBtnsShort();
                ui->find_widget->setEditFocus();
            }
            else
            {
                slotSearchEditClose();
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

void Widget::initMultiFuncWidget()
{
    ui->multi_btn_widget->moveToHide();
    ui->stacked_music_btn->resize(ui->multi_func_widget->width(), ui->stacked_music_btn->height());
    ui->find_widget->moveToHide();
    ui->find_widget->resize(ui->multi_func_widget->width(), ui->find_widget->height());
    ui->stacked_local_btns->setGeometry(QRect(ui->stacked_local_btns->x(), 0
        , ui->multi_func_widget->width(), ui->multi_func_widget->height()));
}

void Widget::refreshImageWidget(const MusicInfo& info)
{
    // 设置歌曲名
    if (!info.title.isEmpty())
    {
        ui->music_info_widget->setMusicName(info.title);
        ui->btn_music_name->setText(info.title);
    }
    else
    {
		QString filename = SETTING_HANDLER->currentMusicUrl().fileName();
		QString str = filename.mid(0, filename.indexOf('.'));
        ui->music_info_widget->setMusicName(str);
        ui->btn_music_name->setText(str);
    }

    // 设置歌手名
    ui->music_info_widget->setSingerName(info.singers.isEmpty() ? "未知歌手" : info.singers);

    // 设置专辑名
    ui->music_info_widget->setAlbumName(info.album.isEmpty() ? "未知专辑" : info.album);

	// 画图片
	ui->music_info_widget->drawImage(info.image);
	ui->music_info_widget->animationShow();
}

void Widget::animateShow(bool fromCursor)
{
	isAnimateHide_ = false;
	int startx = QCursor::pos().x();
	int starty = QCursor::pos().y();
	int startWidth = 0;
	int startHeight = 0;
	int endx;
	int endy;
	if (fromCursor)
	{
		endx = startx - MAIN_WIDGET_WIDTH / 2;
		if (endx < 20)
			endx = 20;
		endy = starty - MAIN_WIDGET_HEIGHT / 2;
		if (endy < 20)
			endy = 20;
	}
	else
	{
		if (isHidden())
		{
            int beginX = x() + MAIN_WIDGET_WIDTH / 2;
            int beginY = y() + MAIN_WIDGET_HEIGHT / 2;
            endx = pressX_;
            endy = pressY_;
		}
		else
		{
            // 在播放hide动画时打断，从当前位置当前大小开始恢复到press位置
            startx = x();
            starty = y();
            startWidth = width();
            startHeight = height();
            endx = pressX_;
            endy = pressY_;
		}
	}

    // 防止界面在被其他窗口遮挡。
    // 排除了 在播放hide动画时打断 的情况，防止show时窗口闪烁
    if (isHidden())
    {
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        QWidget::show();
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    }
    QWidget::show();

	animation_->stop();
    animation_->setEasingCurve(MAIN_WIDGET_SHOW_EASING);
    animation_->setStartValue(QRect(startx, starty, startWidth, startHeight));
    animation_->setEndValue(QRect(endx, endy, MAIN_WIDGET_WIDTH, MAIN_WIDGET_HEIGHT));
    animation_->start();
    isShowAnimation_ = true;
}

void Widget::animateHide()
{
	isAnimateHide_ = true;
    
    int startX = x();
    int startY = y();

    // 界面还在播放动画显示或隐藏时，不会更新pressX_和pressY_的值
    if (animation_->state() != QPropertyAnimation::Running)
    {
        pressX_ = startX;
        pressY_ = startY;
    }

    int newx = QCursor::pos().x();
    int newy = QCursor::pos().y();
    int w = width();
    int h = height();

	animation_->stop();
    animation_->setEasingCurve(MAIN_WIDGET_HIDE_EASING);
    animation_->setStartValue(QRect(startX, startY, w, h));
    animation_->setEndValue(QRect(newx, newy, 0, 0));
    animation_->start();
    isShowAnimation_ = false;
}

Widget::~Widget()
{
    delete ui;
	NeteaseHandler::getInstance()->deleteThis();
	Hook::getInstance()->unInstallHook();
}

void Widget::init()
{
	// 初始化界面
	initMultiFuncWidget();

	MusicInfo info;
	info.title = "歌曲名";
	info.singers = "歌手";
	info.album = "专辑";
	info.image = QImage(":/images/music.png");
	refreshImageWidget(info);

	// 播放之前上次关闭时放的歌
	player_->playCurrentIndex(SETTING_HANDLER->getStruct().musicPosition);
	slotMusicIndexChanged(SETTING_HANDLER->getMusicIndex(), SETTING_HANDLER->getMusicIndex());  // 初始化被播放的那个音乐按钮样式
}

void Widget::uninit()
{
	SETTING_HANDLER->save();
}

// 关闭
void Widget::on_btn_shutdown_clicked()
{
	animateHide();
	shutdownBtnClicked_ = true;
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
	PLAY_MODE newMode = AGAIN;
    switch (SETTING_HANDLER->getStruct().playMode)
    {
    case AGAIN:
    {
		newMode = ONE_AGAIN;
        break;
    }
    case ONE_AGAIN:
    {
		newMode = RANDOM;
        break;
    }
    case RANDOM:
    {
		newMode = AGAIN;
        break;
    }
    }

	setPlayMode(newMode);
}

// 更多按钮
void Widget::on_btn_more_clicked()
{
    switch (ui->stacked_widget->currentIndex())
    {
    case 0:
    case 2:
        if (nullptr != stackedMusicBtnAnimationTimer_)
            stackedMusicBtnAnimationTimer_->deleteLater();

        stackedMusicBtnAnimationTimer_ = new QTimer(this);
        if (ui->multi_btn_widget->isAnimateHide())
        {
            animationStackedMusicBtnSmall();
            connect(stackedMusicBtnAnimationTimer_, &QTimer::timeout, this, [this]()
            {
                stackedMusicBtnAnimationTimer_->deleteLater();
                stackedMusicBtnAnimationTimer_ = nullptr;
                ui->multi_btn_widget->animationShow();
            });
        }
        else
        {
            ui->multi_btn_widget->animationHide();
            connect(stackedMusicBtnAnimationTimer_, &QTimer::timeout, this, [this]()
            {
                stackedMusicBtnAnimationTimer_->deleteLater();
                stackedMusicBtnAnimationTimer_ = nullptr;
                animationStackedMusicBtnBig();
            });
        }
        stackedMusicBtnAnimationTimer_->start(MORE_BTN_WIDGET_ANIMATION_TIME / 2);
        break;
    case 1:  // 处于设置页
        ui->btn_more->setIcon(QIcon(":/svgs/more.svg"));
        //if (ONLINE == SETTING_HANDLER->get_player_mode())
        //{
        //    ui->stacked_widget->setCurrentIndex(0);
        //}
        //else
        {
            ui->stacked_widget->setCurrentIndex(0);
        }

        break;
    }
}

// 最小化
void Widget::on_btn_min_clicked()
{
    //setWindowState(Qt::WindowMinimized);
    animateHide();
}

// 歌曲名按钮
void Widget::on_btn_music_name_clicked()
{
    switch (SETTING_HANDLER->getStruct().playerMode)
    {
    case LOCAL:
    case MYSITE:
	{
		showMusicBtnAt(SETTING_HANDLER->getMusicIndex());
		break;
	}
	case ONLINE:
        //ui->scrollArea_online->ensureWidgetVisible(btn_list_.at(now_music_index_));
        break;
    }
}

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
