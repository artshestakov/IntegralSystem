#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISButtonClear.h"
//-----------------------------------------------------------------------------
//Базовый класс для всех полей редактирования информации
class ISGUI_EXPORT ISFieldEditBase : public QWidget
{
	Q_OBJECT

signals:
	void DataChanged(); //Сигнал при изменении данных
	void ValueChange(const QVariant &Value); //Сигнал при изменении данные передающий измененное значение

public:
	ISFieldEditBase(QWidget *parent);
	virtual ~ISFieldEditBase();

	virtual void SetValue(const QVariant &value) = 0; //Изменить значение
	virtual QVariant GetValue() const = 0; //Получить значение
	virtual void Clear() = 0; //Очистка значения
	virtual void SetVisibleClear(bool Visible) = 0;

	virtual void Invoke();
	virtual bool IsValid() const; //Проверка корректности поля
	virtual void SetFont(const QFont &Font);
	virtual void SetReadOnly(bool read_only);
	virtual void SetPlaceholderText(const QString &placeholder_text);
	virtual void SetRegExp(const QString &RegExp);
	
	void SetCursor(const QCursor &Cursor); //Изменить курсор
	void SetToolTip(const QString &ToolTip); //Изменить всплывающее сообщение-подсказку
	void SetFixedWidth(int Width); //Изменить максимальную ширину виджета редактирования
	void SetSizePolicyHorizontal(QSizePolicy::Policy PolicyHorizontal); //Изменить политику растяжения по горизонтали
	void SetSizePolicyVertical(QSizePolicy::Policy PolicyVertical); //Изменить политику растяжения по горизонтали

	void BlinkRed(); //Подсветить поле красным цветом
	void SetFocus(); //Установить фокус на виджет редактирования

	void SetModificationFlag(bool modification_flag); //Изменить флаг изменения значения
	bool GetModificationFlag() const; //Получить флаг изменения значения

	void CreateHint(const QString &Hint); //Создать посказку (если создана, изменится текст подсказки)

protected:
	virtual void paintEvent(QPaintEvent *PaintEvent);
	void ShowHint(); //Показать подсказку
	virtual void ValueChanged();
	
	void AddWidgetEdit(QWidget *edit_widget, QWidget *field_edit_pointer); //Добавить виджет редактора
	void AddWidgetEditStretch(); //Добавить растяжение в компоновщик виджета редактирования
	void AddMainLayoutStretch(); //Добавить растяжение в главный компоновщик поля
	void AddWidgetToLeft(QWidget *widget); //Добавить виджет в левый компоновщик
	void AddWidgetToRight(QWidget *widget); //Добавить виджет в правый компоновщик
	
	void CreateButtonClear(); //Создание кнопки очистки
	ISButtonClear* GetButtonClear();

private:
	QHBoxLayout *MainLayout;
	QToolButton *ButtonHint;
	QHBoxLayout *LayoutLeft;
	QHBoxLayout *LayoutEdit;
	QHBoxLayout *LayoutRight;
	ISButtonClear *ButtonClear;
	
	QWidget *EditWidget;
	QWidget *FieldEditPointer;

	bool ModificationFlag;
	bool BorderRed;
};
//-----------------------------------------------------------------------------
