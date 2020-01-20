#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISFieldEditBase.h"
#include "ISButtonFile.h"
//-----------------------------------------------------------------------------
class ISFileEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISFileEdit(QWidget *parent = 0);
	virtual ~ISFileEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;
	void SetVisibleClear(bool Visible) override;

protected:
	void SelectFile(); //����� ����� ����� ���������
	void HandlingFile(const QString &FilePath); //��������� �����
	void Open(); //�������� �����
	void Rename(); //�������������� �����
	void Save(); //���������� ����� �� ����

	bool UpdateFile(const QString &FileName, const QString &FileExtension, const QIcon &FileIcon, const QByteArray &FileData) const; //���������� ����� � ����
	bool InsertFile(const QString &FileName, const QString &FileExtension, const QIcon &FileIcon, const QByteArray &FileData); //���������� ����� � ����
	
private:
	ISButtonFile *ButtonFile;
	QMenu *MenuFile;
	QVariant FileID;
};
//-----------------------------------------------------------------------------
