#include "window.hpp"

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMediaPlayer>
#include <QPushButton>
#include <QSlider>
#include <QStyle>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>
#include <QVideoWidget>

namespace
{
const QStringList kVideoExtensions = {
    QStringLiteral("*.mp4"), QStringLiteral("*.mkv"), QStringLiteral("*.avi"),
    QStringLiteral("*.mov"), QStringLiteral("*.wmv"), QStringLiteral("*.webm"),
    QStringLiteral("*.m4v")
};

QToolButton* makeToolButton(const QString& text, const QString& objectName, QWidget* parent)
{
    auto* button = new QToolButton(parent);
    button->setText(text);
    button->setObjectName(objectName);
    button->setToolTip(text);
    button->setCursor(Qt::PointingHandCursor);
    button->setAutoRaise(true);
    return button;
}
}

VideoBrowserWindow::VideoBrowserWindow(QWidget* parent)
    : QMainWindow(parent),
      m_player(new QMediaPlayer(this)),
      m_videoWidget(new QVideoWidget(this)),
      m_playlist(new QListWidget(this)),
      m_titleLabel(new QLabel(tr("选择一个视频开始播放"), this)),
      m_statusLabel(new QLabel(tr("媒体库为空"), this)),
      m_currentTimeLabel(new QLabel(QStringLiteral("00:00"), this)),
      m_totalTimeLabel(new QLabel(QStringLiteral("00:00"), this)),
      m_librarySummary(new QLabel(this)),
      m_progressSlider(new QSlider(Qt::Horizontal, this)),
      m_volumeSlider(new QSlider(Qt::Horizontal, this)),
      m_playButton(nullptr),
      m_previousButton(nullptr),
      m_nextButton(nullptr),
      m_isSeeking(false)
{
    buildUi();
    applyTheme();

    m_player->setVideoOutput(m_videoWidget);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(75);
    m_player->setVolume(75);

    connect(m_player, &QMediaPlayer::positionChanged, this, &VideoBrowserWindow::updatePosition);
    connect(m_player, &QMediaPlayer::durationChanged, this, &VideoBrowserWindow::updateDuration);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &VideoBrowserWindow::handleMediaStatus);
    connect(m_player, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            this, &VideoBrowserWindow::handlePlayerError);
    connect(m_playlist, &QListWidget::itemDoubleClicked, this,
            &VideoBrowserWindow::handlePlaylistSelection);
    connect(m_playlist, &QListWidget::itemClicked, this,
            &VideoBrowserWindow::handlePlaylistSelection);
    connect(m_volumeSlider, &QSlider::valueChanged, m_player, &QMediaPlayer::setVolume);
    connect(m_progressSlider, &QSlider::sliderPressed, this, [this]() { m_isSeeking = true; });
    connect(m_progressSlider, &QSlider::sliderReleased, this, [this]() {
        m_isSeeking = false;
        m_player->setPosition(m_progressSlider->value());
    });
    connect(m_progressSlider, &QSlider::sliderMoved, this, [this](int value) {
        m_currentTimeLabel->setText(formatTime(value));
    });

    resize(1240, 780);
    setMinimumSize(980, 620);
    setWindowTitle(tr("Video Browser"));
    setWindowIcon(style()->standardIcon(QStyle::SP_MediaPlay));
}

