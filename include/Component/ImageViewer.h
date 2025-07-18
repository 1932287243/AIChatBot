﻿#pragma once

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QWheelEvent>
#include <QScrollBar>
#include <QPropertyAnimation>
#include "global.h"

class ImageViewerTitle;
class CustomGraphicsView;
class ImageTool;
QT_FORWARD_DECLARE_CLASS(ImageTool);
QT_FORWARD_DECLARE_CLASS(CustomGraphicsView);
QT_FORWARD_DECLARE_CLASS(ImageViewerTitle);
#include <QWidget>
class ImageViewer : public QWidget
{
	Q_OBJECT

public:
	ImageViewer(const QString& image_path, QWidget* parent);
	~ImageViewer();
	void setPixmap(const  QString& image_path);

protected:
	void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
	void showEvent(QShowEvent* event) Q_DECL_OVERRIDE;
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
	void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;
private:
	void SaveImage();
	void RotationImage(bool direction);
	qreal getRotationAngle(const QTransform& transform);
private:
	QGraphicsPixmapItem* pixmapItem = Q_NULLPTR;
	QGraphicsScene* scene = Q_NULLPTR;
	QImage m_image;
	CustomGraphicsView* view = Q_NULLPTR;
	ImageTool* image_tool = Q_NULLPTR;
	QString fileName;
	QImage transform_image;
	ImageViewerTitle* title = Q_NULLPTR;
};

class CustomGraphicsView :public  QGraphicsView {
	Q_OBJECT
public:
	CustomGraphicsView(QWidget* parent = Q_NULLPTR);
protected:
	void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
private:

};

class ImageTool :public QWidget {
	Q_OBJECT
public:
	explicit ImageTool(QWidget* parent = Q_NULLPTR);
protected:
	bool eventFilter(QObject* target, QEvent* event)Q_DECL_OVERRIDE;
private:
	QLabel* saveImage = Q_NULLPTR;
	QLabel* turnLeft = Q_NULLPTR;
	QLabel* turnRight = Q_NULLPTR;
Q_SIGNALS:
	void on_save_image();
	void on_rotation_image(bool direction);

};

class ImageViewerTitle :public  QWidget {
	Q_OBJECT
public:
	explicit ImageViewerTitle(QWidget* parent = Q_NULLPTR);
protected:
	bool eventFilter(QObject* target, QEvent* event) Q_DECL_OVERRIDE;
private:
	QLabel* close_window = Q_NULLPTR;
	QLabel* title_text = Q_NULLPTR;
Q_SIGNALS:
	void on_close_window();
};
