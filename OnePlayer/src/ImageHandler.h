#pragma once
#include <QImage>
#include "opencv2/core.hpp"

namespace ImageHandler
{
	// 输入图片、需要的宽高、四个圆角、是否模糊，就把图片处理好并返回
	QPixmap cutImage(const QImage& image, int width, int height, int radiusTL
		, int radiusTR, int radiusBL, int radiusBR, bool blur, double brightness);

	// 与上同，但四个圆角相等
	QPixmap cutImage(const QImage& image, int width, int height, int radius, bool blur, double brightness);

	QRgb getMainColor(const QImage& image);

	QString getTextColor(const QImage& image);
	
	cv::Mat QImageToCvMat(const QImage& inImage, bool inCloneImageData = true);

	QImage cvMatToQImage(const cv::Mat& inMat);
	
	QImage roundImage(const QImage& image, int radiusTL, int radiusTR
		, int radiusBL, int radiusBR);
	
	cv::Mat roundCVMat(const cv::Mat& image, int radiusTL, int radiusTR
		, int radiusBL, int radiusBR);

	QImage downloadImage(const QString& url);
}

