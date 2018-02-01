#include "ilamp.h"
#include "ilamp_utils.h"
#include "ilamp_project.h"
#include "cmdline.h"


#include <iostream>
#include <iomanip> 
#include <fstream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <flann/flann.hpp>
#include <Eigen/Dense>

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

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





cmdline::parser cmd_parser;
void cmd_parser_build()
{
	cmd_parser.add<std::string>("project", 'p', "Project file name", true, "G:/Data/Test/Heads.ilp");
	cmd_parser.add<float>("query_x", 'x', "Query point x", false, 0.0f);
	cmd_parser.add<float>("query_y", 'y', "Query point y", false, 0.0f);
	cmd_parser.add<std::string>("output3d", 'o', "Output file name (ply)", false, "Output.ply");
	cmd_parser.add<int>("test", 't', "Use one of the 2d vertices as input", false, -1);	// boolean sample
	cmd_parser.add("lamp", 'l', "Run lamp");
	//cmd_parser.add<std::string>("help", '?', "");
}


static void run_lamp(const std::string& filenameNd, const std::string& filename2d)
{
	//
	// Running lamp in order to generate 2d file from nd file
	//
	std::string lamp_script = std::getenv("ILAMP_LAMP_SCRIPT");

	if (!fs::exists(lamp_script))
	{
		std::cerr << "Lamp python script not found" << std::endl;
		return;
	}

	std::stringstream lamp_cmd;
	lamp_cmd << "python " << lamp_script << ' ' << filenameNd << ' ' << filename2d << " > lamp.log";
	std::system(lamp_cmd.str().c_str());
#if _DEBUG
	std::cout << std::ifstream("lamp.log").rdbuf();
#endif
}


int main(int argc, char* argv[])
{
	//
	// Parse command line
	//
	cmd_parser_build();
	cmd_parser.parse_check(argc, argv);

	//
	// Set variables
	//
	const std::string& project_filename = cmd_parser.get<std::string>("project");
	ilamp_project ilp_prj(project_filename);

#if _DEBUG
	std::cout << std::setw(4) << ilp_prj << std::endl << std::endl;
#endif


	Eigen::Vector2f query(cmd_parser.get<float>("query_x"), cmd_parser.get<float>("query_y"));


	//
	// Create output directory
	//
	fs::path prj_path(project_filename);
	const std::string prj_dir = prj_path.remove_filename().string();
	fs::create_directories(ilp_prj.outputFolder);

	//
	// If true, build a new nd file and run lamp for it
	// Otherwise, use the existent file
	//
	if (cmd_parser.exist("lamp"))
	{
		//
		// Creating nd file
		//
		build_nd_file(ilp_prj.inputFiles, ilp_prj.filenameNd);

		//
		// Run lamp
		//
		run_lamp(ilp_prj.filenameNd, ilp_prj.filename2d);
	}

		

	ILamp<float> ilamp;

	//
	// Import 2d file
	// 
	if (!ilamp.load_data_2d(ilp_prj.filename2d))
		return EXIT_FAILURE;

	//
	// Import Nd file
	// 
	if (!ilamp.load_data_Nd(ilp_prj.filenameNd))
		return EXIT_FAILURE;


	if (ilamp.verts_Nd.size() != ilamp.verts_2d.size())
	{
		std::cerr << "<Error> Vertex arrays do not have the same size. Abort" << std::endl;
		return EXIT_FAILURE;
	}


	const int dimension_2d = ilamp.verts_2d.at(0).rows();	// = 2
	const int dimension_Nd = ilamp.verts_Nd.at(0).rows();	// = N
	const int test_index = cmd_parser.get<int>("test");
	if (test_index > -1 )
		query = ilamp.verts_2d.at(test_index);

	//
	// Set output file name
	//
	std::string output_basename = fs::path(cmd_parser.get<std::string>("output3d")).replace_extension("").string();
	std::stringstream output_file;
	output_file << ilp_prj.outputFolder << "/" << output_basename << '_' << query.x() << '_' << query.y() << ".ply";
	const std::string output_filename = output_file.str();
	const std::string template_filename = ilp_prj.inputFiles[0];

	//
	// Build the kd-tree
	//
	ilamp.build_kdtree(ilp_prj.kdTreeCount);


	//
	// Computing ilamp
	//
	std::cout << "<Info>  Computing vertex  : " << query.transpose() << std::endl;
	auto start_time = std::chrono::system_clock::now();
	
	const auto& q = ilamp.execute(		// 'query' is the 'p' in the paper
		query.x(), query.y(), 
		ilp_prj.numNeighbours, 
		ilp_prj.knnSearchChecks);			

	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start_time;
	std::cout << "<Info>  Elapsed Time      : " << elapsed_seconds.count() << "s\n";
	
	if (test_index > -1)
	{
		const auto& q_orig = ilamp.verts_Nd.at(test_index);
		const auto dist = (q - q_orig).norm();
		std::cout << "<Info>  Distance Error    : " << dist << std::endl;
	}


	write_ply_file(
		output_filename, 
		std::vector<float>(&q[0], q.data() + q.cols()*q.rows()), 
		template_filename);


	std::cout << std::endl;

	return EXIT_SUCCESS;
}






#if 0 // _TEST_ALL_VERTS_2D
	std::string output_filename_Nd = "G:/Data/Heads/heads_out.nd";
	std::string output_filename_dist_Nd = "G:/Data/Heads/heads.dist";

	std::ofstream output_file(output_filename_Nd, std::ios::out);
	std::ofstream output_file_dist(output_filename_dist_Nd, std::ios::out);


	const int k = numNeighbours; // k = numNeighbours

	for (int main_index = 0; main_index < ilamp.verts_2d.size(); ++main_index)
	{
		auto p = ilamp.verts_2d[main_index];

		std::cout << "<Info>  Computing vertex: " << p.transpose() << std::endl;
		auto start_time = std::chrono::system_clock::now();
		const auto& q = ilamp.execute(p.x(), p.y());
		std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start_time;
		std::cout << "<Info>  Elapsed Time    : " << elapsed_seconds.count() << "s\n";

		//
		// Write q to file 
		//
		//output_file << std::fixed << std::setprecision(2) << q.transpose() << std::endl;


		//
		// Write file woth the distances between original q and resultant q
		//
		const auto& q_orig = ilamp.verts_Nd[main_index];
		const auto dist = (q - q_orig).norm();
		output_file_dist << std::fixed << dist << std::endl;
		std::cout << "<Info>  Distance Error  : " << dist << std::endl;
	}

	// 
	// Close files
	//
	output_file.close();
	output_file_dist.close();
#endif
