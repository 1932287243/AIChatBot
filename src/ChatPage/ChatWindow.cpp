﻿#include "ChatPage/ChatWindow.h"
#include <QDebug>
#include <QFrame>

ChatWindow::ChatWindow(const UserData& user_data, QWidget* parent)
	: QWidget(parent)
{
	this->m_userData = user_data;
	this->currentChatWindowIndex = user_data.index;

	QHBoxLayout* main_lay = new QHBoxLayout();
	main_lay->setContentsMargins(0, 0, 0, 0);
	// this->setLayout(main_lay);

	QVBoxLayout* main_vbox = new QVBoxLayout(this);
	//main_vbox->setContentsMargins(0, 0, 0, 0);
	this->setLayout(main_vbox);
	//QPalette pale;
	//pale.setColor(QPalette::ColorGroup::Active,QPalette::Window, Qt::transparent);
	

	this->chat_title = new ChatTitle(user_data.userName, user_data.userHead, this);
	this->chat_list = new QListWidget(this);

	this->chat_list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	this->chat_list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	// QStringList qss;
	// qss.append("QListWidget::item:hover{background:transparent}");
	// qss.append("QListWidget{background-color:transparent}");
	// this->chat_list->setStyleSheet(qss.join(""));
	// 设置样式表
	this->chat_list->setStyleSheet(R"(
		QListWidget {
			color: #333333;
			background-color:transparent;
			border: none;
			background-image: url(:/Resource/ico/logo.svg);
			background-repeat: no-repeat;
			background-position: center;
			background-attachment: fixed;
		}
		QListWidget::item:hover{background:transparent}
	)");
	chat_list->setSpacing(5);
	this->chat_list->setFrameShape(QFrame::NoFrame);
	//this->chat_list->setPalette(pale);

	this->message_edit = new ChatMessageEdit(this);

	// main_lay->addLayout(main_vbox);
	QFrame* line = new QFrame(this);
	QFrame* line2 = new QFrame(this);

	setupFrameStyle(line);
	setupFrameStyle(line2);

	this->file_list = new FileQueueList(this);
	this->file_list->hide();
	main_lay->addWidget(line);
	main_lay->addWidget(this->file_list);

	main_vbox->addWidget(this->chat_title);
	main_vbox->addWidget(line2);
	main_vbox->addWidget(this->chat_list);
	main_vbox->addLayout(main_lay);
	main_vbox->addWidget(this->message_edit);


	connect(this->message_edit, &ChatMessageEdit::SendUserMessage, this, &ChatWindow::dealUserSendMessage, Qt::DirectConnection);
	connect(this->message_edit, &ChatMessageEdit::MyMessageForFileSignal, this, &ChatWindow::IncreaseMessageItemForEXE, Qt::DirectConnection);
	connect(this->message_edit, &ChatMessageEdit::waitingFileQueueButtonClicked, this, [=]() {
		this->file_list->isHidden() ? this->file_list->show() : this->file_list->hide();
		if (this->file_list->isHidden())
			emit this->resizeMainWindowSize(false);
		else
			emit this->resizeMainWindowSize(true);
		});
}

ChatWindow::~ChatWindow()
{
	if (this->image_viewer) {
		this->image_viewer->deleteLater();
		this->image_viewer = Q_NULLPTR;
	}
}

void ChatWindow::setupFrameStyle(QFrame* frame) {
	frame->setFrameShape(QFrame::HLine);           // 设置为水平线
	frame->setFrameShadow(QFrame::Plain);          // 实线而非凹陷/凸起
	frame->setLineWidth(1);                        // 线宽为1像素
	frame->setStyleSheet("color: rgb(169,169,169);");          // 设置线的颜色为灰色
}

UserData ChatWindow::currentUserData() const
{
	return m_userData;
}

void ChatWindow::UpdateLastMessageItem(UserData& user_data)
{
    int count = this->chat_list->count();
    if (count == 0)
        return; // 没有 item，无法更新

    QListWidgetItem* lastItem = this->chat_list->item(count - 1);
    MessageItemWidget* itemWidget = qobject_cast<MessageItemWidget*>(this->chat_list->itemWidget(lastItem));

    if (itemWidget) {
        // 更新 UserData 数据（假设 MessageItemWidget 提供了 setUserData() 方法）
        itemWidget->setUserData(user_data);

        // // 更新 item 大小（可选，取决于内容是否影响 size）
        lastItem->setSizeHint(itemWidget->sizeHint());

        // 自动滚动到最底部（可选）
        this->chat_list->scrollToBottom();
    }
}


