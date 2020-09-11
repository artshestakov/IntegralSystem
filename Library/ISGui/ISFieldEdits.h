#pragma once
#ifndef _ISFIELDEDITS_H_INCLUDED
#define _ISFIELDEDITS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISFieldEditBase.h"
#include "ISValidators.h"
#include "ISImageWidget.h"
#include "PMetaClass.h"
#include "ISEditsQT.h"
#include "ISListWidget.h"
#include "ISInterfaceForm.h"
//-----------------------------------------------------------------------------
class ISCheckEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCheckEdit(QWidget *parent = 0);
	virtual ~ISCheckEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;

	void SetReadOnly(bool read_only) override;
	void SetText(const QString &Text);
	void SetCheckableStrikeOut(bool StrikeOut); //Если включить это свойство, то при активном состоянии текст будет зачеркнутым
	void SetIcon(const QIcon &Icon);
	void SetIconSize(const QSize &Size);

protected:
	QCheckBox* GetCheckBox();

private:
	void CheckableStrikeOut();

private:
	QCheckBox *CheckBox;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISLineEdit : public ISFieldEditBase
{
	Q_OBJECT

signals:
	void ClearPressed();
	void EnterClicked();

public:
	Q_INVOKABLE ISLineEdit(QWidget *parent = 0);
	virtual ~ISLineEdit();

	virtual void SetValue(const QVariant &value) override;
	virtual QVariant GetValue() const override;
	virtual void Clear() override;

	void SetReadOnly(bool read_only) override;
	void SetPlaceholderText(const QString &placeholder_text) override;
	void SetRegExp(const QString &RegExp) override;
	void SetColorText(const QColor &Color) override;
	void SetVisibleClear(bool Visible) override;

	void SetValidator(QValidator *Validator);
	void SetEchoMode(QLineEdit::EchoMode EchoMode);
	void SetCompleter(QCompleter *completer);
	void SetTextAlignment(Qt::Alignment Alignment);
	void SetInputMask(const QString &InputMask);
	void SetFocusPolicy(Qt::FocusPolicy focus_policy);
	void SetTextMargins(const QMargins &Margins);
	void SetTextMargins(int Left, int Top, int Right, int Bottom);
	void SetMaxLength(int Length);
	void SetIcon(const QIcon &Icon);
	void SelectAll();

	void SetUppercase(bool uppercase);
	void SetLowercase(bool lowercase);
	void ResetFontcase(); //Сброс контроля регистра

	void AddAction(QAction *Action, QLineEdit::ActionPosition Position);

	void CreateCompleter(const QStringList &StringList);
	QStringList GetCompleterList() const;

protected:
	void OnUpperText(const QString &Text);
	void OnLowerText(const QString &Text);
	virtual void TextChanged(const QString &Text);
	ISQLineEdit* GetLineEdit();

private:
	ISQLineEdit *LineEdit;
	QCompleter *Completer;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboEdit(QWidget *parent = 0);
	virtual ~ISComboEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;

	QString GetCurrentText() const;
	void SetEditable(bool Editable);
	void SetCurrentItem(int Index);
	void SetCurrentText(const QString &Text);
	void InsertItem(int Index, const QString &Text, const QVariant &UserData);
	void AddItem(const QIcon &IconItem, const QString &Text, const QVariant &UserData);
	void AddItem(const QString &Text, const QVariant &UserData);
	void AddItem(const QString &Text);
	void SetIcon(int Index, const QIcon &Icon);
	void SetWheelScroll(bool WheelScroll);
	void SetDuplicatesEnabled(bool Enabled);
	void HideFirstItem(); //Скрыть первый элемент в списке

	public slots:
	void SetReadOnly(bool read_only);

protected:
	void CurrentIndexChanged(int Index);

private:
	ISQComboBox *ComboBox;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISRadioEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISRadioEdit(QWidget *parent = 0);
	virtual ~ISRadioEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;

protected:
	void AddButton(QRadioButton *RadioButton, const QVariant &Value); //Добавить кнопку

private:
	QHBoxLayout *LayoutPanel;
	QWidget *WidgetPanel;
	QButtonGroup *ButtonGroup;
	QMap<int, QVariant> MapButtons;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISTextEdit : public ISFieldEditBase
{
	Q_OBJECT

	signals:
	void KeyPressEnter();

public:
	Q_INVOKABLE ISTextEdit(QWidget *parent = 0);
	virtual ~ISTextEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;

	void SetReadOnly(bool read_only) override;
	void SetPlaceholderText(const QString &placeholder_text) override;
	void SetFrameShape(QFrame::Shape FrameShape) override;
	void SetSizePolicy(QSizePolicy::Policy PolicyHorizontal, QSizePolicy::Policy PolicyVertical) override;

	void AddText(const QString &Text);
	void SetExecuteEnter(bool Execute);
	void SetUppercase(bool uppercase);
	void SetLowercase(bool lowercase);
	void ResetFontcase(); //Сброс контроля регистра

protected:
	void OnUpperText();
	void OnLowerText();
	ISQTextEdit* GetTextEdit();

private:
	ISQTextEdit *TextEdit;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISBIKEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISBIKEdit(QWidget *parent = 0);
	virtual ~ISBIKEdit();

	bool IsValid() const override;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISKPPEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISKPPEdit(QWidget *parent = 0);
	virtual ~ISKPPEdit();

	bool IsValid() const override;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISOGRNEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOGRNEdit(QWidget *parent = 0);
	virtual ~ISOGRNEdit();

	bool IsValid() const override;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISOKPOEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOKPOEdit(QWidget *parent = 0);
	virtual ~ISOKPOEdit();

	bool IsValid() const override;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISUuidEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUuidEdit(QWidget *parent = 0);
	virtual ~ISUuidEdit();

	void SetValue(const QVariant &value) override;
	bool IsValid() const override;
	void SetReadOnly(bool read_only) override;

private:
	void Generate();

private:
	ISServiceButton *ButtonGenerate;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISVINEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISVINEdit(QWidget *parent = 0);
	~ISVINEdit();

	bool IsValid() const override;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISDateTimeEdit : public ISFieldEditBase
{
	Q_OBJECT

signals:
	void DateTimeChanged();

public:
	Q_INVOKABLE ISDateTimeEdit(QWidget *parent = 0);
	virtual ~ISDateTimeEdit();

	virtual void SetValue(const QVariant &value) override;
	virtual QVariant GetValue() const override;
	virtual void Clear() override;
	virtual void SetReadOnly(bool read_only) override;

	void SetDate(const QDate &Date);
	void SetTime(const QTime &Time);
	void SetRangeDate(const QDate &Minimum, const QDate &Maximum);
	void SetMinimumDate(const QDate &Date);
	void SetMaximumDate(const QDate &Date);

protected:
	void SetVisibleDateEdit(bool Visible);
	void SetVisibleTimeEdit(bool Visible);

private:
	ISQDateEdit *DateEdit;
	ISQTimeEdit *TimeEdit;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISDateEdit : public ISDateTimeEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDateEdit(QWidget *parent = 0);
	virtual ~ISDateEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;

	void SetMinimumDate(const QDate &Date);
	void SetMaximumDate(const QDate &Date);
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISTimeEdit : public ISDateTimeEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTimeEdit(QWidget *parent = 0);
	virtual ~ISTimeEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISBirthdayEdit : public ISDateEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISBirthdayEdit(QWidget *parent = 0);
	virtual ~ISBirthdayEdit();

	void SetValue(const QVariant &value) override;

protected:
	void UpdateLabel(const QDate &Date);

private:
	QLabel *Label;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISINNEdit : public ISLineEdit
{
	Q_OBJECT

signals:
	void SearchFromINN();

public:
	Q_INVOKABLE ISINNEdit(QWidget *parent = 0);
	virtual ~ISINNEdit();

	bool IsValid() const override;
	void SetEnabledSearch(bool Enabled);

protected:
	void TextChanged(const QString &Text) override;

private:
	ISServiceButton *ButtonSearch;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISWeekDayEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISWeekDayEdit(QWidget *parent = 0);
	virtual ~ISWeekDayEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISPathEditDir : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPathEditDir(QWidget *parent = 0);
	virtual ~ISPathEditDir();

	void SetValue(const QVariant &value) override;
	void Clear() override;

protected:
	void SelectDir();
	void OpenDir();

private:
	ISServiceButton *ButtonOpenDir;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISPathEditFile : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPathEditFile(QWidget *parent = 0);
	virtual ~ISPathEditFile();

	void SetValue(const QVariant &value) override;
	void SetFilterFile(const QString &filter_file);

protected:
	void SelectFile();

private:
	QString FilterFile;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISPasswordEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPasswordEdit(QWidget *parent = 0);
	virtual ~ISPasswordEdit();

	void SetVisibleGenerate(bool Visible); //Показывать/непоказывать кнопку генерации пароля
	void SetVisibleCheckBox(bool Visible);

private:
	void PasswordVisibleChanged(int State);
	void GeneratePassword(); //Генерация пароля

private:
	QCheckBox *CheckPasswordVisible;
	ISServiceButton *ButtonGeneratePassword;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISUrlEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUrlEdit(QWidget *parent = 0);
	virtual ~ISUrlEdit();

private:
	void OpenUrl();
	void UrlChanged();

private:
	ISServiceButton *ButtonUrl;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISMonthEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISMonthEdit(QWidget *parent = 0);
	virtual ~ISMonthEdit();

	void SelectCurrentMonth();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISEMailEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISEMailEdit(QWidget *parent = 0);
	virtual ~ISEMailEdit();

private:
	void Send();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISDivisionCodeEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDivisionCodeEdit(QWidget *parent = 0);
	virtual ~ISDivisionCodeEdit();

	bool IsValid() const override;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISTaskPriorityEdit : public ISRadioEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskPriorityEdit(QWidget *parent = 0);
	virtual ~ISTaskPriorityEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISTaskImportantEdit : public ISCheckEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskImportantEdit(QWidget *parent = 0);
	virtual ~ISTaskImportantEdit();

private:
	void ImportantChanged(const QVariant &value);
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISTaskDescriptionEdit : public ISTextEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskDescriptionEdit(QWidget *parent = 0);
	virtual ~ISTaskDescriptionEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISDoubleEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDoubleEdit(QWidget *parent = 0);
	virtual ~ISDoubleEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISMoneyEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISMoneyEdit(QWidget *parent = 0);
	virtual ~ISMoneyEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboTimeEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTimeEdit(QWidget *parent = 0);
	virtual ~ISComboTimeEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISIntegerEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISIntegerEdit(QWidget *parent = 0);
	virtual ~ISIntegerEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;

	void SetMinimum(int Minimum);
	void SetMaximum(int Maximum);
	void SetRange(int Minimum, int Maximum);
	void ResetRange();

private:
	ISIntValidator *IntValidator;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISYearEdit : public ISIntegerEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISYearEdit(QWidget *parent = 0);
	virtual ~ISYearEdit();

	void SelectCurrentYear();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISPhoneEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPhoneEdit(QWidget *parent = 0);
	virtual ~ISPhoneEdit();

	QVariant GetValue() const;
	bool IsValid() const override;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISSearchEdit : public ISLineEdit
{
	Q_OBJECT

signals:
	void Search(const QString &SearchValue);

public:
	ISSearchEdit(QWidget *parent);
	virtual ~ISSearchEdit();

	void Updated();

protected:
	void SearchChanged();
	void AboutToShow();
	void AboutToHide();
	void LastSearchClicked();
	void Timeout();

private:
	ISServiceButton *ButtonLastSearch;
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISColorEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISColorEdit(QWidget *parent = 0);
	virtual ~ISColorEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;

	public slots:
	void SetReadOnly(bool read_only);

protected:
	void SelectColor();

private:
	QWidget *WidgetColor;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISFileEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISFileEdit(QWidget *parent = 0);
	virtual ~ISFileEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;

private:
	void SelectFile();
	void Save();
	void Rename();

private:
	QVariantMap VariantMap;

	ISPushButton *ButtonFile;
	QMenu *Menu;
	QAction *ActionSave;
	QAction *ActionRename;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISImageEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISImageEdit(QWidget *parent = 0);
	virtual ~ISImageEdit();

	virtual void SetValue(const QVariant &value) override;
	virtual QVariant GetValue() const override;
	virtual void Clear() override;

	public slots:
	void SetReadOnly(bool read_only);

private:
	ISImageWidget *ImageWidget;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISSexEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISSexEdit(QWidget *parent = 0);
	virtual ~ISSexEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;

	void SetFont(const QFont &Font);
	void SetReadOnly(bool read_only);

private:
	void ButtonClicked(QAbstractButton *AbstractButton);
	void UpdateIcon();

private:
	QWidget *Widget;
	QButtonGroup *ButtonGroup;
	int CurrentID;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISVolumeEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISVolumeEdit(QWidget *parent = 0);
	virtual ~ISVolumeEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;

protected:
	void ValueChanged() override;
	void VolumeCheck();
	void MediaStateChanged(QMediaPlayer::State NewState);

private:
	QSlider *Slider;
	QLabel *Label;
	ISServiceButton *ButtonCheck;

	QMediaPlayer *MediaPlayer;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISListEditPopup : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void Selected(const QVariant &ID, const QString &Text);
	void Created(int ObjectID);
	void Hided();

public:
	ISListEditPopup(PMetaForeign *meta_foreign, QWidget *ComboBox = 0);
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
	PMetaForeign *MetaForeign;
	PMetaTable *MetaTableForeign;
	QVariant CurrentValue;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISListEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISListEdit(QWidget *parent = 0);
	virtual ~ISListEdit();

	void SetValue(const QVariant &value) override; //Приходит идентификатор
	QVariant GetValue() const override; //Возвращает идентификатор
	void Clear() override;

	void SetReadOnly(bool read_only) override;

	virtual void InvokeList(PMetaForeign *meta_foreign);

	QString GetCurrentText() const; //Получить локальное наименование объекта
	void SetEnabled(bool Enabled);

	void SetSqlFilter(const QString &sql_filter); //Изменить фильтр
	void ClearSqlFilter(); //Очистить SQL-фильтр

protected:
	void SelectedValue(const QVariant &id, const QString &text);

	void ShowPopup();
	void HidedPopup();
	void ShowListForm();

	void CreateObject();
	void EditObject();

private:
	PMetaForeign *MetaForeign;
	PMetaTable *MetaTable;

	ISPushButton *ButtonMain;
	QToolButton *ButtonList;
	QAction *ActionCreate;
	QAction *ActionEdit;
	ISListEditPopup *ListEditPopup;

	QVariant ID;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISTaskExecutorEdit : public ISListEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskExecutorEdit(QWidget *parent = 0);
	virtual ~ISTaskExecutorEdit();

private:
	void DesignateMe();

private:
	ISPushButton *ButtonDesignateMe;
};
//-----------------------------------------------------------------------------
#endif
