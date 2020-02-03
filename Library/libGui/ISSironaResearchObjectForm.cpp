#include "ISSironaResearchObjectForm.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISLocalization.h"
#include "ISDatabase.h"
//-----------------------------------------------------------------------------
static QString QU_RESEARCH = PREPARE_QUERY("UPDATE research SET "
										   "resr_money = (SELECT rstp_value FROM researchtype WHERE rstp_id = :ResearchTypeID), "
										   "resr_moneydoctor = :MoneyDoctor "
										   "WHERE resr_id = :ResearchID "
										   "RETURNING resr_money, resr_moneydoctor");
//-----------------------------------------------------------------------------
ISSironaResearchObjectForm::ISSironaResearchObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	
}
//-----------------------------------------------------------------------------
ISSironaResearchObjectForm::~ISSironaResearchObjectForm()
{

}
//-----------------------------------------------------------------------------
bool ISSironaResearchObjectForm::Save()
{
	bool Result = ISObjectFormBase::Save();
	if (Result)
	{
		int DoctorID = GetFieldValue("Doctor").toInt();
		ISQuery qUpdateMoney(QU_RESEARCH);
		qUpdateMoney.BindValue(":ResearchTypeID", GetFieldValue("Type"));

		if (ISDatabase::GetInstance().GetValue("Doctors", "Bonus", DoctorID).toBool()) //Если выбранный доктор работает по бонусу
		{
			int ResearchTypeID = GetFieldValue("Type").toInt();
			double MoneyDoctor = 0;
			if (ISDatabase::GetInstance().GetValue("Doctors", "IsCenterSeven", DoctorID).toBool()) //Если выбранный доктор из центра 7
			{
				MoneyDoctor = ISDatabase::GetInstance().GetValue("ResearchType", "CenterSeven", ResearchTypeID).toDouble();
			}
			else
			{
				MoneyDoctor = ISDatabase::GetInstance().GetValue("ResearchType", "Bonus", ResearchTypeID).toDouble();
			}
			 
			qUpdateMoney.BindValue(":MoneyDoctor", MoneyDoctor);
		}
		else //Выбранный доктор работает без бонуса - обнулить поле бонуса
		{
			qUpdateMoney.BindValue(":MoneyDoctor", QVariant());
		}
		
		qUpdateMoney.BindValue(":ResearchID", GetObjectID());
		if (qUpdateMoney.ExecuteFirst())
		{
			GetFieldWidget("Money")->SetValue(qUpdateMoney.ReadColumn("resr_money").toDouble());
			GetFieldWidget("MoneyDoctor")->SetValue(qUpdateMoney.ReadColumn("resr_moneydoctor").toDouble());
			SetModificationFlag(false);
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISSironaResearchObjectForm::AfterShowEvent()
{
	ISObjectFormBase::AfterShowEvent();
	if (GetFormType() == ISNamespace::OFT_Copy) //Если форма создается для копирования существущего исследования - очистить поля денег
	{
		GetFieldWidget("Money")->Clear();
		GetFieldWidget("MoneyDoctor")->Clear();
	}
}
//-----------------------------------------------------------------------------
