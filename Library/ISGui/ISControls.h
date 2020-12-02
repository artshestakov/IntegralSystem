#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISControls
{
public:
	ISControls();
	virtual ~ISControls();

	static void SetBackgroundColorWidget(QWidget *Widget, const QColor &Color); //�������� ���� ���� �������

	static QFrame* CreateHorizontalLine(QWidget *ParentWidget); //�������� �������������� �����
	static QFrame* CreateHorizontalLinePlain(QWidget *ParentWidget); //�������� ������ �������������� �����
	static QFrame* CreateVerticalLine(QWidget *ParentWidget); //�������� ������������ �����
	static QFrame* CreateVerticalLinePlain(QWidget *ParentWidget); //�������� ������������ �����

//��������
	static QAction* CreateActionClose(QObject *ParentObject); //�������� "�������"
	static QAction* CreateActionSave(QObject *ParentObject); //�������� "���������"
	static QAction* CreateActionSaveAndClose(QObject *ParentObject); //�������� "��������� � �������"
	static QAction* CreateActionExit(QObject *ParentObject); //�������� "�����"

	static QAction* CreateSeparator(QObject *ParentObject); //������� � �������� ���������

//����������� ����
	static QAction* GetActionContextUndo(QObject *ParentObject); //�������� ��������
	static QAction* GetActionContextRedo(QObject *ParentObject);  //�������� �������
	static QAction* GetActionContextCut(QObject *ParentObject); //�������� ��������
	static QAction* GetActionContextCopy(QObject *ParentObject); //�������� ����������
	static QAction* GetActionContextPaste(QObject *ParentObject); //�������� ��������
	static QAction* GetActionContextDelete(QObject *ParentObject); //�������� �������
	static QAction* GetActionContextSelectAll(QObject *ParentObject); //�������� �������� ���
	static QAction* GetActionContextToUpper(QObject *ParentObject); //�������� ������� �������
	static QAction* GetActionContextToLower(QObject *ParentObject); //�������� ������ �������

//������� ����� ������
	static QAction* CreateActionCreate(QObject *ParentObject);
	static QAction* CreateActionCreateCopy(QObject *ParentObject);
	static QAction* CreateActionEdit(QObject *ParentObject);
	static QAction* CreateActionDelete(QObject *ParentObject);
	static QAction* CreateActionUpdate(QObject *ParentObject);
	static QAction* CreateActionSearch(QObject *ParentObject);
	static QAction* CreateActionSearchClearResults(QObject *ParentObject);
	static QAction* CreateActionPrint(QObject *ParentObject);
	static QAction* CreateActionRecordInformartion(QObject *ParentObject);
	static QAction* CreateActionNoteObject(QObject *ParentObject);
};
//-----------------------------------------------------------------------------
