#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QPainter>
#include <QStackedLayout>
#include "FriendChatList.h"
#include "DefaultWindow.h"
#include "ChatWindow.h"
class ChatPage : public QWidget
{
	Q_OBJECT

public:
	ChatPage(QWidget* parent = nullptr);
	~ChatPage();
public:
	void CreateChatWindow(UserData& user_data);
	void CreateChatWindow1();
	void receiveAIMsg(const QString& msg);
	void DoubleClickCreateChatWindow(UserData& user_data);
	void setCurrentChatWindow(const UserData& user_data);
	void itemChanged(UserData& user_data);
	void setUploadFileItemProgress(const qreal& pos);
	void updateDownloadFileProgress(const qreal& pos);
	void modifyChatListItemData(const UserData & user_data);

protected:
	void paintEvent(QPaintEvent*)Q_DECL_OVERRIDE;
private:
	FriendChatList* friendChat_list = Q_NULLPTR;
	DefaultWindow* default_window = Q_NULLPTR;
	QStackedLayout* stack_layout = Q_NULLPTR;
	UserData user_data;
	// QString AIMessage;
	// UserData user_data;
	QMap<QListWidgetItem*, ChatWindow*>  ChatItemAndChatWindow;
	bool ai_flag = false;
	// void userMsg(const QString& senderUserAccount, const QString& receiverUserAccount, const QString& message);
// private slots:
// 	void 

signals:
	void SendUserMessage(const QString& senderUserAccount, const QString& receiverUserAccount, const QString& message);
	void SendUserMessageForUserFileSignal(const QString& senderUserAccount, const QString& receiverUserAccount, const FileInfoData& file_data);
	void setFileItemProgressSignal(const qreal pos);
	void updateDownloadFileProgressSignal(const qreal& pos);
	void resizeMainWindowSize(bool isShow);
};
