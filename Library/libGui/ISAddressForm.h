#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISAddressBox.h"
#include "ISEdits.h"
//-----------------------------------------------------------------------------
class ISAddressForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISAddressForm(const QString &address_string);
	virtual ~ISAddressForm();

	QString GetString() const;

protected:
	void ControlEnterClicked() override;
	void Apply();
	void ModeList(); //�������� ����� ����� �� �����������
	void ModeFree(); //�������� ����� ����� � ��������� �����
	void SetValue(const QString &AddressText);

	void LoadRegion(); //�������� ��������
	void RegionChanged(const QVariant &RegionUID); //������� ��������� �������

	void LoadCity(const QVariant &RegionCode); //�������� �������
	void CityChanged(const QVariant &CityGUID); //������� ��������� ������

	void LoadStreet(const QVariant &CityGUID); //�������� ����
	void StreetChanged(const QVariant &StreetGUID); //������� ��������� �����

	void LoadHouse(const QVariant &StreetGUID); //�������� �����
	void HouseChanged(const QVariant &HouseGUID); //������� ��������� ����

	int StreetCount(const ISUuid &RegionUID) const; //�������� ���������� ����
	QString GetRegionCode(const ISUuid &RegionUID) const; //�������� ��� ������� �� ��������������
	void FieldEnabled(ISAddressBox *AddressBox, bool Enabled); //�������� ��������� ����
	void UpdatePerfomance(); //�������� �������������

	void SetRegion(const QString &Region);
	void SetCity(const QString &City);
	void SetStreet(const QString &Street);
	void SetHouse(const QString &House);

private:
	ISAddressBox *ComboRegion;
	ISAddressBox *ComboCity;
	ISAddressBox *ComboStreet;
	ISAddressBox *ComboHouse;

	QGroupBox *GroupBox;
	ISQLineEdit *LineEdit;
	QRadioButton *RadioList;
	QRadioButton *RadioFree;
};
//-----------------------------------------------------------------------------
