#pragma once
#include <QImage>
#include "opencv2/core.hpp"

namespace ImageHandler
{
	// 输入图片、需要的宽高、圆角、是否模糊，就把图片处理好并返回
	QPixmap cutImage(const QImage& image, int width, int height, int radius, bool blur, double brightness);

	QRgb getMainColor(const QImage& image);

	QString getTextColor(const QImage& image);
	
	cv::Mat QImageToCvMat(const QImage& inImage, bool inCloneImageData = true);

	QImage cvMatToQImage(const cv::Mat& inMat);
	
	QImage roundImage(const QImage& image, int radius);
	
	cv::Mat roundCVMat(const cv::Mat& image, int radius);
}

