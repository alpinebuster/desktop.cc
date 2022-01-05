#include "url.h"
#include "temporaryfile.h"

#include <QHostAddress>
#include <QTcpServer>

namespace Utils {

QUrl urlFromLocalHostAndFreePort()
{
    QUrl serverUrl;
    QTcpServer server;
    serverUrl.setScheme(urlTcpScheme());
    if (server.listen(QHostAddress::LocalHost) || server.listen(QHostAddress::LocalHostIPv6)) {
        serverUrl.setHost(server.serverAddress().toString());
        serverUrl.setPort(server.serverPort());
    }
    return serverUrl;
}

QUrl urlFromLocalSocket()
{
    QUrl serverUrl;
    serverUrl.setScheme(urlSocketScheme());
    TemporaryFile file("qtcreator-freesocket");
    if (file.open())
        serverUrl.setPath(file.fileName());
    return serverUrl;
}

QString urlSocketScheme()
{
    return QString("socket");
}

QString urlTcpScheme()
{
    return QString("tcp");
}

}
