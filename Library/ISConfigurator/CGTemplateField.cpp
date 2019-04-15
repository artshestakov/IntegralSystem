#include "StdAfx.h"
#include "CGTemplateField.h"
#include "EXDefines.h"
#include "ISSystem.h"
#include "ISAssert.h"
#include "CGSequence.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
CGTemplateField::CGTemplateField() : QObject()
{

}
//-----------------------------------------------------------------------------
CGTemplateField::~CGTemplateField()
{

}
//-----------------------------------------------------------------------------
QString CGTemplateField::GetSqlTextForTemplateSystemFields(const QString &ClassName, const QString &ClassAlias)
{
    QString SqlText;

    QFile File(SCHEMA_TEMPLATE_FIELDS_PATH);
    IS_ASSERT(File.open(QIODevice::ReadOnly), File.errorString());
    QString Content = File.readAll();
    File.close();

    QDomElement Root = ISSystem::GetDomElement(Content);
	QDomNode NodeTemplateFields = Root.firstChild();

	while (!NodeTemplateFields.isNull())
	{
		QString FieldName = NodeTemplateFields.attributes().namedItem("Name").nodeValue();
		QString FieldType = ISMetaData::GetInstanse().GetAssociationTypes().GetTypeDB(NodeTemplateFields.attributes().namedItem("Type").nodeValue());
		QString FieldLocalListName = NodeTemplateFields.attributes().namedItem("LocalListName").nodeValue();
		QString FieldSequence = NodeTemplateFields.attributes().namedItem("Sequence").nodeValue();
		QString FieldDefaultValue = NodeTemplateFields.attributes().namedItem("DefaultValue").nodeValue();
		QString FieldNotNull = NodeTemplateFields.attributes().namedItem("NotNull").nodeValue();

		SqlText += ClassAlias + "_" + FieldName.toLower() + SYMBOL_SPACE + FieldType;

		if (FieldSequence == "true")
		{
			SqlText += " DEFAULT nextval('" + CGSequence::GetSequenceNameForClass(ClassName) + "'::regclass)";
		}

		if (!FieldDefaultValue.isEmpty())
		{
			SqlText += " DEFAULT " + FieldDefaultValue;
		}

		if (FieldNotNull == "true")
		{
			SqlText += " NOT NULL";
		}

		SqlText += ",\n";

		NodeTemplateFields = NodeTemplateFields.nextSibling();
	}

	return SqlText;
}
//-----------------------------------------------------------------------------
