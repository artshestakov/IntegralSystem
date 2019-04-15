#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISInterfaceForm.h"
#include "ISLineEdit.h"
#include "ISListWidget.h"
#include "PMetaClassForeign.h"
#include "PMetaClassTable.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISListEditPopup : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void Selected(const QVariant &ID, const QString &Text);
	void Hided();

public:
	ISListEditPopup(PMetaClassForeign *meta_foreign, QWidget *ComboBox = 0);
	virtual ~ISListEditPopup();

	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent *e);
	void paintEvent(QPaintEvent *e);

	void SetCurrentValue(const QVariant &current_value);

	QString GetSqlFilter() const; //Получить фильтр
	void SetSqlFilter(const QString &sql_filter); //Изменить фильтр
	void ClearSqlFilter(); //Очистить фильтр

protected:
	void Search(const QVariant &value); //Поиск
	void ItemClicked(QListWidgetItem *ListWidgetItem); //Выбор позиции
	void Add(); //Добавить запись
	void LoadDataFromQuery(); //Загрузка справочника
	void Hide();
	void EnterClicked() override;
	void EscapeClicked() override;

private:
	ISLineEdit *LineEdit;
	QLabel *LabelName;
	ISListWidget *ListWidget;
	QLabel *LabelCountRow;
	QLabel *LabelSearch;
	QLabel *LabelEmpty;

	QString SqlFilter;
	PMetaClassForeign *MetaForeign;
	PMetaClassTable *MetaTableForeign;
	QVariant CurrentValue;
};
//-----------------------------------------------------------------------------
