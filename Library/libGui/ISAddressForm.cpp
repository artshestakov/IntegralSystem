#include "ISAddressForm.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISComboEdit.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "ISButtonClose.h"
#include "ISGui.h"
//-----------------------------------------------------------------------------
static QString QS_REGION = PREPARE_QUERY("SELECT fobj_offname, fobj_aoguid, "
										 "(SELECT fscb_socrname FROM _fias_addressobjecttype WHERE fscb_level = fobj_aolevel AND fscb_scname = fobj_shortname) "
										 "FROM _fias_object "
										 "WHERE fobj_actstatus = 1 "
										 "AND fobj_livestatus = '1' "
										 "AND fobj_aolevel = 1 "
										 "ORDER BY fobj_offname");
//-----------------------------------------------------------------------------
static QString QS_CITY = PREPARE_QUERY("SELECT fobj_offname, fobj_aoguid "
									   "FROM _fias_object "
									   "WHERE fobj_actstatus = 1 "
									   "AND fobj_livestatus = '1' "
									   "AND fobj_aolevel = 4 "
									   "AND fobj_regioncode = :RegionCode "
									   "ORDER BY fobj_offname");
//-----------------------------------------------------------------------------
static QString QS_STREET = PREPARE_QUERY("SELECT fobj_offname, fobj_shortname, fobj_aoguid "
										 "FROM _fias_object "
										 "WHERE fobj_actstatus = 1 "
										 "AND fobj_livestatus = '1' "
										 "AND fobj_aolevel = 7 "
										 "AND fobj_parentguid = :CityGUID "
										 "ORDER BY fobj_offname");
//-----------------------------------------------------------------------------
static QString QS_HOUSE = PREPARE_QUERY("SELECT DISTINCT ON(fhos_housenum) fhos_housenum, fhos_houseguid "
										"FROM _fias_house "
										"WHERE fhos_aoguid = :StreetGUID "
										"ORDER BY fhos_housenum");
//-----------------------------------------------------------------------------
static QString QS_STREET_COUNT = PREPARE_QUERY("SELECT COUNT(*) "
											   "FROM _fias_object "
											   "WHERE fobj_actstatus = 1 "
											   "AND fobj_livestatus = '1' "
											   "AND fobj_aolevel = 7 "
											   "AND fobj_parentguid = :RegionUID");
//-----------------------------------------------------------------------------
static QString QS_REGION_CODE = PREPARE_QUERY("SELECT fobj_regioncode "
											  "FROM _fias_object "
											  "WHERE fobj_aoguid = :RegionUID");
