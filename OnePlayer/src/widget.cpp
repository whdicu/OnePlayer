#include "widget.h"
#include "ui_widget.h"
#include "HDBase/DVector.hpp"
#include "HDQt.h"
#include "hook.h"
#include "DStyle.h"
#include "ImageHandler.h"
#include "LocalMusicButton.h"
#include "neteasehandler.h"
#include "NeteaseMusicBtn.h"
#include "NetLoginDialog.h"
#include "onlinemusicbutton.h"
#include "PlayerFFmpeg.h"
#include "PlayerQt.h"
#include "PlayListButton.h"
#include "PlayListEdit.h"
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
	, animateLabel_(new QLabel)
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

    setAcceptDrops(true);
	
	animateLabel_->setWindowFlags(Qt::FramelessWindowHint);
	animateLabel_->setAttribute(Qt::WA_TranslucentBackground);
	animateLabel_->setScaledContents(true);

    // 动画创建
    animation_ = new QPropertyAnimation(animateLabel_, "geometry");
    animation_->setDuration(MAIN_WIDGET_ANIMATION_TIME);
    connect(animation_, &QPropertyAnimation::finished, this, [this]()
    {
		if (isShowAnimation_)
		{
			QWidget::show();
		}
		else
		{
			if (shutdownBtnClicked_)
			{
				QApplication::quit();
			}
			else
			{
				//resize(MAIN_WIDGET_WIDTH, MAIN_WIDGET_HEIGHT);
				move(beforeAniPos_);
				//show();
				//setWindowState(Qt::WindowMinimized);
			}
		}
		animateLabel_->hide();
    });

    stackedMusicBtnAnimation_ = new QPropertyAnimation(ui->stacked_music_btn, "geometry");
    stackedMusicBtnAnimation_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME);

	stackedMusicBtnAnimationSub1_ = new QPropertyAnimation(ui->find_widget, "geometry");
	stackedMusicBtnAnimationSub1_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME);

	stackedLocalBtnsAnimation_ = new QPropertyAnimation(ui->stacked_local_btns, "geometry");
	stackedLocalBtnsAnimation_->setDuration(MORE_BTN_WIDGET_ANIMATION_TIME);

	// 创建播放列表界面开头的添加框
	PlayListEdit* edit = new PlayListEdit(this);
	ui->play_list_layout->addWidget(edit);
	connect(edit, &PlayListEdit::sigAdd, this, [this](const QString& playListName)
	{
		SETTING_HANDLER->addPlayList(playListName, DVector<QString>());
		refreshPlayListBtns();
	});

//    ui->stacked_widget->setCurrentIndex(2);
//
//    ui->find_widget->hide();
//	ui->setting_tab_widget->setMusicDir(SETTING_HANDLER->get_music_dir());
//	ui->setting_tab_widget->setDownloadDir(SETTING_HANDLER->get_download_dir());
//    //ui->label_dir->setText(SETTING_HANDLER->get_music_dir());
//    //ui->label_dir_download->setText(SETTING_HANDLER->get_download_dir());

    setListener();

