#include "imp_dll.h"
#include "ilamp_utils.h"
#include "cmdline.h"
#include "timer.h"
#include "imp_project.h"
#include "vector_read_write_binary.h"


#include <iostream>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;


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
	float query[2] = { cmd_parser.get<float>("query_x"), cmd_parser.get<float>("query_y") };


	Imp_Create_ILamp();

	if (!Imp_LoadProject(project_filename.c_str(), cmd_parser.exist("lamp")))
	{
		std::cerr << "Error: Could not load project file" << std::endl;
		return EXIT_FAILURE;
	}

	if (!Imp_Build())
	{
		std::cerr << "Error: Could not build project" << std::endl;
		return EXIT_FAILURE;
	}


	const int test_index = cmd_parser.get<int>("test");
	if (test_index > -1)
	{
		float* query_ptr = (float*)Imp_GetVertices2d(test_index);
		query[0] = query_ptr[0];
		query[1] = query_ptr[1];
	}

	if (!Imp_Execute(query[0], query[1]))
	{
		std::cerr << "Error: Could not execute query: " << query[0] << ", " << query[1] << std::endl;
		return EXIT_FAILURE;
	}

	float* q = (float*)Imp_GetQ();


	//
	// Output ply file for sanity check
	//
	{
		imp_project ilp_prj(project_filename);

		//
		// Set output file name
		//
		std::string output_basename = fs::path(cmd_parser.get<std::string>("output3d")).replace_extension("").string();
		std::stringstream output_file;
		output_file << ilp_prj.outputFolder << "/" << output_basename << '_' << query[0] << '_' << query[1] << ".ply";
		const std::string output_filename = output_file.str();

		
		std::vector<uint32_t> triangles;
		vector_read(ilp_prj.trianglesFile, triangles);

		write_ply_file(
			output_filename,
			std::vector<float>(q, q + Imp_QRows() * Imp_QCols()),
			triangles);
	}
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