void ChatWindow::IncreaseMessageItem(const UserData& user_data)
{
	QListWidgetItem* item = new QListWidgetItem(this->chat_list);
	item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
	MessageItemWidget* itemWidget = new MessageItemWidget(user_data, this);
	item->setSizeHint(itemWidget->size());
	item->setData(Qt::UserRole, QVariant::fromValue(user_data));
	this->chat_list->addItem(item);
	this->chat_list->setItemWidget(item, itemWidget);
	this->chat_list->scrollToBottom();

	connect(itemWidget, &MessageItemWidget::showImageViewer, this, [=](const QString& image_path) {
		if (this->image_viewer != Q_NULLPTR) {
			this->image_viewer->setPixmap(image_path);
			this->image_viewer->raise();
			this->image_viewer->show();
			return;
		}

		this->image_viewer = new ImageViewer(image_path, Q_NULLPTR);
		this->image_viewer->show();
		});
}

void ChatWindow::dealUserSendMessage(const QString& message)
{
	UserData user_data;
	user_data.alignment = Qt::AlignRight;
	user_data.userAccount = QString::number(GLOB_UserAccount);
	user_data.userName = GLOB_UserName;
	QPixmap userHead = RoundImage::RoundImageFromByteArray(GLOB_UserHeadImagebytes);
	user_data.userHead = userHead;
	user_data.userMessage = message;
	user_data.messageType = ChatMessageType::TEXT;
	user_data.index = this->m_userData.index;
	user_data.receiverUserAccount = this->m_userData.userAccount;

	this->IncreaseMessageItem(user_data);
	emit this->modifyChatListItemData(user_data);

	// if (!this->m_userData.status)
	// 	return;

	// TODO:
	emit this->SendUserMessage(QString::number(GLOB_UserAccount), this->m_userData.userAccount, message);
}

void ChatWindow::setChatWindowData(const UserData& user_data)
{
	this->m_userData = user_data;
}

void ChatWindow::IncreaseMessageItemForEXE(const FileInfoData& file_data)
{
	UserData user_data;
	user_data.alignment = Qt::AlignRight;
	user_data.userAccount = QString::number(GLOB_UserAccount);
	user_data.userName = GLOB_UserName;
	QPixmap userHead = RoundImage::RoundImageFromByteArray(GLOB_UserHeadImagebytes);
	user_data.userHead = userHead;
	user_data.fileInfo = file_data;
	user_data.fileInfo.fileSize = QString::number(file_data.fileSize.toLongLong() * 1.0 * OneByteForMB, 'f', 2) + "MB";
	user_data.messageType = ChatMessageType::USERFILE;
	user_data.index = this->m_userData.index;
	user_data.receiverUserAccount = this->m_userData.userAccount;
	user_data.fileInfo.isUploading = true;
	this->IncreaseMessageItem(user_data);
	emit this->modifyChatListItemData(user_data);
	if (!this->m_userData.status)
		return;

	ReceiverFileUserAccountTemp = this->m_userData.userAccount;
	emit this->SendUserMessageForUserFileSignal(QString::number(GLOB_UserAccount), this->m_userData.userAccount, file_data);
}

void ChatWindow::setUploadFileItemProgress(const qreal& pos)
{
	if (this->m_userData.userAccount != ReceiverFileUserAccountTemp)
		return;
	for (int i = 0; i < this->chat_list->count(); i++) {
		QListWidgetItem* item = this->chat_list->item(i);
		if (item) {
			MessageItemWidget* itemWidget = qobject_cast<MessageItemWidget*>(this->chat_list->itemWidget(item));
			if (itemWidget && itemWidget->currentMessageItemData().fileInfo.isUploading) {
				itemWidget->setSliderPosition(pos);
				return;
			}
		}
	}
}

void ChatWindow::updateDownloadFileItemProgress(const qreal& pos)
{
	if (this->m_userData.userAccount != SenderFileUserAccountTemp)
		return;
	for (int i = 0; i < this->chat_list->count(); i++) {
		QListWidgetItem* item = this->chat_list->item(i);
		if (item) {
			MessageItemWidget* itemWidget = qobject_cast<MessageItemWidget*>(this->chat_list->itemWidget(item));
			if (itemWidget && itemWidget->currentMessageItemData().fileInfo.isDownloading) {
				itemWidget->setSliderPosition(pos);
				return;
			}
		}
	}
}

