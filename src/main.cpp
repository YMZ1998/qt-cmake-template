#include "window.h"

#include <QApplication>
#include <QByteArray>
#include <QCoreApplication>
#include <QtGlobal>

int main(int argc, char* argv[])
{
    // Prefer Windows Media Foundation over the legacy DirectShow backend.
    qputenv("QT_MULTIMEDIA_PREFERRED_PLUGINS", QByteArrayLiteral("windowsmediafoundation"));

    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Video Browser"));
    app.setOrganizationName(QStringLiteral("Qt Demo"));

    VideoBrowserWindow window;
    window.show();

    return app.exec();
}
