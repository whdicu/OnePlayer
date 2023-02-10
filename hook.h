#ifndef HOOK_H
#define HOOK_H
#include <windows.h>
#include <QObject>
class Hook :public QObject
{
	Q_OBJECT
public:
	static Hook* getInstance();
	void installHook();
	void unInstallHook();
    void sendSignal(DWORD key);

private:
	Hook() = default;
	~Hook() = default;

signals:
	void sendKeyType(DWORD);
};

#endif // HOOK_H
