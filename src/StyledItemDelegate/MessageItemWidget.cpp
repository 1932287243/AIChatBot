﻿#include "StyledItemDelegate/MessageItemWidget.h"
#include "markdownhighlighter.h"
// #include <QScreen>

MessageItemWidget::MessageItemWidget(const UserData& userdata, QWidget* parent)
	: QWidget(parent)
{
	QHBoxLayout* main_lay = new QHBoxLayout(this);
	this->setLayout(main_lay);

	this->user_data = userdata;

	this->userHead = new QLabel(this);
	this->userHead->setFixedSize(36, 36);
	this->userHead->setScaledContents(true);
	this->userHead->setPixmap(this->user_data.userHead.scaled(QSize(this->userHead->size() * GLOB_ScaleDpi), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));;

	QVBoxLayout* vbox = new QVBoxLayout;
	// this->userName = new QLabel(this->user_data.userName);
	// this->userName->adjustSize();
	this->bubble = new Bubble(user_data, this);

	QFont font;
	font.setPixelSize(16);

	this->send_time = new QLabel(QDateTime::currentDateTime().toString("MM-dd hh:mm:ss"), this);
	this->send_time->setFont(font);
	this->send_time->adjustSize();
	this->send_time->hide();

	QHBoxLayout* lay = new QHBoxLayout;

	if (this->user_data.alignment == Qt::AlignLeft) {
		// lay->addWidget(this->userName);
		lay->addSpacing(5);
		lay->addWidget(this->send_time);
		lay->addStretch();
		vbox->addLayout(lay);
		vbox->addWidget(this->bubble, 0, Qt::AlignLeft);
		//vbox->addSpacing(3);

		main_lay->addWidget(this->userHead, 0, Qt::AlignTop);
		main_lay->addSpacing(3);
		main_lay->addLayout(vbox);
		main_lay->addStretch();
	}
	else {
		lay->addStretch();
		lay->addWidget(this->send_time);
		lay->addSpacing(5);
		// lay->addWidget(this->userName);
		vbox->addLayout(lay);
		vbox->addWidget(this->bubble, 0, Qt::AlignRight);
		//vbox->addSpacing(3);

		main_lay->addStretch();
		main_lay->addLayout(vbox);
		main_lay->addWidget(this->userHead, 0, Qt::AlignTop);
	}

	this->hoverTimer = new QTimer(this);
	this->hoverTimer->setInterval(1000);
	this->hoverTimer->setSingleShot(true);
	connect(this->hoverTimer, &QTimer::timeout, this->send_time, &QLabel::show, Qt::DirectConnection);
	connect(this->bubble, &Bubble::showImageViewer, this, &MessageItemWidget::showImageViewer, Qt::DirectConnection);
	this->adjustSize();
}

MessageItemWidget::~MessageItemWidget()
{
}

UserData MessageItemWidget::currentMessageItemData() const
{
	return this->user_data;
}

void MessageItemWidget::setSliderPosition(const qreal& position)
{
	this->bubble->setSliderPosition(position);
	if (position >= 1) {
		if (this->user_data.fileInfo.isUploading) {
			this->user_data.fileInfo.isUploading = false;
			this->bubble->setStatus("已发送");
		}
		else if (this->user_data.fileInfo.isDownloading) {
			this->user_data.fileInfo.isDownloading = false;
			this->bubble->setStatus("已接受");
		}
	}
}

void MessageItemWidget::setUserData(UserData& userdata)
{
    this->user_data = userdata;

    if (this->bubble) {
        this->bubble->updateFromUserData(userdata);
    }

    this->adjustSize();
}

void MessageItemWidget::paintEvent(QPaintEvent* event)
{
	QWidget::paintEvent(event);
}

void MessageItemWidget::enterEvent(QEvent* event)
{
	this->hoverTimer->stop();
	this->hoverTimer->start();
	QWidget::enterEvent(event);
}

void MessageItemWidget::leaveEvent(QEvent* event)
{
	this->hoverTimer->stop();
	if (!this->send_time->isHidden())
		this->send_time->hide();
	QWidget::leaveEvent(event);
}

