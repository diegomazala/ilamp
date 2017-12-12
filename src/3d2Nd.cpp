#include <iostream>
#include <iomanip> 
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;


#include "tinyply.h"
uint32_t read_ply_file(const std::string & filename, std::vector<float>& verts)
{
	try
	{
		std::ifstream ss(filename, std::ios::binary);
		tinyply::PlyFile file(ss);
		uint32_t vertexCount = file.request_properties_from_element("vertex", { "x", "y", "z" }, verts);
		file.read(ss);
		return vertexCount;
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
		<< "Usage            : ./<app.exe> <dir> <file_extension> <output_filename>" << std::endl
		<< "Default          : ./3d2Nd.exe ../../../../data/Heads/ ply output.nd" << std::endl
		<< std::endl;

	//
	// Initial parameters
	//
	const fs::path input_dir = (argc > 1) ? argv[1] : "../../../../Data/Heads/";
	std::string file_extension = (argc > 2) ? argv[2] : "ply";
	const std::string output_filename = (argc > 3) ? argv[3] : "output.nd";

	//
	// check if there is the 'dot' in the extension
	//
	if (file_extension.find('.') == file_extension.npos)
		file_extension.insert(file_extension.begin(), '.');

	// 
	// Compose output filename
	//
	std::stringstream output_abs_filename;
	output_abs_filename << input_dir << '/' + output_filename;
	std::ofstream output_file(output_abs_filename.str(), std::ios::out);

	//
	// Iterate through the files in directory
	//
	for (const auto& p : fs::directory_iterator(input_dir))
	{
		if (p.path().extension() == file_extension)
		{
			std::vector<float> verts;
			const auto vertex_count = read_ply_file(p.path().string(), verts);
			if (vertex_count > 0)
			{
				std::cout << p << " : " << vertex_count << " vertices" << std::endl;
				for(const auto v : verts)
					output_file << std::fixed << std::setprecision(2) << v << ' ';
				output_file << std::endl;
			}
			else
			{
				std::cerr << "<Error> Could not load file: " << p << std::endl;
			}
		}
	}

	//
	// Close file
	//
	output_file.close();

	return EXIT_SUCCESS;
}