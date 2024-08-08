#pragma once
#pragma execution_character_set("utf-8")
#include <QMenu>

class QPushButton;

class DSystemTrayMenu : public QMenu
{
	Q_OBJECT

public:
	DSystemTrayMenu(QWidget *parent = nullptr);
	~DSystemTrayMenu();
	void setPlaying(bool isPlaying);
	void setMusicName(const QString& name);

signals:
	void sigName();
	void sigPrevioud();
	void sigPlay();
	void sigNext();
	void sigQuit();

private:
	void initNameItem();
	void initPlayItem();
	void initQuitItem();

	QPushButton* nameBtn_;
	QPushButton* playBtn_;
};
