#include <iostream>
#include <iomanip> 
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include "tinyply.h"
#include "tinyobj.h"
#include "vector_read_write_binary.h"
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;


void join_files(const std::string& vert_file, const std::string& face_file)
{
	std::vector<float> verts;
	vector_read(vert_file, verts);

	std::vector<uint32_t> faces;
	vector_read(face_file, faces);
	int poly = (faces.size() % 4 == 0) ? 4 : 3;

	std::filebuf fb;
	fb.open(fs::path(vert_file).replace_extension(".ply").string(), std::ios::out | std::ios::binary);
	std::ostream outputStream(&fb);

	tinyply::PlyFile ply_out_file;

	ply_out_file.add_properties_to_element("vertex", { "x", "y", "z" }, verts);
	if (!faces.empty())
		ply_out_file.add_properties_to_element("face", { "vertex_indices" }, faces, poly, tinyply::PlyProperty::Type::UINT8);
	
	ply_out_file.write(outputStream, true);
	fb.close();
}



int main(int argc, char* argv[])
{
	//join_files(argv[1], argv[2]);
	//return 0;

	if (argc < 2)
	{
		std::cout
			<< std::fixed << std::endl
			<< "Usage            : ./<app.exe> <input_filename> <output_dir>" << std::endl
			<< "Default          : ./split_3d_file.exe cube.ply C:/tmp/" << std::endl
			<< std::endl;
		return EXIT_FAILURE;
	}

	//
	// Initial parameters
	//
	const fs::path input_filename = argv[1];
	const fs::path output_dir = (argc > 2) ? argv[2] : input_filename.parent_path();
	const std::string file_extension = fs::path(input_filename).extension().string();

	fs::create_directory(output_dir);
	
	if (input_filename.extension() == ".ply")
	{
		std::vector<float> verts;
		std::vector<uint32_t> faces;
		
		std::ifstream ss(input_filename.string(), std::ios::binary);
		tinyply::PlyFile file(ss);
		uint32_t vertex_count = file.request_properties_from_element("vertex", { "x", "y", "z" }, verts);
		uint32_t face_count = file.request_properties_from_element("face", { "vertex_indices" }, faces);
		file.read(ss);

		if (vertex_count > 0 && face_count > 0)
		{
			auto out_filename = output_dir / input_filename.stem();
			vector_write(out_filename.replace_extension(".vert").string(), verts);
			vector_write(out_filename.replace_extension(".face").string(), faces);
		}
		else
		{
			std::cerr << "[Error] Could not load ply file : " << input_filename.string() << std::endl;
			return EXIT_FAILURE;
		}
	}
	else if (input_filename.extension() == ".obj")
	{
		tinyobj::scene_t obj_load;
		if (tinyobj::load(obj_load, input_filename.string()))
		{
			tinyobj::scene_t obj_geo;
			tinyobj::garbage_collect(obj_geo, obj_load);

			auto out_filename = output_dir / input_filename.stem();
			vector_write(out_filename.replace_extension(".vert").string(), obj_geo.attrib.vertices);
			std::vector<uint32_t> faces;
			if (!obj_geo.shapes.empty())
			{
				for (const auto& shape : obj_geo.shapes)
				{
					for (const auto& index : shape.mesh.indices)
					{
						faces.push_back(index.vertex_index);
					}
				}
				vector_write(out_filename.replace_extension(".face").string(), faces);
			}
		}
		else
		{
			std::cerr << "[Error] Could not load obj file : " << input_filename.string() << std::endl;
			return EXIT_FAILURE;
		}
	}
	else
	{
		std::cerr << "Error: File format not supported" << std::endl;
		return EXIT_FAILURE;
	}


	return EXIT_SUCCESS;
}