#include <iostream>
#include <iomanip> 
#include <fstream>
#include <algorithm>
#include <vector>
#include <flann/flann.hpp>
#include <Eigen/Dense>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

template <typename Type>
static Type DegToRad(Type angle_in_degrees)
{
	return angle_in_degrees * (Type)(M_PI / 180.0);
}

template <typename Type>
static Type RadToDeg(Type angle_in_radians)
{
	return angle_in_radians * (Type)(180.0 / M_PI);
}


//#define ASSIMP_DOUBLE_PRECISION	// if defined, use double instead float
typedef float Decimal;

// ../../data/iris.2d ../../data/iris.data 16 4 128
// ../../data/primitives.2d ../../data/primitives.nd 2 4 4

int main(int argc, char* argv[])
{
	std::cout
		<< std::fixed << std::endl
		<< "Usage            : ./<app.exe> <data_file_2d> <data_file_Nd> <number_of_neighbours> <kd_tree_count> <knn_search_checks>" << std::endl
		<< "Default          : ./ilamp.exe iris.2d iris.data" << std::endl
		<< std::endl;
	
	std::string input_filename_2d = "../../data/iris_27d.2d";
	std::string input_filename_Nd = "../../data/iris.27d";
	std::string output_filename_Nd = "../../data/iris_out.27d";
	std::string output_filename_dist_Nd = "../../data/iris_out_dist.27d";

	//if (argc > 1)
	//	input_filename_2d = argv[1];
	//if (argc > 2)
	//	input_filename_Nd = argv[2];

	const int dimension_2d = 2;
	int dimension_Nd = 0;
	const int numNeighbours = (argc > 3) ? atoi(argv[3]) : 16;
	const int kdTreeCount = (argc > 4) ? atoi(argv[4]) : 4;
	const int knnSearchChecks = (argc > 5) ? atoi(argv[5]) : 128;
	
	
	//
	// Import 2d file
	// 
	std::vector<Eigen::Matrix<Decimal, 2, 1>> verts_2d;
	std::ifstream input_file_2d(input_filename_2d, std::ios::in);
	if (input_file_2d.is_open())
	{
		char c;
		float x, y;

		while (!input_file_2d.eof())
		{
			input_file_2d >> x >> c >> y;
			if (input_file_2d.good())
				verts_2d.push_back(Eigen::Matrix<Decimal, 2, 1>(x, y));
		}

		std::cout << "Vertices Loaded 2d: " << verts_2d.size() << std::endl;
	}


	//
	// Import nd file
	// 
	std::vector<Eigen::Matrix<Decimal, Eigen::Dynamic, 1>> verts_Nd;
	std::ifstream input_file_Nd(input_filename_Nd, std::ios::in);
	if (input_file_Nd.is_open())
	{
		// reading dimension nd based on first line in input file
		{
			dimension_Nd = 0;
			std::string line;
			std::getline(input_file_Nd, line);
			std::istringstream tokenStream(line);
			for (std::string each; std::getline(tokenStream, std::string(), ','); dimension_Nd++){}
			input_file_Nd.clear();                 // clear fail and eof bits
			input_file_Nd.seekg(0, std::ios::beg); // back to the start!
		}
		

		char c;
		while (!input_file_Nd.eof())
		{
			Eigen::Matrix<Decimal, Eigen::Dynamic, 1> v(dimension_Nd, 1);
			for (int i=0; i<dimension_Nd - 1; ++i)
				input_file_Nd >> v[i] >> c;
			input_file_Nd >> v[dimension_Nd - 1];
			if (input_file_Nd.good())
				verts_Nd.push_back(v);

			//std::cout << v.transpose() << std::endl;
		}

		std::cout << "Vertices Loaded Nd: " << verts_Nd.size() << std::endl;
	}

	if (verts_Nd.size() != verts_2d.size())
	{
		std::cerr << "Error: Vertex arrays do not have the same size. Abort" << std::endl;
		return EXIT_FAILURE;
	}
	



	const size_t vertex_array_count_2d = verts_2d.size() * 2; // 2d dimension
	Decimal* vertex_array_2d = new Decimal[vertex_array_count_2d];
	memcpy(vertex_array_2d, verts_2d.data(), sizeof(Decimal) * vertex_array_count_2d);

	const size_t vertex_array_count_Nd = verts_2d.size() * dimension_Nd; // Nd dimension
	Decimal* vertex_array_Nd = new Decimal[vertex_array_count_Nd];
	memcpy(vertex_array_Nd, verts_Nd.data(), sizeof(Decimal) * vertex_array_count_Nd);


	// for each vertex find nearest neighbours
	const size_t numInput = vertex_array_count_2d / dimension_2d;
	const size_t numQuery = numInput;

	flann::Matrix<Decimal> dataset_2d(vertex_array_2d, numInput, dimension_2d);
	flann::Matrix<Decimal> query(vertex_array_2d, numQuery, dimension_2d);

	flann::Matrix<int> indices(new int[query.rows * numNeighbours], query.rows, numNeighbours);
	flann::Matrix<Decimal> dists(new Decimal[query.rows * numNeighbours], query.rows, numNeighbours);

	// construct an randomized kd-tree index using 4 kd-trees
	flann::Index<flann::L2<Decimal>> index(dataset_2d, flann::KDTreeIndexParams(kdTreeCount));
	index.buildIndex();

	// do a knn search, using 128 checks
	index.knnSearch(query, indices, dists, numNeighbours, flann::SearchParams(knnSearchChecks));	//flann::SearchParams(128));


																									//
																									// Output info
																									// 
	std::cout << std::fixed
		<< "Dimension         : " << dimension_2d << std::endl
		<< "Dimension         : " << dimension_Nd << std::endl
		<< "NumNeighbours     : " << numNeighbours << std::endl
		<< "KdTreeCount       : " << kdTreeCount << std::endl
		<< "KnnSearchChecks   : " << knnSearchChecks << std::endl
		<< "NumInput/NumQuery : " << numQuery << std::endl;



	// k = numNeighbours = indices.cols

	//for (int i = 0; i < indices.rows; ++i)
	//{
	//	const Decimal qx = query[i][0];
	//	const Decimal qy = query[i][1];
	//	Eigen::Matrix<Decimal, 2, 1> p(qx, qy);
	//
	//	for (int j = 0; j < indices.cols; ++j)	// k
	//	{
	//		// resultant points = neighbours
	//		const int index = indices[i][j];
	//		const Decimal x = static_cast<Decimal>(dataset_2d[index][0]);
	//		const Decimal y = static_cast<Decimal>(dataset_2d[index][1]);
	//	}
	//}

	std::ofstream output_file(output_filename_Nd, std::ios::out);
	std::ofstream output_file_dist(output_filename_dist_Nd, std::ios::out);


	const int k = numNeighbours; // k = numNeighbours = indices.cols

	for (int main_index = 0; main_index < indices.rows; ++main_index)
	//for (int main_index = 0; main_index < 1; ++main_index)
	{
		Eigen::Matrix<Decimal, 2, 1> p(query[main_index][0], query[main_index][1]);	// p = first point

		
		std::vector<Eigen::Matrix<Decimal, Eigen::Dynamic, 1>> Xs;		// Xs = correspondent high dimensional instances of Ys
		std::vector<Eigen::Matrix<Decimal, 2, 1>> Ys;					// Ys = subset with all k closest points to p

		// eq 3
		Decimal										alpha_sum	= 0;
		Eigen::Matrix<Decimal, Eigen::Dynamic, 1>	alpha_i_x_i_sum = Eigen::Matrix<Decimal, Eigen::Dynamic, 1>().Zero(dimension_Nd, 1);
		Eigen::Matrix<Decimal, 2, 1>				alpha_i_y_i_sum = Eigen::Matrix<Decimal, 2, 1>().Zero();



		for (int j = 0; j < k; ++j)	// the first element is equal to p
		{
			// resultant points = neighbours
			const int index = indices[main_index][j];

			//Eigen::Matrix<Decimal, 2, 1> y_i(
			//	static_cast<Decimal>(dataset_2d[index][0]),
			//	static_cast<Decimal>(dataset_2d[index][1]));

	
			const auto& x_i = verts_Nd[index];
			const auto& y_i = verts_2d[index];

			
			Xs.push_back(verts_Nd[index]);
			Ys.push_back(verts_2d[index]);


			// eq 2
			// ###################### verificar no paper. A divisão pode ser por zero
			Decimal squaredNorm = std::max(static_cast<Decimal>((y_i - p).squaredNorm()), static_cast<Decimal>(0.0001));
			Decimal alpha_i = static_cast<Decimal>(1) / squaredNorm;
			
			// eq 3
			alpha_sum += alpha_i;
			alpha_i_x_i_sum += (alpha_i * x_i);
			alpha_i_y_i_sum += (alpha_i * y_i);

		}

		// eq 3
		Eigen::Matrix<Decimal, Eigen::Dynamic, 1>	x_tilde = alpha_i_x_i_sum / alpha_sum;
		Eigen::Matrix<Decimal, 2, 1>				y_tilde = alpha_i_y_i_sum / alpha_sum;
						
		//std::cout << "x~    : " << x_tilde.transpose() << std::endl;
		//std::cout << "y~    : " << y_tilde.transpose() << std::endl;
		//std::cout << "alpha : " << alpha_sum << std::endl;
		
		Eigen::Matrix<Decimal, Eigen::Dynamic, Eigen::Dynamic>	A(k, dimension_2d);
		Eigen::Matrix<Decimal, Eigen::Dynamic, Eigen::Dynamic>	B(k, dimension_Nd);

		for (int j = 0; j < k; ++j)	// the first element is equal to p
		{
			// resultant points = neighbours
			const int index = indices[main_index][j];

			const auto& x_i = verts_Nd[index];
			const auto& y_i = verts_2d[index];

			Decimal squaredNorm = std::max(static_cast<Decimal>((y_i - p).squaredNorm()), static_cast<Decimal>(0.0001));
			Decimal alpha_i = static_cast<Decimal>(1) / squaredNorm;

			// eq 4
			const auto x_hat = x_i - x_tilde;
			const auto y_hat = y_i - y_tilde;

			// eq 6
			A.row(j) = std::sqrt(alpha_i) * y_hat.transpose();
			B.row(j) = std::sqrt(alpha_i) * x_hat.transpose();
		}

		//
		// Compute SVD
		//
		const auto ATB = A.transpose() * B;
		Eigen::JacobiSVD<Eigen::Matrix<Decimal, Eigen::Dynamic, Eigen::Dynamic>> svd;
		//svd.compute(ATB, Eigen::ComputeFullU | Eigen::ComputeFullV);
		svd.compute(ATB, Eigen::ComputeThinU | Eigen::ComputeThinV);

		if (!svd.computeU() || !svd.computeV())
		{
			std::cerr << "<Error> Decomposition error" << std::endl;
			return false;
		}

		//std::cout << "ATB : " << ATB.rows() << ' ' << ATB.cols() << std::endl;
		//std::cout << "U   : " << svd.matrixU().rows() << ' ' << svd.matrixU().cols() << std::endl;
		//std::cout << "V   : " << svd.matrixV().rows() << ' ' << svd.matrixV().cols() << std::endl;

		const auto M = svd.matrixU() * svd.matrixV().transpose();
		//std::cout << "M: " << M.rows() << ' ' << M.cols() << std::endl;

		 
		const auto q = (p - y_tilde).transpose() * M + x_tilde.transpose();
		//std::cout << "q    : " << q.rows() << ' ' << q.cols() << std::endl << q << std::endl;
		output_file << std::fixed << std::setprecision(2) << q << std::endl;


		const auto& q_orig = verts_Nd[main_index];
		const auto dist = (q.transpose() - q_orig).norm();
		output_file_dist << dist << std::endl;
	}

	output_file.close();
	output_file_dist.close();

	delete[] vertex_array_2d;
	delete[] vertex_array_Nd;

	delete[] indices.ptr();
	delete[] dists.ptr();

	return EXIT_SUCCESS;
}