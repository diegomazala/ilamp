#include <iostream>
#include "vector_read_write_binary.h"
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

void invert_normals(std::vector<float>& normals)
{
	for (auto& v : normals)
	{
		v *= -1.f;
	}
}



int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout
			<< std::fixed << std::endl
			<< "Usage            : ./<app.exe> <input_filename> <output_filename>" << std::endl
			<< "Default          : ./invert_normals.exe cube_in.nor cube_out.nor" << std::endl
			<< "Default          : ./invert_normals.exe ../data/Figurantes" << std::endl
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
				std::vector<float> normals;
				vector_read(it->path().string(), normals);
				invert_normals(normals);
				vector_write(it->path().string(), normals);
			}
		}

	}
	else
	{
		std::vector<float> normals;
		vector_read(input_path.string(), normals);
		invert_normals(normals);
		vector_write(output_path.string(), normals);
	}

	return EXIT_SUCCESS;

}