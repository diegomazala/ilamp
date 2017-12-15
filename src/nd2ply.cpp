#include <iostream>
#include <iomanip> 
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;


#include "tinyply.h"
uint32_t write_ply_file(const std::string & filename, std::vector<float>& verts)
{
	try
	{
		std::filebuf ply_fb;
		ply_fb.open(filename, std::ios::out | std::ios::binary);
		std::ostream ply_output_stream(&ply_fb);
		tinyply::PlyFile ply_file;
		ply_file.add_properties_to_element("vertex", { "x", "y", "z" }, verts);
		ply_file.write(ply_output_stream, true);
		ply_fb.close();

	}
	catch (const std::exception & e)
	{
		std::cerr << "Caught exception: " << e.what() << std::endl;
		return 0;
	}
}



int main(int argc, char* argv[])
{
	std::cout
		<< std::fixed << std::endl
		<< "Usage            : ./<app.exe> <input_filename> <output_folder>" << std::endl
		<< "Default          : ./3d2Nd.exe input.nd ../../../../data/Heads/ " << std::endl
		<< std::endl;

	//
	// Initial parameters
	//
	const std::string input_filename = (argc > 1) ? argv[1] : "../../../../Data/Heads/heads_out.nd";
	const fs::path output_dir = (argc > 2) ? argv[2] : "../../../../Data/Heads/iLamp/";
	//const std::string input_filename = (argc > 1) ? argv[1] : "../../data/iris_out.5d";
	//const fs::path output_dir = (argc > 2) ? argv[2] : "../../data/iLamp/";
	fs::create_directories(output_dir);


	//
	// Open file for reading
	//
	std::ifstream input_file(input_filename, std::ios::in);
	if (!input_file.is_open())
	{
		std::cout << "<Error> Could not load file : " << input_filename << std::endl;
		return EXIT_FAILURE;
	}


	int line_count = 0;
	while (!input_file.eof())
	{
		std::string line;
		std::getline(input_file, line);
		
		if (line.size() < 1)
			continue;

		std::vector<float> coords;
		std::istringstream line_stream(line);
		while(!line_stream.eof())
		{ 
			float f;
			line_stream >> f;
			coords.push_back(f);
		}
		
		// 
		// Compose output filename
		//
		const std::string basename = fs::path(input_filename).stem().string();
		std::stringstream output_filename;
		output_filename << output_dir << '/' << basename << '_' << line_count << ".ply";
		
		//
		// Write ply file
		//
		write_ply_file(output_filename.str(), coords);

		++line_count;

		std::cout << "<Info>  File lines processed: " << line_count << std::endl;
	}


	input_file.close();

	return EXIT_SUCCESS;
}