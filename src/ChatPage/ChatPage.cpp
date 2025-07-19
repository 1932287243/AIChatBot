#include "ChatPage/ChatPage.h"


ChatPage::ChatPage(QWidget* parent)
	: QWidget(parent)
{
	GLOB_ScaleDpi = this->devicePixelRatioF();
	QPixmap pixmap;
	if (!pixmap.load(":/Resource/ico/ai_boot.png")) {
		qWarning("Failed to load image");
	}
		
	user_data.userHead = pixmap;
	user_data.userName = QString("PW");

	user_data.receiverUserAccount = "1234";
	user_data.status = false;
	user_data.status_ico = pixmap;
	user_data.status_text = "hello";
	user_data.userMessage = "我是重庆大学领域专用芯片及智能系统实验室的小虫子，很高兴见到你！";
	user_data.index = 1;

	QHBoxLayout* main_lay = new QHBoxLayout(this);
	main_lay->setContentsMargins(0, 0, 0, 0);
	
	this->setLayout(main_lay);

	this->friendChat_list = new ChatMessageList(this);
	this->default_window = new DefaultWindow(this);

	QWidget *stackedContainer = new QWidget;
	this->stack_layout = new QStackedLayout;
	this->stack_layout->setContentsMargins(250, 0, 250, 0);
	this->stack_layout->addWidget(this->default_window);
	stackedContainer->setLayout(this->stack_layout);

	// main_lay->addLayout(this->stack_layout, 4);

	S_FriendPageWindow = new QSplitter(Qt::Horizontal, this);
	S_FriendPageWindow->setHandleWidth(1);

	S_FriendPageWindow->addWidget(this->friendChat_list);
	S_FriendPageWindow->addWidget(stackedContainer);

	S_FriendPageWindow->setSizes({0, INT_MAX}); 
	initialSizes = S_FriendPageWindow->sizes();

	main_lay->addWidget(S_FriendPageWindow);

	connect(this->friendChat_list, &ChatMessageList::FriendChatItemChanged, this, &ChatPage::setCurrentChatWindow, Qt::DirectConnection);
	connect(this->friendChat_list, &ChatMessageList::sendCreateNewSession, this, &ChatPage::CreateChatWindow1, Qt::DirectConnection);
	connect(this->friendChat_list, &ChatMessageList::sendCloseMeassageList, [&](){
		S_FriendPageWindow->setSizes({0, INT_MAX}); 
	});
}

ChatPage::~ChatPage()
{
}

void ChatPage::receiveSidebarIndex(int index){
	if(index == 1){
		S_FriendPageWindow->setSizes(QList<int>() << 300 << 1000);
	}
	if(index == 2){
		CreateChatWindow1();
	}
}

/**
 * @brief 收到好友消息时创建聊天 和聊天窗口
 * @param user_data
 */
void ChatPage::CreateChatWindow(UserData& user_data)
{
	//判断是否已存在此聊天窗口
	int isExist = this->friendChat_list->isExistFriendChatItem(user_data.userAccount);
	if (isExist != -1) {
		this->friendChat_list->setItemData(isExist, user_data);
		ChatWindow* chat_window;
		for (int i = 1; i < this->stack_layout->count(); i++) {
			chat_window = qobject_cast<ChatWindow*>(this->stack_layout->widget(i));
			if (chat_window) {
				if (chat_window->currentUserData().userAccount == user_data.userAccount) {
					chat_window->IncreaseMessageItem(user_data);
					return;
				}
			}
		}
	}
	user_data.index = this->stack_layout->count();
	ChatWindow* chat_window = new ChatWindow(user_data, this);
	ListWidgetItem* item = new ListWidgetItem();
	if (user_data.messageType == ChatMessageType::USERFILE) {
		user_data.userMessage = "[文件]" + user_data.fileInfo.fileName;
	}
	user_data.isUnread = true;
	user_data.unReadMessageNums++;
	item->setData(Qt::UserRole, QVariant::fromValue(user_data));
	this->ChatItemAndChatWindow.insert(item, chat_window);
	this->friendChat_list->increaseFriendItem(item);
	this->stack_layout->addWidget(chat_window);
	for (int i = 1; i < this->stack_layout->count(); i++) {
		chat_window = qobject_cast<ChatWindow*>(this->stack_layout->widget(i));
		if (chat_window) {
			if (chat_window->currentUserData().userAccount == user_data.userAccount) {
				chat_window->IncreaseMessageItem(user_data);
				break;
			}
		}
	}
	connect(chat_window, &ChatWindow::SendUserMessage, this, &ChatPage::SendUserMessage, Qt::DirectConnection);
	// connect(chat_window, &ChatWindow::SendUserMessage, this, &ChatPage::userMsg, Qt::DirectConnection);
	connect(chat_window, &ChatWindow::SendUserMessageForUserFileSignal, this, &ChatPage::SendUserMessageForUserFileSignal, Qt::DirectConnection);
	connect(this, &ChatPage::setFileItemProgressSignal, chat_window, &ChatWindow::setUploadFileItemProgress, Qt::DirectConnection);
	connect(this, &ChatPage::updateDownloadFileProgressSignal, chat_window, &ChatWindow::updateDownloadFileItemProgress, Qt::DirectConnection);
	connect(chat_window, &ChatWindow::modifyChatListItemData, this, &ChatPage::modifyChatListItemData, Qt::DirectConnection);
	connect(chat_window, &ChatWindow::resizeMainWindowSize, this, &ChatPage::resizeMainWindowSize,Qt::DirectConnection);
}

