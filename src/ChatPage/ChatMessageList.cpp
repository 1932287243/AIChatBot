#include "ChatPage/ChatMessageList.h"
#include <QShortcut>

ChatMessageList::ChatMessageList(QWidget* parent)
	: QWidget(parent)
{
	this->setMaximumWidth(500);

	QVBoxLayout* main_vbox = new QVBoxLayout(this);
	this->setLayout(main_vbox);

	QFont font;
	font.setBold(true);
	font.setPixelSize(30);
	QPalette pale;
	pale.setColor(QPalette::WindowText, Qt::black);

	QWidget *titleBar = new QWidget(this);  // 创建容器

	// 创建布局（水平排列）
	QHBoxLayout *layout = new QHBoxLayout(titleBar);
	layout->setContentsMargins(0, 0, 0, 0);  // 去除边距（可选）

	// 左边的 QLabel
	this->title = new QLabel("Message", titleBar);
	this->title->setFont(font);
	this->title->setPalette(pale);
	this->title->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);  // 垂直居中，左对齐

	// 右边的带图标按钮
	QPushButton *iconButton = new QPushButton(titleBar);
	iconButton->setIcon(QIcon(":/Resource/ico/close_sidebar.png"));  
	iconButton->setFixedSize(36, 36);                     // 设置按钮大小
	iconButton->setFlat(true);                            // 无边框风格（可选）
	iconButton->setIconSize(QSize(30, 30));  // 设置图标大小为 16x16 像素

	// 添加到布局
	layout->addWidget(this->title);
	layout->addStretch();            // 自动将按钮推到右边
	layout->addWidget(iconButton);

	new_session = new QPushButton(this);
	new_session->setText("开启新对话");
	new_session->setIcon(QIcon(":/Resource/ico/new_session.png"));
	new_session->setIconSize(QSize(30, 30));  // 控制图标大小
	new_session->setFont(font);
	// 设置无边框样式（可选）
	new_session->setStyleSheet("QPushButton { border: none; background-color: transparent; }");
	// 设置按钮快捷键提示（悬浮提示）
	new_session->setToolTip("Ctrl+N");

	// 创建水平布局
	QHBoxLayout* newSessionLayout = new QHBoxLayout;
	newSessionLayout->addWidget(new_session);           // 添加按钮
	newSessionLayout->addStretch();                     // 在右侧添加伸缩空间，将按钮推到最左边
	newSessionLayout->setContentsMargins(0, 0, 0, 0);    // 可选：去除边距

	// 设置快捷键（Ctrl+N）
	QShortcut* shortcut = new QShortcut(QKeySequence("Ctrl+N"), this);
	connect(shortcut, &QShortcut::activated, new_session, &QPushButton::click);
	connect(new_session, &QPushButton::clicked, this, &ChatMessageList::sendCreateNewSession);
	connect(iconButton, &QPushButton::clicked, this,  &ChatMessageList::sendCloseMeassageList);

	pale.setColor(QPalette::Window, Qt::transparent);
	this->chat_listWidget = new QListWidget(this);
	this->chat_listWidget->setPalette(pale);
	this->chat_listWidget->setFrameShape(QFrame::NoFrame);
	this->chat_listWidget->sortItems(Qt::AscendingOrder);

	FriendChatDelegate* delegate = new  FriendChatDelegate(this);
	this->chat_listWidget->setItemDelegate(delegate);

	main_vbox->addWidget(titleBar);
	main_vbox->addSpacing(8);
	main_vbox->addLayout(newSessionLayout);
	main_vbox->addSpacing(8);
	main_vbox->addWidget(this->chat_listWidget);

	connect(this->chat_listWidget, &QListWidget::itemClicked, this, &ChatMessageList::dealItemClicked, Qt::DirectConnection);
}

ChatMessageList::~ChatMessageList()
{
}

void ChatMessageList::increaseFriendItem(ListWidgetItem* item)
{
	//	ListWidgetItem* my_item = qobject_cast<ListWidgetItem*>(item);
		//my_item->setData(Qt::UserRole, QVariant::fromValue(item->data(Qt::UserRole).value<UserData>()));
	this->chat_listWidget->addItem(item);
	this->chat_listWidget->sortItems(Qt::DescendingOrder);
}

int ChatMessageList::isExistFriendChatItem(const QString& account)
{
	for (int i = 0; i < this->chat_listWidget->count(); i++) {
		QListWidgetItem* item = this->chat_listWidget->item(i);
		UserData user_data = item->data(Qt::UserRole).value<UserData>();
		if (user_data.userAccount == account) {
			return i;
		}
	}
	return -1;
}

void ChatMessageList::setItemSelected(const QString& account)
{
	for (int i = 0; i < this->chat_listWidget->count(); i++) {
		QListWidgetItem* item = this->chat_listWidget->item(i);
		UserData user_data = item->data(Qt::UserRole).value<UserData>();
		if (user_data.userAccount == account) {
			item->setSelected(true);
			return;
		}
	}
	;
}

void ChatMessageList::setItemData(int index, const UserData& user_data)
{
	QListWidgetItem* item = this->chat_listWidget->item(index);
	UserData item_data = item->data(Qt::UserRole).value<UserData>();
	if (!item->isSelected()) {
		if (!item_data.isUnread)
			item_data.isUnread = true;
		item_data.unReadMessageNums++;
	}

	if (user_data.messageType == ChatMessageType::TEXT)
		item_data.userMessage = user_data.userMessage;
	else if (user_data.messageType == ChatMessageType::USERFILE)
		item_data.userMessage = "[文件]" + user_data.fileInfo.fileName;
	item->setData(Qt::UserRole, QVariant::fromValue(item_data));
}

void ChatMessageList::dealItemClicked(QListWidgetItem* item)
{
	UserData user_data = item->data(Qt::UserRole).value<UserData>();
	if (user_data.unReadMessageNums != 0) {
		user_data.unReadMessageNums = 0;
		user_data.isUnread = false;
		item->setData(Qt::UserRole, QVariant::fromValue(user_data));
	}
	emit this->FriendChatItemChanged(user_data);
}

UserData ChatMessageList::getItemData(int index) const
{
	UserData  user_data = this->chat_listWidget->item(index)->data(Qt::UserRole).value<UserData>();
	return user_data;
}

void ChatMessageList::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.save();
	painter.setPen(QColor(0, 0, 0, 20));
	painter.setBrush(Qt::NoBrush);
	painter.drawLine(QPoint(this->rect().topLeft()), QPoint(this->rect().bottomLeft()));
	painter.restore();

	painter.save();
	painter.setPen(Qt::NoPen);
	//painter.setBrush(QColor(220, 220, 220, 55));
	painter.setBrush(Qt::transparent);
	painter.drawRoundedRect(this->rect(), 15, 15);
	painter.restore();
}

ListWidgetItem::ListWidgetItem()
{
}

ListWidgetItem::~ListWidgetItem()
{
}

bool ListWidgetItem::operator<(const QListWidgetItem& other) const
{
	UserData m_user_data = this->data(Qt::UserRole).value<UserData>();
	const ListWidgetItem* item = dynamic_cast<const ListWidgetItem*>(&other);
	UserData other_data = item->data(Qt::UserRole).value<UserData>();
	if (&other) {
		if (m_user_data.status != other_data.status)
			return m_user_data.status < other_data.status;
		else
			return m_user_data.userName < other_data.userAccount;
	}

	return QListWidgetItem::operator<(other);
}
