#include "StdAfx.h"
#include "ISDebug.h"
#include "EXDefines.h"
#include "ISProperty.h"
#include "ISLogger.h"
#include "ISSystem.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
ISDebug::ISDebug() : QObject()
{
	
}
//-----------------------------------------------------------------------------
ISDebug::~ISDebug()
{

}
//-----------------------------------------------------------------------------
ISDebug& ISDebug::GetInstance()
{
	static ISDebug Debug;
	return Debug;
}
//-----------------------------------------------------------------------------
void ISDebug::ShowCaratString(const QString &CoreName, const QString &String)
{
	GetInstance().CaratString(CoreName, String);
}
//-----------------------------------------------------------------------------
void ISDebug::ShowString(const QString &String)
{
	GetInstance().OutputString(String);
}
//-----------------------------------------------------------------------------
void ISDebug::ShowEmptyString(bool AddInLog)
{
	GetInstance().EmptyString(AddInLog);
}
//-----------------------------------------------------------------------------
void ISDebug::ShowDebugString(const QString &String)
{
	GetInstance().DebugString(String);
}
//-----------------------------------------------------------------------------
void ISDebug::ShowInfoString(const QString &String)
{
	GetInstance().InfoString(String);
}
//-----------------------------------------------------------------------------
void ISDebug::ShowWarningString(const QString &String)
{
	GetInstance().WarningString(String);
}
//-----------------------------------------------------------------------------
void ISDebug::ShowCriticalString(const QString &String)
{
	GetInstance().CriticalString(String);
}
//-----------------------------------------------------------------------------
void ISDebug::ShowAssertString(const QString &String)
{
	GetInstance().AssertString(String);
}
//-----------------------------------------------------------------------------
void ISDebug::ShowExceptionString(const QString &String)
{
	GetInstance().ExceptionString(String);
}
//-----------------------------------------------------------------------------
void ISDebug::OutputString(const QString &String, bool AddInLog)
{
	QString NewString = String.simplified();

	if (ISSystem::GetApplicationType() == ISNamespace::AT_CONSOLE)
	{
		QTextStream TextStream(stdout);

        if (ISSystem::GetCurrentOSType() == ISNamespace::OST_Windows)
        {
            TextStream.setCodec(TEXT_CODEC_IBM866);
        }

        TextStream << NewString << endl;
	}
	else if (ISSystem::GetApplicationType() == ISNamespace::AT_GUI)
	{
		qDebug().noquote() << NewString;
	}

	if (AddInLog)
	{
		IS_LOGGER(NewString);
	}

	Mutex.lock();
	if (Messages.count() > 5000)
	{
		Messages.clear();
	}
	Messages.append(NewString);
	Mutex.unlock();

	emit Output(NewString);
}
//-----------------------------------------------------------------------------
void ISDebug::EmptyString(bool AddInLog)
{
	OutputString(QString(), AddInLog);
}
//-----------------------------------------------------------------------------
void ISDebug::CaratString(const QString &CoreName, const QString &String)
{
	OutputString(QString("%1 [%2]: %3").arg(QDateTime::currentDateTime().toString(DATE_TIME_FORMAT_V4)).arg(CoreName).arg(String));
}
//-----------------------------------------------------------------------------
void ISDebug::DebugString(const QString &String)
{
	OutputString(GetCompleteString(ISNamespace::DMT_Debug, String));
}
//-----------------------------------------------------------------------------
void ISDebug::InfoString(const QString &String)
{
	OutputString(GetCompleteString(ISNamespace::DMT_Info, String));
}
//-----------------------------------------------------------------------------
void ISDebug::WarningString(const QString &String)
{
	OutputString(GetCompleteString(ISNamespace::DMT_Warning, String));
}
//-----------------------------------------------------------------------------
void ISDebug::CriticalString(const QString &String)
{
	OutputString(GetCompleteString(ISNamespace::DMT_Critical, String));
}
//-----------------------------------------------------------------------------
void ISDebug::AssertString(const QString &String)
{
	OutputString(GetCompleteString(ISNamespace::DMT_Assert, String));
}
//-----------------------------------------------------------------------------
void ISDebug::ExceptionString(const QString &String)
{
	OutputString(GetCompleteString(ISNamespace::DMT_Exception, String));
}
//-----------------------------------------------------------------------------
void ISDebug::SetAssertMessage(const QString &assert_message)
{
	AssertMessage = assert_message;
}
//-----------------------------------------------------------------------------
QString ISDebug::GetAssertMessage() const
{
	return AssertMessage;
}
//-----------------------------------------------------------------------------
QStringList ISDebug::GetDebugMessages() const
{
	return Messages;
}
//-----------------------------------------------------------------------------
QString ISDebug::GetCompleteString(ISNamespace::DebugMessageType MessageType, const QString &String) const
{
	QString MessageTypeString;

	switch (MessageType)
	{
	case ISNamespace::DMT_Debug: MessageTypeString = "[Debug]"; break;
	case ISNamespace::DMT_Info: MessageTypeString = "[Info]"; break;
	case ISNamespace::DMT_Warning: MessageTypeString = "[Warning]"; break;
	case ISNamespace::DMT_Critical: MessageTypeString = "[Critical]"; break;
	case ISNamespace::DMT_Assert: MessageTypeString = "[Assert]"; break;
	case ISNamespace::DMT_Exception: MessageTypeString = "[Exception]"; break;
	}

	return QString("%1 %2: %3").arg(QDateTime::currentDateTime().toString(DATE_TIME_FORMAT_V4)).arg(MessageTypeString).arg(String);
}
//-----------------------------------------------------------------------------
