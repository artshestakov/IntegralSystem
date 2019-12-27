#include "StdAfx.h"
#include "ISVolumeEdit.h"
#include "ISDefines.h"
#include "ISBuffer.h"
#include "ISLocalization.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISVolumeEdit::ISVolumeEdit(QWidget *parent) : ISFieldEditBase(parent)
{
	setMaximumWidth(300);

	QLabel *LabelImageMinimum = new QLabel(this);
	LabelImageMinimum->setPixmap(BUFFER_ICONS("Volume.Minimum").pixmap(SIZE_20_20));
	LabelImageMinimum->setContentsMargins(0, 0, 0, 5);
	AddWidgetToLeft(LabelImageMinimum);

	Slider = new QSlider(Qt::Horizontal, this);
	Slider->setMinimum(0);
	Slider->setMaximum(100);
	Slider->setTickPosition(QSlider::TicksBelow);
	Slider->setTickInterval(10);
	connect(Slider, &QSlider::valueChanged, this, &ISVolumeEdit::ValueChanged);
	AddWidgetEdit(Slider, this);

	QLabel *LabelImageMaximum = new QLabel(this);
	LabelImageMaximum->setPixmap(BUFFER_ICONS("Volume.Maximum").pixmap(SIZE_20_20));
	LabelImageMaximum->setContentsMargins(0, 0, 0, 5);
	AddWidgetToRight(LabelImageMaximum);

	Label = new QLabel(this);
	Label->setText("0%");
	AddWidgetToRight(Label);

	ButtonCheck = new ISServiceButton(this);
	ButtonCheck->setToolTip(LOCALIZATION("ClickFromCheckLevelVolume"));
	ButtonCheck->setIcon(BUFFER_ICONS("Volume.Check"));
	ButtonCheck->setFocusPolicy(Qt::NoFocus);
	connect(ButtonCheck, &ISServiceButton::clicked, this, &ISVolumeEdit::VolumeCheck);
	AddWidgetToRight(ButtonCheck);

	SetValue(0);

	MediaPlayer = new QMediaPlayer(this);
	MediaPlayer->setMedia(QUrl("qrc:/Audio/VolumeChecking.wav"));
	connect(MediaPlayer, &QMediaPlayer::stateChanged, this, &ISVolumeEdit::MediaStateChanged);
}
//-----------------------------------------------------------------------------
ISVolumeEdit::~ISVolumeEdit()
{

}
//-----------------------------------------------------------------------------
void ISVolumeEdit::SetValue(const QVariant &value)
{
	Slider->setValue(value.toInt());
}
//-----------------------------------------------------------------------------
QVariant ISVolumeEdit::GetValue() const
{
	return Slider->value();
}
//-----------------------------------------------------------------------------
void ISVolumeEdit::Clear()
{
	ISFieldEditBase::Clear();
}
//-----------------------------------------------------------------------------
void ISVolumeEdit::SetVisibleClear(bool Visible)
{
	ISFieldEditBase::SetVisibleClear(Visible);
}
//-----------------------------------------------------------------------------
void ISVolumeEdit::ValueChanged()
{
	ISFieldEditBase::ValueChanged();
	Label->setText(QString("%1%").arg(Slider->value()));
}
//-----------------------------------------------------------------------------
void ISVolumeEdit::VolumeCheck()
{
	ISSystem::SetWaitGlobalCursor(true);
	ButtonCheck->setEnabled(false);
	Slider->setEnabled(false);

	MediaPlayer->setVolume(Slider->value());
	MediaPlayer->play();
}
//-----------------------------------------------------------------------------
void ISVolumeEdit::MediaStateChanged(QMediaPlayer::State NewState)
{
	if (NewState == QMediaPlayer::StoppedState)
	{
		ISSystem::SetWaitGlobalCursor(false);
		ButtonCheck->setEnabled(true);
		Slider->setEnabled(true);
	}
}
//-----------------------------------------------------------------------------
