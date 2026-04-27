/**
 * @file musicmanager.cpp
 * @brief Implementation of MusicManager.
 */
#include "musicmanager.h"
#include "theme.h"

#include <QAudioOutput>
#include <QDebug>
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
    qDebug() << "[Music] playForTheme called, path:" << theme.musicPath;

    if (theme.musicPath.isEmpty()) {
        qDebug() << "[Music] Empty path, stopping";
        stop();
        return;
    }

    // Already playing this track — leave it alone (avoids a hitch on retries).
    if (theme.musicPath == currentPath && player->playbackState() == QMediaPlayer::PlayingState) {
        qDebug() << "[Music] Already playing, skipping";
        return;
    }

    // If the resource is missing, fail silently.
    if (theme.musicPath.startsWith(":/") && !QFile::exists(theme.musicPath)) {
        qDebug() << "[Music] Resource missing:" << theme.musicPath;
        stop();
        return;
    }

    currentPath = theme.musicPath;
    player->stop();
    QUrl url("qrc" + theme.musicPath);
    qDebug() << "[Music] Playing:" << url;
    player->setSource(url);
    player->play();
}

void MusicManager::stop() {
    player->stop();
    currentPath.clear();
}

void MusicManager::setVolume(int percent) {
    output->setVolume(qBound(0, percent, 100) / 100.0f);
}
