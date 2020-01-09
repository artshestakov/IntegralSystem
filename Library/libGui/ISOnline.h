#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISOnline : public QObject
{
	Q_OBJECT

public:
	ISOnline(const ISOnline &) = delete;
	ISOnline(ISOnline &&) = delete;
	ISOnline &operator=(const ISOnline &) = delete;
	ISOnline &operator=(ISOnline &&) = delete;
	~ISOnline();

	static ISOnline& GetInstance();

	void Initialize(int Timeout);
	void Exit(); //Регистрация выхода из программы

protected:
	void Enter(); //Регистрация входа в программу
	
	void Timeout();
	bool eventFilter(QObject *Watched, QEvent *e);
	void Insert(bool Thread, const ISUuid &TypeUID);

private:
	ISOnline();
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
