#include <iostream>
#include "vector_read_write_binary.h"
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

void invert_triangles(std::vector<uint32_t>& indices)
{
	for (auto i = 0; i < indices.size(); i += 3)
	{
		std::swap(indices[i + 1], indices[i + 2]);
	}
}



int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout
			<< std::fixed << std::endl
			<< "Usage            : ./<app.exe> <input_filename> <output_filename>" << std::endl
			<< "Default          : ./invert_triangles.exe cube_in.tri cube_out.tri" << std::endl
			<< "Default          : ./invert_triangles.exe ../data/Figurantes" << std::endl
			<< std::endl;
		return EXIT_FAILURE;
	}


	//
	// Initial parameters
	//
	const fs::path input_path = argv[1];
	const fs::path output_path = (argc > 2) ? argv[2] : input_path;

	if (fs::is_directory(input_path))
	{
		for (auto it = fs::directory_iterator(input_path); it != fs::directory_iterator(); ++it)
		{
			if (it->path().extension() == ".tri")
			{
				std::vector<uint32_t> indices;
				vector_read(it->path().string(), indices);
				invert_triangles(indices);
				vector_write(it->path().string(), indices);
			}
		}

	}
	else
	{
		std::vector<uint32_t> indices;
		vector_read(input_path.string(), indices);
		invert_triangles(indices);
		vector_write(output_path.string(), indices);
	}

	return EXIT_SUCCESS;

}