//	NeteaseHandler::getInstance();

    if (!filepath.isEmpty())  // 有指定打开的歌曲
    {
		QString type = filepath.section('.', -1);
		if (!TYPE_LIST.contains(type))
		{
			QMessageBox::warning(this, tr("警告"), tr("不支持的类型:\n") + type);
			initSuccess_ = false;
			return;
		}

		SETTING_HANDLER->addPlayList(TEMP_PLAY_LIST_NAME, { filepath });
		SETTING_HANDLER->getStruct().playListName = TEMP_PLAY_LIST_NAME;
		SETTING_HANDLER->setMusicIndex(0);
    }

	switch (SETTING_HANDLER->getStruct().playerMode)
	{
	case LOCAL:
	{
		refreshMusicBtns();
		refreshPlayListBtns();
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

	setPlayMode(SETTING_HANDLER->getStruct().playMode);
}

void Widget::slotKeyPressed(const KeyInfo& info)
{
    switch (info.key)
    {
    case 179ul:
        on_btn_play_clicked();
        return;
    case 176ul:
        on_btn_next_clicked();
		return;
    case 177ul:
        on_btn_previoud_clicked();
		return;
    case 178ul:
        //player_->stop();
		return;
    }

	// 当前程序有焦点时，才进行操作
	if (isAncestorOf(QApplication::focusWidget()))
	{
		switch (info.key)
		{
		case 37ul:  // 左
			if (info.ctrlPressed)
				on_btn_previoud_clicked();
			else
				player_->setPosition(player_->getPosition() - 3000);
			break;
		case 38ul:  // 上
			on_btn_up_clicked();
			break;
		case 39ul:  // 右
			if (info.ctrlPressed)
				on_btn_next_clicked();
			else
				player_->setPosition(player_->getPosition() + 3000);
			break;
		case 40ul:  // 下
			on_btn_down_clicked();
			break;
		}
	}
}


void Widget::slotPositionChanged(qint64 pos)
{
    if (!movingProgress_)  // 如果没有手动拖动进度条，才根据音乐进度改变进度条
    {
        if (nullptr == ui->label_now)
        {
            DWarning << "ui->label_now is nullptr!";
            return;
        }

        if (pos > 0)
        {
            SETTING_HANDLER->getStruct().musicPosition = pos;
        }

        auto time_s = pos / 1000;
        auto rest_time = (player_->getDuration() - pos) / 1000;
        ui->label_now->setText(QString::number(time_s / 60).append(":%1").arg(time_s % 60, 2, 10, QLatin1Char('0')));
        ui->label_rest->setText(QString::number(rest_time / 60).append(":%1").arg(rest_time % 60, 2, 10, QLatin1Char('0')));
        ui->progress->setValue(pos);

        ui->music_info_widget->setLyricPos(pos);
    }

    // 进度超过最大，强制播放下一首
    // 音乐播放到末尾切歌操作在 mediaStatusChanged 中，这里只是为了保险起见
    //if (pos > player_->getDuration())
        //on_btn_next_clicked();
}

void Widget::slotSearchEditClose()
{
    ui->find_widget->animationHide();
    animationStackedLocalBtnsLong();
    ui->find_widget->setEditText("");
	// 还原按钮样式
	if (SETTING_HANDLER->getMusicIndex() != ui->find_widget->nowFocusIndex())
		setMusicBtnStyle(ui->find_widget->nowFocusIndex(), &BaseMusicButton::setNormalStyle);
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
		QString path = url.toString();
        path.replace("/", "\\");

        QProcess::startDetached("explorer.exe", { "/select,", path });
		break;
	}
	case 2:  // 从列表中移除
	{
        SETTING_HANDLER->removeMusicFromCurrentPlayList(musicIndex);
        refreshMusicBtns();
		break;
	}
	case 3:  // 删除
	{
        QUrl musicUrl = SETTING_HANDLER->currentPlayList().at(musicIndex);

        int ret = OneMessageBox::information(nullptr, tr("提示"), tr("确认要删除歌曲%1吗，该操作会将歌曲从硬盘中删除。").arg(musicUrl.fileName()), ALL_BTN);
        if (QDialog::Accepted == ret)
        {
            SETTING_HANDLER->removeMusicFromCurrentPlayList(musicIndex);
            refreshMusicBtns();
            
            QFile file(musicUrl.toLocalFile());
            if (file.exists() && file.remove())
            {
                DDebug << "文件删除成功";
            }
            else
            {
                DDebug << "文件删除失败";
                OneMessageBox::information(nullptr, tr("警告"), tr("歌曲删除%1失败").arg(musicUrl.fileName()));
            }
        }
		break;
	}
	}
}

