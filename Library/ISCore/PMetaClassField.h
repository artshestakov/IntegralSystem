#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISNamespace.h"
#include "PMetaClass.h"
#include "PMetaClassIndex.h"
#include "PMetaClassForeign.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT PMetaClassField : public PMetaClass
{
	Q_OBJECT

	Q_PROPERTY(QString Type READ GetType WRITE SetType)
	Q_PROPERTY(int Size READ GetSize WRITE SetSize)
	Q_PROPERTY(bool Upper READ GetUpper WRITE SetUpper)
	Q_PROPERTY(bool Lower READ GetLower WRITE SetLower)
	Q_PROPERTY(QVariant DefaultValue READ GetDefaultValue WRITE SetDefaultValue)
	Q_PROPERTY(QVariant DefaultValueWidget READ GetDefaultValueWidget WRITE SetDefaultValueWidget)
	Q_PROPERTY(QString LabelName READ GetLabelName WRITE SetLabelName)
	Q_PROPERTY(QString LocalListName READ GetLocalListName WRITE SetLocalListName)
	Q_PROPERTY(bool NotNull READ GetNotNull WRITE SetNotNull)
	Q_PROPERTY(bool ReadOnly READ GetReadOnly WRITE SetReadOnly)
	Q_PROPERTY(bool HideFromObject READ GetHideFromObject WRITE SetHideFromObject)
	Q_PROPERTY(bool HideFromList READ GetHideFromList WRITE SetHideFromList)
	Q_PROPERTY(bool NotSearch READ GetNotSearch WRITE SetNotSearch)
	Q_PROPERTY(QString Hint READ GetHint WRITE SetHint)
	Q_PROPERTY(QString PlaceholderText READ GetPlaceholderText WRITE SetPlaceholderText)
	Q_PROPERTY(QString ControlWidget READ GetControlWidget WRITE SetControlWidget)
	Q_PROPERTY(QString RegExp READ GetRegExp WRITE SetRegExp)
	Q_PROPERTY(bool IsSystem READ GetIsSystem WRITE SetIsSystem)
	Q_PROPERTY(QString QueryText READ GetQueryText WRITE SetQueryText)
	Q_PROPERTY(PMetaClassForeign* Foreign READ GetForeign WRITE SetForeign)
	Q_PROPERTY(bool Sequence READ GetSequence WRITE SetSequence)
	Q_PROPERTY(QString LayoutName READ GetLayoutName WRITE SetLayoutName)
	Q_PROPERTY(QString SeparatorName READ GetSeparatorName WRITE SetSeparatorName)

public:
	PMetaClassField(QObject *parent = 0);
	virtual ~PMetaClassField();

	void SetType(const QString &type);
	ISNamespace::FieldType GetType();

	void SetSize(int size);
	int GetSize() const;

	void SetUpper(bool upper);
	bool GetUpper() const;

	void SetLower(bool lower);
	bool GetLower() const;

	void SetDefaultValue(const QVariant &default_value);
	QVariant GetDefaultValue() const;

	void SetDefaultValueWidget(const QVariant &default_value_widget);
	QVariant GetDefaultValueWidget() const;

	void SetLabelName(const QString &label_name);
	QString GetLabelName() const;

	void SetLocalListName(const QString &local_list_name);
	QString GetLocalListName() const;

	void SetNotNull(bool not_null);
	bool GetNotNull() const;

	void SetReadOnly(bool read_only);
	bool GetReadOnly() const;

	void SetHideFromObject(bool HideFromObject);
	bool GetHideFromObject() const;

	void SetHideFromList(bool HideFromList);
	bool GetHideFromList() const;

	void SetNotSearch(bool NotSearch);
	bool GetNotSearch() const;

	void SetHint(const QString &Hint);
	QString GetHint() const;

	void SetPlaceholderText(const QString &PlaceholderText);
	QString GetPlaceholderText() const;

	void SetControlWidget(const QString &ControlWidget);
	QString GetControlWidget() const;

	void SetRegExp(const QString &reg_exp);
	QString GetRegExp() const;

	void SetIsSystem(bool IsSystem);
	bool GetIsSystem() const;

	void SetQueryText(const QString &QueryText);
	QString GetQueryText() const;

	void SetIndex(PMetaClassIndex *index);
	PMetaClassIndex* GetIndex();

	void SetForeign(PMetaClassForeign *foreign);
	PMetaClassForeign* GetForeign();

	void SetSequence(bool sequence);
	bool GetSequence() const;

	void SetLayoutName(const QString &layout_name);
	QString GetLayoutName() const;

	void SetSeparatorName(const QString &separator_name);
	QString GetSeparatorName() const;

	bool IsFieldID() const;
	bool IsFieldUID() const;

private:
	ISNamespace::FieldType Type; //Тип
	int Size; //Размер
	bool Upper; //Только верхний регистр
	bool Lower; //Только нижний регистр
	QVariant DefaultValue; //Значение по умолчанию для базы
	QVariant DefaultValueWidget; //Значение по умолчанию
	QString LabelName; //Имя поля на форме объекта
	QString LocalListName; //Имя поля в списке
	bool NotNull; //Поле не должно быть пустым
	bool ReadOnly; //Редактирование поля запрещено
	bool HideFromObject; //Поле должно быть скрыто в форме объекта
	bool HideFromList; //Поле не должно учавствовать в запросе
	bool NotSearch; //Участие поля в поиске
	QString Hint; //Подсказка для поля
	QString PlaceholderText;
	QString ControlWidget; //Наименование виджета-редактора значения
	QString RegExp; //Регулярное выражение для поля
	bool IsSystem; //Статус системного поля

	QString QueryText; //Текст подзапроса

	PMetaClassIndex *Index; //Индекс
	PMetaClassForeign *Foreign; //Внешний ключ

	bool Sequence; //Последовательность поля
	QString LayoutName; //Наименование компоновщика (для горизонтального размещения поля)
	QString SeparatorName; //Наименование вкладки
};
//-----------------------------------------------------------------------------
