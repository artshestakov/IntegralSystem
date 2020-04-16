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
	void ModeList(); //Включить режим ввода по справочнику
	void ModeFree(); //Включить режим ввода в свободной форме
	void SetValue(const QString &AddressText);

	void LoadRegion(); //Загрузка регионов
	void RegionChanged(const QVariant &RegionUID); //Событие изменения региона

	void LoadCity(const QVariant &RegionCode); //Загрузка городов
	void CityChanged(const QVariant &CityGUID); //Событие изменения города

	void LoadStreet(const QVariant &CityGUID); //Загрузка улиц
	void StreetChanged(const QVariant &StreetGUID); //Событие изменения улицы

	void LoadHouse(const QVariant &StreetGUID); //Загрузка домов
	void HouseChanged(const QVariant &HouseGUID); //Событие изменения дома

	int StreetCount(const ISUuid &RegionUID) const; //Получить количество улиц
	QString GetRegionCode(const ISUuid &RegionUID) const; //Получить код региона по идентификатору
	void FieldEnabled(ISAddressBox *AddressBox, bool Enabled); //Изменить видимость поля
	void UpdatePerfomance(); //Обновить представление

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
