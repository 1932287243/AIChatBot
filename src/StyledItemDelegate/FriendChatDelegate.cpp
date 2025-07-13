#include "StyledItemDelegate/FriendChatDelegate.h"

FriendChatDelegate::FriendChatDelegate(QObject* parent)
	: QStyledItemDelegate(parent)
{
}

FriendChatDelegate::~FriendChatDelegate()
{
}

void FriendChatDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!index.isValid())
        return;

    UserData user_data = index.data(Qt::UserRole).value<UserData>();

    // message 为空直接跳过
    if (user_data.userMessage.isEmpty())
        // return;
		user_data.userMessage = QString("Session");

    QStyleOptionViewItem viewoption(option);
    initStyleOption(&viewoption, index);

    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);

    QRect item_rect = option.rect.adjusted(2, 2, -2, -2);

    // 字体设置（根据需要修改）
    QFont font;
    font.setPointSize(16);  // 控制字体大小
    painter->setFont(font);

    // 消息文本（限制长度，过长截断）
    QString message = user_data.userMessage;
    QFontMetrics fm(font);
    if (fm.horizontalAdvance(message) > 200)  // 控制最大显示宽度
        message = fm.elidedText(message, Qt::ElideRight, 200);

    // message 显示区域：左边距 20px，垂直居中
    int leftMargin = 10;
    QRect messageRect(
        item_rect.left() + leftMargin,
        item_rect.top() + (item_rect.height() - fm.height()) / 2,
        item_rect.width() - leftMargin - 10,  // 右边预留一点边距
        fm.height()
    );

    // 绘制悬浮或选中背景（可选）
    painter->save();
    if (option.state.testFlag(QStyle::State_MouseOver) || option.state.testFlag(QStyle::State_Selected)) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor("#f1f3f9"));
        painter->drawRoundedRect(item_rect, 10, 10);
    }
    painter->restore();

    // 绘制消息文本
    painter->save();
    painter->setPen(Qt::black);
    painter->drawText(messageRect, Qt::AlignLeft | Qt::AlignVCenter, message);
    painter->restore();
}


// void FriendChatDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
// {
// 	if (!index.isValid())
// 		return;

// 	UserData user_data = index.data(Qt::UserRole).value<UserData>();

// 	if (user_data.userHead.isNull() || user_data.userName.isEmpty())
// 		return;

// 	QStyleOptionViewItem viewoption(option);
// 	initStyleOption(&viewoption, index);

// 	painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);
// 	QRect item_rect = option.rect;
// 	item_rect.adjust(2, 2, -2, -2);
	

// 	//绘制头像
// 	QRect userheadRect = QRect(item_rect.left() + 5, item_rect.center().y() - ((item_rect.height() - 15) / 2), item_rect.height() - 15, item_rect.height() - 15);

// 	QLabel temp(user_data.userName);
// 	temp.adjustSize();
// 	QRect userNameRect = QRect(userheadRect.right() + 7, userheadRect.top() + 5, temp.width(), temp.height());

// 	temp.setText(user_data.userMessage);
// 	temp.adjustSize();

// 	QString message;
// 	if (temp.width() > 80)
// 		message = temp.text().left(8) + "...";

// 	else
// 		message = temp.text();

// 	QRect userMessageRect = QRect(userNameRect.left(), userNameRect.bottom() + 5, temp.width(), temp.height());

// 	painter->save();
// 	if (option.state.testFlag(QStyle::State_MouseOver) || option.state.testFlag(QStyle::State_Selected)) {
// 		// painter->setPen(QPen(QColor::fromString("#a5b3cb"), 1));
// 		painter->setPen(QPen(QColor("#a5b3cb"), 1));
// 		// painter->setBrush(QColor::fromString("#f1f3f9"));
// 		painter->setBrush(QColor("#f1f3f9"));
// 		painter->drawRoundedRect(item_rect, 10, 10);
// 	}
// 	else {
// 		painter->setPen(Qt::NoPen);
// 		painter->setBrush(Qt::transparent);
// 		painter->drawRoundedRect(item_rect, 10, 10);
// 	}
// 	painter->restore();

// 	painter->save();
// 	painter->setPen(Qt::NoPen);
// 	painter->setBrush(Qt::NoBrush);
// 	painter->drawPixmap(userheadRect, user_data.userHead.scaled(QSize(userheadRect.size() * GLOB_ScaleDpi), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
// 	painter->restore();

// 	painter->save();
// 	painter->setPen(Qt::black);
// 	painter->setBrush(Qt::NoBrush);
// 	painter->drawText(userNameRect, Qt::AlignCenter, user_data.userName);
// 	painter->restore();

// 	painter->save();
// 	painter->setPen(Qt::gray);
// 	painter->setBrush(Qt::NoBrush);
// 	painter->drawText(userMessageRect, Qt::AlignLeft, message);
// 	painter->restore();

// 	if (user_data.isUnread) {
// 		QRectF unread_rect = QRectF(QPointF(item_rect.right() - 20, (userMessageRect.center().y() - static_cast<qreal>(15) / 2)), QSize(15 , 15));
// 		painter->save();
// 		painter->setPen(Qt::NoPen);
// 		painter->setBrush(Qt::red);
// 		painter->drawEllipse(unread_rect);
// 		painter->restore();

// 		QFont font;
// 		font.setPixelSize(8);

// 		QRectF nums_rect;
// 		nums_rect.setSize(QSize(unread_rect.width() - 4, unread_rect.height() - 4));
// 		nums_rect.moveCenter(unread_rect.center());
// 		painter->save();
// 		painter->setPen(Qt::white);
// 		painter->setFont(font);
// 		painter->setBrush(Qt::NoBrush);
// 		painter->drawText(nums_rect,Qt::AlignCenter,QString::number(user_data.unReadMessageNums));
// 		painter->restore();
// 	}
// }

QSize FriendChatDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QSize size = QStyledItemDelegate::sizeHint(option, index);
	size.setHeight(60);
	return size;
}
