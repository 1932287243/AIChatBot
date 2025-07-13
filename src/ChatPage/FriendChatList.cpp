#include "ChatPage/FriendChatList.h"
#include <QShortcut>

FriendChatList::FriendChatList(QWidget* parent)
	: QWidget(parent)
{
	// this->setFixedWidth(300);
	this->setMaximumWidth(500);

	QVBoxLayout* main_vbox = new QVBoxLayout(this);
	this->setLayout(main_vbox);

	QFont font;
	font.setBold(true);
	font.setPixelSize(20);
	QPalette pale;
	pale.setColor(QPalette::WindowText, Qt::black);

	QHBoxLayout* lay = new QHBoxLayout;

	this->title = new QLabel("Message", this);
	this->title->setFont(font);
	this->title->setPalette(pale);
	this->title->setAlignment(Qt::AlignCenter);

	this->edit = new QLabel(this);
	this->edit->setFixedSize(15, 15);
	this->edit->setCursor(Qt::PointingHandCursor);
	this->edit->setScaledContents(true);
	QPixmap pixmap(":/Resource/ico/friend_edit_sel.png");
	this->edit->setPixmap(pixmap);

	lay->addWidget(this->title);
	// lay->addStretch();
	// lay->addWidget(this->edit);

	new_session = new QPushButton(this);
	new_session->setText("New Session");
	// new_session->setIcon(QIcon(":/Resource/ico/friend_edit_sel.png"));
	// new_session->setIconSize(QSize(16, 16));  // 控制图标大小
	new_session->setFont(font);
	// 设置无边框样式（可选）
	new_session->setStyleSheet("QPushButton { border: none; background-color: transparent; }");
	// 设置按钮快捷键提示（悬浮提示）
	new_session->setToolTip("Ctrl+N");

	// 创建水平布局
	QHBoxLayout* newSessionLayout = new QHBoxLayout;
	newSessionLayout->addWidget(new_session);           // 添加按钮
	newSessionLayout->addStretch();                     // 在右侧添加伸缩空间，将按钮推到最左边
	newSessionLayout->addWidget(this->edit);
	newSessionLayout->setContentsMargins(0, 0, 0, 0);    // 可选：去除边距

	// 设置快捷键（Ctrl+N）
	QShortcut* shortcut = new QShortcut(QKeySequence("Ctrl+N"), this);
	connect(shortcut, &QShortcut::activated, new_session, &QPushButton::click);
	connect(new_session, &QPushButton::clicked, this, &FriendChatList::sendCreateNewSession);

	pale.setColor(QPalette::Window, Qt::transparent);
	this->chat_listWidget = new QListWidget(this);
	this->chat_listWidget->setPalette(pale);
	this->chat_listWidget->setFrameShape(QFrame::NoFrame);
	this->chat_listWidget->sortItems(Qt::AscendingOrder);

	FriendChatDelegate* delegate = new  FriendChatDelegate(this);
	this->chat_listWidget->setItemDelegate(delegate);

	main_vbox->addLayout(lay);
	main_vbox->addSpacing(8);
	main_vbox->addLayout(newSessionLayout);
	main_vbox->addSpacing(8);
	main_vbox->addWidget(this->chat_listWidget);

	connect(this->chat_listWidget, &QListWidget::itemClicked, this, &FriendChatList::dealItemClicked, Qt::DirectConnection);
}

FriendChatList::~FriendChatList()
{
}

void FriendChatList::increaseFriendItem(ListWidgetItem* item)
{
	//	ListWidgetItem* my_item = qobject_cast<ListWidgetItem*>(item);
		//my_item->setData(Qt::UserRole, QVariant::fromValue(item->data(Qt::UserRole).value<UserData>()));
	this->chat_listWidget->addItem(item);
	this->chat_listWidget->sortItems(Qt::DescendingOrder);
}

int FriendChatList::isExistFriendChatItem(const QString& account)
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

void FriendChatList::setItemSelected(const QString& account)
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

void FriendChatList::setItemData(int index, const UserData& user_data)
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

void FriendChatList::dealItemClicked(QListWidgetItem* item)
{
	UserData user_data = item->data(Qt::UserRole).value<UserData>();
	if (user_data.unReadMessageNums != 0) {
		user_data.unReadMessageNums = 0;
		user_data.isUnread = false;
		item->setData(Qt::UserRole, QVariant::fromValue(user_data));
	}
	emit this->FriendChatItemChanged(user_data);
}

UserData FriendChatList::getItemData(int index) const
{
	UserData  user_data = this->chat_listWidget->item(index)->data(Qt::UserRole).value<UserData>();
	return user_data;
}

void FriendChatList::paintEvent(QPaintEvent*)
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