void Widget::slotLocalMusicBtnClicked(DSizeType musicIndex)
{
	DMenu::getButtonMenu()->animateHide();
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

void Widget::slotNeteaseMusicBtnClicked(dint64 musicId)
{
	DSizeType musicIndex = SETTING_HANDLER->getNeteaseSongIndex(musicId);

	if (musicIndex == SETTING_HANDLER->getCurrentPlayListSize()
		|| SETTING_HANDLER->getMusicIndex() == musicIndex)
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

void Widget::slotBGModeChanged(BG_MODE bgMode)
{
	MusicInfo musicInfo = player_->getMusicInfo();
	refreshImageWidget(musicInfo);
}

void Widget::slotNeteasePlayListClicked(const NeteasePlayListInfo& info)
{
	emit ui->multi_btn_widget->sigBtnPlayListClicked();

	switch (SETTING_HANDLER->getStruct().playMode)
	{
	case RANDOM:
		SETTING_HANDLER->clearRandomPlayList();
		break;
	}
	
	SETTING_HANDLER->getStruct().playListName = NETEASE_PLAY_LIST_PREFIX + QString::number(info.id) + '_' + info.name;
	refreshNeteaseMusicBtns(info.id);
	SETTING_HANDLER->setMusicIndex(0);

	player_->playCurrentIndex();
	slotMusicIndexChanged(SETTING_HANDLER->getMusicIndex(), SETTING_HANDLER->getMusicIndex());  // 初始化被播放的那个音乐按钮样式
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
		emit sigPlayOrPause(true);
		ui->music_info_widget->animationHide();
    });

    connect(player_, &PlayerBase::MusicInfoChanged, this, [this](MusicInfo info)
    {
		if (info.title.isEmpty())
		{
			QString filename = SETTING_HANDLER->currentMusicUrl();
			int i1 = filename.lastIndexOf('/') + 1;
			int i2 = filename.indexOf('.', i1);
			info.title = filename.mid(i1, i2 - i1);
		}

		refreshImageWidget(info);

		emit sigChangeSystemIconToolTip(info.title, info.singers, info.album);
    });

	connect(player_, &PlayerBase::errorOccurred, this, [this](PlayMusicError error)
	{
		switch (error)
		{
		case MusicUrlIsEmpty:
			on_btn_next_clicked();
			break;
		default:
			DWarning << "unknow error:" << error;
			break;
		}
	});

    // 使本次播放进度变成上次关闭时的进度
    connect(player_, &PlayerBase::mediaAtEnd, this, &Widget::on_btn_next_clicked);

    // 音乐时长改变
    connect(player_, &PlayerQt::durationChanged, ui->progress, &QSlider::setMaximum);

    // 音乐前进，改变进度条
    connect(player_, &PlayerQt::positionChanged, this, &Widget::slotPositionChanged);

    // 按下进度条，停止根据音乐改变进度条
    connect(ui->progress, &DProgressBar::sigMousePressed, this, [this]()
    {
        movingProgress_ = true;
    });

    // 松开进度条，改变音乐进度
    connect(ui->progress, &DProgressBar::sigMouseReleased, this, [this](int val)
    {
        player_->setPosition(val);
        movingProgress_ = false;
    });

    // 搜索框搜歌曲
	connect(ui->find_widget, &SearchEdit::focusOnBtnAt, this, [this](DSizeType oldFocusIndex, DSizeType newFocusIndex)
	{
		if (SETTING_HANDLER->getMusicIndex() != oldFocusIndex)
			setMusicBtnStyle(oldFocusIndex, &BaseMusicButton::setNormalStyle);
		showMusicBtnAt(newFocusIndex);
	});

    // 点击搜索框内关闭按钮
    connect(ui->find_widget, &SearchEdit::sigBtnCloseClicked, this, &Widget::slotSearchEditClose);

	// 在搜索框按下了回车，等同于按下当前搜索到的音乐按钮
	connect(ui->find_widget, &SearchEdit::sigKeyEnterClicked, this, &Widget::slotLocalMusicBtnClicked);

    // 音乐下标改变
    connect(SETTING_HANDLER, &SettingHandler::sigMusicIndexChanged, this, &Widget::slotMusicIndexChanged);

    // 右侧按钮Widget
	connect(ui->multi_btn_widget, &MultiBtnWidget::sigBtnNeteaseClicked, this, [this]()
	{
		NET_LOGIN_DIALOG->exec();
	});
	connect(ui->multi_btn_widget, &MultiBtnWidget::sigBtnPlayListClicked, this, [this]()
	{
		if (2 == ui->stacked_music_btn->currentIndex())
		{
			ui->multi_btn_widget->setBtnPlayListIcon(QIcon(":/svgs/play_list.svg"));
			ui->stacked_music_btn->setCurrentIndex(0);
		}
		else
		{
			ui->multi_btn_widget->setBtnPlayListIcon(QIcon(":/svgs/back.svg"));
			ui->stacked_music_btn->setCurrentIndex(2);
		}
	});
    connect(ui->multi_btn_widget, &MultiBtnWidget::sigBtnSettingClicked, this, [this]()
    {
        ui->btn_more->setIcon(QIcon(":/svgs/back.svg"));
        ui->stacked_widget->setCurrentIndex(1);
    });
	connect(ui->multi_btn_widget, &MultiBtnWidget::sigBtnSearchClicked, this, [this]()
	{
		
	});

	/****************** 设置页中的一些信号 ********************/
	connect(ui->setting_tab_widget, &SettingTabWidget::sigBGModeChanged, this, &Widget::slotBGModeChanged);
	connect(ui->setting_tab_widget, &SettingTabWidget::sigPlayerModeChanged, this, &Widget::slotPlayerModeChanged);
	connect(ui->setting_tab_widget, &SettingTabWidget::sigMainColorChanged, this, &Widget::refreshMainColor);;
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

    DVector<QString> playList = SETTING_HANDLER->currentPlayList();
    DSizeType index = 0;
    for (const QString& musicUrl : playList)
    {
        BaseMusicButton* btn = addLocalMusicBtn(musicUrl);
        btn->setMusicIndex(index);
        ++index;
    }
}

