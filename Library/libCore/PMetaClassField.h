#pragma once
#ifndef _PMETACLASSFIELD_H_INCLUDED
#define _PMETACLASSFIELD_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
#include "ISNamespace.h"
#include "PMetaClassIndex.h"
#include "PMetaClassForeign.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT PMetaClassField : public PMetaClass
{
public:
	PMetaClassField();
	~PMetaClassField();

	void SetUID(const ISUuid &uid);
	ISUuid GetUID() const;

	void SetName(const QString &name);
	QString GetName() const;

	void SetType(const QString &type);
	ISNamespace::FieldType GetType();

	void SetSize(int size);
	int GetSize() const;

	void SetUpper(bool upper);
	bool GetUpper() const;

	void SetLower(bool lower);
	bool GetLower() const;

	void SetDefaultValue(const QVariant &default_value);
	QVariant GetDefaultValue() const;

	void SetDefaultValueWidget(const QVariant &default_value_widget);
	QVariant GetDefaultValueWidget() const;

	void SetLabelName(const QString &label_name);
	QString GetLabelName() const;

	void SetLocalListName(const QString &local_list_name);
	QString GetLocalListName() const;

	void SetNotNull(bool not_null);
	bool GetNotNull() const;

	void SetReadOnly(bool read_only);
	bool GetReadOnly() const;

	void SetHideFromObject(bool HideFromObject);
	bool GetHideFromObject() const;

	void SetHideFromList(bool HideFromList);
	bool GetHideFromList() const;

	void SetNotSearch(bool NotSearch);
	bool GetNotSearch() const;

	void SetHint(const QString &Hint);
	QString GetHint() const;

	void SetPlaceholderText(const QString &PlaceholderText);
	QString GetPlaceholderText() const;

	void SetControlWidget(const QString &ControlWidget);
	QString GetControlWidget() const;

	void SetRegExp(const QString &reg_exp);
	QString GetRegExp() const;

	void SetIsSystem(bool IsSystem);
	bool GetIsSystem() const;

	void SetQueryText(const QString &QueryText);
	QString GetQueryText() const;

	void SetIndex(PMetaClassIndex *index);
	PMetaClassIndex* GetIndex();

	void SetForeign(PMetaClassForeign *foreign);
	PMetaClassForeign* GetForeign();

	void SetSequence(bool sequence);
	bool GetSequence() const;

	void SetLayoutName(const QString &layout_name);
	QString GetLayoutName() const;

	void SetSeparatorName(const QString &separator_name);
	QString GetSeparatorName() const;

	bool IsFieldID() const;
	bool IsFieldUID() const;

private:
	ISUuid UID;
	ISNamespace::FieldType Type; //���
	QString Name; //��������
	int Size; //������
	bool Upper; //������ ������� �������
	bool Lower; //������ ������ �������
	QVariant DefaultValue; //�������� �� ��������� ��� ����
	QVariant DefaultValueWidget; //�������� �� ���������
	QString LabelName; //��� ���� �� ����� �������
	QString LocalListName; //��� ���� � ������
	bool NotNull; //���� �� ������ ���� ������
	bool ReadOnly; //�������������� ���� ���������
	bool HideFromObject; //���� ������ ���� ������ � ����� �������
	bool HideFromList; //���� �� ������ ������������ � �������
	bool NotSearch; //������� ���� � ������
	QString Hint; //��������� ��� ����
	QString PlaceholderText;
	QString ControlWidget; //������������ �������-��������� ��������
	QString RegExp; //���������� ��������� ��� ����
	bool IsSystem; //������ ���������� ����

	QString QueryText; //����� ����������

	PMetaClassIndex *Index; //������
	PMetaClassForeign *Foreign; //������� ����

	bool Sequence; //������������������ ����
	QString LayoutName; //������������ ������������ (��� ��������������� ���������� ����)
	QString SeparatorName; //������������ �������
};
//-----------------------------------------------------------------------------
#endif
