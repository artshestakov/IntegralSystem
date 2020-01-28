#include "StdAfx.h"
#include "ISSironaReportForm.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISQuery.h"
#include "ISGui.h"
#include "ISFileDialog.h"
#include "ISMessageBox.h"
//-----------------------------------------------------------------------------
static QString QS_DOCTORS = PREPARE_QUERY("SELECT dctr_id, dctr_fio, dctr_bonus "
										  "FROM doctors "
										  "WHERE NOT dctr_isdeleted "
										  "ORDER BY dctr_fio");
//-----------------------------------------------------------------------------
static QString QS_DOCTOR_BONUS = PREPARE_QUERY("SELECT SUM(resr_moneydoctor) "
											   "FROM research "
											   "WHERE NOT resr_isdeleted "
											   "AND resr_doctor = :DoctorID "
											   "AND resr_date BETWEEN :DateBegin AND :DateEnd");
//-----------------------------------------------------------------------------
ISSironaReportForm::ISSironaReportForm(QWidget *parent) : ISInterfaceMetaForm(parent)
{
	GetMainLayout()->setContentsMargins(LAYOUT_MARGINS_5_PX);

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	RangeDateEdit = new ISRangeDateEdit(this);
	LayoutTitle->addWidget(RangeDateEdit);

	ISPushButton *ButtonFilter = new ISPushButton(this);
	ButtonFilter->setText(LOCALIZATION("Apply"));
	connect(ButtonFilter, &ISPushButton::clicked, this, &ISSironaReportForm::LoadData);
	LayoutTitle->addWidget(ButtonFilter);

	LayoutTitle->addStretch();

	ISPushButton *ButtonPrint = new ISPushButton(this);
	ButtonPrint->setText(LOCALIZATION("Sirona.OutputList"));
	ButtonPrint->setIcon(BUFFER_ICONS("Print"));
	connect(ButtonPrint, &ISPushButton::clicked, this, &ISSironaReportForm::OutputList);
	LayoutTitle->addWidget(ButtonPrint);

	QStringList TreeHeader;
	TreeHeader.append(LOCALIZATION("Sirona.Doctor"));
	TreeHeader.append(LOCALIZATION("Sirona.Bonus"));

	TreeWidget = new QTreeWidget(this);
	TreeWidget->setRootIsDecorated(false);
	TreeWidget->setAlternatingRowColors(true);
	TreeWidget->setHeaderLabels(TreeHeader);
	GetMainLayout()->addWidget(TreeWidget);
}
//-----------------------------------------------------------------------------
ISSironaReportForm::~ISSironaReportForm()
{

}
//-----------------------------------------------------------------------------
void ISSironaReportForm::LoadData()
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
			TreeWidgetItem->setFont(1, FONT_APPLICATION_BOLD);

			if (DoctorBonus)
			{
				TreeWidgetItem->setText(1, LOCALIZATION("Sirona.DoctorBonus").arg(GetDoctorBonus(DoctorID)));
			}
			else
			{
				TreeWidgetItem->setText(1, LOCALIZATION("Sirona.DoctorNotWorkBonus"));
			}

			RowID++;
		}
	}

	TreeWidget->setColumnWidth(0, TreeWidget->width() / 4);
}
//-----------------------------------------------------------------------------
void ISSironaReportForm::OutputList()
{
	QString FilePath = ISFileDialog::GetSaveFileName(this, LOCALIZATION("File.Filter.Csv"), LOCALIZATION("Sirona.ReportDoctors"));
	if (FilePath.length())
	{
		QFile FileReport(FilePath);
		if (FileReport.open(QIODevice::WriteOnly))
		{
			for (int i = 0; i < TreeWidget->topLevelItemCount(); ++i)
			{
				QTreeWidgetItem *TreeWidgetItem = TreeWidget->topLevelItem(i);
				QString Data = TreeWidgetItem->text(0) + ";" + TreeWidgetItem->text(1) + "\r\n";
				FileReport.write(Data.toUtf8());
			}

			FileReport.close();
			if (ISMessageBox::ShowQuestion(this, LOCALIZATION("Sirona.Message.Question.OutputListDone")))
			{
				ISGui::OpenFile(FilePath);
			}
		}
	}
}
//-----------------------------------------------------------------------------
double ISSironaReportForm::GetDoctorBonus(int DoctorID) const
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
