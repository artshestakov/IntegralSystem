#pragma once
#ifndef _CGCONFIGURATORFIAS_H_INCLUDED
#define _CGCONFIGURATORFIAS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "CGConfiguratorBase.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGConfiguratorFIAS : public CGConfiguratorBase
{
	Q_OBJECT

public:
	Q_INVOKABLE CGConfiguratorFIAS();
	virtual ~CGConfiguratorFIAS();

public slots:
	void prepare(); //���������� ��������
	void update(); //����������

private:

	//����������
	bool FileHandling(const QFileInfo &FileInfo, const QString &ResultPath); //��������� ����� ��������
	QString GetFileResultName(const QString &FileName) const; //�������� ������������ ����� � ����������� ���������

	//����������
	void InitializeKeys();
	bool FileUpload(const QFileInfo &FileInfo); //�������� ����� � ����
	quint64 GetCountLine(const QString &FilePath) const; //�������� ���������� ����� � �����
	QString GetTableName(const QString &FilePath) const; //�������� ������������ �������
	QStringMap ParseLine(const QString &Content) const; //������� ������
	bool Select(PMetaClassTable *MetaTable, const QStringMap &StringMap) const;
	void Update(PMetaClassTable *MetaTable, const QStringMap &StringMap);
	void Insert(PMetaClassTable *MetaTable, const QStringMap &StringMap);

private:
	QStringMap MapKeys;
};
//-----------------------------------------------------------------------------
#endif
