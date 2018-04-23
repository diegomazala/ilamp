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
		projection = cv::Mat(data.rows, data.rows, data.type());
		for (int i = 0; i < data.rows; ++i)
		{
			pca->project(data.row(i), projection.row(i));
		}
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


	void backProject(const cv::Mat& point, cv::Mat& output)
	{
		output = pca->backProject(point); // re-create the image from the "point"
		output = output.reshape(images[0].channels(), images[0].rows); // reshape from a row vector into image shape
		output = toGrayscale(output); // re-scale for displaying purposes
	}

	void backProject(float* point_array, cv::Mat& output)
	{
		cv::Mat point(1, projection.cols, CV_32F, point_array);
		output = pca->backProject(point); // re-create the image from the "point"
		output = output.reshape(images[0].channels(), images[0].rows); // reshape from a row vector into image shape
		output = toGrayscale(output); // re-scale for displaying purposes
		//cv::imwrite("G:/Data/Figurantes/Textures/_output.jpg", output);
	}

	void backProject(int index, cv::Mat& output)
	{
		output = pca->backProject(projection.row(index)); // re-create the image from the "point"
		output = output.reshape(images[0].channels(), images[0].rows); // reshape from a row vector into image shape
		output = toGrayscale(output); // re-scale for displaying purposes
	}

	void backProject(float* point_array)
	{
		cv::Mat point(1, projection.cols, CV_32F, point_array);
		cv::Mat output = pca->backProject(point); // re-create the image from the "point"
		output = output.reshape(images[0].channels(), images[0].rows); // reshape from a row vector into image shape
		output = toGrayscale(output); // re-scale for displaying purposes
		//cv::imwrite("G:/Data/Figurantes/Textures/_output.jpg", output);
	}


	void mean(cv::Mat& mean_image)
	{
		mean_image = pca->mean.clone();
		mean_image = mean_image.reshape(images[0].channels(), images[0].rows); // reshape from a row vector into image shape
		mean_image = toGrayscale(mean_image); // re-scale for displaying purposes
	}


	static cv::Mat toGrayscale(cv::InputArray _src)
	{
		cv::Mat src = _src.getMat();
		// only allow one channel
		if (src.channels() != 1)
		{
			CV_Error(cv::Error::StsBadArg, "Only Matrices with one channel are supported");
		}
		// create and return normalized image
		cv::Mat dst;
		cv::normalize(_src, dst, 0, 255, cv::NORM_MINMAX, CV_8UC1);
		return dst;
	}



	void save(const std::string& filename)
	{
		std::ofstream pca_file(filename, std::ios::out);
		//pca_file << pca->eigenvalues << std::endl << std::endl;
		for (int i = 0; i < projection.rows; ++i)
		{
			for (int j = 0; j < projection.cols; ++j)
			{
				pca_file << std::fixed << projection.at<float>(i, j) << ' ';
			}
			pca_file << std::endl;
		}
		pca_file.close();
	}


	std::vector<cv::Mat> images;
	cv::Mat data;
	cv::Mat projection;
	std::unique_ptr<cv::PCA> pca;

};






#endif // __PCA_IMAGE_H__