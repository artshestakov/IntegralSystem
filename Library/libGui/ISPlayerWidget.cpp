#include "StdAfx.h"
#include "ISPlayerWidget.h"
#include "ISDefines.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISStyleSheet.h"
//-----------------------------------------------------------------------------
ISPlayerWidget::ISPlayerWidget(const QString &FilePath, bool auto_play, QWidget *parent) : QWidget(parent)
{
	AutoPlay = auto_play;
	VolumeOn = true;
	LastVolume = 50;

	setAttribute(Qt::WA_DeleteOnClose, true);
	
	MediaPlayer = new QMediaPlayer(this);
	MediaPlayer->setVolume(50);
	connect(MediaPlayer, &QMediaPlayer::durationChanged, this, &ISPlayerWidget::MediaDurationChanged);
	connect(MediaPlayer, static_cast<void(QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error), this, &ISPlayerWidget::MediaError);
	connect(MediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &ISPlayerWidget::MediaStatusChanged);
	connect(MediaPlayer, &QMediaPlayer::positionChanged, this, &ISPlayerWidget::MediaPositionChanged);
	connect(MediaPlayer, &QMediaPlayer::stateChanged, this, &ISPlayerWidget::MediaStateChanged);

	CreateInterface();

	if (FilePath.length())
	{
		SetFilePlay(FilePath);
	}

	PropertyAnimation = new QPropertyAnimation(SliderDuration, "value", this);
	PropertyAnimation->setStartValue(0);
}
//-----------------------------------------------------------------------------
ISPlayerWidget::~ISPlayerWidget()
{
	
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::SetFilePlay(const QString &FilePath)
{
	MediaPlayer->setMedia(QUrl::fromLocalFile(FilePath));
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::Play()
{
	MediaPlayer->play();
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::Pause()
{
	MediaPlayer->pause();
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::Stop()
{
	MediaPlayer->stop();
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::Previous()
{
	if (MediaPlayer->state() == QMediaPlayer::PlayingState)
	{
		SliderPress();
		SliderMoved(MediaPlayer->position() - PLAYER_WINDING);
		SliderRelease();
	}
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::Next()
{
	if (MediaPlayer->state() == QMediaPlayer::PlayingState)
	{
		SliderPress();
		SliderMoved(MediaPlayer->position() + PLAYER_WINDING);
		SliderRelease();
	}
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::CreateInterface()
{
	QVBoxLayout *MainLayout = new QVBoxLayout();
	setLayout(MainLayout);

	QHBoxLayout *LayoutPlayer = new QHBoxLayout();
	MainLayout->addLayout(LayoutPlayer);

	ButtonPlay = new ISServiceButton(this);
	ButtonPlay->setToolTip(LANG("Player.Play"));
	ButtonPlay->setIcon(BUFFER_ICONS("Player.Play"));
	connect(ButtonPlay, &ISServiceButton::clicked, this, &ISPlayerWidget::Play);
	LayoutPlayer->addWidget(ButtonPlay);

	ButtonPause = new ISServiceButton(this);
	ButtonPause->setEnabled(false);
	ButtonPause->setToolTip(LANG("Player.Pause"));
	ButtonPause->setIcon(BUFFER_ICONS("Player.Pause"));
	connect(ButtonPause, &ISServiceButton::clicked, this, &ISPlayerWidget::Pause);
	LayoutPlayer->addWidget(ButtonPause);

	ButtonStop = new ISServiceButton(this);
	ButtonStop->setEnabled(false);
	ButtonStop->setToolTip(LANG("Player.Stop"));
	ButtonStop->setIcon(BUFFER_ICONS("Player.Stop"));
	connect(ButtonStop, &ISServiceButton::clicked, this, &ISPlayerWidget::Stop);
	LayoutPlayer->addWidget(ButtonStop);

	LabelCurrentDuration = new QLabel(this);
	LabelCurrentDuration->setText("00:00");
	LayoutPlayer->addWidget(LabelCurrentDuration);

	SliderDuration = new ISSliderDuration(this);
	connect(SliderDuration, &ISSliderDuration::valueChanged, this, &ISPlayerWidget::SliderValueChanged);
	connect(SliderDuration, &ISSliderDuration::sliderPressed, this, &ISPlayerWidget::SliderPress);
	connect(SliderDuration, &ISSliderDuration::sliderMoved, this, &ISPlayerWidget::SliderMoved);
	connect(SliderDuration, &ISSliderDuration::sliderReleased, this, &ISPlayerWidget::SliderRelease);
	LayoutPlayer->addWidget(SliderDuration);

	LabelDuration = new QLabel(this);
	LabelDuration->setText("00:00");
	LayoutPlayer->addWidget(LabelDuration);

	QHBoxLayout *LayoutVolume = new QHBoxLayout();
	MainLayout->addLayout(LayoutVolume);

	ButtonPrevious = new ISServiceButton(this);
	ButtonPrevious->setIcon(BUFFER_ICONS("Player.Previous"));
	ButtonPrevious->setToolTip(LANG("Player.Previous"));
	connect(ButtonPrevious, &ISServiceButton::clicked, this, &ISPlayerWidget::Previous);
	LayoutVolume->addWidget(ButtonPrevious);

	ButtonNext = new ISServiceButton(this);
	ButtonNext->setIcon(BUFFER_ICONS("Player.Next"));
	ButtonNext->setToolTip(LANG("Player.Next"));
	connect(ButtonNext, &ISServiceButton::clicked, this, &ISPlayerWidget::Next);
	LayoutVolume->addWidget(ButtonNext);

	ButtonVolume = new ISServiceButton(this);
	ButtonVolume->setToolTip(LANG("Player.Volume.Off"));
	ButtonVolume->setIcon(BUFFER_ICONS("Volume.Maximum"));
	connect(ButtonVolume, &ISServiceButton::clicked, this, &ISPlayerWidget::VolumeClicked);
	LayoutVolume->addWidget(ButtonVolume);

	SliderVolume = new QSlider(Qt::Horizontal, this);
	SliderVolume->setValue(MediaPlayer->volume());
	SliderVolume->setFixedWidth(120);
	connect(SliderVolume, &QSlider::valueChanged, MediaPlayer, &QMediaPlayer::setVolume);
	LayoutVolume->addWidget(SliderVolume);

	LabelErrorIcon = new QLabel(this);
	LabelErrorIcon->setPixmap(BUFFER_ICONS("Importance.High").pixmap(SIZE_16_16));
	LabelErrorIcon->setVisible(false);
	LayoutVolume->addWidget(LabelErrorIcon);

	LabelErrorText = new QLabel(this);
	LabelErrorText->setVisible(false);
	LayoutVolume->addWidget(LabelErrorText);

	LayoutVolume->addStretch();
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::VolumeClicked()
{
	if (VolumeOn) //Отключить громкость
	{
		LastVolume = MediaPlayer->volume();
		MediaPlayer->setVolume(0);

		ButtonVolume->setToolTip(LANG("Player.Volume.On"));
		ButtonVolume->setIcon(BUFFER_ICONS("Volume.Null"));
	}
	else //Включить громкость
	{
		MediaPlayer->setVolume(LastVolume);

		ButtonVolume->setToolTip(LANG("Player.Volume.Off"));
		ButtonVolume->setIcon(BUFFER_ICONS("Volume.Maximum"));
	}

	VolumeOn = !VolumeOn;
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::SliderValueChanged(int Value)
{
	LabelCurrentDuration->setText(QTime(0, 0).addMSecs(Value).toString(TIME_FORMAT_V4));
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::SliderPress()
{
	if (MediaPlayer->state() == QMediaPlayer::PlayingState)
	{
		MediaPlayer->pause();
		PropertyAnimation->stop();
	}
	else if (MediaPlayer->state() == QMediaPlayer::StoppedState || MediaPlayer->state() == QMediaPlayer::PausedState)
	{
		int SliderValue = SliderDuration->value();
		MediaPlayer->setPosition(SliderValue);
		PropertyAnimation->setStartValue(SliderValue);
		PropertyAnimation->setDuration(MediaPlayer->duration() - SliderValue);
	}
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::SliderMoved(int Position)
{
	MediaPlayer->setPosition(Position);
	PropertyAnimation->setStartValue(Position);
	PropertyAnimation->setDuration(MediaPlayer->duration() - Position);
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::SliderRelease()
{
	if (MediaPlayer->state() == QMediaPlayer::PausedState)
	{
		MediaPlayer->play();
	}
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::MediaDurationChanged(qint64 Duration)
{
	SliderDuration->setMaximum(Duration);
	LabelDuration->setText(QTime(0, 0).addMSecs(Duration).toString(TIME_FORMAT_V4));
	
	PropertyAnimation->setEndValue(Duration);
	PropertyAnimation->setDuration(Duration);

	UpdatePreviousNextEnable();
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::MediaError(QMediaPlayer::Error NewError)
{
	QString ErrorString;
	switch (NewError)
	{
	case QMediaPlayer::ResourceError: ErrorString = LANG("QMediaPlayer.Error.ResourceError"); break;
	case QMediaPlayer::FormatError: ErrorString = LANG("QMediaPlayer.Error.FormatError"); break;
	case QMediaPlayer::NetworkError: ErrorString = LANG("QMediaPlayer.Error.NetworkError"); break;
	case QMediaPlayer::AccessDeniedError: ErrorString = LANG("QMediaPlayer.Error.AccessDeniedError"); break;
	case QMediaPlayer::ServiceMissingError: ErrorString = LANG("QMediaPlayer.Error.ServiceMissingError"); break;
	}

	if (MediaPlayer->error() == QMediaPlayer::NoError)
	{
		LabelErrorText->clear();
	}
	else
	{
		LabelErrorIcon->setVisible(true);
		LabelErrorText->setVisible(true);
		LabelErrorText->setText(LANG("Error") + ": " + ErrorString);
	}
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::MediaStatusChanged(QMediaPlayer::MediaStatus NewStatus)
{
	if (NewStatus == QMediaPlayer::LoadedMedia && AutoPlay)
	{
		AutoPlay = false;
		Play();
	}
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::MediaPositionChanged(qint64 Position)
{
	UpdatePreviousNextEnable();
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::MediaStateChanged(QMediaPlayer::State NewState)
{
	if (NewState == QMediaPlayer::PlayingState)
	{
		ButtonPlay->setEnabled(false);
		ButtonPause->setEnabled(true);
		ButtonStop->setEnabled(true);

		if (PropertyAnimation->state() == QPropertyAnimation::Stopped)
		{
			PropertyAnimation->start();
		}
		else if (PropertyAnimation->state() == QPropertyAnimation::Paused)
		{
			PropertyAnimation->resume();
		}
	}
	else if (NewState == QMediaPlayer::PausedState)
	{
		ButtonPlay->setEnabled(true);
		ButtonPause->setEnabled(false);
		ButtonStop->setEnabled(true);

		PropertyAnimation->pause();
	}
	else if (NewState == QMediaPlayer::StoppedState)
	{
		ButtonPlay->setEnabled(true);
		ButtonPause->setEnabled(false);
		ButtonStop->setEnabled(false);

		PropertyAnimation->stop();
		PropertyAnimation->setStartValue(0);
		PropertyAnimation->setEndValue(MediaPlayer->duration());
		PropertyAnimation->setDuration(MediaPlayer->duration());
		SliderDuration->setValue(0);
	}
}
//-----------------------------------------------------------------------------
void ISPlayerWidget::UpdatePreviousNextEnable()
{
	ButtonPrevious->setEnabled(MediaPlayer->position() - PLAYER_WINDING > 0);
	ButtonNext->setEnabled(MediaPlayer->position() + PLAYER_WINDING < MediaPlayer->duration());
}
//-----------------------------------------------------------------------------
