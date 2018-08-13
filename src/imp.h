#ifndef __MPI_H__
#define __MPI_H__


#include <iostream>
#include <limits>
#include <fstream>
#include <algorithm>
#include <vector>
#include <Eigen/Dense>




template <typename Type>
class Imp
{
public:
	
	Type min_x = std::numeric_limits<float>::max();
	Type max_x = std::numeric_limits<float>::max();
	Type min_y = std::numeric_limits<float>::min();
	Type max_y = std::numeric_limits<float>::min();
	std::vector<Eigen::Matrix<Type, 2, 1>>				verts_2d;
	std::vector<Eigen::Matrix<Type, Eigen::Dynamic, 1>> verts_Nd;
	Eigen::Matrix<Type, Eigen::Dynamic, 1>				q;

public:

	Imp(){}
	~Imp() {}

	virtual void build() = 0;

	virtual Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic> execute(Type px, Type py) = 0;

	
	virtual bool load_data_2d(const std::string& filename)
	{
		min_x = std::numeric_limits<float>::min();
		max_x = std::numeric_limits<float>::max();
		min_y = std::numeric_limits<float>::min();
		max_y = std::numeric_limits<float>::max();

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



};

#endif // __MPI_H__