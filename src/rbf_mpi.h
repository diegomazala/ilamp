#ifndef __RBF_MPI_H__
#define __RBF_MPI_H__

#include "rbf_interpolation.h"
#include <vector>

const float MATH_CONST_e = 2.71828f;
const float MATH_CONST_E = 1.30568f;

template <typename Type>
class RbfFunction
{
public:
	static Type gaussian(Type r, Type c = 0)
	{
		return std::pow(MATH_CONST_e, -MATH_CONST_E * r * r);
	}

	static Type multiquadrics(Type r, Type c = 0)
	{
		return std::sqrt(std::pow(c, 2) + MATH_CONST_E * std::pow(r, 2));
	}

};




template <typename Type>
class RbfMpi
{
public:
	RbfMpi() {}
	~RbfMpi() {}


	Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic> execute(
		const std::vector<Eigen::Matrix<Type, 2, 1>>& verts_2d,
		const std::vector<Eigen::Matrix<Type, Eigen::Dynamic, 1>>& verts_Nd,
		int model_index)
	{

		if (verts_Nd.size() != verts_2d.size())
		{
			std::cerr << "Error: Vertex arrays do not have the same size. Abort" << std::endl;
		}



		Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic> y(verts_2d.size(), 2);
		Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic> x(verts_Nd.size(), verts_Nd[0].size());

		for (std::size_t i = 0; i < y.rows(); ++i)
		{
			for (std::size_t j = 0; j < y.cols(); ++j)
				y(i, j) = verts_2d[i][j];

			for (std::size_t j = 0; j < x.cols(); ++j)
				x(i, j) = verts_Nd[i][j];

		}

		std::size_t N = x.rows();	// rows
		std::size_t m = x.cols();	// cols

		
		Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> phi(N, N);
		Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> b(N, m);
		Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> lambda(N, m);
		Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> lambda_svd(N, m);
		Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> s(N, m);



		// 
		// Asssembling matrices
		//
		for (std::size_t i = 0; i < N; ++i)
		{
			//
			// phi matrix
			//
			for (std::size_t j = i; j < N; ++j)
			{
				auto r = (y.row(i) - y.row(j)).norm();
				phi(i, j) = phi(j, i) = RbfFunction<Type>::multiquadrics(r);
			}
		}


		// 
		// Assemble b matrix
		//
		for (std::size_t i = 0; i < N; ++i)
		{
			for (std::size_t j = 0; j < m; ++j)
			{
				b(i, j) = x(i, j);
			}

		}


		//
		// Solve the system m times to find lambda's
		//
		for (std::size_t k = 0; k < m; ++k)
		{
			// Ax = b
			// phi * lambda = b
			lambda.col(k) = phi.lu().solve(b.col(k));
		}

		for (std::size_t j = 0; j < N; ++j)
		{
			for (std::size_t k = 0; k < m; ++k)
			{
				float sum_i_N = 0.0f;
				for (std::size_t i = 0; i < N; ++i)
				{
					auto r = (y.row(i) - y.row(j)).norm();
					sum_i_N += lambda(i, k) * RbfFunction<Type>::multiquadrics(r);
				}
				s(j, k) = sum_i_N;
			}
		}

		return s.row(model_index);
	}

};



#endif // __RBF_MPI_H__