#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISDebug : public QObject
{
	Q_OBJECT

signals:
	void Output(const QString &String);

public:
	ISDebug(const ISDebug &) = delete;
	ISDebug(ISDebug &&) = delete;
	ISDebug &operator=(const ISDebug &) = delete;
	ISDebug &operator=(ISDebug &&) = delete;
	~ISDebug();

	static ISDebug& GetInstance();

	static void ShowCaratString(const QString &CoreName, const QString &String);
	static void ShowString(const QString &String);
	static void ShowEmptyString(bool AddInLog = true);

	static void ShowDebugString(const QString &String); //����� ����������� ���������
	static void ShowInfoString(const QString &String); //����� ��������������� ���������
	static void ShowWarningString(const QString &String); //����� ��������������
	static void ShowCriticalString(const QString &String); //����� ������������ ���������
	static void ShowAssertString(const QString &String); //����� ��������� �������
	static void ShowExceptionString(const QString &String); //����� ��������� ����������

	void OutputString(const QString &String, bool AddInLog = true); //����� ���������
	void EmptyString(bool AddInLog = true); //�������� ������ ������

	void CaratString(const QString &CoreName, const QString &String); //����� ��������� � �����
	void DebugString(const QString &String); //����� ����������� ���������
	void InfoString(const QString &String); //����� ��������������� ���������
	void WarningString(const QString &String); //����� ��������������
	void CriticalString(const QString &String); //����� ������������ ���������
	void AssertString(const QString &String); //����� ��������� �������
	void ExceptionString(const QString &String); //����� ��������� ����������

	void SetAssertMessage(const QString &assert_message);
	QString GetAssertMessage() const;
	QStringList GetDebugMessages() const;

protected:
	QString GetCompleteString(ISNamespace::DebugMessageType MessageType, const QString &String) const; //������������ ������
	
private:
	ISDebug();

	QStringList Messages;
	QString AssertMessage;
	QMutex Mutex;
};
//-----------------------------------------------------------------------------