void Widget::refreshNeteaseMusicBtns(dint64 playListId)
{
	QLayoutItem* child;
	while (child = ui->music_layout->itemAt(0))
	{
		ui->music_layout->removeItem(child);
		if (child->widget())
			delete child->widget();
	}

	auto songs = NETEASE_HANDLER->getSongsfromPlayList(playListId);
	DVector<QString> neteasePlayList;
	for (const auto& song : songs)
	{
		neteasePlayList.pushBack(QString::number(song.id));
		addNeteaseMusicBtn(song);
	}

	SETTING_HANDLER->setCurrentNeteaseSongsInfo(songs);
	SETTING_HANDLER->addPlayList(SETTING_HANDLER->getStruct().playListName, neteasePlayList, true);
}

void Widget::refreshPlayListBtns()
{
	QLayoutItem* child;
	while (child = ui->play_list_layout->itemAt(1))
	{
		ui->play_list_layout->removeItem(child);
		if (child->widget())
			delete child->widget();
	}

	DList<QString> playListNames = SETTING_HANDLER->getStruct().playListMap.keys();
	for (const QString& playListName : playListNames)
	{
		//QString filename = url.fileName();
		//QString str = filename.mid(0, filename.indexOf('.'));
		PlayListButton* btn = new PlayListButton(playListName, this);
		btn->setContextMenuPolicy(Qt::CustomContextMenu);
		if (SETTING_HANDLER->getStruct().playListName == playListName)
			btn->setChoosed();

		connect(btn, &PlayListButton::sigPlayListClicked, this, [btn, this](const QString& playListName)
		{
			SETTING_HANDLER->getStruct().playListName = playListName;
			//SETTING_HANDLER->getStruct().musicPosition = 0;
			SETTING_HANDLER->setMusicIndex(0);

			switch (SETTING_HANDLER->getStruct().playMode)
			{
			case RANDOM:
				SETTING_HANDLER->clearRandomPlayList();
				break;
			}

			refreshMusicBtns();
			player_->playCurrentIndex();

			slotMusicIndexChanged(SETTING_HANDLER->getMusicIndex(), SETTING_HANDLER->getMusicIndex());  // 初始化被播放的那个音乐按钮样式
		});
		connect(btn, &PlayListButton::sigDeleteClicked, this, [this](const QString& playListName)
		{
			if (playListName == SETTING_HANDLER->getStruct().playListName)
			{
				OneMessageBox::warning(this, tr("警告"), tr("正在播放的列表不可删除！"));
				return;
			}

			QString filePath = QCoreApplication::applicationDirPath();
			filePath += "/config/play_lists/" + playListName + PLF_FORMAT;

			QFile file(filePath);
			if (!file.exists())
			{
				OneMessageBox::warning(this, tr("警告"), tr("文件 %1 不存在！").arg(filePath));
				return;
			}
			file.remove();

			SETTING_HANDLER->refreshPlayList();
			refreshPlayListBtns();
		});
		
		//connect(btn, &QPushButton::customContextMenuRequested, this, [btn](const QPoint& pos)
		//	{
		//		DMenu* menu = DMenu::getButtonMenu();
		//		menu->show(btn->getMusicIndex());
		//	});
		ui->play_list_layout->addWidget(btn);
	}

	if (SETTING_HANDLER->getNeteaseInfo().hasLogin)
	{
		auto allPlayLists = NETEASE_HANDLER->getAllPlayListsInfo();
		appendNeteasePlayListBtns(allPlayLists);
	}
}

