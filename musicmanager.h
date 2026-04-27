/**
 * @file musicmanager.h
 * @brief Thin wrapper around QMediaPlayer for per-level background music.
 *
 * Silently no-ops if a theme has no music path or the resource is
 * missing so missing assets never crash the game.
 */
#ifndef MUSICMANAGER_H
#define MUSICMANAGER_H

#include <QObject>
#include <QString>

class QMediaPlayer;
class QAudioOutput;
struct LevelTheme;

class MusicManager : public QObject {
    Q_OBJECT
public:
    explicit MusicManager(QObject* parent = nullptr);
    ~MusicManager() override;

    /**
     * @brief Play the track associated with a theme, looping.
     *
     * If the theme's musicPath is empty or the resource does not exist,
     * stops any current track and does nothing else.
     */
    void playForTheme(const LevelTheme& theme);

    /// Stop playback immediately.
    void stop();

    /// Set master volume (0 – 100).
    void setVolume(int percent);

private:
    QMediaPlayer* player;
    QAudioOutput* output;
    QString currentPath;
};

#endif // MUSICMANAGER_H
