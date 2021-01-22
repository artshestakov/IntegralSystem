#pragma once
#ifndef _ISFIELDEDITBASE_H_INCLUDED
#define _ISFIELDEDITBASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
//������� ����� ��� ���� ����� �������������� ����������
class ISFieldEditBase : public QWidget
{
	Q_OBJECT

signals:
	void DataChanged(); //������ ��� ��������� ������
	void ValueChange(const QVariant &Value); //������ ��� ��������� ������ ���������� ���������� ��������

public:
	ISFieldEditBase(QWidget *parent);
	virtual ~ISFieldEditBase();

	virtual void SetValue(const QVariant &value) = 0; //�������� ��������
	virtual QVariant GetValue() const = 0; //�������� ��������
	virtual void Clear() = 0; //������� ��������

	virtual void Invoke();
	virtual bool IsValid() const; //�������� ������������ ����
	virtual void SetFont(const QFont &Font);
	virtual void SetReadOnly(bool read_only);
	virtual void SetEnabled(bool Enabled);
	virtual void SetPlaceholderText(const QString &placeholder_text);
	virtual void SetFrameShape(QFrame::Shape FrameShape);
	virtual void SetRegExp(const QString &RegExp);
	virtual void SetColorText(const QColor &Color);
	virtual void SetSizePolicy(QSizePolicy::Policy PolicyHorizontal, QSizePolicy::Policy PolicyVertical);
	virtual void SetSizePolicyHorizontal(QSizePolicy::Policy PolicyHorizontal); //�������� �������� ���������� �� �����������
	virtual void SetSizePolicyVertical(QSizePolicy::Policy PolicyVertical); //�������� �������� ���������� �� �����������
	virtual void SetVisibleClear(bool Visible);

	bool IsReadOnly() const;
	
	void SetCursor(const QCursor &Cursor); //�������� ������
	void SetToolTip(const QString &ToolTip); //�������� ����������� ���������-���������
	void SetFixedWidth(int Width); //�������� ������������ ������ ������� ��������������

	void BlinkRed(); //���������� ���� ������� ������
	void Blink(const QColor &Color);

	void SetFocus(); //���������� ����� �� ������ ��������������

	void SetModificationFlag(bool modification_flag); //�������� ���� ��������� ��������
	bool GetModificationFlag() const; //�������� ���� ��������� ��������

	void CreateHint(const QString &Hint); //������� �������� (���� �������, ��������� ����� ���������)

protected:
	virtual void paintEvent(QPaintEvent *PaintEvent);
	void ShowHint(); //�������� ���������
	virtual void ValueChanged();
	
	void AddWidgetEdit(QWidget *edit_widget, QWidget *field_edit_pointer); //�������� ������ ���������
	void AddWidgetEditStretch(); //�������� ���������� � ����������� ������� ��������������
	void AddMainLayoutStretch(); //�������� ���������� � ������� ����������� ����
	void AddWidgetToLeft(QWidget *widget); //�������� ������ � ����� �����������
	void AddWidgetToRight(QWidget *widget); //�������� ������ � ������ �����������
	
	void CreateButtonClear(); //�������� ������ �������

private:
	QHBoxLayout *MainLayout;
	QToolButton *ButtonHint;
	QHBoxLayout *LayoutLeft;
	QHBoxLayout *LayoutEdit;
	QHBoxLayout *LayoutRight;
	ISServiceButton *ButtonClear;
	
	QWidget *EditWidget;
	QWidget *FieldEditPointer;

	bool ModificationFlag;
	bool IsBlinkBorder;
	QColor BlinkColor;

	bool ReadOnly;
};
//-----------------------------------------------------------------------------
#endif
