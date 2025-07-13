#include "socket_client.h"
#include <QCoreApplication>

SocketClient::SocketClient(QObject *parent)
    : QObject(parent),
      socket(new QTcpSocket(this)),
      cinStream(stdin),
      coutStream(stdout)
{
    connect(socket, &QTcpSocket::connected, this, &SocketClient::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &SocketClient::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &SocketClient::onDisconnected);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &SocketClient::onError);
}

void SocketClient::connectToServer(const QString &host, quint16 port) {
    socket->connectToHost(host, port);
}

void SocketClient::onConnected() {
    coutStream << "Connected to Python server." << Qt::endl;
    // sendInput();
}

void SocketClient::onReadyRead() {
    QByteArray data = socket->readAll();
	emit msgFromAI(QString::fromUtf8(data));
    // coutStream << "AI: " << QString::fromUtf8(data) << Qt::endl;
    // sendInput();
}

void SocketClient::onDisconnected() {
    coutStream << "Disconnected from server." << Qt::endl;
    QCoreApplication::quit();
}

void SocketClient::onError(QAbstractSocket::SocketError socketError) {
    Q_UNUSED(socketError);
    qWarning() << "Socket error:" << socket->errorString();
}

void SocketClient::sendInput() {
    coutStream << "You: " << Qt::flush;
    QString userInput = cinStream.readLine();
    if (userInput == "q") {
        socket->disconnectFromHost();
        QCoreApplication::quit();
        return;
    }

    socket->write(userInput.toUtf8());
    socket->flush();
}

void SocketClient::userMsg(const QString& senderUserAccount, const QString& receiverUserAccount, const QString& message){
	qDebug() << message;
	socket->write(message.toUtf8());
    socket->flush();
}