#include <QtWidgets/QApplication>
#include <QScreen>
#include <QFontDatabase>

#include <QPixmap>
#include <QBuffer>
#include <QByteArray>

#include "socket_client.h"
#include "ChatPage/ChatPage.h"
#include "global.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	
	QApplication::setApplicationVersion("1.0");
	int font_id = QFontDatabase::addApplicationFont(":/Resource/font/flache.ttf");
	QString family = QFontDatabase::applicationFontFamilies(font_id).at(0);
	QFont font;
	font.setHintingPreference(QFont::PreferNoHinting);
	font.setFamily(family);
	a.setFont(font);

	QPixmap pixmap;
	if (!pixmap.load(":/Resource/ico/user.png")) {
		qWarning("Failed to load image");
		return {};
	}

	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	buffer.open(QIODevice::WriteOnly);
	pixmap.save(&buffer, "PNG");
	GLOB_UserHeadImagebytes = byteArray;

	struct UserData user_data;
	user_data.userHead = QPixmap(":/Resource/ico/ai_boot.png");
	user_data.userName = QString("0");
	user_data.userAccount = "0";
	user_data.receiverUserAccount = "1234";
	user_data.status = false;
	user_data.status_ico = QPixmap(":/Resource/ico/ai_boot.png");
	user_data.status_text = "hello";
	user_data.userMessage = "我是AI助手，很高兴见到你！";
	user_data.index = 0;
	
	SocketClient client;
	client.connectToServer("127.0.0.1", 8888); // 改为你的 Python 服务器 IP 和端口

	ChatPage chat_page;
	chat_page.setWindowOpacity(0.96);
	chat_page.CreateChatWindow(user_data);
	chat_page.show();
	QObject::connect(&client, &SocketClient::msgFromAI, &chat_page, &ChatPage::receiveAIMsg);
	QObject::connect(&chat_page, &ChatPage::SendUserMessage, &client, &SocketClient::userMsg);

	return a.exec();
}
