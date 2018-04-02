#ifndef __RBF_IMP_H__
#define __RBF_IMP_H__

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
class RbfImp : public Imp<Type>
{
public:
	RbfImp() {}
	~RbfImp() {}


	void build()
	{
		if (verts_Nd.size() != verts_2d.size())
		{
			std::cerr << "Error: Vertex arrays do not have the same size. Abort" << std::endl;
		}

		y = Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic>(verts_2d.size(), 2);
		x = Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic>(verts_Nd.size(), verts_Nd[0].size());

		for (std::size_t i = 0; i < y.rows(); ++i)
		{
			for (std::size_t j = 0; j < y.cols(); ++j)
				y(i, j) = verts_2d[i][j];

			for (std::size_t j = 0; j < x.cols(); ++j)
				x(i, j) = verts_Nd[i][j];

		}

		std::size_t N = x.rows();	// rows
		std::size_t m = x.cols();	// cols


		phi = Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic>(N, N);
		lambda = Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic>(N, m);
		const Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic>& b = x;
		


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
		// Solve the system m times to find lambda's
		//
		for (std::size_t k = 0; k < m; ++k)
		{
			// Ax = b
			// phi * lambda = b
			lambda.col(k) = phi.lu().solve(b.col(k));
		}

	}



	Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic> execute(Type px, Type py)
	{
		const std::size_t N = x.rows();
		const std::size_t m = x.cols();
		
		Eigen::Matrix<Type, 1, 2> p(px, py);

		
		//
		// Computing s_p
		//
		this->q = Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic>(m, 1);
		for (std::size_t k = 0; k < m; ++k)
		{
			Type sum_i_N = 0.0f;
			for (std::size_t i = 0; i < N; ++i)
			{
				auto r = (y.row(i) - p).norm();
				sum_i_N += lambda(i, k) * RbfFunction<Type>::multiquadrics(r);
			}
			this->q(k) = sum_i_N;
		}
		return this->q;


#if 0	
		// 
		// Computing s for all rbf centers (y)
		//
		Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic> s(N, m);
		for (std::size_t j = 0; j < N; ++j)
		{
			for (std::size_t k = 0; k < m; ++k)
			{
				Type sum_i_N = 0.0f;
				for (std::size_t i = 0; i < N; ++i)
				{
					auto r = (y.row(i) - y.row(j)).norm();
					sum_i_N += lambda(i, k) * RbfFunction<Type>::multiquadrics(r);
				}
				s(j, k) = sum_i_N;
			}
		}
		this->q = s.row(model_index);
#endif

	}

	Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic> y;
	Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic> x;
	Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic> phi;
	Eigen::Matrix<Type, Eigen::Dynamic, Eigen::Dynamic> lambda;
};



#endif // __RBF_IMP_H__