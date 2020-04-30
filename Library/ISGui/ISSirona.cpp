#include "ISSirona.h"
#include "ISQuery.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISGui.h"
#include "ISBuffer.h"
#include "ISDefinesGui.h"
#include "ISDatabase.h"
#include "ISFileDialog.h"
//-----------------------------------------------------------------------------
static QString QU_ADMISSION_FINISHED = PREPARE_QUERY2("UPDATE appointment SET "
													 "apnm_finished = true "
													 "WHERE apnm_id = :ObjectID");
//-----------------------------------------------------------------------------
static QString QS_DOCTORS = PREPARE_QUERY2("SELECT dctr_id, dctr_fio, dctr_bonus "
										  "FROM doctors "
										  "WHERE NOT dctr_isdeleted "
										  "ORDER BY dctr_fio");
//-----------------------------------------------------------------------------
static QString QS_DOCTOR_BONUS = PREPARE_QUERY2("SELECT SUM(resr_moneydoctor) "
											   "FROM research "
											   "WHERE NOT resr_isdeleted "
											   "AND resr_doctor = :DoctorID "
											   "AND resr_date BETWEEN :DateBegin AND :DateEnd");
//-----------------------------------------------------------------------------
static QString QS_RESEARCH = PREPARE_QUERY2("SELECT pmtp_uid, resr_money "
										   "FROM research "
										   "LEFT JOIN paymenttype ON pmtp_id = resr_payment "
										   "WHERE resr_id IN(:IN)");
//-----------------------------------------------------------------------------
static QString QU_RESEARCH = PREPARE_QUERY2("UPDATE research SET "
										   "resr_money = (SELECT rstp_value FROM researchtype WHERE rstp_id = :ResearchTypeID), "
										   "resr_moneydoctor = :MoneyDoctor "
										   "WHERE resr_id = :ResearchID "
										   "RETURNING resr_money, resr_moneydoctor");
