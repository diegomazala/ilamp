#ifndef __MPI_H__
#define __MPI_H__


#include <iostream>
#include <cfloat>
#include <fstream>
#include <algorithm>
#include <vector>
#include <Eigen/Dense>
#include "imp_project.h"
#include "lamp.h"
#include "vector_read_write_binary.h"


template <typename Type>
class Imp
{
public:

	imp_project project;
	
	Type min_x = FLT_MAX;
	Type max_x = FLT_MIN;
	Type min_y = FLT_MAX;
	Type max_y = FLT_MIN;
	
	std::vector<Eigen::Matrix<Type, 2, 1>>				verts_2d;
	std::vector<Eigen::Matrix<Type, Eigen::Dynamic, 1>> verts_Nd;
	Eigen::Matrix<Type, Eigen::Dynamic, 1>				q;

public:

	Imp(){}
	~Imp() {}

	virtual void build() = 0;

	virtual Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic> execute(Type px, Type py) = 0;

	virtual bool load_project(const std::string& project_filename, bool compute_lamp)
	{
		project = {project_filename};

		//
		// Import Nd file
		// 
		verts_Nd.resize(project.inputFiles.size());
		for (auto i = 0; i < project.inputFiles.size(); ++i)
		{
			std::vector<Type> verts;
			if (vector_read<Type>(project.inputFiles[i], verts))
				verts_Nd[i] = Eigen::Map<Eigen::Matrix<float, 1, Eigen::Dynamic>>(verts.data(), 1, verts.size());
		}

		//
		// If true, build a new nd file and run lamp for it
		// Otherwise, use the existent file
		//
		if (compute_lamp)
		{
			run_lamp(project.filename2d);
			std::cerr << "<Info>  Lamp file saved: " << project.filename2d << std::endl;
		}

		//
		// Import 2d file
		// 
		if (!load_data_2d(project.filename2d))
			return false;


		if (verts_Nd.size() != verts_2d.size())
		{
			std::cerr << "<Error> Vertex arrays do not have the same size. " << verts_Nd.size() << " != " << verts_2d.size() << ". Abort" << std::endl;
			return false;
		}

		return true;
	}

	
	virtual bool load_data_2d(const std::string& filename)
	{
		min_x = FLT_MAX;
		max_x = FLT_MIN;
		min_y = FLT_MAX;
		max_y = FLT_MIN;


		std::ifstream input_file_2d(filename, std::ios::in);
		if (input_file_2d.is_open())
		{
			char c;
			Type x, y;

			while (!input_file_2d.eof())
			{
				//input_file_2d >> x >> c >> y;
				input_file_2d >> x >> y;
				if (input_file_2d.good())
				{
					verts_2d.push_back(Eigen::Matrix<Type, 2, 1>(x, y));

					min_x = std::min(min_x, x);
					min_y = std::min(min_y, y);
					max_x = std::max(max_x, x);
					max_y = std::max(max_y, y);
	#if _DEBUG
					std::cout << "<Info>  Vertex 2d Loaded  : " << std::fixed << x << ' ' << y << std::endl;
	#endif
				}
			
			}

			std::cout << "<Info>  Arrays 2d Loaded  : " << verts_2d.size() << ", " << verts_2d[0].rows() << std::endl;
			input_file_2d.close();
			return true;
		}
		else
		{
			std::cerr << "<Error> Could not load Nd file: " << filename << "\nAbort." << std::endl;
			return false;
		}
	}


	//virtual void load_data_Nd(const std::vector<std::string>& input_files)
	//{
	//	verts_Nd.resize(input_files.size());
	//	for (auto i = 0; i < input_files.size(); ++i)
	//	{
	//		vector_read(input_files[i], verts_Nd[i]);
	//	}
	//}


	virtual bool load_data_Nd(const std::string& filename)
	{
		std::ifstream input_file_Nd(filename, std::ios::in);
		if (input_file_Nd.is_open())
		{

			// reading dimension nd based on first line in input file
			size_t dimension_Nd = 0;
			{
				std::string line;
				std::getline(input_file_Nd, line);
				std::istringstream tokenStream(line);
				std::string str;
				for (std::string each; std::getline(tokenStream, str, ' '); dimension_Nd++) {}
				input_file_Nd.clear();                 // clear fail and eof bits
				input_file_Nd.seekg(0, std::ios::beg); // back to the start!
			}


			char c;
			while (!input_file_Nd.eof())
			{
				Eigen::Matrix<Type, Eigen::Dynamic, 1> v(dimension_Nd, 1);
				for (int i = 0; i < dimension_Nd; ++i)
					input_file_Nd >> v[i];

				if (input_file_Nd.good())
					verts_Nd.push_back(v);

				//std::cout << v.transpose() << std::endl;
			}

			std::cout << "<Info>  Arrays Nd Loaded  : " << verts_Nd.size() << ", " << verts_Nd[0].rows() << std::endl;
			return true;
		}
		else
		{
			std::cerr << "<Error> Could not load Nd file: " << filename << "\nAbort." << std::endl;
			return false;
		}
	}


	void run_lamp(const std::string& out_filename2d)
	{
		Lamp<float> lamp;

		//
		// Build X matrix
		//
		lamp.X = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>(verts_Nd.size(), verts_Nd[0].size());
		//X << Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>>(ilamp.verts_Nd[0].data(), ilamp.verts_Nd.size(), ilamp.verts_Nd[0].size());
		for (auto i = 0; i < verts_Nd.size(); ++i)
			lamp.X.row(i) << Eigen::Map<Eigen::Matrix<float, 1, Eigen::Dynamic>>(verts_Nd[i].data(), 1, verts_Nd[0].size());

		//
		// Run lamp
		//
		lamp.compute_control_points_by_distance();
		lamp.fit();
		lamp.save_matrix_to_file(out_filename2d);
		//std::cerr << "<Info>  Lamp file saved: " << ilp_prj.filename2d << std::endl;
	}


};

#endif // __MPI_H__