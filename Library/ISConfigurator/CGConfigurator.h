#pragma once
//-----------------------------------------------------------------------------
#include "isconfigurator_global.h"
#include "CGSection.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISCONFIGURATOR_EXPORT CGConfigurator : public QCoreApplication
{
	Q_OBJECT

public:
	CGConfigurator(int &argc, char **argv);
	virtual ~CGConfigurator();

	void InterpreterMode();

    bool CheckExistDatabase(bool &Connected);
	bool Execute(const QString &Argument);
	bool Execute(const QString &Argument, const QString &SubArgument);

	ISNamespace::ConsoleArgumentType CheckArguments() const; //�������� ���������� ��������� ������

protected:
	QString GetClassName(const QString &Argument);
	QStringList ParseInputCommand(const QString &Command);
	void ProgressMessage(const QString &Message);
	void Exit();

protected slots:
	void help(); //�������� ������

private:
	QVector<CGSection*> Arguments;
};
//-----------------------------------------------------------------------------