void Widget::refreshMainColor(QColor mainColor)
{
	ui->widget_btn->setStyleSheet(QString("#widget_btn { background-color: %1; }").arg(DStyle::color2Str(mainColor)));
}

void Widget::appendNeteasePlayListBtns(const DVector<NeteasePlayListInfo>& infos)
{
	for (const auto& info : infos)
	{
		PlayListButton* btn = new PlayListButton(info, this);
		connect(btn, &PlayListButton::sigNeteasePlayListClicked, this, &Widget::slotNeteasePlayListClicked);
		ui->play_list_layout->addWidget(btn);
	}
}

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

BaseMusicButton* Widget::addNeteaseMusicBtn(const NeteaseSongInfo& info)
{
	NeteaseMusicBtn* btn = new NeteaseMusicBtn(info, this);
	btn->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(btn, &BaseMusicButton::clicked, this, &Widget::slotNeteaseMusicBtnClicked);
	connect(btn, &QPushButton::customContextMenuRequested, this, [btn](const QPoint& pos)
	{
		//DMenu* menu = DMenu::getButtonMenu();
		//menu->show(btn->getMusicIndex());
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

	if (SETTING_HANDLER->getMusicIndex() != index)
		setMusicBtnStyle(index, &BaseMusicButton::setSelectStyle);
}

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

void Widget::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void Widget::dropEvent(QDropEvent *event)
{
    // 在别的页不接受拖入事件
    if (ui->stacked_widget->currentIndex() != 0)
        return;

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

    DVector<QString> currentPlayList = SETTING_HANDLER->currentPlayList();
	DVector<QString> list;
	QList<QUrl> all = event->mimeData()->urls();
    for (const QUrl& url : all)
    {
        QString type = url.toLocalFile().section('.', -1);

        // 已经存在的歌也不添加
        if (TYPE_LIST.contains(type) && !currentPlayList.contains(url.toLocalFile()))
            list.pushBack(url.toLocalFile());
    }

	int ret = OneMessageBox::information(nullptr, tr("提示"),
		tr("把这%1首歌添加到歌单%2?").arg(list.size()).arg(SETTING_HANDLER->getStruct().playListName), ALL_BTN);

	if (QDialog::Accepted == ret)
	{
		if (SETTING_HANDLER->notExistPlayList())
			SETTING_HANDLER->addPlayList("新播放列表", list);
		else
			SETTING_HANDLER->add2CurrentPlayList(list);
	}

	refreshMusicBtns();


    // 重新设置滚动条的数值
    //ui->scrollArea->verticalScrollBar()->setMaximum(btn_list_.size());
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

	stackedMusicBtnAnimation_->setEasingCurve(STACKED_MUSIC_BTN_SMALL);
    stackedMusicBtnAnimation_->setStartValue(QRect(nowX, nowY, nowWidth, nowHeight));
    stackedMusicBtnAnimation_->setEndValue(QRect(nowX, nowY, STACKED_MUSIC_BTN_WIDTH, nowHeight));
    stackedMusicBtnAnimation_->start();

	stackedMusicBtnAnimationSub1_->setEasingCurve(STACKED_MUSIC_BTN_SMALL);
	stackedMusicBtnAnimationSub1_->setStartValue(QRect(0, ui->find_widget->y(), ui->find_widget->width(), ui->find_widget->height()));
	stackedMusicBtnAnimationSub1_->setEndValue(QRect(0, ui->find_widget->y(), STACKED_MUSIC_BTN_WIDTH, ui->find_widget->height()));
	stackedMusicBtnAnimationSub1_->start();
	
	stackedLocalBtnsAnimation_->setEasingCurve(STACKED_MUSIC_BTN_SMALL);
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

	stackedMusicBtnAnimation_->setEasingCurve(STACKED_MUSIC_BTN_BIG);
    stackedMusicBtnAnimation_->setStartValue(QRect(oldX, oldY, oldWidth, oldHeight));
    stackedMusicBtnAnimation_->setEndValue(QRect(oldX, oldY, ui->multi_func_widget->width(), oldHeight));
    stackedMusicBtnAnimation_->start();

	stackedMusicBtnAnimationSub1_->setEasingCurve(STACKED_MUSIC_BTN_BIG);
	stackedMusicBtnAnimationSub1_->setStartValue(QRect(0, ui->find_widget->y(), ui->find_widget->width(), ui->find_widget->height()));
	stackedMusicBtnAnimationSub1_->setEndValue(QRect(0, ui->find_widget->y(), ui->multi_func_widget->width(), ui->find_widget->height()));
	stackedMusicBtnAnimationSub1_->start();

	stackedLocalBtnsAnimation_->setEasingCurve(STACKED_MUSIC_BTN_BIG);
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
		pressPos_ = pos;
    }
}

void Widget::mouseMoveEvent(QMouseEvent *ev)
{
    if (thisIsMoveWindow_)
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        auto global_pos = ev->globalPosition().toPoint();
#else
		auto global_pos = ev->globalPos();
#endif
        move(global_pos - pressPos_);
    }
}

