#include <iostream>
#include <iomanip> 
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include "cmdline.h"
#include "tinyply.h"
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;




cmdline::parser cmd_parser;
void cmd_parser_build()
{
	cmd_parser.add<std::string>("template", 't', "Original mesh file (ply)", true);
	cmd_parser.add<std::string>("filtered_in", 'i', "Input mesh filtered (ply)", true);
	cmd_parser.add<std::string>("filtered_out", 'o', "Output mesh filtered (ply)", true);
	cmd_parser.add<std::string>("result", 'r', "Result mesh combined", true, "result.ply");
}


int main(int argc, char* argv[])
{
	//
	// Parse command line
	//
	cmd_parser_build();
	cmd_parser.parse_check(argc, argv);


	//
	// Set variables
	//
	const std::string& template_filename = cmd_parser.get<std::string>("template");
	const std::string& filtered_in_filename = cmd_parser.get<std::string>("filtered_in");
	const std::string& filtered_out_filename = cmd_parser.get<std::string>("filtered_out");
	const std::string& result_filename = cmd_parser.get<std::string>("result");


	std::vector<float> verts_template, verts_out, verts_in;
	std::vector<float> norms, uvCoords;
	std::vector<uint32_t> faces;
	std::vector<uint8_t> colors;

	// 
	// Read template ply file
	//
	try
	{
		std::ifstream ss_temp(template_filename, std::ios::binary);
		tinyply::PlyFile file_template(ss_temp);

		uint32_t vertexCount_template = file_template.request_properties_from_element("vertex", { "x", "y", "z" }, verts_template);
		uint32_t normalCount_template = file_template.request_properties_from_element("vertex", { "nx", "ny", "nz" }, norms);
		uint32_t colorCount = file_template.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }, colors);
		uint32_t faceCount = file_template.request_properties_from_element("face", { "vertex_indices" }, faces, 3);
		uint32_t faceTexcoordCount = file_template.request_properties_from_element("face", { "texcoord" }, uvCoords, 6);

		std::cout << "Reading [" << vertexCount_template << "] " << template_filename << std::endl;

		file_template.read(ss_temp);
	}
	catch (const std::exception & e)
	{
		std::cerr << "Error: Could not load " << template_filename << ". " << e.what() << std::endl;
		return EXIT_FAILURE;
	}


	// 
	// Read filtered input ply file
	//
	try
	{
		std::ifstream ss_in(filtered_in_filename, std::ios::binary);
		tinyply::PlyFile file_in(ss_in);

		uint32_t vertexCount_in = file_in.request_properties_from_element("vertex", { "x", "y", "z" }, verts_in);

		std::cout << "Reading [" << vertexCount_in << "] "<< filtered_in_filename << std::endl;

		file_in.read(ss_in);
	}
	catch (const std::exception & e)
	{
		std::cerr << "Error: Could not load " << filtered_in_filename << ". " << e.what() << std::endl;
		return EXIT_FAILURE;
	}


	// 
	// Read filtered output ply file
	//
	try
	{
		std::ifstream ss_out(filtered_out_filename, std::ios::binary);
		tinyply::PlyFile file_out(ss_out);

		uint32_t vertexCount_out = file_out.request_properties_from_element("vertex", { "x", "y", "z" }, verts_out);

		std::cout << "Reading [" << vertexCount_out << "] " << filtered_out_filename << std::endl;

		file_out.read(ss_out);
	}
	catch (const std::exception & e)
	{
		std::cerr << "Error: Could not load " << filtered_out_filename << ". " << e.what() << std::endl;
		return EXIT_FAILURE;
	}


	if (verts_template.size() != verts_in.size() || verts_template.size() != verts_out.size())
	{
		std::cerr << "Error: Vertex count does not match. The meshes must have the same topology" << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<float> verts_result(verts_out.size());

	//
	// Computing displaced vertices
	//
	for (auto i = 0; i < verts_result.size(); ++i)
	{
		verts_result[i] = verts_out[i] + verts_template[i] - verts_in[i]; // out + displacement
	}


	//
	// Write ply file
	//
	try
	{
		std::cout
			<< "Saving  [" << std::size_t(verts_result.size() / 3.0f)
			<< "] vertices [" << std::size_t(verts_result.size() / 3.0f) << "] normals "
			<< result_filename << std::endl;

		std::filebuf fb;
		fb.open(result_filename, std::ios::out | std::ios::binary);
		std::ostream outputStream(&fb);

		tinyply::PlyFile ply_out_file;

		ply_out_file.add_properties_to_element("vertex", { "x", "y", "z" }, verts_result);
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
		std::cerr << "Error: Could not save " << result_filename << ". " << e.what() << std::endl;
		return EXIT_FAILURE;
	}



	return EXIT_SUCCESS;
}