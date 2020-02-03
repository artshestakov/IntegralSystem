#include "StdAfx.h"
#include "CGConfiguratorFIAS.h"
#include "ISDefines.h"
#include "ISDebug.h"
#include "ISLocalization.h"
#include "ISCommandLine.h"
#include "ISSystem.h"
#include "ISMetaData.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_FIAS_KEYS = PREPARE_QUERY("SELECT fkey_tablename, fkey_fieldname "
											"FROM _FIAS_Key "
											"WHERE NOT fkey_isdeleted");
//-----------------------------------------------------------------------------
CGConfiguratorFIAS::CGConfiguratorFIAS() : CGConfiguratorBase()
{

}
//-----------------------------------------------------------------------------
CGConfiguratorFIAS::~CGConfiguratorFIAS()
{

}
//-----------------------------------------------------------------------------
void CGConfiguratorFIAS::prepare()
{
	ISDebug::ShowString(LANG("Configurator.FIAS.EnterPathUnloading") + ':');
	QString UnloadPath = ISCommandLine::GetText(); //Путь к файлам ФИАС
	if (!UnloadPath.length()) //Если путь не введен
	{
		ISDebug::ShowWarningString(LANG("Configurator.FIAS.EnterPathEmpty"));
		return;
	}

	QDir UnloadDir(UnloadPath);
	if (!UnloadDir.exists()) //Если папка по введенному пути не существует
	{
		ISDebug::ShowWarningString(LANG("Configurator.FIAS.PathNotExist").arg(UnloadPath));
		return;
	}

	ISDebug::ShowString(LANG("Configurator.FIAS.EnterPathResult") + ':');
	QString ResultPath = ISCommandLine::GetText();
	if (!ResultPath.length())
	{
		ISDebug::ShowWarningString(LANG("Configurator.FIAS.EnterPathEmpty"));
		return;
	}

	QDir ResultDir(ResultPath);
	if (!ResultDir.exists())
	{
		ISDebug::ShowWarningString(LANG("Configurator.FIAS.PathNotExist").arg(ResultPath));
		return;
	}

	if (ResultDir.entryInfoList(QDir::Files).count())
	{
		ISDebug::ShowWarningString(LANG("Configurator.FIAS.ResultPathNotEmpty").arg(ResultPath));
		return;
	}

	QFileInfoList FileInfoList = UnloadDir.entryInfoList(QStringList() << QString("*.%1").arg(EXTENSION_XML), QDir::Files, QDir::Name);
	for (const QFileInfo &FileInfo : FileInfoList) //Обход всех файлов в директории
	{
		FileHandling(FileInfo, ResultPath);
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorFIAS::update()
{
	ISDebug::ShowString(LANG("Configurator.FIAS.EnterPathPrepareFiles") + ':');
	QString DirPath = ISCommandLine::GetText();
	if (!DirPath.length()) //Если путь не введен
	{
		ISDebug::ShowWarningString(LANG("Configurator.FIAS.EnterPathEmpty"));
		return;
	}

	QDir Dir(DirPath);
	if (!Dir.exists()) //Если папка по введенному пути не существует
	{
		ISDebug::ShowWarningString(LANG("Configurator.FIAS.PathNotExist").arg(DirPath));
		return;
	}

	InitializeKeys();
	QFileInfoList FileInfoList = Dir.entryInfoList(QDir::Files, QDir::Name);
	for (const QFileInfo &FileInfo : FileInfoList) //Обход всех файлов в директории
	{
		FileUpload(FileInfo);
	}
}
//-----------------------------------------------------------------------------
bool CGConfiguratorFIAS::FileHandling(const QFileInfo &FileInfo, const QString &ResultPath)
{
	ISDebug::ShowEmptyString();
	ISDebug::ShowInfoString(LANG("Configurator.FIAS.FileHandling.Start").arg(FileInfo.baseName()));
	QFile FileOriginal(FileInfo.absoluteFilePath());
	if (!FileOriginal.open(QIODevice::ReadOnly))
	{
		ISDebug::ShowWarningString(LANG("Configurator.FIAS.FileNotOpen").arg(FileOriginal.fileName()).arg(FileOriginal.errorString()));
		return false;
	}

	QFile FileResult(ResultPath + '/' + GetFileResultName(FileInfo.baseName()));
	if (!FileResult.open(QIODevice::Append))
	{
		FileOriginal.close();
		ISDebug::ShowWarningString(LANG("Configurator.FIAS.FileNotOpen").arg(FileResult.fileName()).arg(FileResult.errorString()));
		return false;
	}

	qint64 ProcessedSize = 0;
	while (!FileOriginal.atEnd()) //Обход блоков файла
	{
		qint64 MaxLen = (1000 * 1024) * 10; //10mb
		qint64 FileSize = FileOriginal.size();
		if (FileSize > MaxLen) //Если размер файла меньше макисмального размера блока - один проход
		{
			ProcessedSize += MaxLen;
			ISDebug::ShowInfoString(LANG("Configurator.FIAS.WriteToFile").arg(GetFileResultName(FileInfo.baseName())).arg(ProcessedSize / (1000 * 1024)).arg(FileSize / (1000 * 1024)));
		}

		QString Content = FileOriginal.read(MaxLen);
		QRegExp RegExp(REG_EXP_FIAS_CONTENT);
		int Pos = 0;
		QStringList StringList;
		while ((Pos = RegExp.indexIn(Content, Pos)) != -1) //Поиск совпадений по регулярному выражению
		{
			Pos += RegExp.matchedLength();
			Content = RegExp.cap(0);
			Content.replace("/><", "/>\r\n<"); //Разбиение на строки
			StringList = Content.split("\r\n"); //Преобразование в список
			Content.clear(); //Очистка временной переменной
		}

		while (StringList.count()) //Обход всех совпадений
		{
			QString StringFirst = StringList.takeFirst();
			if (StringFirst.contains("\t")) //Если есть табуляция
			{
				StringFirst.replace("\t", QString());
			}
			FileResult.write(StringFirst.toUtf8());
			FileResult.write("\r\n");
		}
		FileResult.flush();
	}

	ISDebug::ShowInfoString(LANG("Configurator.FIAS.FileHandling.End").arg(FileInfo.baseName()));
	FileResult.close();
	FileOriginal.close();
	return true;
}
//-----------------------------------------------------------------------------
QString CGConfiguratorFIAS::GetFileResultName(const QString &FileName) const
{
	return FileName.split('_').at(1);
}
//-----------------------------------------------------------------------------
void CGConfiguratorFIAS::InitializeKeys()
{
	ISQuery qSelect(QS_FIAS_KEYS);
	qSelect.SetShowLongQuery(false);
	if (qSelect.Execute())
	{
		while (qSelect.Next())
		{
			MapKeys.insert(qSelect.ReadColumn("fkey_tablename").toString(), qSelect.ReadColumn("fkey_fieldname").toString());
		}
	}
}
//-----------------------------------------------------------------------------
bool CGConfiguratorFIAS::FileUpload(const QFileInfo &FileInfo)
{
	ISDebug::ShowEmptyString();
	ISDebug::ShowInfoString(LANG("Configurator.FIAS.FileHandling.Start").arg(FileInfo.baseName()));

	QString TableName = "_FIAS_" + GetTableName(FileInfo.absoluteFilePath());
	if (!ISMetaData::GetInstanse().CheckExistTable(TableName))
	{
		ISDebug::ShowWarningString(LANG("Configurator.FIAS.TableNotExist").arg(TableName));
		return false;
	}

	QFile File(FileInfo.absoluteFilePath());
	if (!File.open(QIODevice::ReadOnly))
	{
		return false;
	}

	quint64 CountLine = GetCountLine(FileInfo.absoluteFilePath());
	int Inserted = 0;
	double Percent = 0;
	while (!File.atEnd()) //Чтение файла построчно
	{
		Inserted++;
		Percent = ((double)Inserted * 100) / (double)CountLine;
		Percent = round(Percent * 100) / 100;
		if (!fmod((double)Inserted, 1000))
		{
			ISDebug::ShowInfoString(LANG("Configurator.FIAS.Processed").arg(TableName).arg(Inserted).arg(CountLine).arg(Percent));
		}

		QString StringXML = File.readLine();
		QStringMap StringMap = ParseLine(StringXML);
		PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(TableName);
		if (Select(MetaTable, StringMap))
		{
			Update(MetaTable, StringMap);
		}
		else
		{
			Insert(MetaTable, StringMap);
		}
	}

	File.close();
	ISDebug::ShowInfoString(LANG("Configurator.FIAS.FileHandling.End").arg(FileInfo.baseName()));
	return true;
}
//-----------------------------------------------------------------------------
QString CGConfiguratorFIAS::GetTableName(const QString &FilePath) const
{
	QString TableName;
	QFile File(FilePath);
	if (File.open(QIODevice::ReadOnly))
	{
		while (!File.atEnd())
		{
			QRegExp RegExp(REG_EXP_FIAS_TABLE_NAME);
			int Pos = 0;
			while ((Pos = RegExp.indexIn(File.readLine(), Pos)) != -1)
			{
				Pos += RegExp.matchedLength();
				TableName = RegExp.cap(0);
				ISSystem::RemoveLastSymbolFromString(TableName);
			}
			break;
		}
		File.close();
	}

	return TableName;
}
//-----------------------------------------------------------------------------
QStringMap CGConfiguratorFIAS::ParseLine(const QString &Content) const
{
	QStringMap StringMap;
	QDomDocument DomDocument;
	if (DomDocument.setContent(Content))
	{
		QDomElement DomElement = DomDocument.documentElement();
		QDomNamedNodeMap DomNamedNodeMap = DomElement.attributes();
		for (int i = 0; i < DomNamedNodeMap.count(); ++i)
		{
			QDomNode NodeItem = DomNamedNodeMap.item(i);
			StringMap.insert(NodeItem.nodeName(), NodeItem.nodeValue());
		}
	}
	return StringMap;
}
//-----------------------------------------------------------------------------
quint64 CGConfiguratorFIAS::GetCountLine(const QString &FilePath) const
{
	ISDebug::ShowInfoString(LANG("Configurator.FIAS.GetCountLine"));
	quint64 CountLine = 0;
	QFile File(FilePath);
	if (File.open(QIODevice::ReadOnly))
	{
		while (!File.atEnd())
		{
			File.readLine();
			CountLine++;
		}
		File.close();
	}
	return CountLine;
}
//-----------------------------------------------------------------------------
bool CGConfiguratorFIAS::Select(PMetaClassTable *MetaTable, const QStringMap &StringMap) const
{
	QString KeyField = MapKeys.value(MetaTable->GetName());
	QString SqlText = "SELECT COUNT(*) FROM " + MetaTable->GetName() + " WHERE " + MetaTable->GetAlias() + '_' + KeyField + " = :Value";

	ISQuery qSelectFIAS(SqlText);
	qSelectFIAS.SetShowLongQuery(false);
	qSelectFIAS.BindValue(":Value", StringMap.value(KeyField));
	if (qSelectFIAS.ExecuteFirst())
	{
		int Count = qSelectFIAS.ReadColumn("count").toInt();
		if (Count)
		{
			return true;
		}
	}

	return false;
}
//-----------------------------------------------------------------------------
void CGConfiguratorFIAS::Update(PMetaClassTable *MetaTable, const QStringMap &StringMap)
{
	QString UpdateText = "UPDATE " + MetaTable->GetName() + " SET \n";
	QString WhereText = "WHERE ";
	QStringMap Parameters;

	for (const auto &MapItem : StringMap.toStdMap())
	{
		if (MapKeys.value(MetaTable->GetName()) == MapItem.first)
		{
			WhereText += MetaTable->GetAlias() + '_' + MapItem.first + " = :Value";
			Parameters.insert(":Value", MapItem.second);
		}
		else
		{
			UpdateText += MetaTable->GetAlias() + '_' + MapItem.first + " = :" + MapItem.first + ", \n";
			Parameters.insert(':' + MapItem.first, MapItem.second);
		}
	}

	ISSystem::RemoveLastSymbolFromString(UpdateText, 3);
	ISQuery qUpdateFIAS(UpdateText + " \n" + WhereText);
	qUpdateFIAS.SetShowLongQuery(false);
	for (const auto &Parameter : Parameters.toStdMap())
	{
		qUpdateFIAS.BindValue(Parameter.first, Parameter.second);
	}
	qUpdateFIAS.Execute();
}
//-----------------------------------------------------------------------------
void CGConfiguratorFIAS::Insert(PMetaClassTable *MetaTable, const QStringMap &StringMap)
{
	QString SqlInsert = "INSERT INTO " + MetaTable->GetName() + '(';
	QString SqlValues = "VALUES(";

	for (const QString &FieldName : StringMap.keys())
	{
		SqlInsert += MetaTable->GetAlias() + '_' + FieldName + ", ";
		SqlValues += ':' + FieldName + ", ";
	}

	ISSystem::RemoveLastSymbolFromString(SqlInsert, 2);
	ISSystem::RemoveLastSymbolFromString(SqlValues, 2);
	SqlInsert += ')';
	SqlValues += ')';

	ISQuery qInsertFIAS(SqlInsert + " \n" + SqlValues);
	qInsertFIAS.SetShowLongQuery(false);
	for (const auto &MapItem : StringMap.toStdMap())
	{
		qInsertFIAS.BindValue(':' + MapItem.first, MapItem.second);
	}
	qInsertFIAS.Execute();
}
//-----------------------------------------------------------------------------
