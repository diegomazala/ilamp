#include "imp_dll.h"
#include "cmdline.h"
#include "timer.h"

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


	//Imp_Create_ILamp();


#if 0
	std::vector<uint32_t> triangles;
	vector_read(ilp_prj.trianglesFile, triangles);

	write_ply_file(
		output_filename, 
		std::vector<float>(&ilamp.q[0], ilamp.q.data() + ilamp.q.cols()*ilamp.q.rows()),
		triangles);
#endif
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
