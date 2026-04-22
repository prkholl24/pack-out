/**
 * @file musicmanager.cpp
 * @brief Implementation of MusicManager.
 */
#include "musicmanager.h"
#include "theme.h"

#include <QAudioOutput>
#include <QFile>
#include <QMediaPlayer>
#include <QUrl>

MusicManager::MusicManager(QObject* parent)
    : QObject(parent)
    , player(new QMediaPlayer(this))
    , output(new QAudioOutput(this))
{
    player->setAudioOutput(output);
    output->setVolume(0.35f);
    player->setLoops(QMediaPlayer::Infinite);
}

MusicManager::~MusicManager() = default;

void MusicManager::playForTheme(const LevelTheme& theme) {
    if (theme.musicPath.isEmpty()) {
        stop();
        return;
    }

    // Already playing this track — leave it alone (avoids a hitch on retries).
    if (theme.musicPath == currentPath && player->playbackState() == QMediaPlayer::PlayingState)
        return;

    // If the resource is missing, fail silently.
    if (theme.musicPath.startsWith(":/") && !QFile::exists(theme.musicPath)) {
        stop();
        return;
    }

    currentPath = theme.musicPath;
    player->stop();
    player->setSource(QUrl("qrc" + theme.musicPath));
    player->play();
}

void MusicManager::stop() {
    player->stop();
    currentPath.clear();
}

void MusicManager::setVolume(int percent) {
    output->setVolume(qBound(0, percent, 100) / 100.0f);
}
