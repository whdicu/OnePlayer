#pragma once
#include <QImage>
#include "opencv2/core.hpp"

namespace ImageHandler
{
	// 将图片保持比例缩放到指定大小，会去除多余部分
	cv::Mat fitImage(const cv::Mat& image, int width, int height);

	// 对图片模糊处理
	cv::Mat blurImage(const cv::Mat& image, int blurRadius);

	// 调整图像亮度
	cv::Mat lightImage(const cv::Mat& image, double brightness);

	// 裁剪图片
	cv::Mat cutImage(const cv::Mat& image, const QRect& rect);

	QRgb getMainColor(const cv::Mat& image);

	QString getTextColor(const cv::Mat& image);
	
	cv::Mat QImageToCvMat(const QImage& inImage, bool inCloneImageData = true);

	QImage cvMatToQImage(const cv::Mat& inMat);
	
	QImage roundImage(const QImage& image, int radius, bool onlyTop = false);
	
	cv::Mat roundCVMat(const cv::Mat& image, int radiusTL, int radiusTR
		, int radiusBL, int radiusBR);

	QImage downloadImage(const QString& url);
}

