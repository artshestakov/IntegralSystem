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
	void SelectFile(); //Выбор файла через проводник
	void HandlingFile(const QString &FilePath); //Обработка файла
	void Open(); //Открытие файла
	void Rename(); //Переименование файла
	void Save(); //Сохранение файла на диск

	bool UpdateFile(const QString &FileName, const QString &FileExtension, const QIcon &FileIcon, const QByteArray &FileData) const; //Обновление файла в базе
	bool InsertFile(const QString &FileName, const QString &FileExtension, const QIcon &FileIcon, const QByteArray &FileData); //Добавление файла в базу
	
private:
	ISButtonFile *ButtonFile;
	QMenu *MenuFile;
	QVariant FileID;
};
//-----------------------------------------------------------------------------
