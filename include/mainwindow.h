#pragma once

#include <QMainWindow>
#include "sideBar.h"
#include "socket_client.h"
#include "ChatPage/ChatPage.h"
#include "global.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( int argc, char **argv, QWidget *parent = nullptr);
    ~MainWindow();
  
private:
	void initChatPage(UserData& user_data);
    std::unique_ptr<SideBar> sideBar;
	std::unique_ptr<ChatPage> chatPage;
	std::unique_ptr<SocketClient> client;
};