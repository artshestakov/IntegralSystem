#include "ISAudioPlayerForm.h"
#include "ISBuffer.h"
#include "ISLocalizationOld.h"
#include "ISDialogsCommon.h"
#include "ISAlgorithmOld.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISAudioPlayerForm::ISAudioPlayerForm()
    : ISInterfaceForm(nullptr),
    BytePointer(nullptr),
    Buffer(nullptr)
{
    setWindowIcon(BUFFER_ICONS("AudioPlayer"));
    setWindowTitle(LANG("AudioPlayer.Title"));
    resize(500, 150);
    ForbidResize();
    GetMainLayout()->setContentsMargins(QMargins(10, 10, 10, 10));

    QHBoxLayout *LayoutTitle = new QHBoxLayout();
    GetMainLayout()->addLayout(LayoutTitle);

    ButtonPlay = new ISServiceButton(BUFFER_ICONS("AudioPlayer.Play"), LANG("AudioPlayer.Play"), this);
    ButtonPlay->setFocusPolicy(Qt::NoFocus);
    connect(ButtonPlay, &ISServiceButton::clicked, this, &ISAudioPlayerForm::PlayClicked);
    LayoutTitle->addWidget(ButtonPlay);

    ButtonPause = new ISServiceButton(BUFFER_ICONS("AudioPlayer.Pause"), LANG("AudioPlayer.Pause"), this);
    ButtonPause->setEnabled(false);
    ButtonPause->setFocusPolicy(Qt::NoFocus);
    connect(ButtonPause, &ISServiceButton::clicked, this, &ISAudioPlayerForm::PauseClicked);
    LayoutTitle->addWidget(ButtonPause);

    ButtonStop = new ISServiceButton(BUFFER_ICONS("AudioPlayer.Stop"), LANG("AudioPlayer.Stop"), this);
    ButtonStop->setEnabled(false);
    ButtonStop->setFocusPolicy(Qt::NoFocus);
    connect(ButtonStop, &ISServiceButton::clicked, this, &ISAudioPlayerForm::StopClicked);
    LayoutTitle->addWidget(ButtonStop);

    ButtonPosBack = new ISServiceButton(BUFFER_ICONS("AudioPlayer.Position.Back"), LANG("AudioPlayer.Position.Back"), this);
    ButtonPosBack->setEnabled(false);
    ButtonPosBack->setFocusPolicy(Qt::NoFocus);
    connect(ButtonPosBack, &ISServiceButton::clicked, this, &ISAudioPlayerForm::PosBackClicked);
    LayoutTitle->addWidget(ButtonPosBack);

    ButtonPosForward = new ISServiceButton(BUFFER_ICONS("AudioPlayer.Position.Forward"), LANG("AudioPlayer.Position.Forward"), this);
    ButtonPosForward->setEnabled(false);
    ButtonPosForward->setFocusPolicy(Qt::NoFocus);
    connect(ButtonPosForward, &ISServiceButton::clicked, this, &ISAudioPlayerForm::PosForwardClicked);
    LayoutTitle->addWidget(ButtonPosForward);

    LayoutTitle->addStretch();

    QHBoxLayout *LayoutPosition = new QHBoxLayout();
    GetMainLayout()->addLayout(LayoutPosition);

    LabelPosition = new QLabel("00:00 / 00:00", this);
    LayoutPosition->addWidget(LabelPosition);

    SliderPosition = new QSlider(Qt::Horizontal, this);
    LayoutPosition->addWidget(SliderPosition);

    QHBoxLayout *LayoutVolume = new QHBoxLayout();
    GetMainLayout()->addLayout(LayoutVolume);

    ButtonMute = new ISServiceButton(BUFFER_ICONS("AudioPlayer.Mute.Disabled"), LANG("AudioPlayer.Mute.Disabled"), this);
    ButtonMute->setFocusPolicy(Qt::NoFocus);
    ButtonMute->setFlat(true);
    connect(ButtonMute, &ISServiceButton::clicked, this, &ISAudioPlayerForm::MuteClicked);
    LayoutVolume->addWidget(ButtonMute);

    SliderVolume = new QSlider(Qt::Horizontal, this);
    SliderVolume->setRange(0, 100);
    SliderVolume->setValue(100);
    SliderVolume->setFixedWidth(100);
    SliderVolume->setToolTip(LANG("AudioPlayer.Volume").arg(100));
    connect(SliderVolume, &QSlider::valueChanged, this, &ISAudioPlayerForm::SliderVolumeChanged);
    LayoutVolume->addWidget(SliderVolume);

    LayoutVolume->addStretch();

    GetMainLayout()->addStretch();

    MediaPlayer = new QMediaPlayer(this, QMediaPlayer::LowLatency);
    connect(MediaPlayer, &QMediaPlayer::durationChanged, this, &ISAudioPlayerForm::DurationChanged);
    connect(MediaPlayer, static_cast<void(QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error), this, &ISAudioPlayerForm::Error);
    connect(MediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &ISAudioPlayerForm::MediaStatusChanged);
    connect(MediaPlayer, &QMediaPlayer::mutedChanged, this, &ISAudioPlayerForm::MutedChanged);
    connect(MediaPlayer, &QMediaPlayer::positionChanged, this, &ISAudioPlayerForm::PositionChanged);
    connect(MediaPlayer, &QMediaPlayer::stateChanged, this, &ISAudioPlayerForm::StateChanged);

    connect(SliderPosition, &QSlider::valueChanged, MediaPlayer, &QMediaPlayer::setPosition);
    connect(SliderPosition, &QSlider::sliderPressed, MediaPlayer, &QMediaPlayer::pause);
    connect(SliderPosition, &QSlider::sliderMoved, MediaPlayer, &QMediaPlayer::setPosition);
    connect(SliderPosition, &QSlider::sliderReleased, MediaPlayer, &QMediaPlayer::play);
}
//-----------------------------------------------------------------------------
ISAudioPlayerForm::~ISAudioPlayerForm()
{

}
//-----------------------------------------------------------------------------
ISAudioPlayerForm& ISAudioPlayerForm::Instance()
{
    static ISAudioPlayerForm AudioPlayerForm;
    return AudioPlayerForm;
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
    CloseEvent->ignore();
    if (MediaPlayer->state() == QMediaPlayer::PlayingState)
    {
        MediaPlayer->stop();
    }
    MediaPlayer->setMedia(QMediaContent());
    POINTER_DELETE(Buffer);
    POINTER_DELETE(BytePointer);
    hide();
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
void ISAudioPlayerForm::PosBackClicked()
{
    int Value = SliderPosition->value();
    Value = Value <= AUDIO_PLAYER_POS ? 0 : (Value - AUDIO_PLAYER_POS);
    SliderPosition->setValue(Value);
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::PosForwardClicked()
{
    int Value = SliderPosition->value() + AUDIO_PLAYER_POS;
    Value = Value >= MediaPlayer->duration() ? SliderPosition->maximum() : Value;
    SliderPosition->setValue(Value);
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::MuteClicked()
{
    MediaPlayer->setMuted(!MediaPlayer->isMuted());
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::SliderVolumeChanged(int Volume)
{
    SliderVolume->setToolTip(LANG("AudioPlayer.Volume").arg(Volume));
    MediaPlayer->setVolume(Volume);
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::DurationChanged(qint64 Duration)
{
    TimeDuration = QTime::fromMSecsSinceStartOfDay(Duration).toString(FORMAT_TIME_V4);
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
    ButtonMute->setToolTip(Muted ? LANG("AudioPlayer.Mute.Enabled") : LANG("AudioPlayer.Mute.Disabled"));
    ButtonMute->setIcon(Muted ? BUFFER_ICONS("AudioPlayer.Mute.Enabled") : BUFFER_ICONS("AudioPlayer.Mute.Disabled"));
    Q_UNUSED(Muted);
}
//-----------------------------------------------------------------------------
void ISAudioPlayerForm::PositionChanged(qint64 Position)
{
    LabelPosition->setText(QTime::fromMSecsSinceStartOfDay(Position).toString(FORMAT_TIME_V4) + " / " + TimeDuration);
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
        ButtonPosForward->setEnabled(true);
        ButtonPosBack->setEnabled(true);
        break;

    case QMediaPlayer::PausedState:
        ButtonPlay->setEnabled(true);
        ButtonPause->setEnabled(false);
        ButtonPosForward->setEnabled(false);
        ButtonPosBack->setEnabled(false);
        break;

    case QMediaPlayer::StoppedState:
        ButtonPlay->setEnabled(true);
        ButtonPause->setEnabled(false);
        ButtonStop->setEnabled(false);
        ButtonPosForward->setEnabled(false);
        ButtonPosBack->setEnabled(false);
        break;
    }
}
//-----------------------------------------------------------------------------
