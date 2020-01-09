#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISAutoLocking : public QObject
{
	Q_OBJECT

signals:
	void AutoLock();

public:
	ISAutoLocking(const ISAutoLocking &) = delete;
	ISAutoLocking(ISAutoLocking &&) = delete;
	ISAutoLocking &operator=(const ISAutoLocking &) = delete;
	ISAutoLocking &operator=(ISAutoLocking &&) = delete;
	~ISAutoLocking();

	static ISAutoLocking& GetInstance();
	void Initialize();
	void StartTimer(); //Запуск таймера
	void RestartTimer(); //Перезапуск таймера
	void SetInterval(int interval); //Изменить интервал
	bool GetLock() const; //Получить флаг блокировки
	bool GetInitialized() const; //Получить флаг инициализаци

	void LockApplication(); //Блокировка программы
	void UnlockApplication(); //Разблокировка программы

protected:
	void Timeout();

private:
	ISAutoLocking();
	
	QTimer *TimerLock;
	bool Lock; //Флаг блокировки
	bool Initialized; //Флаг инициализации
};
//-----------------------------------------------------------------------------
