#include "ISConfig.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISConfig::ISConfig()
	: ErrorString(NO_ERROR_STRING),
	ErrorLine(0),
	ErrorColumn(0),
	Settings(nullptr)
{
	//��������� �������� ��� ���������������� ������
	VectorTemplate = std::vector<ISConfigParameter>
	{
		//��������� ������
		{ CONFIG_TEMPLATE_SERVER, "Connection/Host",		QVariant::String,	true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "Connection/Port",		QVariant::Int,		true,	QVariant(),				1, UINT16_MAX },
		{ CONFIG_TEMPLATE_SERVER, "Connection/Database",	QVariant::String,	true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "Connection/Login",		QVariant::String,	true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "Connection/Password",	QVariant::String,	true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "TCPServer/Include",		QVariant::Bool,		true,	false,					0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "TCPServer/Port",			QVariant::Int,		true,	CARAT_TCP_PORT,			1, UINT16_MAX },
		{ CONFIG_TEMPLATE_SERVER, "TCPServer/WorkerCount",	QVariant::Int,		true,	1,						1, (int)std::thread::hardware_concurrency() * 2 },
		{ CONFIG_TEMPLATE_SERVER, "AMI/Include",			QVariant::Bool,		true,	false,					0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "AMI/Host",				QVariant::String,	true,	LOCAL_HOST_ADDRESS,		0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "AMI/Port",				QVariant::Int,		true,	ASTERISK_AMI_PORT,		1, UINT16_MAX },
		{ CONFIG_TEMPLATE_SERVER, "AMI/Login",				QVariant::String,	true,	"admin",				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "AMI/Password",			QVariant::String,	true,	"admin",				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "AMI/RecordDir",			QVariant::String,	false,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "Monitor/Include",		QVariant::Bool,		true,	false,					0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "Monitor/Timeout",		QVariant::Int,		true,	60,						0, DAY_IN_SECONDS },
		{ CONFIG_TEMPLATE_SERVER, "Other/UpdateClientDir",	QVariant::String,	false,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_SERVER, "Other/Configuration",	QVariant::String,	true,	QVariant(),				0, 0 },

		//���������� ������
		{ CONFIG_TEMPLATE_CLIENT, "Connection/Host",		QVariant::String,	true,	QVariant(),				0, 0 },
        { CONFIG_TEMPLATE_CLIENT, "Connection/Port",		QVariant::Int,		true,	QVariant(),				1, UINT16_MAX },
		{ CONFIG_TEMPLATE_CLIENT, "RememberUser/Include",	QVariant::Bool,		true,	QVariant(),				0, 0 },
		{ CONFIG_TEMPLATE_CLIENT, "RememberUser/Login",		QVariant::String,	true,	QVariant(),				0, 0 },
        { CONFIG_TEMPLATE_CLIENT, "Protocol/Port",			QVariant::Int,		true,	CARAT_TCP_PORT,			1, UINT16_MAX }
	};
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISConfig::~ISConfig()
{
	if (Settings)
	{
		delete Settings;
	}
	CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISConfig& ISConfig::Instance()
{
	static ISConfig Config;
	return Config;
}
//-----------------------------------------------------------------------------
QString ISConfig::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISConfig::IsValid()
{
	//��������� �� ������� �������������
	if (!Settings)
	{
		ErrorString = "Not initialized";
		return false;
	}

	//��������� ��������� �� �������������
	for (const ISConfigParameter &ConfigParameter : VectorTemplate)
	{
		if (ConfigParameter.TemplateName != TemplateName)
		{
			continue;
		}
		QVariant Value = GetValue(ConfigParameter.Name);

		//���� �������� ���������� ��� ����������, � ���� ��� �������� �� ��������� � �� ������ - ������
		if (ConfigParameter.NotNull && !ConfigParameter.DefaultValue.isValid() && Value.toString().isEmpty())
		{
			ErrorString = "Parameter \"" + ConfigParameter.Name + "\" is empty";
			return false;
		}
	}

	//��������� ������������ ����������
	for (const ISConfigParameter &ConfigParameter : VectorTemplate)
	{
		if (ConfigParameter.TemplateName != TemplateName)
		{
			continue;
		}

		if (ConfigParameter.Type == QVariant::Int) //���� ������� �������� - �����
		{
			bool Ok = true;
			QVariant Value = GetValue(ConfigParameter.Name);
			int IntValue = Value.toInt(&Ok);
			if (!Ok) //�� ������� �������� �������� � �����
			{
				ErrorString = QString("Invalid paramter value %1=%2").arg(ConfigParameter.Name).arg(Value.toString());
				return false;
			}

			//��������� ��������� �������� � ��������
			if (IntValue < ConfigParameter.Minimum || IntValue > ConfigParameter.Maximum)
			{
				ErrorString = QString("Parameter %1=%2 out of range [%3;%4]").arg(ConfigParameter.Name).arg(IntValue).arg(ConfigParameter.Minimum).arg(ConfigParameter.Maximum);
				return false;
			}
		}
		else if (ConfigParameter.Type == QVariant::Bool) //���� ������� �������� - ����������
		{
			//��������� ������������ ��������
			QString StringValue = GetValue(ConfigParameter.Name).toString();
			if (!(StringValue.toLower() == "true" || StringValue.toLower() == "false"))
			{
				ErrorString = QString("Invalid paramter value %1=%2 (allowed \"true\" or \"false\")").arg(ConfigParameter.Name).arg(StringValue);
				return false;
			}
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISConfig::ReInitialize(const QString &template_name)
{
	//������� ��������� � ��������������
	POINTER_DELETE(Settings);
	return Initialize(template_name);
}
//-----------------------------------------------------------------------------
bool ISConfig::Initialize(const QString &template_name)
{
	if (Settings) //���� ��������� ���������� - ������ ���� ��� ������������������
	{
		ErrorString = "Already initialized";
		return false;
	}
	TemplateName = template_name;

	PathConfigFile = QCoreApplication::applicationDirPath() + '/' + TemplateName + SYMBOL_POINT + EXTENSION_INI;
	Settings = new QSettings(PathConfigFile, QSettings::IniFormat);
	QSettings::Status SettingsStatus = Settings->status();
	bool Result = SettingsStatus == QSettings::NoError;
	if (Result)
	{
		//���� ���������������� ���� ���������� - ������ ��� � ������ � ��������� ������������� ����������, ����� ������ ���� �� �������
		Result = QFile::exists(PathConfigFile) ? Update() : Create();
	}
	else
	{
		switch (SettingsStatus)
		{
		case QSettings::NoError: break;
		case QSettings::AccessError: ErrorString = "Access error with path " + PathConfigFile; break;
		case QSettings::FormatError: ErrorString = "Invalid format"; break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
QVariant ISConfig::GetValue(const QString &ParameterName)
{
	QVariant Value;
	CRITICAL_SECTION_LOCK(&CriticalSection);
	bool Contains = Settings->contains(ParameterName);
	if (Contains)
	{
		Value = Settings->value(ParameterName);
		if (Value.toString().isEmpty())
		{
			Value = GetDefaultValue(ParameterName);
		}
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	IS_ASSERT(Contains, QString("Not found key \"%1\" in file \"%2\"").arg(ParameterName).arg(Settings->fileName()));
	return Value;
}
//-----------------------------------------------------------------------------
void ISConfig::SetValue(const QString &ParameterName, const QVariant &Value)
{
	IS_ASSERT(Settings->contains(ParameterName), QString("Not found key \"%1\" in file \"%2\"").arg(ParameterName).arg(Settings->fileName()));
	Settings->setValue(ParameterName, Value);
}
//-----------------------------------------------------------------------------
void ISConfig::SaveForce()
{
	if (Settings)
	{
		Settings->sync();
	}
}
//-----------------------------------------------------------------------------
bool ISConfig::Update()
{
	bool FlagChanged = false; //���� ���������
	for (const QString &Key : Settings->allKeys()) //��������� ������ ���� �� ������� � �������
	{
		if (!ContainsKey(Key)) //���� ������ ����� � ������� ��� - ������� ��� �� ������� �����
		{
			Settings->remove(Key);
			FlagChanged = true;
		}
	}

	//������ ���������, �� ��������� �� ����� ���������� � �������
	for (const ISConfigParameter &ConfigParameter : VectorTemplate) //������� ��������� �� �������
	{
		if (ConfigParameter.TemplateName == TemplateName && !Settings->contains(ConfigParameter.Name)) //������ ����� � ������� ���������������� ����� ��� - ���������
		{
			Settings->setValue(ConfigParameter.Name, ConfigParameter.DefaultValue.toString());
			FlagChanged = true;
		}
	}

	if (FlagChanged) //���� ��������� ���� �������� - ��������������
	{
		SaveForce();
		if (Settings->status() != QSettings::NoError)
		{
			ErrorString = "Not update config file. Error code: " + QString::number(Settings->status());
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISConfig::Create()
{
	//������� ��� ��������� �������� ������� � ��������� �� � ���������������� ����
	for (const ISConfigParameter &ConfigParameter : VectorTemplate)
	{
		if (ConfigParameter.TemplateName == TemplateName)
		{
			Settings->setValue(ConfigParameter.Name, ConfigParameter.DefaultValue.toString());
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISConfig::ContainsKey(const QString &Key)
{
	for (const ISConfigParameter &ConfigParameter : VectorTemplate)
	{
		if (ConfigParameter.TemplateName == TemplateName && ConfigParameter.Name == Key)
		{
			return true;
		}
	}
	return false;
}
//-----------------------------------------------------------------------------
QVariant ISConfig::GetDefaultValue(const QString &Key) const
{
	for (const ISConfigParameter &ConfigParameter : VectorTemplate)
	{
		if (ConfigParameter.TemplateName == TemplateName && ConfigParameter.Name == Key)
		{
			return ConfigParameter.DefaultValue;
		}
	}
	return QVariant();
}
//-----------------------------------------------------------------------------
