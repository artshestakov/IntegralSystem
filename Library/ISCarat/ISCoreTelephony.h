#pragma once
//-----------------------------------------------------------------------------
#include "iscarat_global.h"
#include "ISCaratCoreApplication.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISCARAT_EXPORT ISCoreTelephony : public ISCaratCoreApplication
{
	Q_OBJECT

public:
	ISCoreTelephony(int &argc, char **argv);
	virtual ~ISCoreTelephony();

	bool Invoke() override;

protected:
	void Timeout();
	void HandlingCDR(int ID); //Обработка записи CDR

	QVariant GetDialEnd(int Duration, int BillSec) const; //Получить длительность ожидания
	QVariant GetDirection(const ISUuid &DirectionUID) const; //Получить идентификатор направления
	QVariant GetDialStatus(int ID, const QString &DialStatus) const; //Получить идентификатор статуса вызова
	QString GetPattern(int ID, const QString &String) const; //Получить внутренний номер
	QVariant GetUser(int ID, const QVariant &Pattern) const; //Получить идентификатор пользователя
	bool CheckExistCall(const QString &UniqueID) const; //Проверка существования вызова с UniqueID в базе

private:
	QTimer *Timer;
	int AsteriskDigitNumbers;
};
//-----------------------------------------------------------------------------
