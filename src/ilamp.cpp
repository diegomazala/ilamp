#include "ilamp.h"


#include "ilamp_project.h"
#include <iostream>
#include <iomanip> 
#include <fstream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <flann/flann.hpp>
#include <Eigen/Dense>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

template <typename Type>
static Type DegToRad(Type angle_in_degrees)
{
	return angle_in_degrees * (Type)(M_PI / 180.0);
}

template <typename Type>
static Type RadToDeg(Type angle_in_radians)
{
	return angle_in_radians * (Type)(180.0 / M_PI);
}


typedef float Decimal;




int main(int argc, char* argv[])
{
	ilamp_project prj;
	prj.test();
	return 0 ;

	std::cout
		<< std::fixed << std::endl
		<< "Usage            : ./<app.exe> <data_file_2d> <data_file_Nd> <number_of_neighbours> <kd_tree_count> <knn_search_checks>" << std::endl
		<< "Default          : ./ilamp.exe iris.2d iris.data 16 4 128" << std::endl
		<< std::endl;

	ILamp<Decimal> ilamp;

	//std::string input_filename_2d = "../../data/iris_5d.2d";
	//std::string input_filename_Nd = "../../data/iris.5d";
	//std::string output_filename_Nd = "../../data/iris_out.5d";
	//std::string output_filename_dist_Nd = "../../data/iris_out_dist.5d";

	std::string input_filename_2d = "G:/Data/Heads/heads.2d";
	std::string input_filename_Nd = "G:/Data/Heads/heads.nd";
	std::string output_filename_Nd = "G:/Data/Heads/heads_out.nd";
	std::string output_filename_dist_Nd = "G:/Data/Heads/heads.dist";

	//std::string input_filename_2d = "../../../../Data/Primitives/primitives.2d";
	//std::string input_filename_Nd = "../../../../Data/Primitives/primitives.nd";
	//std::string output_filename_Nd = "../../../../Data/Primitives/primitives_out.nd";
	//std::string output_filename_dist_Nd = "../../../../Data/Primitives/primitives.dist";

	//if (argc > 1)
	//	input_filename_2d = argv[1];
	//if (argc > 2)
	//	input_filename_Nd = argv[2];

	const uint16_t numNeighbours = (argc > 3) ? atoi(argv[3]) : 4;
	const uint16_t kdTreeCount = (argc > 4) ? atoi(argv[4]) : 4;
	const uint16_t knnSearchChecks = (argc > 5) ? atoi(argv[5]) : 128;
	//const int numNeighbours = 4;
	//const int kdTreeCount = 4;
	//const int knnSearchChecks = 128;

	

	//
	// Import 2d file
	// 
	
	if (!ilamp.load_data_2d(input_filename_2d))
		return EXIT_FAILURE;

	//
	// Import Nd file
	// 
	if (!ilamp.load_data_Nd(input_filename_Nd))
		return EXIT_FAILURE;


	if (ilamp.verts_Nd.size() != ilamp.verts_2d.size())
	{
		std::cerr << "<Error> Vertex arrays do not have the same size. Abort" << std::endl;
		return EXIT_FAILURE;
	}


	const int dimension_2d = ilamp.verts_2d.at(0).rows();	// = 2
	const int dimension_Nd = ilamp.verts_Nd.at(0).rows();	// = N
	

	//
	// Build the kd-tree
	//
	ilamp.build_kdtree(kdTreeCount);


	//
	// Output info
	// 
	std::cout << std::fixed
		<< "<Info>  Dimension         : " << dimension_2d << std::endl
		<< "<Info>  Dimension         : " << dimension_Nd << std::endl
		<< "<Info>  NumNeighbours     : " << numNeighbours << std::endl
		<< "<Info>  KdTreeCount       : " << kdTreeCount << std::endl
		<< "<Info>  KnnSearchChecks   : " << knnSearchChecks << std::endl
		<< std::endl;


	std::ofstream output_file(output_filename_Nd, std::ios::out);
	std::ofstream output_file_dist(output_filename_dist_Nd, std::ios::out);


	const int k = numNeighbours; // k = numNeighbours

	for (int main_index = 0; main_index < ilamp.verts_2d.size(); ++main_index)
	{
		const auto& p = ilamp.verts_2d[main_index];
		
		std::cout << "<Info>  Computing vertex: " << p.transpose() << std::endl;

		auto start_time = std::chrono::system_clock::now();
		
		const auto& q = ilamp.execute(p.x(), p.y());

		std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start_time;
		std::cout << "<Info>  Elapsed Time: " << elapsed_seconds.count() << "s\n";

		//
		// Write q to file 
		//
		output_file << std::fixed << std::setprecision(2) << q.transpose() << std::endl;
		

		//
		// Write file woth the distances between original q and resultant q
		//
		const auto& q_orig = ilamp.verts_Nd[main_index];
		const auto dist = (q - q_orig).norm();
		output_file_dist << dist << std::endl;
	}

	// 
	// Close files
	//
	output_file.close();
	output_file_dist.close();

	std::cout << std::endl;

	return EXIT_SUCCESS;
}