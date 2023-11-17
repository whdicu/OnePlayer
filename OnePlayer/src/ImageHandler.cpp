#include "ImageHandler.h"
#include "opencv2/imgproc.hpp"
#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QElapsedTimer>

QPixmap ImageHandler::cutImage(const QImage& image, int width, int height, int radius, bool blur, double brightness)
{
	cv::Mat blurredMat;
	if (blur)
	{
		QElapsedTimer time;
		time.start();
		cv::GaussianBlur(QImageToCvMat(image), blurredMat, cv::Size(101, 101), 18);  // 31 8
		qDebug() << "GaussianBlur:" << time.elapsed();
	}
	else
		blurredMat = QImageToCvMat(image);

	// 将mat裁剪
	int widthByHeight = blurredMat.cols * height / width;

	double scale = 1.0;
	if (blurredMat.rows > widthByHeight)  // 竖直长条形
	{
		// 根据宽度缩放图像到指定大小
		scale = static_cast<double>(width) / blurredMat.cols;
	}
	else  // 横向长条形
	{
		// 根据高度缩放图像到指定大小
		scale = static_cast<double>(height) / blurredMat.rows;
	}
	cv::Mat resizedImage;
	cv::resize(blurredMat, resizedImage, cv::Size(), scale, scale);

	// 裁剪到与label相同大小
	cv::Rect roi((resizedImage.cols - width) / 2, (resizedImage.rows - height) / 2, width, height);
	cv::Mat croppedImage = resizedImage(roi);

	// 降低图片亮度
	if (croppedImage.type() == CV_8UC4)
		cv::cvtColor(croppedImage, croppedImage, cv::COLOR_RGBA2RGB);
	cv::Mat darkened_image = brightness * croppedImage;

	//qDebug() << croppedImage.cols << croppedImage.rows;
	//qDebug() << labelWidth << labelHeight;

	QImage aaa = cvMatToQImage(darkened_image);
	//qDebug() << aaa.width() << aaa.height();
	return QPixmap::fromImage(roundImage(aaa, radius));


	// 将mat添加圆角
	//cv::Mat mask(resizedImage.size(), CV_8UC1, cv::Scalar(255));
	//cv::rectangle(mask, roi, cv::Scalar(0), radius, cv::LINE_8);
	//cv::Mat result;
	//resizedImage.copyTo(result, mask);
	//return QPixmap::fromImage(cvMatToQImage(result));
	//
	//QImage blurredImage = cvMatToQImage(blurredMat);
	//QImage scaledImage = blurredImage.scaled(ui.label_background->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
	//
	//qDebug() << ui.label_background->size() << scaledImage.size();
	
}

QRgb ImageHandler::getMainColor(const QImage& image)
{
	unsigned long long r = 0, g = 0, b = 0, a = 0;
	unsigned long long cnt = 0;
	// 下半部分图像抽样取点
	for (int i = 0; i < image.width(); i += 4)
	{
		// 原来只采样图片下半部分
		//for (int j = image.height() / 2; j < image.height(); j += 4)
		for (int j = 0; j < image.height(); j += 4)
		{
			QRgb pix = image.pixel(i, j);
			r += qRed(pix);
			g += qGreen(pix);
			b += qBlue(pix);
			a += qAlpha(pix);
			++cnt;
		}
	}
	return qRgba(r / cnt, g / cnt, b / cnt, a / cnt);
}

QString ImageHandler::getTextColor(const QImage& image)
{
	QRgb c3 = getMainColor(image);

	return QString("#%1%2%3").arg((int)qMin(qRed(c3) * 1.3, 255.0), 2, 16, QChar('0'))
		.arg((int)qMin(qGreen(c3) * 1.3, 255.0), 2, 16, QChar('0'))
		.arg((int)qMin(qBlue(c3) * 1.3, 255.0), 2, 16, QChar('0'));

	/*if (qRed(c3) + qGreen(c3) + qBlue(c3) < 384)
		return "#b6d1c8";
	else
		return "#5c5c66";*/
}