Bubble::Bubble(const UserData& user_data, QWidget* parent) :QWidget(parent)
{
	this->user_data = user_data;
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	QPalette pale;
	pale.setColor(QPalette::Text, Qt::white);
	// pale.setColor(QPalette::Window, Qt::red);
	QFont font;
	if (user_data.messageType == ChatMessageType::TEXT) {
		font.setPixelSize(24);
		QHBoxLayout* main_lay = new QHBoxLayout(this);
		main_lay->setContentsMargins(2, 2, 2, 2);
		this->setLayout(main_lay);
	
		this->text = new QTextEdit( this);
		this->text->setFrameStyle(QFrame::NoFrame);
		this->text->setReadOnly(true);

		this->text->setFont(font);
		this->text->setPlainText(user_data.userMessage);
		// 设置语法高亮器
    	new MarkdownHighlighter(this->text->document());
		
		QFontMetrics metrics(this->text->font());
	
		this->text->setStyleSheet("background-color: rgba(255, 255, 255, 0);");  
		
		// 计算文字的换行后高度
		QString content = user_data.userMessage;
		int textHeight = metrics.boundingRect(QRect(0, 0, maxWidth, 1000), Qt::TextWordWrap, content).height();
		int textWidth = metrics.boundingRect(QRect(0, 0, maxWidth, 1000), Qt::TextWordWrap, content).width();
		this->text->setFixedSize(textWidth + 20, textHeight + 10);
		
		this->text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // 不显示滚动条
		this->text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		this->setFixedSize(text->width() + 20, text->height() + 10);
		main_lay->addWidget(this->text, 0, Qt::AlignCenter);
		this->border_rect = this->rect().adjusted(2, 2, -2, -2);
	}
	else if (user_data.messageType == ChatMessageType::USERFILE) {
		if (user_data.fileInfo.FileType == FILETYPE::PHOTO) {
			QHBoxLayout* main_lay = new QHBoxLayout(this);
			main_lay->setContentsMargins(0, 0, 0, 0);
			this->image = new ImageContainer(user_data.fileInfo.filePath, this);
			main_lay->addWidget(this->image);
			return;
		}

		QPixmap pixmap;
		QVBoxLayout* main_vbox = new QVBoxLayout(this);
		this->setLayout(main_vbox);
		if (user_data.fileInfo.FileType == FILETYPE::EXE)
			pixmap.load(":/Resource/ico/exe.png");
		else if (user_data.fileInfo.FileType == FILETYPE::MUSIC)
			pixmap.load(":/Resource/ico/mp3.png");
		//else if (user_data.fileInfo.FileType == FILETYPE::PHOTO)
		//	pixmap.load(":/Resource/ico/photo.png");
		this->fileIco = new QLabel(this);
		this->fileIco->setFixedSize(40, 40);
		this->fileIco->setScaledContents(true);
		this->fileIco->setPixmap(pixmap.scaled(QSize(this->fileIco->size() * GLOB_ScaleDpi), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

		this->fileName = new QLabel(user_data.fileInfo.fileName, this);
		this->fileName->setPalette(pale);
		this->fileName->adjustSize();

		QHBoxLayout* hlay = new QHBoxLayout;
		font.setPixelSize(11);
		this->fileSize = new QLabel(user_data.fileInfo.fileSize, this);
		this->fileSize->setFont(font);
		this->fileSize->setPalette(pale);
		this->fileSize->adjustSize();

		font.setPixelSize(9);
		this->status = new QLabel(this);
		this->status->setAlignment(Qt::AlignCenter);
		this->status->adjustSize();
		this->status->setPalette(pale);
		this->status->setFont(font);
		if (user_data.fileInfo.isDownloading) {
			this->status->setText("接受中");
		}
		if (user_data.fileInfo.isUploading) {
			this->status->setText("上传中");
		}

		hlay->addWidget(this->fileSize);
		hlay->addStretch();
		hlay->addWidget(this->status);

		this->slider = new ProgressSlider(this);
		QHBoxLayout* lay = new QHBoxLayout;

		QVBoxLayout* vbox = new QVBoxLayout;
		vbox->addWidget(this->fileName, 0, Qt::AlignLeft);
		vbox->addLayout(hlay);

		lay->addWidget(this->fileIco);
		lay->addLayout(vbox);

		main_vbox->addLayout(lay);
		main_vbox->addWidget(this->slider, 0, Qt::AlignCenter);

		this->border_rect = this->rect().adjusted(2, 2, -2, -2);
	}
}

Bubble::~Bubble()
{
}

void Bubble::updateFromUserData(UserData& new_data)
{
    this->user_data = new_data;
	int width;
    if (new_data.messageType == ChatMessageType::TEXT && this->text) {
        // this->text->setText(new_data.userMessage);
        // this->text->adjustSize();
        // this->setFixedSize(this->text->width() + 20, this->text->height() + 10);

		QFontMetrics metrics(this->text->font());
		QString content = new_data.userMessage;

		// 计算文字的换行后高度
		int textHeight = metrics.boundingRect(QRect(0, 0, maxWidth, 1000), Qt::TextWordWrap, content).height();
		int textWidth = metrics.boundingRect(QRect(0, 0, maxWidth, 1000), Qt::TextWordWrap, content).width();
		
		if(textWidth > maxWidth){
		// 	int insertPos = content.length() - 1;  // 倒数第二个字符之后的位置
		// 	new_data.userMessage.insert(insertPos, '\n');
			// 找到最后一个空格的位置
			int lastSpace = content.lastIndexOf(' ');
			// 如果找到了，就插入换行符
			if (lastSpace != -1) {
				new_data.userMessage.insert(lastSpace + 1, '\n');
			}
			else{
				int insertPos = content.length() - 1;  // 倒数第二个字符之后的位置
				new_data.userMessage.insert(insertPos, '\n');
			}

		}
		this->text->setFixedSize(textWidth + 20, textHeight + 10);
		this->text->setPlainText(content);
		// this->text->adjustSize();

		// 最后，调整整个 Widget 的大小，给文字留边距
		this->setFixedSize(this->text->width() + 20, this->text->height() + 10);
    }
    this->update(); // 触发 paintEvent 重绘
}


void Bubble::setSliderPosition(const qreal& position)
{
	if (this->slider)
		this->slider->setSliderPosition(position);
}

void Bubble::setStatus(const QString& text)
{
	if (this->status) {
		this->status->setText(text);
		this->status->adjustSize();
	}
}

QString Bubble::imagePath() const
{
	return this->user_data.fileInfo.filePath;
}

void Bubble::paintEvent(QPaintEvent*)
{
	if (this->user_data.fileInfo.FileType != FILETYPE::PHOTO) {
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setPen(Qt::NoPen);
		// painter.setBrush(QColor("#0099ff"));
		// QColor semiTransparentBlue(0, 153, 255, 128);  // 半透明蓝色
		QColor semiTransparentBlue(255, 255, 255, 0);  // 半透明蓝色
		painter.setBrush(semiTransparentBlue);

		QRect currentRect = this->rect().adjusted(2, 2, -2, -2);
		painter.drawRoundedRect(currentRect, 10.0f, 10.0f);
	}
}

void Bubble::showEvent(QShowEvent* event)
{
	this->border_rect = this->rect().adjusted(2, 2, -2, -2);
	this->update();
	QWidget::showEvent(event);
}

void Bubble::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (this->user_data.fileInfo.FileType == FILETYPE::PHOTO)
		emit this->showImageViewer(this->user_data.fileInfo.filePath);
	QWidget::mouseDoubleClickEvent(event);
}

ProgressSlider::ProgressSlider(QWidget* parent) :QWidget(parent)
{
	this->setFixedHeight(5);
}

void ProgressSlider::setSliderPosition(const qreal& position)
{
	this->m_position = position;
	this->update();
}

void ProgressSlider::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.save();
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::lightGray);
	painter.drawRoundedRect(this->rect(), 3, 3);
	painter.restore();

	painter.save();
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::green);
	painter.drawRoundedRect(QRect(QPoint(this->rect().topLeft()), QSize(this->width() * this->m_position, this->height())), 3.0f, 3.0f);
	painter.restore();
	QWidget::paintEvent(event);
}

void ProgressSlider::showEvent(QShowEvent* event)
{
	this->setFixedWidth(this->parentWidget()->width() - 30);
	QWidget::showEvent(event);
}

ImageContainer::ImageContainer(const QString& path, QWidget* parent) :QWidget(parent)
{
	this->pixmap.load(path);
	this->pixmap.setDevicePixelRatio(GLOB_ScaleDpi);
	QSize size = pixmap.size();
	if (size.width() != size.height())
		this->setFixedSize(330, 180);
	if (size.width() == size.height())
		this->setFixedSize(330, 330);
}

void ImageContainer::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing, true);
	painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
	QPainterPath path;
	path.addRoundedRect(this->rect(), 8.5f, 8.5f, Qt::AbsoluteSize);
	painter.setClipPath(path);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::NoBrush);
	painter.drawPixmap(this->rect(), this->pixmap.scaled(QSize(this->size() * GLOB_ScaleDpi), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	QWidget::paintEvent(event);
}