void VideoBrowserWindow::buildUi()
{
    auto* central = new QWidget(this);
    auto* rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(22, 18, 22, 18);
    rootLayout->setSpacing(14);

    auto* headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(12);
    auto* brand = new QLabel(tr("VIDEO BROWSER"), this);
    brand->setObjectName(QStringLiteral("brand"));
    headerLayout->addWidget(brand);
    headerLayout->addStretch();

    auto* openFolderButton = new QPushButton(tr("打开文件夹"), this);
    auto* openFileButton = new QPushButton(tr("打开视频"), this);
    openFolderButton->setObjectName(QStringLiteral("secondaryButton"));
    openFileButton->setObjectName(QStringLiteral("primaryButton"));
    openFolderButton->setCursor(Qt::PointingHandCursor);
    openFileButton->setCursor(Qt::PointingHandCursor);
    headerLayout->addWidget(openFolderButton);
    headerLayout->addWidget(openFileButton);
    rootLayout->addLayout(headerLayout);

    auto* contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(16);

    auto* libraryPanel = new QWidget(this);
    libraryPanel->setObjectName(QStringLiteral("libraryPanel"));
    libraryPanel->setMinimumWidth(265);
    libraryPanel->setMaximumWidth(330);
    auto* libraryLayout = new QVBoxLayout(libraryPanel);
    libraryLayout->setContentsMargins(16, 16, 16, 16);
    libraryLayout->setSpacing(10);
    auto* libraryTitle = new QLabel(tr("媒体库"), libraryPanel);
    libraryTitle->setObjectName(QStringLiteral("sectionTitle"));
    libraryLayout->addWidget(libraryTitle);
    m_librarySummary->setObjectName(QStringLiteral("mutedText"));
    libraryLayout->addWidget(m_librarySummary);
    libraryLayout->addSpacing(4);
    m_playlist->setObjectName(QStringLiteral("playlist"));
    m_playlist->setIconSize(QSize(22, 22));
    m_playlist->setSpacing(4);
    m_playlist->setSelectionMode(QAbstractItemView::SingleSelection);
    libraryLayout->addWidget(m_playlist, 1);
    contentLayout->addWidget(libraryPanel);

    auto* playerPanel = new QWidget(this);
    auto* playerLayout = new QVBoxLayout(playerPanel);
    playerLayout->setContentsMargins(0, 0, 0, 0);
    playerLayout->setSpacing(12);
    m_videoWidget->setObjectName(QStringLiteral("videoSurface"));
    m_videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
    playerLayout->addWidget(m_videoWidget, 1);

    auto* titleLayout = new QHBoxLayout();
    titleLayout->addWidget(m_titleLabel);
    titleLayout->addStretch();
    m_statusLabel->setObjectName(QStringLiteral("mutedText"));
    titleLayout->addWidget(m_statusLabel);
    playerLayout->addLayout(titleLayout);

    auto* timelineLayout = new QHBoxLayout();
    timelineLayout->setSpacing(8);
    timelineLayout->addWidget(m_currentTimeLabel);
    m_progressSlider->setRange(0, 0);
    m_progressSlider->setObjectName(QStringLiteral("progressSlider"));
    timelineLayout->addWidget(m_progressSlider, 1);
    timelineLayout->addWidget(m_totalTimeLabel);
    playerLayout->addLayout(timelineLayout);

    auto* controlsLayout = new QHBoxLayout();
    controlsLayout->setSpacing(8);
    m_previousButton = makeToolButton(QStringLiteral("|<"), QStringLiteral("controlButton"), this);
    m_playButton = makeToolButton(QStringLiteral("播放"), QStringLiteral("playButton"), this);
    m_nextButton = makeToolButton(QStringLiteral(">|"), QStringLiteral("controlButton"), this);
    controlsLayout->addWidget(m_previousButton);
    controlsLayout->addWidget(m_playButton);
    controlsLayout->addWidget(m_nextButton);
    controlsLayout->addSpacing(12);
    auto* volumeLabel = new QLabel(tr("音量"), this);
    volumeLabel->setObjectName(QStringLiteral("mutedText"));
    controlsLayout->addWidget(volumeLabel);
    m_volumeSlider->setFixedWidth(120);
    controlsLayout->addWidget(m_volumeSlider);
    controlsLayout->addStretch();
    playerLayout->addLayout(controlsLayout);

    contentLayout->addWidget(playerPanel, 1);
    rootLayout->addLayout(contentLayout, 1);
    setCentralWidget(central);

    connect(openFolderButton, &QPushButton::clicked, this, &VideoBrowserWindow::openFolder);
    connect(openFileButton, &QPushButton::clicked, this, &VideoBrowserWindow::openVideo);
    connect(m_playButton, &QToolButton::clicked, this, &VideoBrowserWindow::togglePlayback);
    connect(m_previousButton, &QToolButton::clicked, this, &VideoBrowserWindow::playPrevious);
    connect(m_nextButton, &QToolButton::clicked, this, &VideoBrowserWindow::playNext);
}

