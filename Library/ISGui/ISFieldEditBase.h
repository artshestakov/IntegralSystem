#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISButtonClear.h"
//-----------------------------------------------------------------------------
//������� ����� ��� ���� ����� �������������� ����������
class ISGUI_EXPORT ISFieldEditBase : public QWidget
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
	virtual void SetVisibleClear(bool Visible) = 0;

	virtual void Invoke();
	virtual bool IsValid() const; //�������� ������������ ����
	virtual void SetFont(const QFont &Font);
	virtual void SetReadOnly(bool read_only);
	virtual void SetPlaceholderText(const QString &placeholder_text);
	virtual void SetRegExp(const QString &RegExp);
	
	void SetCursor(const QCursor &Cursor); //�������� ������
	void SetToolTip(const QString &ToolTip); //�������� ����������� ���������-���������
	void SetFixedWidth(int Width); //�������� ������������ ������ ������� ��������������
	void SetSizePolicyHorizontal(QSizePolicy::Policy PolicyHorizontal); //�������� �������� ���������� �� �����������
	void SetSizePolicyVertical(QSizePolicy::Policy PolicyVertical); //�������� �������� ���������� �� �����������

	void BlinkRed(); //���������� ���� ������� ������
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
	ISButtonClear* GetButtonClear();

private:
	QHBoxLayout *MainLayout;
	QToolButton *ButtonHint;
	QHBoxLayout *LayoutLeft;
	QHBoxLayout *LayoutEdit;
	QHBoxLayout *LayoutRight;
	ISButtonClear *ButtonClear;
	
	QWidget *EditWidget;
	QWidget *FieldEditPointer;

	bool ModificationFlag;
	bool BorderRed;
};
//-----------------------------------------------------------------------------