//-----------------------------------------------------------------------------
ISAddressForm::ISAddressForm(const QString &AddressString, QWidget *parent) : ISInterfaceDialogForm(parent)
{
	setWindowTitle(LANG("Address"));
	setWindowIcon(BUFFER_ICONS("Address"));
	GetMainLayout()->setContentsMargins(MARGINS_LAYOUT_10_PX);
	setFixedSize(530, 230);

	QFormLayout *FormLayout = new QFormLayout();
	GetMainLayout()->addLayout(FormLayout);

	ComboRegion = new ISAddressBox(this);
	ComboRegion->lineEdit()->setPlaceholderText(LANG("Address.Region.PlaceholderText"));
	connect(ComboRegion, &ISAddressBox::ChangeValue, this, &ISAddressForm::RegionChanged);
	connect(ComboRegion, &ISAddressBox::ChangeValue, this, &ISAddressForm::UpdatePerfomance);
	FormLayout->addRow(LANG("Address.Region") + ':', ComboRegion);

	ComboCity = new ISAddressBox(this);
	ComboCity->lineEdit()->setPlaceholderText(LANG("Address.City.PlaceholderText"));
	connect(ComboCity, &ISAddressBox::ChangeValue, this, &ISAddressForm::CityChanged);
	FormLayout->addRow(LANG("Address.City") + ':', ComboCity);

	ComboStreet = new ISAddressBox(this);
	ComboStreet->lineEdit()->setPlaceholderText(LANG("Address.Street.PlaceholderText"));
	connect(ComboStreet, &ISAddressBox::ChangeValue, this, &ISAddressForm::StreetChanged);
	FormLayout->addRow(LANG("Address.Street") + ':', ComboStreet);

	ComboHouse = new ISAddressBox(this);
	ComboHouse->lineEdit()->setPlaceholderText(LANG("Address.House.PlaceholderText"));
	connect(ComboHouse, &ISAddressBox::ChangeValue, this, &ISAddressForm::HouseChanged);
	FormLayout->addRow(LANG("Address.House") + ':', ComboHouse);

	QHBoxLayout *LayoutGroupBox = new QHBoxLayout();
	LayoutGroupBox->setContentsMargins(MARGINS_LAYOUT_2_PX);

	GroupBox = new QGroupBox(this);
	GroupBox->setTitle(LANG("Address.PerformanceAddress"));
	GroupBox->setLayout(LayoutGroupBox);
	GetMainLayout()->addWidget(GroupBox);

	GetMainLayout()->addStretch();

	LineEdit = new ISQLineEdit(GroupBox);
	LineEdit->setReadOnly(false);
	LineEdit->setFrame(false);
	LineEdit->SetVisibleClear(false);
	LineEdit->setStyleSheet(QString());
	GroupBox->layout()->addWidget(LineEdit);

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	LayoutBottom->setContentsMargins(MARGINS_LAYOUT_NULL);
	GetMainLayout()->addLayout(LayoutBottom);

	QLabel *LabelInput = new QLabel(this);
	LabelInput->setText(LANG("Address.Mode") + ':');
	LayoutBottom->addWidget(LabelInput);

	RadioList = new QRadioButton(this);
	RadioList->setText(LANG("Address.Mode.List"));
	RadioList->setShortcut(Qt::Key_F1);
	connect(RadioList, &QRadioButton::clicked, this, &ISAddressForm::ModeList);
	LayoutBottom->addWidget(RadioList);

	RadioFree = new QRadioButton(this);
	RadioFree->setText(LANG("Address.Mode.Free"));
	RadioFree->setShortcut(Qt::Key_F2);
	connect(RadioFree, &QRadioButton::clicked, this, &ISAddressForm::ModeFree);
	LayoutBottom->addWidget(RadioFree);

	LayoutBottom->addStretch();

	ISPushButton *ButtonApply = new ISPushButton(this);
	ButtonApply->setText(LANG("Address.Select"));
	ButtonApply->setIcon(BUFFER_ICONS("Apply.Blue"));
	connect(ButtonApply, &ISPushButton::clicked, this, &ISAddressForm::Apply);
	LayoutBottom->addWidget(ButtonApply);

	ISButtonClose *ButtonClose = new ISButtonClose(this);
	connect(ButtonClose, &ISButtonClose::clicked, this, &ISAddressForm::close);
	LayoutBottom->addWidget(ButtonClose);

	if (AddressString.length())
	{
		if (ISSystem::AddressIsList(AddressString)) //Если адрес был заполнен через справочник
		{
			ModeList();
			SetValue(AddressString);
		}
		else //Адрес был заполнен вручную
		{
			ModeFree();
			LineEdit->setText(AddressString);
		}
	}
	else
	{
		ModeList();
	}
}
//-----------------------------------------------------------------------------
ISAddressForm::~ISAddressForm()
{

}
//-----------------------------------------------------------------------------
QString ISAddressForm::GetString() const
{
	return LineEdit->text();
}
//-----------------------------------------------------------------------------
void ISAddressForm::ControlEnterClicked()
{
	Apply();
}
//-----------------------------------------------------------------------------
void ISAddressForm::Apply()
{
	SetResult(true);
	close();
}
//-----------------------------------------------------------------------------
void ISAddressForm::ModeList()
{
	LoadRegion();
	
	ComboRegion->setEnabled(true);
	ComboCity->setEnabled(false);
	ComboStreet->setEnabled(false);
	ComboHouse->setEnabled(false);
	GroupBox->setTitle(LANG("Address.PerformanceAddress"));
	LineEdit->setReadOnly(true);
	LineEdit->setFrame(false);
	LineEdit->clear();
	RadioList->setChecked(true);
	
	ComboRegion->setFocus();
}
//-----------------------------------------------------------------------------
void ISAddressForm::ModeFree()
{
	ComboRegion->Clear();

	ComboRegion->setEnabled(false);
	ComboCity->setEnabled(false);
	ComboStreet->setEnabled(false);
	ComboHouse->setEnabled(false);
	GroupBox->setTitle(LANG("Address.AddressInFreeForm"));
	LineEdit->setReadOnly(false);
	LineEdit->setFrame(true);
	LineEdit->clear();
	RadioFree->setChecked(true);
	
	LineEdit->setFocus();
}
//-----------------------------------------------------------------------------
void ISAddressForm::SetValue(const QString &AddressText)
{
	QStringList StringList = AddressText.split(SYMBOL_FIAS_SPLIT);
	StringList.removeOne(QString());
	if (0 < StringList.count())
	{
		QString Region = StringList.at(0);
		SetRegion(Region);
	}

	if (1 < StringList.count())
	{
		QString CityOrStreet = StringList.at(1);
		if (ComboCity->isEnabled())
		{
			SetCity(CityOrStreet);
		}
		else
		{
			SetStreet(CityOrStreet);
		}
	}

	if (2 < StringList.count())
	{
		QString Street = StringList.at(2);
		SetStreet(Street);
	}

	if (3 < StringList.count())
	{
		QString House = StringList.at(3);
		SetHouse(House);
	}
}
//-----------------------------------------------------------------------------
void ISAddressForm::LoadRegion()
{
	ISQuery qSelectRegion(QS_REGION);
	if (qSelectRegion.Execute())
	{
		while (qSelectRegion.Next()) //Обход всех регионов
		{
			QString RegionName = qSelectRegion.ReadColumn("fobj_offname").toString();
			ISUuid RegionUID = ISUuid(qSelectRegion.ReadColumn("fobj_aoguid"));
			QString ObjectType = qSelectRegion.ReadColumn("fscb_socrname").toString();
			ComboRegion->addItem(RegionName + SYMBOL_SPACE + ObjectType.toLower(), RegionUID);
		}
	}

	ComboRegion->clearEditText();
}
//-----------------------------------------------------------------------------
void ISAddressForm::RegionChanged(const QVariant &RegionUID)
{
	ISGui::SetWaitGlobalCursor(true);

	if (RegionUID.isValid())
	{
		if (StreetCount(RegionUID)) //Если по региону есть улицы (такие регионы как Москва, Санкт-Петербург, Байконур и т.д.)
		{
			FieldEnabled(ComboCity, false);
			FieldEnabled(ComboStreet, true);
			LoadStreet(RegionUID);
		}
		else //Регион не является городом
		{
			FieldEnabled(ComboCity, true);
			LoadCity(GetRegionCode(RegionUID));
		}
	}
	else
	{
		FieldEnabled(ComboStreet, false);
		FieldEnabled(ComboCity, false);
	}

	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISAddressForm::LoadCity(const QVariant &RegionCode)
{
	ComboCity->Clear();
	ISQuery qSelectCity(QS_CITY);
	qSelectCity.BindValue(":RegionCode", RegionCode);
	if (qSelectCity.Execute())
	{
		while (qSelectCity.Next())
		{
			QString CityName = qSelectCity.ReadColumn("fobj_offname").toString();
			ISUuid CityUID = qSelectCity.ReadColumn("fobj_aoguid");
			ComboCity->addItem(CityName, CityUID);
		}
	}
	ComboCity->clearEditText();
	ComboCity->setCurrentIndex(-1);
}
//-----------------------------------------------------------------------------
void ISAddressForm::CityChanged(const QVariant &CityGUID)
{
	ISGui::SetWaitGlobalCursor(true);

	if (CityGUID.isValid())
	{
		FieldEnabled(ComboStreet, true);
		LoadStreet(CityGUID);
	}
	else
	{
		FieldEnabled(ComboStreet, false);
	}

	UpdatePerfomance();
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISAddressForm::LoadStreet(const QVariant &CityGUID)
{
	ComboStreet->Clear();
	ISQuery qSelectCity(QS_STREET);
	qSelectCity.BindValue(":CityGUID", CityGUID);
	if (qSelectCity.Execute())
	{
		while (qSelectCity.Next())
		{
			QString StreetName = qSelectCity.ReadColumn("fobj_offname").toString();
			QString ShortName = qSelectCity.ReadColumn("fobj_shortname").toString();
			ISUuid StreetGUID = qSelectCity.ReadColumn("fobj_aoguid");
			ComboStreet->addItem(StreetName + SYMBOL_SPACE + ShortName, StreetGUID);
		}
	}
	ComboStreet->clearEditText();
	ComboStreet->setCurrentIndex(-1);
}
//-----------------------------------------------------------------------------
void ISAddressForm::StreetChanged(const QVariant &StreetGUID)
{
	ISGui::SetWaitGlobalCursor(true);

	if (StreetGUID.isValid())
	{
		FieldEnabled(ComboHouse, true);
		LoadHouse(StreetGUID);
	}
	else
	{
		FieldEnabled(ComboHouse, false);
	}

	UpdatePerfomance();
	ISGui::SetWaitGlobalCursor(false);
}
//-----------------------------------------------------------------------------
void ISAddressForm::LoadHouse(const QVariant &StreetGUID)
{
	ComboHouse->Clear();
	ISQuery qSelectHouse(QS_HOUSE);
	qSelectHouse.BindValue(":StreetGUID", StreetGUID);
	if (qSelectHouse.Execute())
	{
		while (qSelectHouse.Next())
		{
			QString HouseNum = qSelectHouse.ReadColumn("fhos_housenum").toString();
			ISUuid HouseUID = qSelectHouse.ReadColumn("fhos_houseguid").toString();
			ComboHouse->addItem(HouseNum, HouseUID);
		}
	}
	ComboHouse->clearEditText();
	ComboHouse->setCurrentIndex(-1);
}
//-----------------------------------------------------------------------------
void ISAddressForm::HouseChanged(const QVariant &HouseGUID)
{
	UpdatePerfomance();
}
//-----------------------------------------------------------------------------
int ISAddressForm::StreetCount(const ISUuid &RegionUID) const
{
	int Result = 0;
	ISQuery qSelect(QS_STREET_COUNT);
	qSelect.BindValue(":RegionUID", RegionUID);
	if (qSelect.ExecuteFirst())
	{
		Result = qSelect.ReadColumn("count").toInt();
	}

	return Result;
}
//-----------------------------------------------------------------------------
QString ISAddressForm::GetRegionCode(const ISUuid &RegionUID) const
{
	QString RegionCode;
	ISQuery qSelect(QS_REGION_CODE);
	qSelect.BindValue(":RegionUID", RegionUID);
	if (qSelect.ExecuteFirst())
	{
		RegionCode = qSelect.ReadColumn("fobj_regioncode").toString();
	}

	return RegionCode;
}
//-----------------------------------------------------------------------------
void ISAddressForm::FieldEnabled(ISAddressBox *AddressBox, bool Enabled)
{
	AddressBox->setEnabled(Enabled);
	if (Enabled)
	{
		AddressBox->setFocus();
	}
	else
	{
		AddressBox->Clear();
	}
}
//-----------------------------------------------------------------------------
void ISAddressForm::UpdatePerfomance()
{
	QString String = ComboRegion->currentText() + SYMBOL_FIAS_SPLIT;

	if (ComboCity->currentText().length())
	{
		String += ComboCity->currentText() + SYMBOL_FIAS_SPLIT;
	}

	if (ComboStreet->currentText().length())
	{
		String += ComboStreet->currentText() + SYMBOL_FIAS_SPLIT;
	}

	if (ComboHouse->currentText().length())
	{
		String += ComboHouse->currentText() + SYMBOL_FIAS_SPLIT;
	}

	ISSystem::RemoveLastSymbolFromString(String, 2);
	LineEdit->setText(String);
}
//-----------------------------------------------------------------------------
void ISAddressForm::SetRegion(const QString &Region)
{
	for (int i = 0; i < ComboRegion->count(); ++i)
	{
		if (ComboRegion->itemText(i) == Region)
		{
			ComboRegion->setCurrentIndex(i);
			RegionChanged(ComboRegion->currentData());
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void ISAddressForm::SetCity(const QString &City)
{
	for (int i = 0; i < ComboCity->count(); ++i)
	{
		if (ComboCity->itemText(i) == City)
		{
			ComboCity->setCurrentIndex(i);
			CityChanged(ComboCity->currentData());
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void ISAddressForm::SetStreet(const QString &Street)
{
	for (int i = 0; i < ComboStreet->count(); ++i)
	{
		if (ComboStreet->itemText(i) == Street)
		{
			ComboStreet->setCurrentIndex(i);
			StreetChanged(ComboStreet->currentData());
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void ISAddressForm::SetHouse(const QString &House)
{
	for (int i = 0; i < ComboHouse->count(); ++i)
	{
		if (ComboHouse->itemText(i) == House)
		{
			ComboHouse->setCurrentIndex(i);
			HouseChanged(ComboHouse->currentData());
			break;
		}
	}
}
//-----------------------------------------------------------------------------