void Widget::mouseReleaseEvent(QMouseEvent* event)
{
    thisIsMoveWindow_ = false;
	beforeAniPos_ = pos();
}

void Widget::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->pos().y() < 20)
	{
		ui->scrollArea->verticalScrollBar()->setValue(0);
	}
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    //qDebug() << "aaa->" << event->key();
    switch (event->key())
    {
    case 32:  // space
        on_btn_play_clicked();
        break;
    case 16777249:  // ctrl
        pressedCtrl_ = true;
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

void Widget::initMultiFuncWidget()
{
    ui->multi_btn_widget->moveToHide();
    ui->stacked_music_btn->resize(ui->multi_func_widget->width(), ui->stacked_music_btn->height());
    ui->find_widget->moveToHide();
    ui->find_widget->resize(ui->multi_func_widget->width(), ui->find_widget->height());
    ui->stacked_local_btns->setGeometry(QRect(ui->stacked_local_btns->x(), 0
        , ui->multi_func_widget->width(), ui->multi_func_widget->height()));
}

void Widget::drawImage(const QImage& image)
{
	cv::Mat originMat = ImageHandler::QImageToCvMat(image.isNull() ? QImage(":/images/music.png") : image);
	cv::Mat fitMat = originMat;
	switch (SETTING_HANDLER->getStruct().bgMode)
	{
	case ONLY_LEFT:
		ui->label_bg_image->clear();
		break;
	case FULL_WIDGET:
	{
		fitMat = ImageHandler::fitImage(originMat, ui->label_bg_image->width()
			, ui->label_bg_image->height());
		cv::Mat blurMat = ImageHandler::blurImage(fitMat, 20);
		cv::Mat lightMat = ImageHandler::lightImage(blurMat, 0.65);
		QImage lightImage = ImageHandler::cvMatToQImage(lightMat);
		QImage roundImage = ImageHandler::roundImage(lightImage, 40, true);

		ui->label_bg_image->setPixmap(QPixmap::fromImage(roundImage));
		break;
	}
	default:
		break;
	}

	// 画图片
	ui->music_info_widget->drawBGMat(fitMat);
	ui->music_info_widget->drawMainMat(originMat);
	ui->music_info_widget->animationShow();
}

void Widget::refreshImageWidget(const MusicInfo& info)
{
	// 图片要过一会儿才下载完
	if (!info.imgIsReady)
	{
		DWarning << "-> image is not ready!";
	}
	else
	{
		// 画图片
		drawImage(info.image);
	}

    // 设置歌曲名
	QString musicName;
    if (info.title.isEmpty())
    {
		QString filename = SETTING_HANDLER->currentMusicUrl();
		int i1 = filename.lastIndexOf('/') + 1;
		int i2 = filename.indexOf('.', i1);
		musicName = filename.mid(i1, i2 - i1);
    }
    else
    {
		musicName = info.title;
    }
	ui->music_info_widget->setMusicName(musicName);
	ui->btn_music_name->setText(musicName);
	setWindowTitle(musicName);

    // 设置歌手名
    ui->music_info_widget->setSingerName(info.singers.isEmpty() ? "未知歌手" : info.singers);

    // 设置专辑名
    ui->music_info_widget->setAlbumName(info.album.isEmpty() ? "未知专辑" : info.album);

	// 设置歌词
	ui->music_info_widget->setLyrics(info.lyricStr);
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
		move(endx, endy);
	}
	else
	{
		if (animateLabel_->isHidden())
		{
            int beginX = animateLabel_->x() + MAIN_WIDGET_WIDTH / 2;
            int beginY = animateLabel_->y() + MAIN_WIDGET_HEIGHT / 2;
            endx = beforeAniPos_.x();
            endy = beforeAniPos_.y();
		}
		else
		{
            // 在播放hide动画时打断，从当前位置当前大小开始恢复到press位置
            startx = animateLabel_->x();
            starty = animateLabel_->y();
            startWidth = animateLabel_->width();
            startHeight = animateLabel_->height();
            endx = beforeAniPos_.x();
            endy = beforeAniPos_.y();
		}
	}

    // 防止界面在被其他窗口遮挡。
    // 排除了 在播放hide动画时打断 的情况，防止show时窗口闪烁
    //if (isHidden())

    //{
    //    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    //    QWidget::show();
    //    setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    //}
	animateLabel_->show();

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

	int startX = animateLabel_->x();
	int startY = animateLabel_->y();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 2)
	animateLabel_->setPixmap(grab());