void VideoBrowserWindow::applyTheme()
{
    setStyleSheet(QStringLiteral(
        "QMainWindow, QWidget { background: #111315; color: #f1f3f4; font-family: 'Segoe UI', 'Microsoft YaHei'; }"
        "#brand { color: #f5c451; font-size: 18px; font-weight: 700; }"
        "#libraryPanel { background: #191c1f; border: 1px solid #292e33; border-radius: 8px; }"
        "#videoSurface { background: #08090a; border: 1px solid #292e33; border-radius: 8px; }"
        "#sectionTitle { font-size: 17px; font-weight: 650; }"
        "#mutedText { color: #8e989f; font-size: 12px; }"
        "#playlist { background: transparent; border: none; outline: none; }"
        "#playlist::item { padding: 11px 9px; border-radius: 5px; color: #cbd0d3; }"
        "#playlist::item:hover { background: #242a2e; }"
        "#playlist::item:selected { background: #3c3320; color: #f5c451; }"
        "QPushButton { border: 1px solid #353c42; border-radius: 5px; padding: 9px 15px; background: #20252a; color: #e9ecee; }"
        "QPushButton:hover { background: #2c3338; border-color: #5b646b; }"
        "#primaryButton { background: #f5c451; border-color: #f5c451; color: #181818; font-weight: 650; }"
        "#primaryButton:hover { background: #ffd56d; }"
        "#progressSlider::groove:horizontal { height: 4px; background: #343a3f; border-radius: 2px; }"
        "#progressSlider::sub-page:horizontal { background: #f5c451; border-radius: 2px; }"
        "#progressSlider::handle:horizontal { width: 12px; height: 12px; margin: -4px 0; background: #f5c451; border-radius: 6px; }"
        "QSlider::groove:horizontal { height: 4px; background: #343a3f; border-radius: 2px; }"
        "QSlider::sub-page:horizontal { background: #87939b; border-radius: 2px; }"
        "QSlider::handle:horizontal { width: 11px; height: 11px; margin: -3px 0; background: #dbe0e3; border-radius: 5px; }"
        "#controlButton, #playButton { border: none; background: transparent; }"
        "#controlButton { color: #abb4ba; font-size: 16px; padding: 5px 8px; }"
        "#controlButton:hover { color: #ffffff; }"
        "#playButton { color: #f5c451; font-weight: 650; padding: 6px 14px; }"
        "#playButton:hover { color: #ffd56d; }"));
}

void VideoBrowserWindow::openFolder()
{
    const QString path = QFileDialog::getExistingDirectory(this, tr("选择视频文件夹"), QDir::homePath());
    if (!path.isEmpty())
        scanFolder(path);
}

void VideoBrowserWindow::openVideo()
{
    const QString path = QFileDialog::getOpenFileName(
        this, tr("打开视频"), QDir::homePath(),
        tr("视频文件 (*.mp4 *.mkv *.avi *.mov *.wmv *.webm *.m4v)"));
    if (!path.isEmpty()) {
        addVideoFile(path);
        loadVideo(path);
    }
}

void VideoBrowserWindow::scanFolder(const QString& path)
{
    m_playlist->clear();
    m_videoFiles.clear();
    const QDir directory(path);
    const QFileInfoList files =
        directory.entryInfoList(kVideoExtensions, QDir::Files, QDir::Name | QDir::IgnoreCase);
    for (const QFileInfo& file : files)
        addVideoFile(file.absoluteFilePath());

    updateLibrarySummary();
    if (!m_videoFiles.isEmpty()) {
        m_playlist->setCurrentRow(0);
        loadVideo(m_videoFiles.first());
    } else {
        m_titleLabel->setText(tr("没有找到视频文件"));
        m_statusLabel->setText(tr("请选择其他文件夹"));
    }
}