void ChatPage::receiveAIMsg(const QString& msg){
	bool first_flag = false;
	if(msg == "0x8a8a8a8a"){
		// qDebug() << "start";
		user_data.userMessage = "";
		ai_flag = true;
		first_flag = true;
	}

	if(msg == "0xa8a8a8a8"){
		// qDebug() << "end";
		ai_flag = false;
		user_data.userMessage = "我是重庆大学领域专用芯片及智能系统实验室的小虫子，很高兴见到你！";
		return;
	}

	if(!first_flag){
		if(ai_flag)
			user_data.userMessage += msg;
		else
			user_data.userMessage = msg;
	}

	QString cur_session = QString("%1").arg(this->stack_layout->currentIndex()-1);

	int isExist = this->friendChat_list->isExistFriendChatItem(cur_session);
	// qDebug() << "isExist:" << isExist;

	ChatWindow* chat_window;
	for (int i = 1; i < this->stack_layout->count(); i++) {
		chat_window = qobject_cast<ChatWindow*>(this->stack_layout->widget(i));
		if (chat_window) {
			if (chat_window->currentUserData().userAccount == cur_session) {
				if(ai_flag && (!first_flag))
					chat_window->UpdateLastMessageItem(user_data);
				else
					chat_window->IncreaseMessageItem(user_data);

				return;
			}
		}
	}
}

void ChatPage::CreateChatWindow1()
{
	user_data.userAccount = QString("%1").arg(user_data.index);
	user_data.userName = QString("%1").arg(user_data.index);

	user_data.index = this->stack_layout->count();
	ChatWindow* chat_window = new ChatWindow(user_data, this);
	ListWidgetItem* item = new ListWidgetItem();
	if (user_data.messageType == ChatMessageType::USERFILE) {
		user_data.userMessage = "[文件]" + user_data.fileInfo.fileName;
	}
	user_data.isUnread = true;
	user_data.unReadMessageNums++;

	item->setData(Qt::UserRole, QVariant::fromValue(user_data));
	this->ChatItemAndChatWindow.insert(item, chat_window);
	this->friendChat_list->increaseFriendItem(item);
	this->stack_layout->addWidget(chat_window);
	
	for (int i = 1; i < this->stack_layout->count(); i++) {
		chat_window = qobject_cast<ChatWindow*>(this->stack_layout->widget(i));
		if (chat_window) {
			if (chat_window->currentUserData().userAccount == user_data.userAccount) {
				chat_window->IncreaseMessageItem(user_data);
				break;
			}
		}
	}
	this->stack_layout->setCurrentIndex(user_data.index);
	connect(chat_window, &ChatWindow::SendUserMessage, this, &ChatPage::SendUserMessage, Qt::DirectConnection);
	connect(chat_window, &ChatWindow::SendUserMessageForUserFileSignal, this, &ChatPage::SendUserMessageForUserFileSignal, Qt::DirectConnection);
	connect(this, &ChatPage::setFileItemProgressSignal, chat_window, &ChatWindow::setUploadFileItemProgress, Qt::DirectConnection);
	connect(this, &ChatPage::updateDownloadFileProgressSignal, chat_window, &ChatWindow::updateDownloadFileItemProgress, Qt::DirectConnection);
	connect(chat_window, &ChatWindow::modifyChatListItemData, this, &ChatPage::modifyChatListItemData, Qt::DirectConnection);
	connect(chat_window, &ChatWindow::resizeMainWindowSize, this, &ChatPage::resizeMainWindowSize,Qt::DirectConnection);
}

