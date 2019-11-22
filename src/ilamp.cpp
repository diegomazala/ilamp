#include "ilamp.h"
#include "ilamp_utils.h"
#include "ilamp_project.h"
#include "cmdline.h"
#include "lamp.h"
#include "timer.h"

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
#if 1
	Lamp<float> lamp;
	if (!lamp.load_matrix_from_file(filenameNd))
	{
		std::cerr << "Error: Could not load file with nd points" << std::endl;
		return;
	}

	lamp.compute_control_points_by_distance();
	lamp.fit();
	lamp.save_matrix_to_file(filename2d);
	std::cerr << "<Info>  Lamp file saved: " << filename2d << std::endl;

#else
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
#endif
}

template <typename decimal_t>
bool load_matrix_from_files(Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>& X, const std::vector<std::string>& input_files, const std::size_t cols)
{
	X = Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>(input_files.size(), cols);

	auto file_size = cols * sizeof(decimal_t);
	try
	{
		for (auto i = 0; i < input_files.size(); ++i)
		{
			std::vector<float> verts;
			const auto vertex_count = vector_read(input_files[i], verts);
			if (vertex_count > 0)
				X.row(i) << Eigen::Map<Eigen::Matrix<float, 1, Eigen::Dynamic>>(verts.data(), 1, cols);
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << "<Error>  Could not read input files for matrix X" << std::endl;
		return false;
	}

	return true;
}

template <typename decimal_t>
void load_Nd_data(std::vector<Eigen::Matrix<decimal_t, Eigen::Dynamic, 1>>& verts_Nd, const std::vector<std::string>& input_files, const std::size_t cols)
{
	auto file_size = cols * sizeof(decimal_t);

	verts_Nd.resize(input_files.size());
	for (auto i = 0; i < input_files.size(); ++i)
	{
		std::vector<float> verts;
		const auto vertex_count = vector_read(input_files[i], verts);
		if (vertex_count > 0)
			verts_Nd[i] = Eigen::Map<Eigen::Matrix<float, 1, Eigen::Dynamic>>(verts.data(), 1, cols);
	}
}

int main(int argc, char* argv[])
{

	std::cout << std::fixed;
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


	ILamp<float> ilamp;
	
	//
	// Import Nd file
	// 
	//if (!ilamp.load_data_Nd(ilp_prj.filenameNd))
	//	return EXIT_FAILURE;
	load_Nd_data(ilamp.verts_Nd, ilp_prj.inputFiles, ilp_prj.vertexCount * 3);


	//
	// If true, build a new nd file and run lamp for it
	// Otherwise, use the existent file
	//
	if (cmd_parser.exist("lamp"))
	{
		Lamp<float> lamp;

		//
		// Build X matrix
		//
		lamp.X = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>(ilamp.verts_Nd.size(), ilamp.verts_Nd[0].size());
		//X << Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>>(ilamp.verts_Nd[0].data(), ilamp.verts_Nd.size(), ilamp.verts_Nd[0].size());
		for (auto i = 0; i < ilamp.verts_Nd.size(); ++i)
			lamp.X.row(i) << Eigen::Map<Eigen::Matrix<float, 1, Eigen::Dynamic>>(ilamp.verts_Nd[i].data(), 1, ilamp.verts_Nd[0].size());

		//
		// Run lamp
		//
		lamp.compute_control_points_by_distance();
		lamp.fit();
		lamp.save_matrix_to_file(ilp_prj.filename2d);
		std::cerr << "<Info>  Lamp file saved: " << ilp_prj.filename2d << std::endl;
	}

	//
	// Import 2d file
	// 
	if (!ilamp.load_data_2d(ilp_prj.filename2d))
		return EXIT_FAILURE;



	if (ilamp.verts_Nd.size() != ilamp.verts_2d.size())
	{
		std::cerr << "<Error> Vertex arrays do not have the same size. " << ilamp.verts_Nd.size() << " != " << ilamp.verts_2d.size() << ". Abort" << std::endl;
		return EXIT_FAILURE;
	}


	const int dimension_2d = ilamp.verts_2d.at(0).rows();	// = 2
	const int dimension_Nd = ilamp.verts_Nd.at(0).rows();	// = N
	const int test_index = cmd_parser.get<int>("test");
	if (test_index > -1)
		query = ilamp.verts_2d.at(test_index);

	//
	// Set output file name
	//
	std::string output_basename = fs::path(cmd_parser.get<std::string>("output3d")).replace_extension("").string();
	std::stringstream output_file;
	output_file << ilp_prj.outputFolder << "/" << output_basename << '_' << query.x() << '_' << query.y() << ".ply";
	const std::string output_filename = output_file.str();

	//
	// Build the kd-tree
	//
	timer tm_build_kdtree;
	tm_build_kdtree.start();
	{
		ilamp.set_kdtree(
			ilp_prj.kdTreeCount,
			ilp_prj.numNeighbours,
			ilp_prj.knnSearchChecks);
		//
		ilamp.build();
	}
	std::cout << std::fixed << "<Info>  Building KDTree   : " << tm_build_kdtree.diff_msec_now() << " msec " << std::endl;


	//
	// Computing ilamp
	//
	//std::cout << "<Info>  Computing vertex  : " << query.transpose() << std::endl;
	timer tm_computing_ilamp;
	tm_computing_ilamp.start();
	{
		//const auto& q = ilamp.execute(		// 'query' is the 'p' in the paper
		//	query.x(), query.y(), 
		//	ilp_prj.numNeighbours, 
		//	ilp_prj.knnSearchChecks);			

		ilamp.execute(query.x(), query.y());
	}
	std::cout << std::fixed << "<Info>  ILamp Computing   : " << tm_computing_ilamp.diff_msec_now() << " msec " << std::endl;

	
	if (test_index > -1)
	{
		const auto& q_orig = ilamp.verts_Nd.at(test_index);
		const auto dist = (ilamp.q - q_orig).norm();
		std::cout << "<Info>  Distance Error    : " << dist << std::endl;
	}

	std::vector<uint32_t> triangles;
	vector_read(ilp_prj.trianglesFile, triangles);

	write_ply_file(
		output_filename, 
		std::vector<float>(&ilamp.q[0], ilamp.q.data() + ilamp.q.cols()*ilamp.q.rows()),
		triangles);

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
