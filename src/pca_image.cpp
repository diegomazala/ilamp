#include "pca_image.h"
#include <iostream>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;





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

struct params
{
	cv::Mat data;
	int ch;
	int rows;
	cv::PCA pca;
	std::string winName;
};


static void onTrackbar(int pos, void* ptr)
{
	pos = std::max(5, pos);
	
	std::cout << "Retained Variance = " << pos << "%   ";
	std::cout << "re-calculating PCA..." << std::flush;
	
	double var = pos / 100.0;
	struct params *p = (struct params *)ptr;
	p->pca = cv::PCA(p->data, cv::Mat(), cv::PCA::DATA_AS_ROW, var);
	cv::Mat point = p->pca.project(p->data.row(0));
	cv::Mat reconstruction = p->pca.backProject(point);
	reconstruction = reconstruction.reshape(p->ch, p->rows);
	reconstruction = toGrayscale(reconstruction);
	cv::imshow(p->winName, reconstruction);
	std::cout << "done!   # of principal components: " << p->pca.eigenvectors.rows << std::endl;
}




int main(int argc, char** argv)
{
	PcaImage pca_img;

	cv::CommandLineParser parser(argc, argv, "{@input||image list}{@output||pca filename}{help h||show help message}");
	if (parser.has("help"))
	{
		parser.printMessage();
		exit(0);
	}

	//// Get the path to file list
	std::string imgList = parser.get<std::string>("@input");
	if (imgList.empty())
	{
		parser.printMessage();
		exit(1);
	}

	std::string outputFilename = parser.get<std::string>("@output");
	if (outputFilename.empty())
	{
		outputFilename = fs::path(imgList).replace_extension(".pca").string();
	}


	// vector to hold the images
	// Read in the data. This can fail if not valid
	try 
	{
		pca_img.loadImages(imgList);
	}
	catch (cv::Exception& e) 
	{
		std::cerr << "Error opening file \"" << imgList << "\". Reason: " << e.msg << std::endl;
		exit(1);
	}

	const int num_channels = pca_img.images[0].channels();
	const int num_rows = pca_img.images[0].rows;

	// Quit if there are not enough images for this demo.
	if (pca_img.images.size() <= 1)
	{
		std::string error_message = "This demo needs at least 2 images to work. Please add more images to your data set!";
		CV_Error(cv::Error::StsError, error_message);
	}

	// Reshape and stack images into a rowMatrix
	pca_img.formatImagesForPCA();

	pca_img.run();

	pca_img.save(outputFilename);

	cv::Mat point = pca_img.pca->project(pca_img.data.row(0)); // project into the eigenspace, thus the image becomes a "point"
	cv::Mat reconstruction = pca_img.pca->backProject(point); // re-create the image from the "point"
	reconstruction = reconstruction.reshape(num_channels, num_rows); // reshape from a row vector into image shape
	reconstruction = toGrayscale(reconstruction); // re-scale for displaying purposes
	
	cv::Mat mean = pca_img.pca->mean.clone();
	mean = mean.reshape(num_channels, num_rows); // reshape from a row vector into image shape
	mean = toGrayscale(mean); // re-scale for displaying purposes
	
	//cv::imwrite("../data/heads/pca_mean.jpg", mean);

	// init highgui window
	std::string winName = "Reconstruction | press 'q' to quit";
	cv::namedWindow(winName, cv::WINDOW_NORMAL);
	// params struct to pass to the trackbar handler
	params p;
	p.data = pca_img.data;
	p.ch = pca_img.images[0].channels();
	p.rows = pca_img.images[0].rows;
	p.pca = *pca_img.pca;
	p.winName = winName;
	
	// create the tracbar
	int pos = 100;
	cv::createTrackbar("Retained Variance (%)", winName, &pos, 100, onTrackbar, (void*)&p);
	// display until user presses q
	imshow(winName, reconstruction);
	char key = 0;
	while (key != 'q')
		key = (char)cv::waitKey();


	return EXIT_FAILURE;
}