/**
 * @brief 好友 列表双击 时 创建聊天列表和聊天窗口
 * @param user_data
 */
void ChatPage::DoubleClickCreateChatWindow(UserData& user_data)
{
	qDebug() << "DoubleClickCreateChatWindow";
	int isExist = this->friendChat_list->isExistFriendChatItem(user_data.userAccount);
	if (isExist != -1) {
		this->friendChat_list->setItemSelected(user_data.userAccount);
		UserData temp = this->friendChat_list->getItemData(isExist);
		this->stack_layout->setCurrentIndex(temp.index);
	}
	else {
		user_data.index = this->stack_layout->count();
		ChatWindow* chat_window = new ChatWindow(user_data, this);
		ListWidgetItem* item = new ListWidgetItem;
		item->setData(Qt::UserRole, QVariant::fromValue(user_data));
		this->ChatItemAndChatWindow.insert(item, chat_window);
		this->stack_layout->addWidget(chat_window);
		this->stack_layout->setCurrentWidget(chat_window);
		this->friendChat_list->increaseFriendItem(item);
		this->friendChat_list->setItemSelected(user_data.userAccount);
		connect(chat_window, &ChatWindow::SendUserMessage, this, &ChatPage::SendUserMessage, Qt::DirectConnection);
		// connect(chat_window, &ChatWindow::SendUserMessage, this, &ChatPage::userMsg, Qt::DirectConnection);
		connect(chat_window, &ChatWindow::SendUserMessageForUserFileSignal, this, &ChatPage::SendUserMessageForUserFileSignal, Qt::DirectConnection);
		connect(this, &ChatPage::setFileItemProgressSignal, chat_window, &ChatWindow::setUploadFileItemProgress, Qt::DirectConnection);
		connect(this, &ChatPage::updateDownloadFileProgressSignal, chat_window, &ChatWindow::updateDownloadFileItemProgress, Qt::DirectConnection);
		connect(chat_window, &ChatWindow::modifyChatListItemData, this, &ChatPage::modifyChatListItemData, Qt::DirectConnection);
		connect(chat_window, &ChatWindow::resizeMainWindowSize, this, &ChatPage::resizeMainWindowSize, Qt::DirectConnection);

	}
}

/**
 * @brief 聊天列表点击时切换聊天窗口
 * @param user_data
 */
void ChatPage::setCurrentChatWindow(const UserData& user_data)
{
	this->stack_layout->setCurrentIndex(user_data.index);
}

/**
 * @brief 当好友上线时 更新item和聊天窗口的数据
 * @param user_data
 */
void ChatPage::itemChanged(UserData& user_data)
{
	UserData temp;
	for (QListWidgetItem* item : this->ChatItemAndChatWindow.keys()) {
		temp = item->data(Qt::UserRole).value<UserData>();
		if (temp.userAccount == user_data.userAccount) {
			user_data.index = temp.index;
			item->setData(Qt::UserRole, QVariant::fromValue(user_data));
			this->ChatItemAndChatWindow.value(item)->setChatWindowData(user_data);
			return;
		}
	}
}

void ChatPage::setUploadFileItemProgress(const qreal& pos)
{
	emit this->setFileItemProgressSignal(pos);
}

void ChatPage::updateDownloadFileProgress(const qreal& pos)
{
	emit this->updateDownloadFileProgressSignal(pos);
}

void ChatPage::modifyChatListItemData(const UserData& user_data)
{
	int isExist = this->friendChat_list->isExistFriendChatItem(user_data.receiverUserAccount);
	if (isExist != -1)
		this->friendChat_list->setItemData(isExist, user_data);
}

void ChatPage::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::transparent);
	painter.drawRoundedRect(this->rect(), 15, 15);
}