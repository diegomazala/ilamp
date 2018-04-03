#ifndef __ILAMP_H__
#define __ILAMP_H__


#include "imp.h"
#include <memory>
#include <iostream>
#include <iomanip> 
#include <fstream>
#include <algorithm>
#include <vector>
#include <flann/flann.hpp>
#include <Eigen/Dense>



template <typename Type>
class ILamp: public Imp<Type>
{
public:
	std::unique_ptr<flann::Matrix<Type>> dataset_2d = nullptr;
	std::unique_ptr<flann::Index<flann::L2<Type>>> kdtree = nullptr;

	uint16_t kdtree_count = 4;
	uint16_t num_neighbours = 4;
	uint16_t knn_search_checks = 128;



public:
	ILamp(){}
	~ILamp() {}

	void set_kdtree(uint16_t _kdtree_count, uint16_t _num_neighbours, uint16_t _knn_search_checks)
	{
		kdtree_count = _kdtree_count;
		num_neighbours = _num_neighbours;
		knn_search_checks = _knn_search_checks;
	}


	virtual void build()
	{
		//
		// Build dataset
		//
		const uint8_t dimension_2d = 2;
		dataset_2d.reset(new flann::Matrix<Type>(verts_2d.data()->data(), verts_2d.size(), dimension_2d));
		//
		// Construct an randomized kd-tree index using 4 kd-trees
		//
		kdtree.reset(new flann::Index<flann::L2<Type>>(*dataset_2d.get(), flann::KDTreeIndexParams(kdtree_count)));
		kdtree->buildIndex();
	}



	Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic>	execute(Type px, Type py)
	{
		try
		{

			const size_t dimension_2d = verts_2d.at(0).rows();
			const size_t dimension_Nd = verts_Nd.at(0).rows();

			Eigen::Matrix<Type, 2, 1> p(px, py);

			//std::vector<Eigen::Matrix<Type, Eigen::Dynamic, 1>> Xs;	// Xs = correspondent high dimensional instances of Ys
			//std::vector<Eigen::Matrix<Type, 2, 1>> Ys;					// Ys = subset with all k closest points to p


			// eq 3
			Type										alpha_sum = 0;
			Eigen::Matrix<Type, Eigen::Dynamic, 1>	alpha_i_x_i_sum = Eigen::Matrix<Type, Eigen::Dynamic, 1>().Zero(dimension_Nd, 1);
			Eigen::Matrix<Type, 2, 1>				alpha_i_y_i_sum = Eigen::Matrix<Type, 2, 1>().Zero();


			//
			// Search for neighbours
			//
			const uint8_t query_count = 1;	// only one point (x,y)
			flann::Matrix<Type> query(p.data(), query_count, dimension_2d);
			flann::Matrix<int> kdtree_indices(new int[query_count * num_neighbours], query_count, num_neighbours);
			flann::Matrix<Type> kdtree_dists(new Type[query_count * num_neighbours], query_count, num_neighbours);
			kdtree->knnSearch(query, kdtree_indices, kdtree_dists, num_neighbours, flann::SearchParams(knn_search_checks));


			for (int j = 0; j < num_neighbours; ++j)
			{
				const int index = kdtree_indices[0][j];

				if (index < 0 || index >(verts_Nd.size() - 1) || index > (verts_2d.size() - 1))
					continue;

				const auto& x_i = verts_Nd[index];
				const auto& y_i = verts_2d[index];

				//Xs.push_back(verts_Nd[index]);
				//Ys.push_back(ilamp.verts_2d.[index]);

				// eq 2
				// ###################### 
				// Verificar no paper. A divisão pode ser por zero
				// ###################### 
				Type squaredNorm = std::max(static_cast<Type>((y_i - p).squaredNorm()), static_cast<Type>(0.0001));
				Type alpha_i = static_cast<Type>(1) / squaredNorm;

				// eq 3
				alpha_sum += alpha_i;
				alpha_i_x_i_sum += (alpha_i * x_i);
				alpha_i_y_i_sum += (alpha_i * y_i);
			}


			// eq 3
			Eigen::Matrix<Type, Eigen::Dynamic, 1>	x_tilde = alpha_i_x_i_sum / alpha_sum;
			Eigen::Matrix<Type, 2, 1>				y_tilde = alpha_i_y_i_sum / alpha_sum;


			Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic>	A(num_neighbours, dimension_2d);
			Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic>	B(num_neighbours, dimension_Nd);

			for (int j = 0; j < num_neighbours; ++j)
			{
				const int index = kdtree_indices[0][j];
			
				if (index < 0 || index >(verts_Nd.size() - 1) || index >(verts_2d.size() - 1))
					continue;

				const auto& x_i = verts_Nd[index];
				const auto& y_i = verts_2d[index];

				Type squaredNorm = std::max(static_cast<Type>((y_i - p).squaredNorm()), static_cast<Type>(0.0001));
				Type alpha_i = static_cast<Type>(1) / squaredNorm;

				// eq 4
				const auto x_hat = x_i - x_tilde;
				const auto y_hat = y_i - y_tilde;

				// eq 6
				A.row(j) = std::sqrt(alpha_i) * y_hat.transpose();
				B.row(j) = std::sqrt(alpha_i) * x_hat.transpose();
			}

			//
			// Free matrices used n kdtree 
			//
			delete[] kdtree_indices.ptr();
			delete[] kdtree_dists.ptr();

			//
			// Compute SVD
			//
			const auto ATB = A.transpose() * B;
			Eigen::JacobiSVD<Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic>> svd;
			//svd.compute(ATB, Eigen::ComputeFullU | Eigen::ComputeFullV);
			svd.compute(ATB, Eigen::ComputeThinU | Eigen::ComputeThinV);

			if (!svd.computeU() || !svd.computeV())
			{
				//throw("<Error> Decomposition error");
				std::ofstream out("ilamp_svd.log");
				out << "<Error> Decomposition error" << std::endl;
				throw(std::exception("Decomposition error"));
			}


			//
			// Compute M
			//
			const auto M = svd.matrixU() * svd.matrixV().transpose();

			//const auto q = (p - y_tilde).transpose() * M + x_tilde.transpose();
			//return 
			this->q = (p - y_tilde).transpose() * M + x_tilde.transpose();
		}
		catch (const std::exception& ex)
		{
			//std::cerr << "Error: " << ex.what() << std::endl;
			std::ofstream out("ilamp_execute.log");
			out << "<Error> " << ex.what() << std::endl;
		}

		return q;
	}

};


#endif // __ILAMP_H__