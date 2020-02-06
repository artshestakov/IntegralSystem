#include "ISPhoneForm.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISServiceButton.h"
#include "ISPushButton.h"
#include "ISSystem.h"
#include "ISNotificationService.h"
#include "ISQueryPool.h"
#include "ISConstants.h"
#include "ISQueryText.h"
//-----------------------------------------------------------------------------
static QString QI_MAKE_CALL = PREPARE_QUERY("INSERT INTO _makecallhistory(mchs_number) "
											"VALUES(:Number)");
//-----------------------------------------------------------------------------
ISPhoneForm::ISPhoneForm(QWidget *parent) : ISInterfaceForm(parent)
{
	Sound = nullptr;

	setWindowFlags(Qt::Popup);
	setAttribute(Qt::WA_DeleteOnClose, false);

	QVBoxLayout *LayoutFrame = new QVBoxLayout();
	LayoutFrame->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_5_PX);

	QFrame *Frame = new QFrame(this);
	Frame->setFrameShape(QFrame::Box);
	Frame->setFrameShadow(QFrame::Raised);
	Frame->setLayout(LayoutFrame);
	GetMainLayout()->addWidget(Frame);

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	LayoutFrame->addLayout(LayoutTitle);

	EditNumber = new ISLineEdit(this);
	EditNumber->SetPlaceholderText(LANG("InputtheNumber"));
	EditNumber->SetReadOnly(true);
	EditNumber->SetFocusPolicy(Qt::NoFocus);
	LayoutTitle->addWidget(EditNumber);

	ISServiceButton *ButtonBackspace = new ISServiceButton(this);
	ButtonBackspace->setIcon(BUFFER_ICONS("Arrow.Left"));
	ButtonBackspace->setFlat(true);
	connect(ButtonBackspace, &ISServiceButton::clicked, this, &ISPhoneForm::BackspaceNumber);
	LayoutTitle->addWidget(ButtonBackspace);

	GridLayout = new QGridLayout();
	GridLayout->setSpacing(5);
	GridLayout->setContentsMargins(DEFINES_GUI.MARGINS_LAYOUT_NULL);
	LayoutFrame->addLayout(GridLayout);

	ButtonGroup = new QButtonGroup(this);
	connect(ButtonGroup, static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), this, &ISPhoneForm::ButtonClicked);

	CreateButton(1, 0, 0, QString(SYMBOL_1), "PhoneButton_1");
	CreateButton(2, 0, 1, QString(SYMBOL_2), "PhoneButton_2");
	CreateButton(3, 0, 2, QString(SYMBOL_3), "PhoneButton_3");
	CreateButton(4, 1, 0, QString(SYMBOL_4), "PhoneButton_4");
	CreateButton(5, 1, 1, QString(SYMBOL_5), "PhoneButton_5");
	CreateButton(6, 1, 2, QString(SYMBOL_6), "PhoneButton_6");
	CreateButton(7, 2, 0, QString(SYMBOL_7), "PhoneButton_7");
	CreateButton(8, 2, 1, QString(SYMBOL_8), "PhoneButton_8");
	CreateButton(9, 2, 2, QString(SYMBOL_9), "PhoneButton_9");
	CreateButton(-1, 3, 0, "*", "PhoneButton_Star");
	CreateButton(0, 3, 1, QString(SYMBOL_0), "PhoneButton_0");
	CreateButton(-1, 3, 2, "#", "PhoneButton_Hash");

	QHBoxLayout *LayoutBottom = new QHBoxLayout();
	LayoutFrame->addLayout(LayoutBottom);

	LayoutBottom->addStretch();

	ISPushButton *ButtonCall = new ISPushButton(this);
	ButtonCall->setText(LANG("Call"));
	ButtonCall->setToolTip(LANG("ClickedFromMakeCallWithInputNumber"));
	ButtonCall->setIcon(BUFFER_ICONS("Phone.Correcting.True"));
	ButtonCall->setFlat(true);
	connect(ButtonCall, &ISPushButton::clicked, this, &ISPhoneForm::Call);
	LayoutBottom->addWidget(ButtonCall, 0, Qt::AlignHCenter);

	ButtonCall->setMenu(new QMenu(ButtonCall));
	ButtonCall->menu()->addAction(new QAction("action 1", ButtonCall));
	ButtonCall->menu()->addAction(new QAction("action 2", ButtonCall));

	ISPushButton *ButtonHide = new ISPushButton(this);
	ButtonHide->setText(LANG("Hide"));
	ButtonHide->setToolTip(LANG("HidePanelInputNumber"));
	ButtonHide->setIcon(BUFFER_ICONS("ArrowDown"));
	ButtonHide->setFlat(true);
	connect(ButtonHide, &ISPushButton::clicked, this, &ISPhoneForm::Hide);
	LayoutBottom->addWidget(ButtonHide);

	LayoutBottom->addStretch();
}
//-----------------------------------------------------------------------------
ISPhoneForm::~ISPhoneForm()
{

}
//-----------------------------------------------------------------------------
void ISPhoneForm::keyPressEvent(QKeyEvent *e)
{
	ISInterfaceForm::keyPressEvent(e);

	switch (e->key())
	{
	case Qt::Key_1: ButtonClicked(ButtonGroup->button(1)); break;
	case Qt::Key_2: ButtonClicked(ButtonGroup->button(2)); break;
	case Qt::Key_3: ButtonClicked(ButtonGroup->button(3)); break;
	case Qt::Key_4: ButtonClicked(ButtonGroup->button(4)); break;
	case Qt::Key_5: ButtonClicked(ButtonGroup->button(5)); break;
	case Qt::Key_6: ButtonClicked(ButtonGroup->button(6)); break;
	case Qt::Key_7: ButtonClicked(ButtonGroup->button(7)); break;
	case Qt::Key_8: ButtonClicked(ButtonGroup->button(8)); break;
	case Qt::Key_9: ButtonClicked(ButtonGroup->button(9)); break;
	case Qt::Key_0: ButtonClicked(ButtonGroup->button(0)); break;
	case Qt::Key_Backspace: BackspaceNumber(); break;
	}
}
//-----------------------------------------------------------------------------
void ISPhoneForm::CreateButton(int ID, int RowIndex, int ColumnIndex, const QString &Text, const QString &AudioResuourceName)
{
	ISPushButton *Button = new ISPushButton(this);
	Button->setText(Text);
	Button->setFont(DEFINES_GUI.FONT_APPLICATION_BOLD);
	Button->setCursor(CURSOR_POINTING_HAND);
	Button->setFixedSize(QSize(40, 30));
	Button->setObjectName(AudioResuourceName);
	GridLayout->addWidget(Button, RowIndex, ColumnIndex, 1, 1, Qt::AlignCenter);

	ButtonGroup->addButton(Button, ID);
}
//-----------------------------------------------------------------------------
void ISPhoneForm::BackspaceNumber()
{
	QString Number = EditNumber->GetValue().toString();
	ISSystem::RemoveLastSymbolFromString(Number);
	EditNumber->SetValue(Number);
}
//-----------------------------------------------------------------------------
void ISPhoneForm::ButtonClicked(QAbstractButton *AbstractButton)
{
	if (Sound)
	{
		delete Sound;
		Sound = nullptr;
	}

	Sound = new QSound(BUFFER_AUDIO(AbstractButton->objectName()), this);
	Sound->play();
	
	EditNumber->SetValue(EditNumber->GetValue().toString() + AbstractButton->text());
}
//-----------------------------------------------------------------------------
void ISPhoneForm::Call()
{
	HideAnimation(400);
	ISNotificationService::ShowNotification(LANG("CallWithNumber").arg(EditNumber->GetValue().toString()));

	QVariantMap VariantMap;
	VariantMap.insert(":Number", EditNumber->GetValue());
	ISQueryPool::GetInstance().AddQuery(QI_MAKE_CALL, VariantMap);
}
//-----------------------------------------------------------------------------
void ISPhoneForm::Hide()
{
	HideAnimation(400);
}
//-----------------------------------------------------------------------------
