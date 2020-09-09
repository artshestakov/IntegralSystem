#pragma once
#ifndef _ISINTERFACEFORM_H_INCLUDED
#define _ISINTERFACEFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
//!������� ����� ����� ����������
class ISGUI_EXPORT	ISInterfaceForm : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(ISUuid FormUID READ GetFormUID)

public:
	ISInterfaceForm(QWidget *parent = 0, Qt::WindowFlags Flags = 0);
	virtual ~ISInterfaceForm();

	void ForbidResize(); //��������� �������� ������ �����
	void SetVisibleShadow(bool Visible); //�������� ��������� ����
	ISUuid GetFormUID() const; //�������� ���������� ������������� �����

protected:
	virtual void showEvent(QShowEvent *e);
	virtual void resizeEvent(QResizeEvent *e);
	virtual void keyPressEvent(QKeyEvent *KeyEvent);

	QVBoxLayout* GetMainLayout(); //�������� ��������� �� ������� ����������� �����
	bool IsShowed() const;

	void FlashingStart(int Interval, const QColor &Color); //������ �������� ������� ���� �����
	void FlashingTimeout(); //������� ��������� ����� �������� ������� ���� �����
	void FlashingStop(); //��������� �������� ������� ���� �����

	virtual void AfterShowEvent();
	virtual void SetWindowState();
	virtual void EscapeClicked();
	virtual void EnterClicked();
	virtual void ControlEnterClicked();
	virtual void PasteClicked();

private:
	QVBoxLayout *MainLayout;
	QLabel *LabelShadow;
	QTimer *FlashingTimer;
	bool ShowedFlag;
	ISUuid FormUID;
};
//-----------------------------------------------------------------------------
#endif