void VideoBrowserWindow::addVideoFile(const QString& path)
{
    if (m_videoFiles.contains(path))
        return;
    m_videoFiles.append(path);
    auto* item = new QListWidgetItem(QFileInfo(path).fileName(), m_playlist);
    item->setToolTip(path);
    item->setData(Qt::UserRole, path);
    item->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    updateLibrarySummary();
}

void VideoBrowserWindow::loadVideo(const QString& path)
{
    const int index = m_videoFiles.indexOf(path);
    if (index >= 0 && m_playlist->currentRow() != index)
        m_playlist->setCurrentRow(index);
    m_player->setMedia(QUrl::fromLocalFile(path));
    m_titleLabel->setText(QFileInfo(path).completeBaseName());
    m_statusLabel->setText(tr("准备播放"));
    m_progressSlider->setValue(0);
    m_totalTimeLabel->setText(QStringLiteral("00:00"));
    m_currentTimeLabel->setText(QStringLiteral("00:00"));
    m_player->play();
    updatePlaybackButton();
}

void VideoBrowserWindow::playSelected()
{
    const auto* item = m_playlist->currentItem();
    if (item != nullptr)
        loadVideo(item->data(Qt::UserRole).toString());
}

void VideoBrowserWindow::togglePlayback()
{
    if (m_player->mediaStatus() == QMediaPlayer::NoMedia) {
        playSelected();
        return;
    }
    if (m_player->state() == QMediaPlayer::PlayingState)
        m_player->pause();
    else
        m_player->play();
    updatePlaybackButton();
}

void VideoBrowserWindow::playPrevious()
{
    if (m_videoFiles.isEmpty())
        return;
    const int row = (m_playlist->currentRow() - 1 + m_videoFiles.size()) % m_videoFiles.size();
    m_playlist->setCurrentRow(row);
    playSelected();
}

void VideoBrowserWindow::playNext()
{
    if (m_videoFiles.isEmpty())
        return;
    const int row = (m_playlist->currentRow() + 1) % m_videoFiles.size();
    m_playlist->setCurrentRow(row);
    playSelected();
}

void VideoBrowserWindow::handlePlaylistSelection(QListWidgetItem* item)
{
    if (item != nullptr)
        loadVideo(item->data(Qt::UserRole).toString());
}

void VideoBrowserWindow::updatePosition(qint64 position)
{
    if (!m_isSeeking)
        m_progressSlider->setValue(static_cast<int>(position));
    updateTimeLabels();
}

void VideoBrowserWindow::updateDuration(qint64 duration)
{
    m_progressSlider->setRange(0, static_cast<int>(duration));
    m_totalTimeLabel->setText(formatTime(duration));
    updateTimeLabels();
}

void VideoBrowserWindow::handleMediaStatus(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia || status == QMediaPlayer::BufferedMedia) {
        m_statusLabel->setText(tr("正在播放"));
        updatePlaybackButton();
    } else if (status == QMediaPlayer::EndOfMedia) {
        m_statusLabel->setText(tr("播放结束"));
        updatePlaybackButton();
    }
}

void VideoBrowserWindow::handlePlayerError(QMediaPlayer::Error error)
{
    if (error == QMediaPlayer::NoError)
        return;
    m_statusLabel->setText(tr("播放失败：%1").arg(m_player->errorString()));
    m_playButton->setText(QStringLiteral("播放"));
}

void VideoBrowserWindow::updatePlaybackButton()
{
    m_playButton->setText(m_player->state() == QMediaPlayer::PlayingState
                              ? QStringLiteral("暂停")
                              : QStringLiteral("播放"));
}

void VideoBrowserWindow::updateTimeLabels()
{
    m_currentTimeLabel->setText(formatTime(m_player->position()));
}

QString VideoBrowserWindow::formatTime(qint64 milliseconds) const
{
    const qint64 totalSeconds = milliseconds / 1000;
    return QStringLiteral("%1:%2")
        .arg(totalSeconds / 60, 2, 10, QLatin1Char('0'))
        .arg(totalSeconds % 60, 2, 10, QLatin1Char('0'));
}

void VideoBrowserWindow::updateLibrarySummary()
{
    m_librarySummary->setText(tr("%1 个视频").arg(m_videoFiles.size()));
}
