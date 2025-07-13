#include "Component/RoundImage.h"

QPixmap RoundImage::RoundImageFromStringPath(const QString& imagePath)
{
	//QPixmap pixmap(imagePath);
	//QSize realSize = QSize(pixmap.size());
	//QPixmap resultPixmap(realSize);
	//resultPixmap.fill(Qt::transparent);
	//QPainter painter(&resultPixmap);
	//painter.setRenderHints(QPainter::Antialiasing, true);
	//painter.setRenderHints(QPainter::SmoothPixmapTransform);
	//QPainterPath path;
	//path.addRoundedRect(QRect(QPoint(0, 0), QSize(resultPixmap.size())), realSize.width() / 2, realSize.height() / 2);
	//painter.setClipPath(path);
	//painter.drawPixmap(QRect(QPoint(0, 0), QSize(resultPixmap.size())), pixmap.scaled(resultPixmap.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	//return resultPixmap;

	QPixmap pixmap(imagePath);
	QSize realSize = QSize(pixmap.width() * GLOB_ScaleDpi, pixmap.height() * GLOB_ScaleDpi);
	QPixmap resultPixmap(realSize);
	resultPixmap.fill(Qt::transparent);
	QPainter painter(&resultPixmap);
	painter.setRenderHints(QPainter::Antialiasing, true);
	painter.setRenderHints(QPainter::SmoothPixmapTransform);
	QPainterPath path;
	path.addRoundedRect(QRect(QPoint(0, 0), QSize(realSize)), realSize.width() / 2, realSize.height() / 2);
	painter.setClipPath(path);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::NoBrush);
	painter.drawPixmap(QRect(QPoint(0, 0), QSize(realSize)), pixmap.scaled(QSize(realSize), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	resultPixmap.setDevicePixelRatio(GLOB_ScaleDpi);
	return resultPixmap;
}

QPixmap RoundImage::RoundImageFromByteArray(const QByteArray& data)
{
    QPixmap pixmap;
    if (!pixmap.loadFromData(data)) {
        qWarning() << "Failed to load pixmap from data";
        return QPixmap();
    }

    constexpr qreal scaleDpi = 1.0; // 先用1.0测试，确认没问题后再换GLOB_ScaleDpi

    QSize realSize = QSize(pixmap.width() * scaleDpi, pixmap.height() * scaleDpi);
    if (realSize.width() <= 0 || realSize.height() <= 0) {
        qWarning() << "Invalid size for pixmap:" << realSize;
        return QPixmap();
    }

    QPixmap resultPixmap(realSize);
    resultPixmap.fill(Qt::transparent);

    QPainter painter(&resultPixmap);
    if (!painter.isActive()) {
        qWarning() << "Painter is not active!";
        return QPixmap();
    }
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    QPainterPath path;
    path.addRoundedRect(QRect(QPoint(0, 0), realSize), realSize.width() / 2, realSize.height() / 2);
    painter.setClipPath(path);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::NoBrush);

    painter.drawPixmap(QRect(QPoint(0, 0), realSize), pixmap.scaled(realSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    painter.end();

    resultPixmap.setDevicePixelRatio(scaleDpi);
    return resultPixmap;
}

// QPixmap RoundImage::RoundImageFromByteArray(const QByteArray& data)
// {
// 	QPixmap pixmap;
// 	pixmap.loadFromData(data);
// 	QSize realSize = QSize(pixmap.width() * GLOB_ScaleDpi, pixmap.height() * GLOB_ScaleDpi);
// 	QPixmap resultPixmap(realSize);
// 	resultPixmap.fill(Qt::transparent);
// 	QPainter painter(&resultPixmap);
// 	painter.setRenderHints(QPainter::Antialiasing, true);
// 	painter.setRenderHints(QPainter::SmoothPixmapTransform);
// 	QPainterPath path;
// 	path.addRoundedRect(QRect(QPoint(0, 0), QSize(realSize)), realSize.width() / 2, realSize.height() / 2);
// 	painter.setClipPath(path);
// 	painter.setPen(Qt::NoPen);
// 	painter.setBrush(Qt::NoBrush);
// 	painter.drawPixmap(QRect(QPoint(0, 0), QSize(realSize)), pixmap.scaled(QSize(realSize), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
// 	resultPixmap.setDevicePixelRatio(GLOB_ScaleDpi);
// 	return resultPixmap;
// }

QPixmap RoundImage::SquareImageFromPath(const QString& imagePath)
{
	QPixmap pixmap(imagePath);
	QSize realSize = QSize(640 * GLOB_ScaleDpi, 640 * GLOB_ScaleDpi);
	QPixmap resultPixmap(realSize);
	resultPixmap.fill(Qt::transparent);
	QPainter painter(&resultPixmap);
	painter.setRenderHints(QPainter::Antialiasing, true);
	painter.setRenderHints(QPainter::SmoothPixmapTransform);
	QPainterPath path;
	path.addRoundedRect(QRect(QPoint(0, 0), QSize(realSize)), static_cast<qreal>(640) / 15, static_cast<qreal>(640) / 15);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::NoBrush);
	painter.setClipPath(path);
	painter.drawPixmap(QRect(QPoint(0, 0), QSize(realSize)), pixmap.scaled(QSize(realSize), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	resultPixmap.setDevicePixelRatio(GLOB_ScaleDpi);
	return resultPixmap;
}

RoundImage::RoundImage()
{
}

RoundImage::~RoundImage()
{
}