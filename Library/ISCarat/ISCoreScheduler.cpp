#include "ISCoreScheduler.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISConfig.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
static QString QS_FILE = PREPARE_QUERY("SELECT file_id "
									   "FROM _file "
									   "ORDER BY file_id");
//-----------------------------------------------------------------------------
static QString QD_FILE = PREPARE_QUERY("DELETE FROM _file "
									   "WHERE file_id = :FileID");
//-----------------------------------------------------------------------------
ISCoreScheduler::ISCoreScheduler(int &argc, char **argv) : ISCaratCoreApplication(argc, argv)
{

}
//-----------------------------------------------------------------------------
ISCoreScheduler::~ISCoreScheduler()
{

}
//-----------------------------------------------------------------------------
bool ISCoreScheduler::Invoke()
{
	bool Result = ISCaratCoreApplication::Invoke();
	if (Result)
	{
		Timer = new QTimer(this);
		Timer->setInterval(1000 * 60 * 60); //1 ���
		connect(Timer, &QTimer::timeout, this, &ISCoreScheduler::Timeout);
		Timeout();
		Started();
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISCoreScheduler::Timeout()
{
	Timer->stop();
	ClearFiles();
	Timer->start();
}
//-----------------------------------------------------------------------------
void ISCoreScheduler::ClearFiles()
{
	ISVectorInt VectorInt;

	//������ ���� ��������������� ������
	ISQuery qSelectFile(QS_FILE);
	qSelectFile.SetShowLongQuery(false);
	if (qSelectFile.Execute())
	{
		while (qSelectFile.Next())
		{
			VectorInt.emplace_back(qSelectFile.ReadColumn("file_id").toInt());
		}
	}
	
	for (PMetaTable *MetaTable : ISMetaData::GetInstanse().GetTables()) //����� ���� ���������
	{
		for (PMetaField *MetaField : MetaTable->Fields) //����� ���� ���������������� ����� ������� ��������
		{
			if (MetaField->Type == ISNamespace::FT_File) //���� ��� ������ �������� ���� �������� ��������� ������
			{
				QString SqlQuery = "SELECT " + MetaTable->Alias.toLower() + '_' + MetaField->Name.toLower() + " \n";
				SqlQuery += "FROM " + MetaTable->Name.toLower() + " \n";
				SqlQuery += "WHERE " + MetaTable->Alias.toLower() + '_' + MetaField->Name.toLower() + " IS NOT NULL";
				ISQuery qSelect(SqlQuery);
				qSelect.SetShowLongQuery(false);
				if (qSelect.Execute())
				{
					while (qSelect.Next())
					{
						VectorRemoveAll(VectorInt, qSelect.ReadColumn(MetaTable->Alias + '_' + MetaField->Name).toInt());
					}
				}
			}
		}
	}

	int Index = 0;
	for (int FileID : VectorInt) //����� � �������� �� ����������� ������
	{
		++Index;
		ISLOGGER_INFO(QString("Deleting file %1 of %2").arg(Index).arg(VectorInt.size()));

		ISQuery qDeleteFile(QD_FILE);
		qDeleteFile.SetShowLongQuery(false);
		qDeleteFile.BindValue(":FileID", FileID);
		qDeleteFile.Execute();
	}
}
//-----------------------------------------------------------------------------