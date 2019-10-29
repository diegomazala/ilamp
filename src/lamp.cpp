#include <iostream>
#include <fstream>
#include "lamp.h"








template<typename decimal_t>
static bool load_from_file(const std::string& filename, char separator, Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>& matrix)
{
	std::ifstream file(filename);

	if (!file)
		return false;

	size_t n_cols = 0;
	{
		char c;
		decimal_t number;
		std::string line;
		std::getline(file, line);
		std::istringstream fin(line);
		while (fin >> number >> c)
		{
			n_cols++;
		}
		if (file.good())
			n_cols++;
	}
	file.seekg(0, std::ios::beg);

	size_t n_rows = 0;
	while (!file.eof())
	{
		std::getline(file, std::string());
		n_rows++;
	}
	file.seekg(0, std::ios::beg);
	

	matrix = Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>(n_rows, n_cols);
	//std::cout << matrix.rows() << ' ' << matrix.cols() << std::endl;

	size_t i = 0;
	while (i < n_rows && !file.eof())
	{
		std::string line;
		std::getline(file, line);
		std::istringstream fin(line);
		size_t j = 0;
		char c;
		while (fin >> matrix(i, j++) >> c) {}
		//std::cout << matrix.row(i) << std::endl << std::endl;
		i++;
	}

	//std::cout << matrix << std::endl;

	return true;
}




int main(int argc, char* argv[])
{
	using decimal_t = double;

	if (argc < 2)
	{
		std::cout
			<< std::fixed << std::endl
			<< "Usage            : ./<app.exe> <input_file> <output_file> <control_points_file> " << std::endl
			<< std::endl;
		return EXIT_FAILURE;
	}
	
	std::string nd_filename = "../../data/iris.data";
	std::string output_filename = "output.2d";
	std::string control_points_filename;

	if (argc > 1)
		nd_filename = argv[1];
	if (argc > 2)
		output_filename = argv[2];
	if (argc > 3)
		control_points_filename = argv[3];
	

	Lamp<decimal_t> lamp;

	//
	// Read data from file
	//
	if (!lamp.load_matrix_from_file(nd_filename))
	{
		std::cerr << "Error: Could not load file with nd points" << std::endl;
		return EXIT_FAILURE;
	}

	lamp.compute_control_points_by_distance();
	lamp.fit();

	
	//std::cout << "y:\n" << lamp.Y << std::endl;

	lamp.save_matrix_to_file(output_filename);

	return EXIT_SUCCESS;
}