#else
	animateLabel_->setPixmap(QPixmap::grabWidget(this));
#endif
	animateLabel_->show();
	hide();
	
    // 界面还在播放动画显示或隐藏时，不会更新pressX_和pressY_的值
    if (animation_->state() != QPropertyAnimation::Running)
    {
		beforeAniPos_ = pos();
    }

    int newx = QCursor::pos().x();
    int newy = QCursor::pos().y();
    int w = animateLabel_->width();
    int h = animateLabel_->height();

	animation_->stop();
    animation_->setEasingCurve(MAIN_WIDGET_HIDE_EASING);
    animation_->setStartValue(QRect(startX, startY, w, h));
    animation_->setEndValue(QRect(newx, newy, 0, 0));
    animation_->start();
    isShowAnimation_ = false;
}

void Widget::move(int xx, int yy)
{
	move(QPoint(xx, yy));
}

void Widget::move(const QPoint& p)
{
	animateLabel_->move(p);
	QWidget::move(p);
}

Widget::~Widget()
{
	if (animateLabel_)
		animateLabel_->deleteLater();
    delete ui;
}

void Widget::init()
{
	ui->stacked_music_btn->setCurrentIndex(0);

	NETEASE_HANDLER->init();
	bool isNeteaseLogin = !SETTING_HANDLER->getNeteaseInfo().cookie.isEmpty() && NETEASE_HANDLER->checkLoginStatus();
	
	// 初始化界面
	initMultiFuncWidget();

	refreshMainColor(SETTING_HANDLER->getStruct().mainColor);

	MusicInfo info;
	info.title = "歌曲名";
	info.singers = "歌手";
	info.album = "专辑";
	info.image = QImage(":/images/music.png");
	refreshImageWidget(info);

	if (isNeteaseLogin)
	{
		ImageDownloadCallBack* callBack = new ImageDownloadCallBack(this);
		connect(callBack, &ImageDownloadCallBack::sigImageSet, this, [this, callBack](SharedImage image)
		{
			ui->multi_btn_widget->setBtnNeteaseInfo(*image, SETTING_HANDLER->getNeteaseInfo().nickname);
			callBack->deleteLater();
		});
		ImageHandler::downloadImage(SETTING_HANDLER->getNeteaseInfo().avatarUrl + "?param=80y80", callBack);

		auto allPlayLists = NETEASE_HANDLER->getAllPlayListsInfo();
		appendNeteasePlayListBtns(allPlayLists);
	}

	// 如果上次的播放列表是网易云的，则先获取播放列表
	if (SETTING_HANDLER->getStruct().playListName.startsWith(NETEASE_PLAY_LIST_PREFIX)
		&& isNeteaseLogin)
	{
		int i = SETTING_HANDLER->getStruct().playListName.indexOf('_', NETEASE_PLAY_LIST_PREFIX.size());
		if (-1 != i)
		{
			QString idStr = SETTING_HANDLER->getStruct().playListName.mid(NETEASE_PLAY_LIST_PREFIX.size(), i - NETEASE_PLAY_LIST_PREFIX.size());
			
			bool ok = false;
			dint64 id = idStr.toLongLong(&ok);
			if (ok)
			{
				//NeteasePlayListInfo info;
				//info.name = SETTING_HANDLER->getStruct().playListName.mid(i + 1);
				//info.id = id;
				refreshNeteaseMusicBtns(id);
			}
		}
	}

	if (SETTING_HANDLER->getStruct().playListName == TEMP_PLAY_LIST_NAME)
		player_->playCurrentIndex();
	else
		player_->playCurrentIndex(SETTING_HANDLER->getStruct().musicPosition);

	slotMusicIndexChanged(SETTING_HANDLER->getMusicIndex(), SETTING_HANDLER->getMusicIndex());  // 初始化被播放的那个音乐按钮样式


	// 播放之前上次关闭时放的歌
	if (!SETTING_HANDLER->getStruct().playOnStart)
	{
		on_btn_play_clicked();
	}
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 2)
	animateLabel_->setPixmap(grab());
