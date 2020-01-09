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
	void StartTimer(); //������ �������
	void RestartTimer(); //���������� �������
	void SetInterval(int interval); //�������� ��������
	bool GetLock() const; //�������� ���� ����������
	bool GetInitialized() const; //�������� ���� ������������

	void LockApplication(); //���������� ���������
	void UnlockApplication(); //������������� ���������

protected:
	void Timeout();

private:
	ISAutoLocking();
	
	QTimer *TimerLock;
	bool Lock; //���� ����������
	bool Initialized; //���� �������������
};
//-----------------------------------------------------------------------------
