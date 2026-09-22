#include "window.h"

#include <QApplication>
#include <QByteArray>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QtGlobal>

int main(int argc, char* argv[])
{
    // Make plugins discoverable when launched from another working directory.
    const QString executableDir = QFileInfo(QString::fromLocal8Bit(argv[0])).absolutePath();
    QCoreApplication::setLibraryPaths(QStringList() << executableDir);
    qputenv("QT_QPA_PLATFORM_PLUGIN_PATH",
            QDir::toNativeSeparators(executableDir + QStringLiteral("/platforms")).toLocal8Bit());

    // Prefer Windows Media Foundation over the legacy DirectShow backend.
    qputenv("QT_MULTIMEDIA_PREFERRED_PLUGINS", QByteArrayLiteral("windowsmediafoundation"));

    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Video Browser"));
    app.setOrganizationName(QStringLiteral("Qt Demo"));

    VideoBrowserWindow window;
    window.show();

    return app.exec();
}