#else
	animateLabel_->setPixmap(QPixmap::grabWidget(this));
#endif
}

void Widget::uninit()
{
	NETEASE_HANDLER->uninit();
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
		emit sigPlayOrPause(true);
    }
    else
    {
        ui->btn_play->setIcon(QIcon(":/svgs/play.svg"));
		emit sigPlayOrPause(false);
	}
}

// 上一首
void Widget::on_btn_previoud_clicked()
{
    ui->btn_play->setIcon(QIcon(":/svgs/pause.svg"));
    player_->playPrevious();
	emit sigPlayOrPause(true);
}

// 下一首
void Widget::on_btn_next_clicked()
{
    ui->btn_play->setIcon(QIcon(":/svgs/pause.svg"));
    player_->playNext();
	emit sigPlayOrPause(true);
}

// 音量减
void Widget::on_btn_down_clicked()
{
	float volume = SETTING_HANDLER->downVolume();
    player_->setVolume(volume);
}

// 音量加
void Widget::on_btn_up_clicked()
{
	float volume = SETTING_HANDLER->upVolume();
	player_->setVolume(volume);
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
			stackedMusicBtnAnimationTimer_->start(MORE_BTN_WIDGET_ANIMATION_TIME / 3);
        }
        else
        {
			// 如果在播放列表页，则切换到音乐页
			if (2 == ui->stacked_music_btn->currentIndex())
			{
				ui->multi_btn_widget->setBtnPlayListIcon(QIcon(":/svgs/play_list.svg"));
				ui->stacked_music_btn->setCurrentIndex(0);
			}

            ui->multi_btn_widget->animationHide();
            connect(stackedMusicBtnAnimationTimer_, &QTimer::timeout, this, [this]()
            {
                stackedMusicBtnAnimationTimer_->deleteLater();
                stackedMusicBtnAnimationTimer_ = nullptr;
                animationStackedMusicBtnBig();
            });
			stackedMusicBtnAnimationTimer_->start(MORE_BTN_WIDGET_ANIMATION_TIME / 3);
        }

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

void Widget::slotPlayerModeChanged(PLAYER_MODE playerMode)
{
    switch (playerMode)
    {
    case LOCAL:
    //    init_local();
        break;
    case MYSITE:
    //    init_mysite();
        break;
    case ONLINE:
    //    init_online();
        break;
    case NETEASE:
    //    init_netease();
        break;
    }
    SETTING_HANDLER->getStruct().playerMode = playerMode;
}


//void Widget::on_btn_search_clicked()
//{
//    QString word = ui->le_search->text();
//    if (word.isEmpty())
//        return;
//
//    clear_button(ui->music_layout_online);
//
//    DVector<MusicInfo> list = OnlineHandler::getInstance()->search_online_music(word);
//    for (const MusicInfo& info : list)
//    {
//        add_online_music(info);
//    }
//}
