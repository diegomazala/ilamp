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
	cmd_parser.add("normals", 'n', "Compute normals difference");
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
	bool use_normals = cmd_parser.exist("normals");

	std::vector<float> verts_template, verts_in;
	std::vector<float> norms_template, norms_in;
	std::vector<uint8_t> colors_in;
	
	std::vector<uint32_t> faces_out;
	std::vector<uint8_t> colors_out;
	std::vector<float> verts_out, norms_out, uvCoords_out;

	// 
	// Read template ply file
	//
	try
	{
		std::ifstream ss_temp(template_filename, std::ios::binary);
		tinyply::PlyFile file_template(ss_temp);

		uint32_t vertexCount_template = file_template.request_properties_from_element("vertex", { "x", "y", "z" }, verts_template);
		uint32_t normalCount_template = file_template.request_properties_from_element("vertex", { "nx", "ny", "nz" }, norms_template);

		file_template.read(ss_temp);
	}
	catch (const std::exception & e)
	{
		std::cerr << "Error: Could not load " << template_filename << ". " << e.what() << std::endl;
		return EXIT_FAILURE;
	}


	use_normals = (use_normals && !norms_template.empty());


	// 
	// Read filtered input ply file
	//
	try
	{
		std::ifstream ss_in(filtered_in_filename, std::ios::binary);
		tinyply::PlyFile file_in(ss_in);

		uint32_t vertexCount_in = file_in.request_properties_from_element("vertex", { "x", "y", "z" }, verts_in);
		uint32_t normalCount_in = file_in.request_properties_from_element("vertex", { "nx", "ny", "nz" }, norms_in);
		uint32_t colorCount_in = file_in.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }, colors_in);

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
		uint32_t normalCount_out = file_out.request_properties_from_element("vertex", { "nx", "ny", "nz" }, norms_out);
		uint32_t color_out = file_out.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }, colors_out);
		uint32_t faceCount = file_out.request_properties_from_element("face", { "vertex_indices" }, faces_out, 3);
		uint32_t faceTexcoordCount = file_out.request_properties_from_element("face", { "texcoord" }, uvCoords_out, 6);

		file_out.read(ss_out);
	}
	catch (const std::exception & e)
	{
		std::cerr << "Error: Could not load " << filtered_in_filename << ". " << e.what() << std::endl;
		return EXIT_FAILURE;
	}


	if (verts_template.size() != verts_in.size() || verts_template.size() != verts_out.size())
	{
		std::cerr << "Error: Vertex count does not match. The meshes must have the same topology" << std::endl;
		return EXIT_FAILURE;
	}


	auto it_vert_template = verts_template.cbegin();
	auto it_vert_in = verts_in.cbegin();
	auto it_vert_out = verts_out.begin();


	std::vector<float> verts_displace(verts_out.size()), norms_displace(norms_template.size());
	auto it_vert_disp = verts_displace.begin();
	

	//
	// Computing displaced vertices
	//
	if (use_normals && norms_displace.size() > 0)
	{
		auto it_norm_template = norms_template.begin();
		auto it_norm_in = norms_in.begin();
		auto it_norm_out = norms_out.begin();
		auto it_norm_disp = norms_displace.begin();

		for (;
			it_vert_template != verts_template.end();
			++it_vert_template, ++it_vert_in, ++it_vert_out, ++it_vert_disp, ++it_norm_disp)
		{
#if 0	// V2
			(*it_vert_disp) = (*it_vert_template) - (*it_vert_in);
			(*it_vert_out) = (*it_vert_out) + (*it_vert_disp);
			
			(*it_norm_disp) = (*it_norm_template) - (*it_norm_in);
			(*it_norm_out) = (*it_norm_out) + (*it_norm_disp);
#else	// V3
			(*it_vert_disp) = (*it_vert_template) - (*it_vert_in);
			(*it_vert_out) = (*it_norm_out) + (*it_vert_disp) - (*it_norm_template);
#endif
		}
	}
	else // V1 -> norms_template
	{
		for (;
			it_vert_template != verts_template.end() &&
			it_vert_in != verts_in.end() &&
			it_vert_out != verts_out.end();
			++it_vert_template, ++it_vert_in, ++it_vert_out, ++it_vert_disp)
		{
			(*it_vert_disp) = (*it_vert_template) - (*it_vert_in);
			(*it_vert_out) = (*it_vert_out) + (*it_vert_disp);
		}
	}


	//
	// Write ply file
	//
	try
	{
		std::filebuf fb;
		fb.open(result_filename, std::ios::out | std::ios::binary);
		std::ostream outputStream(&fb);

		tinyply::PlyFile ply_out_file;

		ply_out_file.add_properties_to_element("vertex", { "x", "y", "z" }, verts_out);
#if 1
		if (!norms_out.empty())
			ply_out_file.add_properties_to_element("vertex", { "nx", "ny", "nz" }, norms_out);
#else
		if (!norms_template.empty())
			ply_out_file.add_properties_to_element("vertex", { "nx", "ny", "nz" }, norms_template);
#endif
		if (!colors_out.empty())
			ply_out_file.add_properties_to_element("vertex", { "red", "green", "blue", "alpha" }, colors_out);
		if (!faces_out.empty())
			ply_out_file.add_properties_to_element("face", { "vertex_indices" }, faces_out, 3, tinyply::PlyProperty::Type::UINT8);
		if (!uvCoords_out.empty())
			ply_out_file.add_properties_to_element("face", { "texcoord" }, uvCoords_out, 6, tinyply::PlyProperty::Type::UINT8);

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