cv::Mat ImageHandler::QImageToCvMat(const QImage& inImage,
	bool inCloneImageData)
{
	switch (inImage.format())
	{
		// 8-bit, 4 channel
	case QImage::Format_RGBA8888:
	{
		QImage   swapped = inImage.rgbSwapped();

		cv::Mat  mat(swapped.height(), swapped.width(),
			CV_8UC4, const_cast<uchar*>(swapped.bits()),
			swapped.bytesPerLine());

		return (inCloneImageData ? mat.clone() : mat);
	}
	case QImage::Format_ARGB32:
	{
		cv::Mat  mat(inImage.height(), inImage.width(),
			CV_8UC4, const_cast<uchar*>(inImage.bits()),
			inImage.bytesPerLine());

		return (inCloneImageData ? mat.clone() : mat);
	}
	case QImage::Format_RGB32:
	{
		cv::Mat  mat(inImage.height(), inImage.width(),
			CV_8UC4, const_cast<uchar*>(inImage.bits()),
			inImage.bytesPerLine());

		return (inCloneImageData ? mat.clone() : mat);
	}

	// 8-bit, 3 channel
	case QImage::Format_RGB888:
	{
		if (!inCloneImageData)
			qWarning() << "ASM::QImageToCvMat() - \
								  						Conversion requires cloning since we use \
																				a temporary QImage";

		//QImage   swapped = inImage.rgbSwapped();
		cv::Mat mat = cv::Mat(inImage.height(), inImage.width(),
			CV_8UC3, const_cast<uchar*>(inImage.bits()),
			inImage.bytesPerLine()).clone();
		cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);  // 快
		return mat;
	}

	// 8-bit, 1 channel
	case QImage::Format_Indexed8:
	case QImage::Format_Grayscale8:
	{
		cv::Mat  mat(inImage.height(), inImage.width(),
			CV_8UC1, const_cast<uchar*>(inImage.bits()),
			inImage.bytesPerLine());

		return (inCloneImageData ? mat.clone() : mat);
	}

	default:
		qWarning() << "ASM::QImageToCvMat() - QImage format \
							  					not handled in switch:" << inImage.format();
		break;
	}

	return cv::Mat();
}

QImage ImageHandler::cvMatToQImage(const cv::Mat& inMat)
{
	switch (inMat.type())
	{
		// 8-bit, 4 channel
	case CV_8UC4:
	{
		QImage image(inMat.data,
			inMat.cols, inMat.rows,
			static_cast<int>(inMat.step),
			QImage::Format_ARGB32);

		return image.copy();
	}

	// 8-bit, 3 channel
	case CV_8UC3:
	{
		cv::cvtColor(inMat, inMat, cv::COLOR_BGR2RGB);  // 快

		QImage image(inMat.data,
			inMat.cols, inMat.rows,
			static_cast<int>(inMat.step),
			QImage::Format_RGB888);

		//return image.rgbSwapped();
		return image.copy();
	}

	// 8-bit, 1 channel
	case CV_8UC1:
	{
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)

		QImage image(inMat.data,
			inMat.cols, inMat.rows,
			static_cast<int>(inMat.step),
			QImage::Format_Grayscale8);

#else
		static QVector<QRgb>  sColorTable;

		// only create our color table the first time
		if (sColorTable.isEmpty())
		{
			sColorTable.resize(256);

			for (int i = 0; i < 256; ++i)
			{
				sColorTable[i] = qRgb(i, i, i);
			}
		}

		QImage image(inMat.data,
			inMat.cols, inMat.rows,
			static_cast<int>(inMat.step),
			QImage::Format_Indexed8);

		image.setColorTable(sColorTable);
#endif

		return image.copy();
	}

	default:
		qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
		break;
	}

	return QImage();
}

QImage ImageHandler::roundImage(const QImage& image, int radius)
{
	QImage roundedImage(image.size(), QImage::Format_ARGB32_Premultiplied);
	roundedImage.fill(Qt::transparent);

	QPainter painter(&roundedImage);
	painter.setRenderHint(QPainter::Antialiasing);

	QPainterPath path;
	path.addRoundedRect(roundedImage.rect(), radius, radius);
	painter.setClipPath(path);
	painter.drawImage(roundedImage.rect(), image);

	return roundedImage;
}

cv::Mat ImageHandler::roundCVMat(const cv::Mat& image, int radius)
{
	// 创建遮罩层
	cv::Mat mask(image.size(), CV_8UC1, cv::Scalar(0));
	cv::Rect rectTopLeft(0, 0, 2 * radius, 2 * radius);
	cv::Rect rectTopRight(image.cols - 2 * radius, 0, 2 * radius, 2 * radius);
	cv::Rect rectBottomLeft(0, image.rows - 2 * radius, 2 * radius, 2 * radius);
	cv::Rect rectBottomRight(image.cols - 2 * radius, image.rows - 2 * radius, 2 * radius, 2 * radius);

	cv::circle(mask, rectTopLeft.tl() + cv::Point(radius, radius), radius, cv::Scalar(255), -1, cv::LINE_AA);
	cv::circle(mask, rectTopRight.tl() + cv::Point(-radius, radius), radius, cv::Scalar(255), -1, cv::LINE_AA);
	cv::circle(mask, rectBottomLeft.tl() + cv::Point(radius, -radius), radius, cv::Scalar(255), -1, cv::LINE_AA);
	cv::circle(mask, rectBottomRight.tl() + cv::Point(-radius, -radius), radius, cv::Scalar(255), -1, cv::LINE_AA);

	// 应用遮罩层
	cv::Mat result;
	image.copyTo(result, mask);
	return result;
}
