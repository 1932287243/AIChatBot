#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTextStream>

class SocketClient : public QObject {
    Q_OBJECT

public:
    explicit SocketClient(QObject *parent = nullptr);
    void connectToServer(const QString &host, quint16 port);
	void userMsg(const QString& senderUserAccount, const QString& receiverUserAccount, const QString& message);

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void sendInput();

private:
    QTcpSocket *socket;
    QTextStream cinStream;
    QTextStream coutStream;

signals:
	void msgFromAI(const QString& msg);

};

