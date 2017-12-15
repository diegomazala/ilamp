#include <iostream>
#include <iomanip> 
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include "tinyply.h"

int main(int argc, char* argv[])
{
	std::cout
		<< std::fixed << std::endl
		<< "Usage            : ./<app.exe> <source> <target> <output_filename>" << std::endl
		<< "Default          : ./replace_vertices.exe source.ply target.ply output.ply " << std::endl
		<< std::endl;

	//
	// Initial parameters
	//
	const std::string source_filename = (argc > 1) ? argv[1] : "../../../../Data/Heads/Head05.ply";
	const std::string target_filename = (argc > 2) ? argv[2] : "../../../../Data/Heads/iLamp/heads_out_4.ply";
	const std::string output_filename = (argc > 3) ? argv[3] : "../../../../Data/Heads/Head05_ilamp.ply";


	try
	{
		// 
		// Read source ply file
		//
		std::ifstream ss(source_filename, std::ios::binary);
		tinyply::PlyFile file(ss);
		std::vector<float> verts;
		std::vector<float> norms;
		std::vector<uint8_t> colors;
		std::vector<uint32_t> faces;
		std::vector<float> uvCoords;

		uint32_t vertexCount, normalCount, colorCount, faceCount, faceTexcoordCount, faceColorCount;
		vertexCount = normalCount = colorCount = faceCount = faceTexcoordCount = faceColorCount = 0;

		vertexCount = file.request_properties_from_element("vertex", { "x", "y", "z" }, verts);
		normalCount = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }, norms);
		colorCount = file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }, colors);

		faceCount = file.request_properties_from_element("face", { "vertex_indices" }, faces, 3);
		faceTexcoordCount = file.request_properties_from_element("face", { "texcoord" }, uvCoords, 6);

		file.read(ss);

		//
		// Read target vertices
		//
		std::ifstream target_ss(target_filename, std::ios::binary);
		tinyply::PlyFile target_file(target_ss);
		std::vector<float> target_verts;
		uint32_t target_vertexCount = target_file.request_properties_from_element("vertex", { "x", "y", "z" }, target_verts);
		target_file.read(target_ss);

		if (target_vertexCount != vertexCount)
		{
			throw(std::exception("Vertex count does not match"));
		}


		//
		// Write ply file
		//
		std::filebuf fb;
		fb.open(output_filename, std::ios::out | std::ios::binary);
		std::ostream outputStream(&fb);

		tinyply::PlyFile ply_out_file;

		ply_out_file.add_properties_to_element("vertex", { "x", "y", "z" }, target_verts);
		if (!norms.empty())
			ply_out_file.add_properties_to_element("vertex", { "nx", "ny", "nz" }, norms);
		if (!colors.empty())
			ply_out_file.add_properties_to_element("vertex", { "red", "green", "blue", "alpha" }, colors);
		if (!faces.empty())
			ply_out_file.add_properties_to_element("face", { "vertex_indices" }, faces, 3, tinyply::PlyProperty::Type::UINT8);
		if (!uvCoords.empty())
			ply_out_file.add_properties_to_element("face", { "texcoord" }, uvCoords, 6, tinyply::PlyProperty::Type::UINT8);

		ply_out_file.write(outputStream, true);

		fb.close();
	}
	catch (const std::exception & e)
	{
		std::cerr << "Caught exception: " << e.what() << std::endl;
	}



	return EXIT_SUCCESS;
}