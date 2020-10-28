#pragma once
#ifndef _ISFILEDIALOG_H_INCLUDED
#define _ISFILEDIALOG_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISFileDialog : public QFileDialog
{
	Q_OBJECT

public:
	ISFileDialog(QWidget *parent = 0);
	virtual ~ISFileDialog();

	static QString GetOpenFileNameImage(QWidget *parent); //�������� ���� � ������������ �����������
	static QString GetSaveFileNameImage(QWidget *parent, const QString &FileName = QString()); //�������� ���� � ������������ �����������
	static QString GetOpenFileName(QWidget *parent, const QString &FilePath = QString(), const QString &FilterName = QString()); //�������� ���� � ������������ �����
	static QString GetSaveFileName(QWidget *parent, const QString &FilterName, const QString &FileName = QString()); //�������� ���� � ������������ �����
	static QString GetDirectoryPath(QWidget *parent); //�������� ���� � ����������
	static QStringList GetOpenFileNames(QWidget *parent, const QString &FilterName = QString()); //�������� ���� � ����������� ������
};
//-----------------------------------------------------------------------------
#endif
