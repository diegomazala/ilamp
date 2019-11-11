#include <iostream>
#include "vector_read_write_binary.h"
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

void convert_quad_to_tri(const std::vector<uint32_t>& quads, std::vector<uint32_t>& tris)
{
	for (auto i = 0; i < quads.size(); i += 4)
	{
		tris.push_back(quads[i + 0]);
		tris.push_back(quads[i + 1]);
		tris.push_back(quads[i + 2]);

		tris.push_back(quads[i + 2]);
		tris.push_back(quads[i + 3]);
		tris.push_back(quads[i + 0]);
	}
}



int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout
			<< std::fixed << std::endl
			<< "Usage            : ./<app.exe> <input_filename> <output_filename>" << std::endl
			<< "Default          : ./quad_to_tri.exe cube.quad cube.tri" << std::endl
			<< std::endl;
		return EXIT_FAILURE;
	}


	//
	// Initial parameters
	//
	const fs::path input_filename = argv[1];
	const fs::path output_filename = (argc > 2) ? argv[2] : fs::path(input_filename).replace_extension(".tri");

	std::vector<uint32_t> quads;
	vector_read(input_filename.string(), quads);


	std::vector<uint32_t> tris;

	convert_quad_to_tri(quads, tris);

	vector_write(output_filename.string(), tris);


	return EXIT_SUCCESS;

}