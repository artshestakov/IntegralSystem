#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
//-----------------------------------------------------------------------------
class ISSYSTEM_EXPORT ISVersion : public QObject
{
	Q_OBJECT

public:
	ISVersion(const ISVersion &) = delete;
	ISVersion(ISVersion &&) = delete;
	ISVersion &operator=(const ISVersion &) = delete;
	ISVersion &operator=(ISVersion &&) = delete;
	~ISVersion();

	static ISVersion& GetInstance();
	void Initialize();
	void SetVersion(const QString &version); //�������� ������

	bool IsValid() const; //���������� ������
	QString GetVersion() const; //�������� ������ ������
	int GetMajor() const; //�������� ����� �������� ������
	int GetMinor() const; //�������� ����� �������� ������
	int GetRevision() const; //�������� ����� ������� �� �����������
	int GetBuild() const; //�������� ����� ������
	QDate GetDate() const; //�������� ���� ������
	QTime GetTime() const; //�������� ����� ������
	QString GetHash() const; //�������� ��� �����������
	QString GetPlatform() const; //�������� ��������� ����������

protected:
	QString GetFileData(const QString &FileName) const;

private:
	ISVersion();

	QString VersionString;
	int Major;
	int Minor;
	int Revision;
	int Build;
	QDate Date;
	QTime Time;
	QString Hash;
	QString Platform;
};
//-----------------------------------------------------------------------------
