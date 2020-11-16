#include "ISAudioPlayerForm.h"
#include "ISBuffer.h"
#include "ISDefinesGui.h"
#include "ISLocalization.h"
#include "ISMessageBox.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISAudioPlayerForm::ISAudioPlayerForm(QWidget *parent)
	: ISInterfaceForm(parent),
	BytePointer(nullptr),
	Buffer(nullptr)
{
	setWindowIcon(BUFFER_ICONS("AudioPlayer"));
	setWindowTitle(LANG("AudioPlayer.Title"));
	GetMainLayout()->setContentsMargins(ISDefines::Gui::MARGINS_LAYOUT_10_PX);

	QHBoxLayout *LayoutTitle = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutTitle);

	ButtonPlay = new ISServiceButton(BUFFER_ICONS("AudioPlayer.Play"), LANG("AudioPlayer.Play"), this);
	connect(ButtonPlay, &ISServiceButton::clicked, this, &ISAudioPlayerForm::PlayClicked);
	LayoutTitle->addWidget(ButtonPlay);

	ButtonPause = new ISServiceButton(BUFFER_ICONS("AudioPlayer.Pause"), LANG("AudioPlayer.Pause"), this);
	ButtonPause->setEnabled(false);
	connect(ButtonPause, &ISServiceButton::clicked, this, &ISAudioPlayerForm::PauseClicked);
	LayoutTitle->addWidget(ButtonPause);

	ButtonStop = new ISServiceButton(BUFFER_ICONS("AudioPlayer.Stop"), LANG("AudioPlayer.Stop"), this);
	ButtonStop->setEnabled(false);
	connect(ButtonStop, &ISServiceButton::clicked, this, &ISAudioPlayerForm::StopClicked);
	LayoutTitle->addWidget(ButtonStop);

	LayoutTitle->addStretch();

	SliderPosition = new QSlider(Qt::Horizontal, this);
	GetMainLayout()->addWidget(SliderPosition);

	QHBoxLayout *LayoutVolume = new QHBoxLayout();
	GetMainLayout()->addLayout(LayoutVolume);

	QLabel *LabelVolumeLeft = new QLabel(this);
	LabelVolumeLeft->setPixmap(BUFFER_ICONS("AudioPlayer.Volume.Down").pixmap(ISDefines::Gui::SIZE_32_32));
	LayoutVolume->addWidget(LabelVolumeLeft);

	SliderVolume = new QSlider(Qt::Horizontal, this);
	SliderVolume->setRange(0, 100);
	SliderVolume->setValue(100);
	connect(SliderVolume, &QSlider::valueChanged, this, &ISAudioPlayerForm::SliderVolumeChanged);
	LayoutVolume->addWidget(SliderVolume);

	QLabel *LabelVolumeUp = new QLabel(this);
	LabelVolumeUp->setPixmap(BUFFER_ICONS("AudioPlayer.Volume.Up").pixmap(ISDefines::Gui::SIZE_32_32));
	LayoutVolume->addWidget(LabelVolumeUp);

	MediaPlayer = new QMediaPlayer(this, QMediaPlayer::LowLatency);
	connect(MediaPlayer, &QMediaPlayer::durationChanged, this, &ISAudioPlayerForm::DurationChanged);
	connect(MediaPlayer, static_cast<void(QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error), this, &ISAudioPlayerForm::Error);
	connect(MediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &ISAudioPlayerForm::MediaStatusChanged);
	connect(MediaPlayer, &QMediaPlayer::mutedChanged, this, &ISAudioPlayerForm::MutedChanged);
	connect(MediaPlayer, &QMediaPlayer::positionChanged, this, &ISAudioPlayerForm::PositionChanged);
	connect(MediaPlayer, &QMediaPlayer::stateChanged, this, &ISAudioPlayerForm::StateChanged);
}
//-----------------------------------------------------------------------------
ISAudioPlayerForm::~ISAudioPlayerForm()
{
	
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::SetMedia(const QString &FilePath)
{
	QFile File(FilePath);
	if (File.open(QIODevice::ReadWrite))
	{
		QByteArray ByteArray = File.readAll();
		File.close();
		SetMedia(ByteArray);
	}
	else
	{
		ISMessageBox::ShowWarning(this, File.errorString());
	}
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::SetMedia(const QByteArray &ByteArray)
{
	POINTER_DELETE(BytePointer);
	BytePointer = new QByteArray(ByteArray);

	Buffer = new QBuffer(BytePointer, this);
	if (Buffer->open(QIODevice::ReadWrite))
	{
		Buffer->reset();
		MediaPlayer->setMedia(QMediaContent(), Buffer);
		MediaPlayer->play();
	}
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::closeEvent(QCloseEvent *CloseEvent)
{
	if (MediaPlayer->state() == QMediaPlayer::PlayingState)
	{
		MediaPlayer->stop();
	}
	MediaPlayer->setMedia(QMediaContent());
	POINTER_DELETE(Buffer);
	POINTER_DELETE(BytePointer);
	ISInterfaceForm::closeEvent(CloseEvent);
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::PlayClicked()
{
	MediaPlayer->play();
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::PauseClicked()
{
	MediaPlayer->pause();
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::StopClicked()
{
	MediaPlayer->stop();
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::SliderVolumeChanged(int Volume)
{
	MediaPlayer->setVolume(Volume);
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::DurationChanged(qint64 Duration)
{
	SliderPosition->setMaximum(Duration);
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::Error(QMediaPlayer::Error Error)
{
	Q_UNUSED(Error);
	ISMessageBox::ShowWarning(this, MediaPlayer->errorString());
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::MediaStatusChanged(QMediaPlayer::MediaStatus MediaStatus)
{
	Q_UNUSED(MediaStatus);
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::MutedChanged(bool Muted)
{
	Q_UNUSED(Muted);
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::PositionChanged(qint64 Position)
{
	SliderPosition->setValue(Position);
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::StateChanged(QMediaPlayer::State State)
{
	switch (State)
	{
	case QMediaPlayer::PlayingState:
		ButtonPlay->setEnabled(false);
		ButtonPause->setEnabled(true);
		ButtonStop->setEnabled(true);
		break;

	case QMediaPlayer::PausedState:
		ButtonPlay->setEnabled(true);
		ButtonPause->setEnabled(false);
		break;

	case QMediaPlayer::StoppedState:
		ButtonPlay->setEnabled(true);
		ButtonPause->setEnabled(false);
		ButtonStop->setEnabled(false);
		break;
	}
}
//-----------------------------------------------------------------------------
