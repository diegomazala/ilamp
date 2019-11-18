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


int split_file(const fs::path& input_filename, const fs::path output_dir)
{
	const std::string file_extension = fs::path(input_filename).extension().string();

	if (input_filename.extension() == ".ply")
	{
		std::vector<float> verts, norms, uv;
		std::vector<uint32_t> faces;

		std::ifstream ss(input_filename.string(), std::ios::binary);
		tinyply::PlyFile file(ss);

		uint32_t vertex_count = file.request_properties_from_element("vertex", { "x", "y", "z" }, verts);
		uint32_t normal_count = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }, norms);
		uint32_t uv_count = file.request_properties_from_element("vertex", { "u", "v" }, uv);
		uint32_t face_count = file.request_properties_from_element("face", { "vertex_indices" }, faces);
		file.read(ss);

		if (vertex_count > 0 && face_count > 0)
		{
			auto out_filename = output_dir / input_filename.stem();
			vector_write(out_filename.replace_extension(".vert").string(), verts);
			std::string extension = ((faces.size() % 3 == 0) ? (".tri") : (".quad"));
			vector_write(out_filename.replace_extension(extension).string(), faces);
			if (norms.size() > 0)
				vector_write(out_filename.replace_extension(".nor").string(), norms);
			if (uv.size() > 0)
				vector_write(out_filename.replace_extension(".uv").string(), uv);
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
#if 1
			tinyobj::scene_t& obj_geo = obj_load;
#else
			tinyobj::scene_t obj_geo = obj_load;
			tinyobj::garbage_collect(obj_geo, obj_load);
#endif

			auto out_filename = output_dir / input_filename.stem();
			vector_write(out_filename.replace_extension(".vert").string(), obj_geo.attrib.vertices);
			if (obj_geo.attrib.normals.size() > 0)
				vector_write(out_filename.replace_extension(".nor").string(), obj_geo.attrib.normals);
			if (obj_geo.attrib.texcoords.size() > 0)
				vector_write(out_filename.replace_extension(".uv").string(), obj_geo.attrib.texcoords);

			if (!obj_geo.shapes.empty())
			{
				for (const auto& shape : obj_geo.shapes)
				{
					auto vert_per_face = (uint8_t)shape.mesh.num_face_vertices[0];
					std::string extension = shape.name + ((vert_per_face == 3) ? (".tri") : (".quad"));

					std::vector<uint32_t> indices(shape.mesh.indices.size());
					for (auto i = 0; i < indices.size(); ++i)
					{
						indices[i] = shape.mesh.indices[i].vertex_index;
					}
					vector_write(out_filename.replace_extension(extension).string(), indices);
				}
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
	const fs::path input_path = argv[1];
	const fs::path output_dir = ((argc > 2) ? argv[2] : (fs::is_directory(input_path) ? input_path : input_path.parent_path()));

	fs::create_directory(output_dir);

	if (fs::is_directory(input_path))
	{
		for (auto it = fs::directory_iterator(input_path); it != fs::directory_iterator(); ++it)
		{
			if (it->path().extension() == ".obj" || it->path().extension() == ".ply")
				split_file(*it, output_dir);
		}
	}
	else
	{
		return split_file(input_path, output_dir);
	}
}