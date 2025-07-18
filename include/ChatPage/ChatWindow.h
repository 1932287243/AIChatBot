﻿#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QListWidget>
#include <QLineEdit>
#include <QEvent>
#include <QKeyEvent>
#include <QFileInfo>
#include <QFileDialog>
#include <QTextEdit>
#include <QScrollBar>
#include <QTimer>
#include <QPushButton>
#include "global.h"
#include "StyledItemDelegate/MessageDelegate.h"
#include "Component/RoundImage.h"
#include "Component/smoothlistwidget.h"
#include "StyledItemDelegate/MessageItemWidget.h"
#include "Component/ImageViewer.h"
#include "FileQueueList.h"
class ChatTitle;
class ChatMessageEdit;
class SendMessageButton;
class QFrame;
class ChatWindow : public QWidget
{
	Q_OBJECT

public:
	explicit ChatWindow(const UserData& user_data, QWidget* parent = nullptr);
	~ChatWindow();
	UserData currentUserData()const;
	void IncreaseMessageItem(const UserData& user_data);
	void UpdateLastMessageItem(UserData& user_data);
	void dealUserSendMessage(const QString& message);
	void setChatWindowData(const UserData& user_data);
	void IncreaseMessageItemForEXE(const FileInfoData& file_data);
	void setUploadFileItemProgress(const qreal& pos);
	void updateDownloadFileItemProgress(const qreal& pos);
private:
	UserData m_userData;
	ChatTitle* chat_title = Q_NULLPTR;
	int currentChatWindowIndex = 0;
	QListWidget* chat_list = Q_NULLPTR;
	ChatMessageEdit* message_edit = Q_NULLPTR;
	FileQueueList* file_list = Q_NULLPTR;
	ImageViewer* image_viewer = Q_NULLPTR;
	QFrame* line;
	QFrame* line2;
private:
	void setupFrameStyle(QFrame* frame);
	
signals:
	void SendUserMessage(const QString& senderUserAccount, const QString& receiverUserAccount, const QString& message);
	void SendUserMessageForUserFileSignal(const QString& senderUserAccount, const QString& receiverUserAccount, const FileInfoData& file_data);
	void modifyChatListItemData(const  UserData& user_data);
	void resizeMainWindowSize(bool isShow);
};

class ChatTitle :public QWidget {
public:
	explicit ChatTitle(const QString& user_name, const QPixmap& user_head, QWidget* parent);
	~ChatTitle();
protected:
	// void paintEvent(QPaintEvent*)Q_DECL_OVERRIDE;
private:
	QLabel* userHead = Q_NULLPTR;
	QLabel* userName = Q_NULLPTR;
	QGraphicsDropShadowEffect* shadow = Q_NULLPTR;
};

class ChatMessageEdit :public QWidget {
	Q_OBJECT
public:
	explicit ChatMessageEdit(QWidget* parent = Q_NULLPTR);
	void addEmoji(const QString& image_path);
protected:
	// void paintEvent(QPaintEvent* event)Q_DECL_OVERRIDE;
	bool eventFilter(QObject* target, QEvent* event) Q_DECL_OVERRIDE;

private:
	QTextEdit* message_edit = Q_NULLPTR;
	// SendMessageButton* send_button = Q_NULLPTR
	QPushButton* send_button = Q_NULLPTR;
	QLabel* file_button = Q_NULLPTR;
	QLabel* emoji_button = Q_NULLPTR;
	QLabel* waitTransferFile_button = Q_NULLPTR;
Q_SIGNALS:
	void SendUserMessage(const QString& message);
	void MyMessageSignal(const QString& message);
	void MyMessageForFileSignal(const FileInfoData& file_data);
	void waitingFileQueueButtonClicked();
	void showImageViewSignal();
};

class SendMessageButton :public QWidget {
public:
	SendMessageButton(QWidget* parent = Q_NULLPTR);
protected:
	void paintEvent(QPaintEvent*)Q_DECL_OVERRIDE;
};
