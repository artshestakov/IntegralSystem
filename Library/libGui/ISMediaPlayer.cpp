#include "ISMediaPlayer.h"
#include "ISConstants.h"
#include "ISSettings.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISMediaPlayer::ISMediaPlayer() : QMediaPlayer()
{
	connect(this, &ISMediaPlayer::stateChanged, this, &ISMediaPlayer::StateChanged);
}
//-----------------------------------------------------------------------------
ISMediaPlayer::~ISMediaPlayer()
{

}
//-----------------------------------------------------------------------------
ISMediaPlayer& ISMediaPlayer::GetInstance()
{
	static ISMediaPlayer MediaPlayer;
	return MediaPlayer;
}
//-----------------------------------------------------------------------------
void ISMediaPlayer::Play(const QString &BufferAudio)
{
	if (state() == QMediaPlayer::StoppedState)
	{
		QString Path = BufferAudio;
		Path.remove(0, 1);
		Path.insert(0, "qrc:/");

		setMedia(QUrl(Path));
		setVolume(SETTING_INT(CONST_UID_SETTING_SOUND_VOLUME));
		play();
	}
	else
	{
		Queue.emplace_back(BufferAudio);
	}
}
//-----------------------------------------------------------------------------
void ISMediaPlayer::StateChanged(QMediaPlayer::State NewState)
{
	if (!Queue.empty())
	{
		Play(VectorTakeFront(Queue));
	}
}
//-----------------------------------------------------------------------------