ChatTitle::ChatTitle(const QString& user_name, const QPixmap& user_head, QWidget* parent) :QWidget(parent)
{
	QHBoxLayout* main_lay = new QHBoxLayout(this);
	this->setLayout(main_lay);

	this->shadow = new QGraphicsDropShadowEffect(this);
	this->shadow->setOffset(0, 0);
	this->shadow->setColor(Qt::gray);
	this->shadow->setBlurRadius(8);
	this->setGraphicsEffect(this->shadow);

	// QPixmap pixmap;
	// if (!pixmap.load(":/Resource/ico/close_sidebar.png")) {
	// 	qWarning("Failed to load image");
	// 	// return {};
	// }
	// else{
	// 	qDebug() << "open success";
	// }
	// this->userHead = new QLabel(this);
	// this->userHead->setFixedSize(40, 40);
	// this->userHead->setScaledContents(true);
	
	// this->userHead->setPixmap(pixmap.scaled(QSize(this->userHead->size() * GLOB_ScaleDpi), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

	this->userName = new QLabel("Session", this);
	this->userName->adjustSize();

	// 设置字体大小
	QFont font;
	font.setPointSize(18);  // 设置字号为18
	this->userName->setFont(font);

	// 设置文本居中
	this->userName->setAlignment(Qt::AlignCenter);
	// this->userName->setStyleSheet("	color: #333333;background-color:rgba(255,255,255,0);border: none;");
	this->userName->setStyleSheet(R"(
			QLabel {
				color: #333333;
				background-color:transparent;
				border: none;
			}
		)");
	// main_lay->addWidget(this->userHead);
	main_lay->addWidget(this->userName);
	// main_lay->addStretch();
}

ChatTitle::~ChatTitle()
{
}

// void ChatTitle::paintEvent(QPaintEvent*)
// {
// 	QPainter painter(this);
// 	painter.setRenderHint(QPainter::Antialiasing);
// 	painter.setPen(Qt::NoPen);
// 	painter.setBrush(Qt::white);
// 	painter.drawRoundedRect(this->rect(), 10, 10);
// }

ChatMessageEdit::ChatMessageEdit(QWidget* parent) :QWidget(parent)
{
	this->setFixedHeight(130);
	QVBoxLayout* main_vbox = new QVBoxLayout(this);
	this->setLayout(main_vbox);
	main_vbox->setContentsMargins(0, 0, 0, 0);

	QFont font;
	font.setPixelSize(24);
	this->message_edit = new QTextEdit(this);
	this->message_edit->setFont(font);
	this->message_edit->setFrameShape(QFrame::NoFrame);
	this->message_edit->setPlaceholderText("Enter Something...");
	this->message_edit->installEventFilter(this);

	this->message_edit->setStyleSheet(R"(
			QTextEdit {
				color: #333333;
				background-color:transparent;
				border: none;
			}
		)");

	QHBoxLayout* lay = new QHBoxLayout;

	this->file_button = new QLabel(this);

	this->file_button->setFixedSize(23, 23);
	this->file_button->setScaledContents(true);
	QPixmap pixmap(":/Resource/ico/file_unsel.png");
	pixmap.setDevicePixelRatio(GLOB_ScaleDpi);
	this->file_button->setPixmap(pixmap);
	this->file_button->setCursor(Qt::PointingHandCursor);
	this->file_button->setAttribute(Qt::WA_Hover);
	this->file_button->installEventFilter(this);

	lay->setSpacing(15);
	lay->addWidget(this->file_button);
	lay->addStretch();

	QHBoxLayout* lay_1 = new QHBoxLayout;
	this->send_button = new QPushButton("发送", this);

	// 设置样式表
	this->send_button->setStyleSheet(R"(
		QPushButton {
			background-color: #f0f0f0;       /* 浅灰 */
			color: #66cc99;                  /* 淡绿色 */
			border: none;                   /* 无边框 */
			padding: 6px 12px;              /* 可选：内边距 */
			border-radius: 4px;             /* 可选：圆角 */
			font-size: 16pt;
		}
		QPushButton:pressed {
			background-color: #66cc99;      /* 绿色背景 */
			color: white;                   /* 白色字体 */
			font-size: 16pt;
		}
	)");
	
	lay_1->addStretch();
	lay_1->addWidget(this->send_button);
	main_vbox->addSpacing(5);

	main_vbox->addLayout(lay);
	main_vbox->addWidget(this->message_edit);
	main_vbox->addLayout(lay_1);
}

