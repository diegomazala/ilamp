#include <iostream>
#include "vector_read_write_binary.h"
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

void convert_quad_to_tri(const std::vector<uint32_t>& quads, std::vector<uint32_t>& tris, bool invert_face = false)
{
	if (invert_face)
	{
		for (auto i = 0; i < quads.size(); i += 4)
		{
			tris.push_back(quads[i + 2]);
			tris.push_back(quads[i + 1]);
			tris.push_back(quads[i + 0]);

			tris.push_back(quads[i + 0]);
			tris.push_back(quads[i + 3]);
			tris.push_back(quads[i + 2]);
		}
	}
	else
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
}



int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout
			<< std::fixed << std::endl
			<< "Usage            : ./<app.exe> <input_filename> <output_filename>" << std::endl
			<< "Default          : ./quad_to_tri.exe cube.quad cube.tri " << std::endl
			<< std::endl;
		return EXIT_FAILURE;
	}

	bool invert_face = false; // (std::string(argv[argc - 1]) == "inv");

	//
	// Initial parameters
	//
	const fs::path input_path = argv[1];
	fs::path output_path = (argc > 2) ? argv[2] : input_path;

	if (fs::is_directory(input_path))
	{
		for (auto it = fs::directory_iterator(input_path); it != fs::directory_iterator(); ++it)
		{
			if (it->path().extension() == ".quad")
			{
				std::vector<uint32_t> quads;
				vector_read(it->path().string(), quads);
				std::vector<uint32_t> tris;
				convert_quad_to_tri(quads, tris, invert_face);
				auto out_filename = it->path(); 
				out_filename.replace_extension(".tri");
				vector_write(out_filename.string(), tris);
				std::cout << "File saved: " << out_filename.string() << std::endl;
			}
		}
	}
	else
	{
		output_path.replace_extension(".tri");
		std::vector<uint32_t> quads;
		vector_read(input_path.string(), quads);
		std::vector<uint32_t> tris;
		convert_quad_to_tri(quads, tris, invert_face);
		vector_write(output_path.string(), tris);

		std::cout << "File saved: " << output_path.string() << std::endl;
	}

	

	return EXIT_SUCCESS;

}