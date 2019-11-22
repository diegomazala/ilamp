#ifndef __ILAMP_UTILS_H__
#define __ILAMP_UTILS_H__

#include <iostream>
#include <iomanip> 
#include <fstream>
#include <algorithm>
#include <vector>

#include "vector_read_write_binary.h"


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


static void build_nd_file(const std::vector<std::string>& models_file_array, const std::string& output_filename)
{
	std::ofstream output_file(output_filename, std::ios::out);

	for (const std::string& filename : models_file_array)
	{
#if _DEBUG
		std::cout << filename << std::endl;
#endif

		std::vector<float> verts;

#if 0
		const auto vertex_count = read_ply_file(filename, verts);
#else
		const auto vertex_count = vector_read(filename, verts);
#endif
		if (vertex_count > 0)
		{
			for (const auto v : verts)
				output_file << std::fixed << std::setprecision(2) << v << ' ';
			output_file << std::endl;
		}

	}

	output_file.close();
}



static bool write_ply_file(const std::string & filename, std::vector<float>& verts)
{
	try
	{
		std::filebuf ply_fb;
		ply_fb.open(filename, std::ios::out | std::ios::binary);
		std::ostream ply_output_stream(&ply_fb);
		tinyply::PlyFile ply_file;
		ply_file.add_properties_to_element("vertex", { "x", "y", "z" }, verts);
		ply_file.write(ply_output_stream, true);
		ply_fb.close();
		return true;
	}
	catch (const std::exception & e)
	{
		std::cerr << "Caught exception: " << e.what() << std::endl;
		return false;
	}
}




static bool write_ply_file(const std::string& output_filename, std::vector<float>& target_verts, const std::string& template_filename)
{
	try
	{
		// 
		// Read source ply file
		//
		std::ifstream ss(template_filename, std::ios::binary);
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


		if (target_verts.size() != verts.size())
		{
			throw std::runtime_error("Vertex count does not match");
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


		std::cout << "<Info>  Ply file saved    : " << output_filename << std::endl;

		return true;
	}
	catch (const std::exception & e)
	{
		std::cerr << "Caught exception: " << e.what() << std::endl;
		return false;
	}

}


static bool write_ply_file(const std::string& output_filename, std::vector<float>& verts, std::vector<uint32_t>& triangles)
{
	try
	{
		//
		// Write ply file
		//
		std::filebuf fb;
		fb.open(output_filename, std::ios::out | std::ios::binary);
		std::ostream outputStream(&fb);

		tinyply::PlyFile ply_out_file;

		ply_out_file.add_properties_to_element("vertex", { "x", "y", "z" }, verts);
		if (!triangles.empty())
			ply_out_file.add_properties_to_element("face", { "vertex_indices" }, triangles, 3, tinyply::PlyProperty::Type::UINT8);

		ply_out_file.write(outputStream, true);

		fb.close();

		std::cout << "<Info>  Ply file saved    : " << output_filename << std::endl;

		return true;
	}
	catch (const std::exception & e)
	{
		std::cerr << "Caught exception: " << e.what() << std::endl;
		return false;
	}

}




static bool write_ply_file(const std::string& output_filename, size_t verts_count, float* p_verts_array, float* p_normals_array, float* p_uv_array, int* p_faces_array)
{
	if (!p_verts_array)
		return false;

	try
	{
		//
		// Write ply file
		//
		std::filebuf fb;
		fb.open(output_filename, std::ios::out | std::ios::binary);
		std::ostream outputStream(&fb);

		tinyply::PlyFile ply_out_file;

		ply_out_file.add_properties_to_element("vertex", { "x", "y", "z" }, std::vector<float>(p_verts_array, p_verts_array + verts_count * 3));

		if (p_normals_array)
			ply_out_file.add_properties_to_element("vertex", { "nx", "ny", "nz" }, std::vector<float>(p_normals_array, p_normals_array + verts_count * 3));
		if (p_faces_array)
			ply_out_file.add_properties_to_element("face", { "vertex_indices" }, std::vector<int>(p_faces_array, p_faces_array + verts_count * 3), 3, tinyply::PlyProperty::Type::UINT8);
		if (p_uv_array)
			ply_out_file.add_properties_to_element("face", { "texcoord" }, std::vector<float>(p_normals_array, p_normals_array + verts_count * 3), 6, tinyply::PlyProperty::Type::UINT8);

		ply_out_file.write(outputStream, true);

		fb.close();


		std::cout << "<Info>  Ply file saved    : " << output_filename << std::endl;

		return true;
	}
	catch (const std::exception & e)
	{
		std::cerr << "Caught exception: " << e.what() << std::endl;
		return false;
	}

}



bool replace_vertices(const std::string& source_filename, const std::string& target_filename, const std::string& output_filename)
{
	//
	// Initial parameters
	//
	//const std::string source_filename = (argc > 1) ? argv[1] : "../../../../Data/Heads/Head05.ply";
	//const std::string target_filename = (argc > 2) ? argv[2] : "../../../../Data/Heads/iLamp/heads_out_4.ply";
	//const std::string output_filename = (argc > 3) ? argv[3] : "../../../../Data/Heads/Head05_ilamp.ply";


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

		return true;
	}
	catch (const std::exception & e)
	{
		std::cerr << "Caught exception: " << e.what() << std::endl;
		return false;
	}

}

#endif // __ILAMP_UTILS_H__