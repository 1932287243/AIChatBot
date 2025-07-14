#pragma once

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QDialog>
#include <QMenu>
#include <QAction>
#include <QBitmap>
#include <QScopedPointer>
#include <QPainterPath>
#include <QTimer>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
// #include  "../../FramelessWindow/include/framelesswindow.h"
#include "Component/ImageViewer.h"
#include <QFontMetrics>
#include "global.h"

QT_BEGIN_NAMESPACE
class Bubble;
class ProgressSlider;
class ImageContainer;
QT_END_NAMESPACE

class MessageItemWidget : public QWidget
{
	Q_OBJECT

public:
	explicit MessageItemWidget(const UserData& userdata, QWidget* parent);
	~MessageItemWidget();
	UserData currentMessageItemData()const;
	void setSliderPosition(const qreal& position);
	void setUserData(UserData& userdata);

protected:
	void paintEvent(QPaintEvent* event) override;
	void enterEvent(QEvent* event) override;
	void leaveEvent(QEvent* event) override;

private:
	QLabel* userHead = Q_NULLPTR;
	QLabel* userName = Q_NULLPTR;
	UserData user_data;
	Bubble* bubble = Q_NULLPTR;
	QTimer* hoverTimer = Q_NULLPTR;
	QLabel* send_time = Q_NULLPTR;
Q_SIGNALS:
	void showImageViewer(const QString& image_path);
};
class Bubble :public QWidget {
	Q_OBJECT
public:
	explicit Bubble(const UserData& user_data, QWidget* parent = Q_NULLPTR);
	~Bubble();
	void setSliderPosition(const qreal& position);
	void setStatus(const QString& text);
	QString imagePath()const;
	void updateFromUserData(UserData& new_data);
protected:
	void paintEvent(QPaintEvent* event)Q_DECL_OVERRIDE;
	void showEvent(QShowEvent* event)Q_DECL_OVERRIDE;
	void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
private:
	QLabel* text = Q_NULLPTR;
	QLabel* fileIco = Q_NULLPTR;
	QLabel* fileName = Q_NULLPTR;
	QLabel* fileSize = Q_NULLPTR;
	ProgressSlider* slider = Q_NULLPTR;
	QLabel* status = Q_NULLPTR;
	QRect border_rect;
	ImageContainer* image = Q_NULLPTR;
	UserData user_data;
	int maxWidth = 800;  // 设置最大宽度
Q_SIGNALS:
	void showImageViewer(const QString& image_path);
};

class ProgressSlider :public QWidget {
	Q_OBJECT
public:
	explicit ProgressSlider(QWidget* parent = Q_NULLPTR);
	void setSliderPosition(const qreal& position);

protected:
	void paintEvent(QPaintEvent* event)Q_DECL_OVERRIDE;
	void showEvent(QShowEvent* event)Q_DECL_OVERRIDE;
private:
	qreal  m_position = 0.0;
};

class ImageContainer :public QWidget {
	Q_OBJECT
public:
	ImageContainer(const QString& path, QWidget* parent = Q_NULLPTR);
protected:
	void paintEvent(QPaintEvent* event)Q_DECL_OVERRIDE;
private:
	QString path;
	QLabel* image = Q_NULLPTR;
	QPixmap pixmap;
};
