#pragma once

#include <QMainWindow>
#include <QMediaPlayer>
#include <QStringList>

class QLabel;
class QListWidget;
class QListWidgetItem;
class QSlider;
class QToolButton;
class QVideoWidget;

class VideoBrowserWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit VideoBrowserWindow(QWidget* parent = nullptr);
    ~VideoBrowserWindow() override = default;

private slots:
    void openFolder();
    void openVideo();
    void playSelected();
    void togglePlayback();
    void playPrevious();
    void playNext();
    void handlePlaylistSelection(QListWidgetItem* item);
    void updatePosition(qint64 position);
    void updateDuration(qint64 duration);
    void handleMediaStatus(QMediaPlayer::MediaStatus status);
    void handlePlayerError(QMediaPlayer::Error error);

private:
    void buildUi();
    void applyTheme();
    void scanFolder(const QString& path);
    void addVideoFile(const QString& path);
    void loadVideo(const QString& path);
    void updatePlaybackButton();
    void updateTimeLabels();
    void updateLibrarySummary();
    QString formatTime(qint64 milliseconds) const;

    QMediaPlayer* m_player;
    QVideoWidget* m_videoWidget;
    QListWidget* m_playlist;
    QLabel* m_titleLabel;
    QLabel* m_statusLabel;
    QLabel* m_currentTimeLabel;
    QLabel* m_totalTimeLabel;
    QLabel* m_librarySummary;
    QSlider* m_progressSlider;
    QSlider* m_volumeSlider;
    QToolButton* m_playButton;
    QToolButton* m_previousButton;
    QToolButton* m_nextButton;
    QStringList m_videoFiles;
    bool m_isSeeking;
};
