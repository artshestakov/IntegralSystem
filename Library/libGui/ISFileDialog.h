#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISFileDialog : public QFileDialog
{
	Q_OBJECT

public:
	ISFileDialog(QWidget *parent = 0);
	virtual ~ISFileDialog();

	static QString GetOpenFileNameImage(QWidget *parent); //�������� ���� � ������������ �����������
	static QString GetSaveFileNameImage(QWidget *parent, const QString &FileName = QString()); //�������� ���� � ������������ �����������
	static QString GetSaveFileNameAsteriskRecord(QWidget *parent, const QString &FileName = QString()); //�������� ���� � ����������� ������ ���������
	static QString GetOpenFileName(QWidget *parent, const QString &FilePath = QString(), const QString &FilterName = QString()); //�������� ���� � ������������ �����
	static QString GetSaveFileName(QWidget *parent, const QString &FilterName, const QString &FileName = QString()); //�������� ���� � ������������ �����
	static QString GetDirectoryPath(QWidget *parent); //�������� ���� � ����������
	static QStringList GetOpenFilesName(QWidget *parent, const QString &FilterName = QString()); //�������� ���� � ����������� ������
};
//-----------------------------------------------------------------------------
