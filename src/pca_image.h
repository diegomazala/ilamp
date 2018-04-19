#ifndef __PCA_IMAGE_H__
#define __PCA_IMAGE_H__

#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <algorithm>
#include <memory>

class PcaImage
{
public:
	PcaImage() {}

	bool loadImages(const std::string& input_filename)
	{
		std::ifstream file(input_filename.c_str(), std::ifstream::in);
		if (!file)
		{
			std::string error_message = "No valid input file was given, please check the given filename.";
			CV_Error(cv::Error::StsBadArg, error_message);
			return false;
		}

		std::string line;
		while (getline(file, line))
		{
			images.push_back(cv::imread(line, cv::IMREAD_GRAYSCALE));
		}
		return true;
	}

	void run()
	{
		pca.reset(new cv::PCA(data, cv::Mat(), cv::PCA::DATA_AS_ROW));
	}


	void formatImagesForPCA()
	{
		int rows = images[0].rows;
		int cols = images[0].cols;
		int image_count = (int)images.size();

		data = cv::Mat(image_count, rows * cols, CV_32F);
		for (int i = 0; i < image_count; i++)
		{
			cv::Mat image_row = images[i].clone().reshape(1, 1);
			cv::Mat row_i = data.row(i);
			image_row.convertTo(row_i, CV_32F);
		}
	}


	void save(const std::string& filename)
	{
		std::ofstream pca_file(filename, std::ios::out);
		//pca_file << pca->eigenvalues << std::endl << std::endl;
		for (int i = 0; i < data.rows; ++i)
		{
			const cv::Mat& point = pca->project(data.row(i)); // project into the eigenspace, thus the image becomes a "point"
			for (int j = 0; j < point.cols; ++j)
			{
				pca_file << std::fixed << point.at<float>(j) << ' ';
			}
			pca_file << std::endl;
		}
		pca_file.close();
	}


	std::vector<cv::Mat> images;
	cv::Mat data;
	std::unique_ptr<cv::PCA> pca;

};




#endif // __PCA_IMAGE_H__