#include "ISSironaResearchObjectForm.h"
#include "ISQuery.h"
#include "ISMessageBox.h"
#include "ISDatabase.h"
//-----------------------------------------------------------------------------
static QString QU_RESEARCH = PREPARE_QUERY2("UPDATE research SET "
										   "resr_money = (SELECT rstp_value FROM researchtype WHERE rstp_id = :ResearchTypeID), "
										   "resr_moneydoctor = :MoneyDoctor "
										   "WHERE resr_id = :ResearchID "
										   "RETURNING resr_money, resr_moneydoctor");
//-----------------------------------------------------------------------------
ISSironaResearchObjectForm::ISSironaResearchObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
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

		if (ISDatabase::Instance().GetValue("Doctors", "Bonus", DoctorID).toBool()) //���� ��������� ������ �������� �� ������
		{
			int ResearchTypeID = GetFieldValue("Type").toInt();
			double MoneyDoctor = 0;
			if (ISDatabase::Instance().GetValue("Doctors", "IsCenterSeven", DoctorID).toBool()) //���� ��������� ������ �� ������ 7
			{
				MoneyDoctor = ISDatabase::Instance().GetValue("ResearchType", "CenterSeven", ResearchTypeID).toDouble();
			}
			else
			{
				MoneyDoctor = ISDatabase::Instance().GetValue("ResearchType", "Bonus", ResearchTypeID).toDouble();
			}
			 
			qUpdateMoney.BindValue(":MoneyDoctor", MoneyDoctor);
		}
		else //��������� ������ �������� ��� ������ - �������� ���� ������
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
	if (GetFormType() == ISNamespace::OFT_Copy) //���� ����� ��������� ��� ����������� ������������ ������������ - �������� ���� �����
	{
		GetFieldWidget("Money")->Clear();
		GetFieldWidget("MoneyDoctor")->Clear();
	}
}
//-----------------------------------------------------------------------------
