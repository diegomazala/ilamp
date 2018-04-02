#include "ilamp.h"
#include "ilamp_utils.h"
#include "ilamp_project.h"
#include "rbf_imp.h"
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

		
	RbfImp<float> rbf_imp;

	//
	// Import 2d file
	// 
	if (!rbf_imp.load_data_2d(ilp_prj.filename2d))
		return EXIT_FAILURE;

	//
	// Import Nd file
	// 
	if (!rbf_imp.load_data_Nd(ilp_prj.filenameNd))
		return EXIT_FAILURE;


	if (rbf_imp.verts_Nd.size() != rbf_imp.verts_2d.size())
	{
		std::cerr << "<Error> Vertex arrays do not have the same size. Abort" << std::endl;
		return EXIT_FAILURE;
	}


	const int dimension_2d = rbf_imp.verts_2d.at(0).rows();	// = 2
	const int dimension_Nd = rbf_imp.verts_Nd.at(0).rows();	// = N
	const int test_index = cmd_parser.get<int>("test");
	if (test_index > -1 )
		query = rbf_imp.verts_2d.at(test_index);

	//
	// Set output file name
	//
	std::string output_basename = fs::path(cmd_parser.get<std::string>("output3d")).replace_extension("").string();
	std::stringstream output_file;
	output_file << ilp_prj.outputFolder << "/" << output_basename << '_' << query.x() << '_' << query.y() << ".ply";
	const std::string output_filename = output_file.str();
	const std::string template_filename = ilp_prj.inputFiles[0];


	//
	// Computing rbf mpi
	//
	//std::cout << "<Info>  Computing vertex  : " << query.transpose() << std::endl;
	auto start_time = std::chrono::system_clock::now();
	
	rbf_imp.model_index = test_index;

	rbf_imp.execute(query.x(), query.y());

	

	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start_time;
	std::cout << "<Info>  Elapsed Time      : " << elapsed_seconds.count() << "s\n";
	

	if (test_index > -1)
	{
		const auto& q_orig = rbf_imp.verts_Nd.at(test_index);
		const auto dist = (rbf_imp.q - q_orig).norm();
		std::cout << "<Info>  Distance Error    : " << dist << std::endl;
	}


	write_ply_file(
		output_filename, 
		std::vector<float>(rbf_imp.q.data(), rbf_imp.q.data() + rbf_imp.q.cols() * rbf_imp.q.rows()),
		template_filename);


	std::cout << std::endl;

	return EXIT_SUCCESS;
}



