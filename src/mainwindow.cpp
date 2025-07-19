#include "mainwindow.h"
#include <QHBoxLayout>
#include <QBuffer>

MainWindow::MainWindow( int argc, char **argv, QWidget *parent){

	this->setWindowOpacity(0.96);
	this->setMinimumSize(1440, 960);
	this->showMaximized();

	// 创建中心窗口部件
	QWidget *centralWidget = new QWidget(this);

	QHBoxLayout* mainLayout = new QHBoxLayout;
	mainLayout->setContentsMargins(0, 0, 0, 0);

	client = std::make_unique<SocketClient>();
	client->connectToServer("127.0.0.1", 8888);

	struct UserData user_data;
	initChatPage(user_data);
	chatPage = std::make_unique<ChatPage>();
	chatPage->CreateChatWindow(user_data);
	chatPage->setCurrentChatWindow(user_data);

	sideBar = std::make_unique<SideBar>();

	mainLayout->addWidget(sideBar.get());
	mainLayout->addWidget(chatPage.get());

	// 将布局设置到中心部件上
	centralWidget->setLayout(mainLayout);

	// 设置中心部件
	this->setCentralWidget(centralWidget);

	connect(client.get(), &SocketClient::msgFromAI, chatPage.get(), &ChatPage::receiveAIMsg);
	connect(chatPage.get(), &ChatPage::SendUserMessage, client.get(), &SocketClient::userMsg);
	connect(sideBar.get(), &SideBar::sendSidebarIndex, chatPage.get(), &ChatPage::receiveSidebarIndex);
}

MainWindow::~MainWindow(){

}

void MainWindow::initChatPage(UserData& user_data){
	QPixmap pixmap;
	if (!pixmap.load(":/Resource/ico/user.png")) {
		qWarning("Failed to load image");
	}

	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	buffer.open(QIODevice::WriteOnly);
	pixmap.save(&buffer, "PNG");
	GLOB_UserHeadImagebytes = byteArray;

	user_data.userHead = QPixmap(":/Resource/ico/ai_boot.png");
	user_data.userName = QString("0");
	user_data.userAccount = "0";
	user_data.receiverUserAccount = "1234";
	user_data.status = false;
	user_data.status_ico = QPixmap(":/Resource/ico/ai_boot.png");
	user_data.status_text = "hello";
	user_data.userMessage = "我是重庆大学领域专用芯片及智能系统实验室的小虫子，很高兴见到你！";
	user_data.index = 0;
}