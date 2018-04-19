#include "pca_image.h"
#include <iostream>
#include <chrono>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;






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
	reconstruction = PcaImage::toGrayscale(reconstruction);
	cv::imshow(p->winName, reconstruction);
	std::cout << "done!   # of principal components: " << p->pca.eigenvectors.rows << std::endl;
}




int main(int argc, char** argv)
{
	PcaImage pca_img;
	auto start_time = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start_time;

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
	start_time = std::chrono::system_clock::now();
	try 
	{
		pca_img.loadImages(imgList);
	}
	catch (cv::Exception& e) 
	{
		std::cerr << "Error opening file \"" << imgList << "\". Reason: " << e.msg << std::endl;
		return EXIT_FAILURE;
	}
	elapsed_seconds = std::chrono::system_clock::now() - start_time;
	std::cout << "<Info>  Loading images    : " << elapsed_seconds.count() << "s\n";

	const int num_channels = pca_img.images[0].channels();
	const int num_rows = pca_img.images[0].rows;

	// Quit if there are not enough images for this demo.
	if (pca_img.images.size() <= 1)
	{
		std::string error_message = "This demo needs at least 2 images to work. Please add more images to your data set!";
		CV_Error(cv::Error::StsError, error_message);
	}

	// Reshape and stack images into a rowMatrix
	start_time = std::chrono::system_clock::now();
	{
		pca_img.formatImagesForPCA();
	}
	elapsed_seconds = std::chrono::system_clock::now() - start_time;
	std::cout << "<Info>  Format for PCA    : " << elapsed_seconds.count() << "s\n";

	start_time = std::chrono::system_clock::now();
	{
		pca_img.run();
	}
	elapsed_seconds = std::chrono::system_clock::now() - start_time;
	std::cout << "<Info>  Run PCA           : " << elapsed_seconds.count() << "s\n";

	start_time = std::chrono::system_clock::now();
	{
		pca_img.save(outputFilename);
	}
	elapsed_seconds = std::chrono::system_clock::now() - start_time;
	std::cout << "<Info>  Save Projection   : " << elapsed_seconds.count() << "s\n";

#if 0
	start_time = std::chrono::system_clock::now();
	cv::Mat point = pca_img.pca->project(pca_img.data.row(0)); // project into the eigenspace, thus the image becomes a "point"
	elapsed_seconds = std::chrono::system_clock::now() - start_time;
	std::cout << "<Info>  Projection        : " << elapsed_seconds.count() << "s\n";

	start_time = std::chrono::system_clock::now();
	cv::Mat reconstruction = pca_img.pca->backProject(point); // re-create the image from the "point"
	reconstruction = reconstruction.reshape(num_channels, num_rows); // reshape from a row vector into image shape
	reconstruction = PcaImage::toGrayscale(reconstruction); // re-scale for displaying purposes
	elapsed_seconds = std::chrono::system_clock::now() - start_time;
	std::cout << "<Info>  Back projection   : " << elapsed_seconds.count() << "s\n";
#else
	start_time = std::chrono::system_clock::now();
	cv::Mat reconstruction;
	pca_img.backProject(pca_img.projection.row(0), reconstruction);
	elapsed_seconds = std::chrono::system_clock::now() - start_time;
	std::cout << "<Info>  Back projection   : " << elapsed_seconds.count() << "s\n";
#endif
	
	//cv::imwrite("G:/Data/Figurantes/Textures/_data_clone.jpg", data_clone);
	//cv::imwrite("G:/Data/Figurantes/Textures/_reconstruction.jpg", reconstruction);

	//cv::Mat mean = pca_img.pca->mean.clone();
	//mean = mean.reshape(num_channels, num_rows); // reshape from a row vector into image shape
	//mean = PcaImage::toGrayscale(mean); // re-scale for displaying purposes
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