// void ChatMessageEdit::paintEvent(QPaintEvent* event)
// {
// 	QPainter painter(this);
// 	painter.setRenderHint(QPainter::Antialiasing);
// 	painter.setPen(QColor(0, 0, 0, 50));
// 	painter.setBrush(Qt::NoBrush);
// 	painter.drawLine(QPoint(this->rect().topLeft()), QPoint(this->rect().topRight()));
// 	QWidget::paintEvent(event);
// }

bool ChatMessageEdit::eventFilter(QObject* target, QEvent* event)
{
	QPixmap pixmap;
	if (target == this->message_edit) {
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent* key = reinterpret_cast<QKeyEvent*>(event);
			if (key->key() == Qt::Key_Return) {
				if (this->message_edit->toPlainText().isEmpty())
					return true;
				emit this->SendUserMessage(this->message_edit->toPlainText());
				this->message_edit->clear();
				return true;
			}
		}
	}

	if (target == this->file_button) {
		if (event->type() == QEvent::HoverEnter) {
			pixmap.load(":/Resource/ico/file_sel.png");
			this->file_button->setPixmap(pixmap);
			return true;
		}
		if (event->type() == QEvent::HoverLeave) {
			pixmap.load(":/Resource/ico/file_unsel.png");
			this->file_button->setPixmap(pixmap);
			return true;
		}
		if (event->type() == QEvent::MouseButtonPress) {
			QString filePath = QFileDialog::getOpenFileName(this, "选择文件", "/");
			if (!filePath.isEmpty()) {
				FileInfoData data;
				data.filePath = filePath;
				QFileInfo info(filePath);
				if (info.suffix() == "exe")
					data.FileType = FILETYPE::EXE;
				else if (info.suffix() == "mp3")
					data.FileType = FILETYPE::MUSIC;
				else if (info.suffix() == "png" || info.suffix() == "jpg")
					data.FileType = FILETYPE::PHOTO;
				QString fileName = info.fileName();
				data.fileName = fileName;
				data.fileSize = QString::number(info.size());
				emit this->MyMessageForFileSignal(data);
			}

			return true;
		}
	}
	// if (target == this->emoji_button) {
	// 	if (event->type() == QEvent::HoverEnter) {
	// 		pixmap.load(":/Resource/ico/emoji_sel.png");
	// 		this->emoji_button->setPixmap(pixmap);
	// 		return true;
	// 	}
	// 	if (event->type() == QEvent::HoverLeave) {
	// 		pixmap.load(":/Resource/ico/emoji_unsel.png");
	// 		this->emoji_button->setPixmap(pixmap);
	// 		return true;
	// 	}
	// 	if (event->type() == QEvent::MouseButtonPress) {
	// 		emit this->showImageViewSignal();
	// 		return true;
	// 	}
	// }
	if (target == this->waitTransferFile_button) {
		if (event->type() == QEvent::HoverEnter) {
			pixmap.load(":/Resource/ico/wait_trans_sel.png");
			this->waitTransferFile_button->setPixmap(pixmap);
			return true;
		}
		if (event->type() == QEvent::HoverLeave) {
			pixmap.load(":/Resource/ico/wait_trans_unsel.png");
			this->waitTransferFile_button->setPixmap(pixmap);
			return true;
		}
		if (event->type() == QEvent::MouseButtonPress) {
			emit this->waitingFileQueueButtonClicked();
			return true;
		}
	}
	return QWidget::eventFilter(target, event);
}

// void ChatMessageEdit::addEmoji(const QString& image_path)
// {
// 	QTextCursor cursor = this->message_edit->textCursor();

// 	// 加载图片
// 	QFont font;
// 	font.setPixelSize(13);
// 	QTextImageFormat imageFormat;
// 	imageFormat.setFont(font);
// 	imageFormat.setWidth(20);
// 	imageFormat.setHeight(20);
// 	imageFormat.setName(image_path);

// 	// 插入图片到光标位置
// 	cursor.insertImage(imageFormat);
// 	message_edit->setTextCursor(cursor);
// }

SendMessageButton::SendMessageButton(QWidget* parent) :QWidget(parent)
{
	this->setFixedSize(60, 30);
	this->setCursor(Qt::PointingHandCursor);
}

void SendMessageButton::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.save();
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor("#6a4cff"));
	painter.drawRoundedRect(this->rect(), 10, 10);
	painter.restore();

	QFont font;
	font.setBold(true);
	painter.save();
	painter.setPen(Qt::white);
	painter.setFont(font);
	painter.setBrush(Qt::NoBrush);
	painter.drawText(this->rect(), Qt::AlignCenter, QString("Send"));
	painter.restore();
}