//-----------------------------------------------------------------------------
ISSirona::Object::Object() : ISObjectInterface()
{

}
//-----------------------------------------------------------------------------
ISSirona::Object::~Object()
{

}
//-----------------------------------------------------------------------------
void ISSirona::Object::RegisterMetaTypes() const
{
	qRegisterMetaType<ISSirona::AppointmentListForm*>("ISSirona::AppointmentListForm");
	qRegisterMetaType<ISSirona::ResearchListForm*>("ISSirona::ResearchListForm");
	qRegisterMetaType<ISSirona::ResearchObjectForm*>("ISSirona::ResearchObjectForm");
	qRegisterMetaType<ISSirona::ResearchTypeObjectForm*>("ISSirona::ResearchTypeObjectForm");
	qRegisterMetaType<ISSirona::ReportForm*>("ISSirona::ReportForm");
	qRegisterMetaType<ISSirona::CostListForm*>("ISSirona::CostListForm");
	qRegisterMetaType<ISSirona::CostObjectForm*>("ISSirona::CostObjectForm");
}
//-----------------------------------------------------------------------------
void ISSirona::Object::BeforeShowMainWindow() const
{

}
//-----------------------------------------------------------------------------
void ISSirona::Object::InitializePlugin() const
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISSirona::AppointmentListForm::AppointmentListForm(QWidget *parent) : ISListBaseForm("Appointment", parent)
{
	QAction *ActionFinished = new QAction(this);
	ActionFinished->setText(LANG("Sirona.AdmissionFinished"));
	ActionFinished->setToolTip(LANG("Sirona.AdmissionFinished"));
	ActionFinished->setIcon(BUFFER_ICONS("Apply.Blue"));
	connect(ActionFinished, &QAction::triggered, this, &AppointmentListForm::AdmissionFinished);
	AddAction(ActionFinished, true, true);

	RadioAll = new QRadioButton(this);
	RadioAll->setText(LANG("Sirona.Admission.Filter.All"));
	GetToolBar()->addWidget(RadioAll);

	RadioActual = new QRadioButton(this);
	RadioActual->setChecked(true);
	RadioActual->setText(LANG("Sirona.Admission.Filter.Actial"));
	GetToolBar()->addWidget(RadioActual);

	RadioFinished = new QRadioButton(this);
	RadioFinished->setText(LANG("Sirona.Admission.Filter.Finished"));
	GetToolBar()->addWidget(RadioFinished);

	QButtonGroup *ButtonGroup = new QButtonGroup(this);
	ButtonGroup->addButton(RadioAll);
	ButtonGroup->addButton(RadioActual);
	ButtonGroup->addButton(RadioFinished);
	connect(ButtonGroup, static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), this, &AppointmentListForm::FilterChange);
}
//-----------------------------------------------------------------------------
ISSirona::AppointmentListForm::~AppointmentListForm()
{

}
//-----------------------------------------------------------------------------
void ISSirona::AppointmentListForm::AdmissionFinished()
{
	QString PatientName = GetCurrentRecordValue("Patient").toString();
	bool Finished = GetCurrentRecordValue("Finished").toBool();
	if (Finished) //Если приём уже окончен
	{
		ISMessageBox::ShowInformation(this, LANG("Sirona.Message.Information.AdmissionAlreadyFinished").arg(PatientName));
		return;
	}

	if (ISMessageBox::ShowQuestion(this, LANG("Sirona.Message.Question.AdmissionFinished").arg(PatientName).arg(GetCurrentRecordValue("DateTime").toDate().toString(FORMAT_DATE_V2))))
	{
		ISQuery qUpdate(QU_ADMISSION_FINISHED);
		qUpdate.BindValue(":ObjectID", GetObjectID());
		if (qUpdate.Execute())
		{
			Update();
		}
	}
}
//-----------------------------------------------------------------------------
void ISSirona::AppointmentListForm::FilterChange(QAbstractButton *AbstractButton)
{
	if (AbstractButton == RadioAll)
	{
		GetQueryModel()->ClearClassFilter();
	}
	else if (AbstractButton == RadioActual)
	{
		GetQueryModel()->SetClassFilter("NOT apnm_finished");
	}
	else if (AbstractButton == RadioFinished)
	{
		GetQueryModel()->SetClassFilter("apnm_finished");
	}

	Update();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISSirona::CostListForm::CostListForm(QWidget *parent) : ISListBaseForm("Cost", parent)
{
	LabelSum = new QLabel(this);
	LabelSum->setFont(ISDefines::Gui::FONT_TAHOMA_11);
	LabelSum->setText(LANG("Sirona.TotalMoney.Cost").arg(0));
	GetStatusBar()->addWidget(LabelSum);
}
//-----------------------------------------------------------------------------
ISSirona::CostListForm::~CostListForm()
{

}
//-----------------------------------------------------------------------------
void ISSirona::CostListForm::LoadDataAfterEvent()
{
	ISListBaseForm::LoadDataAfterEvent();

	if (GetSqlModel()->rowCount())
	{
		ISVectorInt VectorInt = GetIDs();
		QString SqlIN;
		for (int ID : VectorInt)
		{
			SqlIN += QString::number(ID) + ", ";
		}
		SqlIN.chop(2);

		ISQuery qSelect("SELECT SUM(cost_sum) FROM cost WHERE cost_id IN(" + SqlIN + ')');
		if (qSelect.ExecuteFirst())
		{
			LabelSum->setText(LANG("Sirona.TotalMoney.Cost").arg(qSelect.ReadColumn("sum").toDouble()));
		}
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISSirona::CostObjectForm::CostObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{

}
//-----------------------------------------------------------------------------
ISSirona::CostObjectForm::~CostObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISSirona::CostObjectForm::AfterShowEvent()
{
	ISObjectFormBase::AfterShowEvent();

	if (GetFormType() == ISNamespace::OFT_Edit)
	{
		GetFieldWidget("Sum")->SetReadOnly(true);
	}
}
//-----------------------------------------------------------------------------
bool ISSirona::CostObjectForm::Save()
{
	bool Result = ISObjectFormBase::Save();
	if (Result && GetFormType() != ISNamespace::OFT_Edit)
	{
		GetFieldWidget("Sum")->SetReadOnly(true);
	}

	return Result;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISSirona::ResearchListForm::ResearchListForm(QWidget *parent) : ISListBaseForm("Research", parent)
{
	LabelSummary = new QLabel(this);
	LabelSummary->setFont(ISDefines::Gui::FONT_TAHOMA_11);
	LabelSummary->setText(LANG("Sirona.TotalMoney.Research").arg(0).arg(0).arg(0));
	GetStatusBar()->addWidget(LabelSummary);
}
//-----------------------------------------------------------------------------
ISSirona::ResearchListForm::~ResearchListForm()
{

}
//-----------------------------------------------------------------------------
void ISSirona::ResearchListForm::LoadDataAfterEvent()
{
	ISListBaseForm::LoadDataAfterEvent();
	double Cash = 0;
	double NonCash = 0;

	if (GetSqlModel()->rowCount())
	{
		ISVectorInt VectorInt = GetIDs();
		QString SqlIN;
		for (int ID : VectorInt)
		{
			SqlIN += QString::number(ID) + ", ";
		}
		SqlIN.chop(2);

		QString SqlText = QS_RESEARCH;
		ISQuery qSelect(SqlText.replace(":IN", SqlIN));
		if (qSelect.Execute())
		{
			while (qSelect.Next())
			{
				ISUuid PaymentUID = qSelect.ReadColumn("pmtp_uid");
				double Money = qSelect.ReadColumn("resr_money").toDouble();

				if (PaymentUID == "{1D7CED21-735E-4DF9-99B9-3CA30990AFC9}") //Наличные
				{
					Cash += Money;
				}
				else if (PaymentUID == "{6C5C84DD-0BF6-4AD7-8D62-441D8CD90AEF}") //Безналичные
				{
					NonCash += Money;
				}
			}
		}
	}

	LabelSummary->setText(LANG("Sirona.TotalMoney.Research").arg(Cash).arg(NonCash).arg(Cash + NonCash));
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISSirona::ResearchObjectForm::ResearchObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{

}
//-----------------------------------------------------------------------------
ISSirona::ResearchObjectForm::~ResearchObjectForm()
{

}
//-----------------------------------------------------------------------------
bool ISSirona::ResearchObjectForm::Save()
{
	bool Result = ISObjectFormBase::Save();
	if (Result)
	{
		int DoctorID = GetFieldValue("Doctor").toInt();
		ISQuery qUpdateMoney(QU_RESEARCH);
		qUpdateMoney.BindValue(":ResearchTypeID", GetFieldValue("Type"));

		if (ISDatabase::Instance().GetValue("Doctors", "Bonus", DoctorID).toBool()) //Если выбранный доктор работает по бонусу
		{
			int ResearchTypeID = GetFieldValue("Type").toInt();
			double MoneyDoctor = 0;
			if (ISDatabase::Instance().GetValue("Doctors", "IsCenterSeven", DoctorID).toBool()) //Если выбранный доктор из центра 7
			{
				MoneyDoctor = ISDatabase::Instance().GetValue("ResearchType", "CenterSeven", ResearchTypeID).toDouble();
			}
			else
			{
				MoneyDoctor = ISDatabase::Instance().GetValue("ResearchType", "Bonus", ResearchTypeID).toDouble();
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
void ISSirona::ResearchObjectForm::AfterShowEvent()
{
	ISObjectFormBase::AfterShowEvent();
	if (GetFormType() == ISNamespace::OFT_Copy) //Если форма создается для копирования существущего исследования - очистить поля денег
	{
		GetFieldWidget("Money")->Clear();
		GetFieldWidget("MoneyDoctor")->Clear();
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISSirona::ResearchTypeObjectForm::ResearchTypeObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id) : ISObjectFormBase(form_type, meta_table, parent, object_id)
{
	connect(GetFieldWidget("Bonus"), &ISFieldEditBase::ValueChange, this, &ResearchTypeObjectForm::BonusChanged);
}
//-----------------------------------------------------------------------------
ISSirona::ResearchTypeObjectForm::~ResearchTypeObjectForm()
{

}
//-----------------------------------------------------------------------------
void ISSirona::ResearchTypeObjectForm::BonusChanged(const QVariant &value)
{
	if (value.isValid())
	{
		double Bonus = value.toDouble();
		double CenterSeven = Bonus / 2;
		GetFieldWidget("CenterSeven")->SetValue(CenterSeven);
	}
	else
	{
		GetFieldWidget("CenterSeven")->Clear();
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISSirona::ReportForm::ReportForm(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_5_PX);

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	RangeDateEdit = new ISRangeDateEdit(this);
	LayoutTitle->addWidget(RangeDateEdit);

	ISPushButton *ButtonFilter = new ISPushButton(this);
	ButtonFilter->setText(LANG("Apply"));
	connect(ButtonFilter, &ISPushButton::clicked, this, &ReportForm::LoadData);
	LayoutTitle->addWidget(ButtonFilter);

	LayoutTitle->addStretch();

	ISPushButton *ButtonPrint = new ISPushButton(this);
	ButtonPrint->setText(LANG("Sirona.OutputList"));
	ButtonPrint->setIcon(BUFFER_ICONS("Print"));
	connect(ButtonPrint, &ISPushButton::clicked, this, &ReportForm::OutputList);
	LayoutTitle->addWidget(ButtonPrint);

	QStringList TreeHeader;
	TreeHeader.append(LANG("Sirona.Doctor"));
	TreeHeader.append(LANG("Sirona.Bonus"));

	TreeWidget = new QTreeWidget(this);
	TreeWidget->setRootIsDecorated(false);
	TreeWidget->setAlternatingRowColors(true);
	TreeWidget->setHeaderLabels(TreeHeader);
	GetMainLayout()->addWidget(TreeWidget);
}
//-----------------------------------------------------------------------------
ISSirona::ReportForm::~ReportForm()
{

}
//-----------------------------------------------------------------------------
void ISSirona::ReportForm::LoadData()
{
	while (TreeWidget->topLevelItemCount())
	{
		QTreeWidgetItem *TreeWidgetItem = TreeWidget->takeTopLevelItem(0);
		delete TreeWidgetItem;
		TreeWidgetItem = nullptr;
	}

	int RowID = 1;
	ISQuery qSelect(QS_DOCTORS);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			int DoctorID = qSelect.ReadColumn("dctr_id").toInt();
			QString DoctorFIO = qSelect.ReadColumn("dctr_fio").toString();
			bool DoctorBonus = qSelect.ReadColumn("dctr_bonus").toBool();

			QTreeWidgetItem *TreeWidgetItem = new QTreeWidgetItem(TreeWidget);
			TreeWidgetItem->setText(0, QString::number(RowID) + ") " + DoctorFIO);
			TreeWidgetItem->setSizeHint(0, QSize(TreeWidgetItem->sizeHint(0).width(), 30));
			TreeWidgetItem->setFont(1, ISDefines::Gui::FONT_APPLICATION_BOLD);

			if (DoctorBonus)
			{
				TreeWidgetItem->setText(1, LANG("Sirona.DoctorBonus").arg(GetDoctorBonus(DoctorID)));
			}
			else
			{
				TreeWidgetItem->setText(1, LANG("Sirona.DoctorNotWorkBonus"));
			}

			++RowID;
		}
	}
	TreeWidget->setColumnWidth(0, TreeWidget->width() / 4);
}
//-----------------------------------------------------------------------------
void ISSirona::ReportForm::OutputList()
{
	QString FilePath = ISFileDialog::GetSaveFileName(this, LANG("File.Filter.Csv"), LANG("Sirona.ReportDoctors"));
	if (FilePath.length())
	{
		QFile FileReport(FilePath);
		if (FileReport.open(QIODevice::WriteOnly))
		{
			for (int i = 0; i < TreeWidget->topLevelItemCount(); ++i)
			{
				QTreeWidgetItem *TreeWidgetItem = TreeWidget->topLevelItem(i);
				QString Data = TreeWidgetItem->text(0) + ';' + TreeWidgetItem->text(1) + "\r\n";
				FileReport.write(Data.toUtf8());
			}

			FileReport.close();
			if (ISMessageBox::ShowQuestion(this, LANG("Sirona.Message.Question.OutputListDone")))
			{
				ISGui::OpenFile(FilePath);
			}
		}
	}
}
//-----------------------------------------------------------------------------
double ISSirona::ReportForm::GetDoctorBonus(int DoctorID) const
{
	double Bonus = 0;
	ISQuery qSelect(QS_DOCTOR_BONUS);
	qSelect.BindValue(":DoctorID", DoctorID);
	qSelect.BindValue(":DateBegin", RangeDateEdit->GetValue().BeginValue);
	qSelect.BindValue(":DateEnd", RangeDateEdit->GetValue().EndValue);
	if (qSelect.ExecuteFirst())
	{
		Bonus = qSelect.ReadColumn("sum").toDouble();
	}
	return Bonus;
}
//-----------------------------------------------------------------------------
