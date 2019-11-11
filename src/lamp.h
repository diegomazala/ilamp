
#ifndef _LAMP_H_
#define _LAMP_H_

#include <Eigen/Dense>
#include <random>
#include <sstream>

template <typename decimal_t>
class Lamp
{
	
public:
	Lamp() {}
	~Lamp() {}


	bool save_matrix_to_file(const std::string& filename)
	{
		std::ofstream file(filename);

		if (!file.is_open())
			return false;

		file << std::fixed;

		for (auto i = 0; i < Y.rows(); i++)
		{
			for (auto j = 0; j < Y.cols(); ++j)
			{
				file << Y(i, j) << ' ';
			}
			file << '\n';
		}

		//std::cout << matrix << std::endl;

		return true;
	}


	bool load_matrix_from_file(const std::string& filename)
	{
		std::ifstream file(filename);

		if (!file.is_open())
			return false;

		size_t n_cols = 0;
		{
			decimal_t number;
			std::string line;
			std::getline(file, line);
			std::istringstream fin(line);
			while (fin >> number)
			{
				n_cols++;
			}
		}
		file.clear();
		file.seekg(0, std::ios::beg);

		size_t n_rows = 0;
		while (!file.eof())
		{
			std::string line;
			std::getline(file, line);
			if (!line.empty())
				n_rows++;
		}
		file.clear();
		file.seekg(0, std::ios::beg);

		this->X = Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>(n_rows, n_cols);
		//std::cout << matrix.rows() << ' ' << matrix.cols() << std::endl;

		for (auto i = 0; i < n_rows; i++)
		{
			for (auto j = 0; j < n_cols; ++j)
			{
				file >> X(i, j);
			}
			file.ignore(0, '\n');
		}

		//std::cout << matrix << std::endl;

		return true;
	}

	Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> fit()
	{
		//
		// Check if there are control points
		if (control_points.rows() < 2)
		{
			compute_random_control_points();
		}

		compute_control_points_centroid();
		center_control_points();
		setup_control_points_data();
		scale_control_points();
		compute_weights();
		compute_xy_tilde();
		compute_M();
		compute_Y();
		return this->Y;
	}

	void compute_control_points_by_distance()
	{
		auto n_ctps = 3;
		control_points = Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>(n_ctps, 2);
		control_points_ids = Eigen::Matrix<Eigen::DenseIndex, Eigen::Dynamic, Eigen::Dynamic>(n_ctps, 1);

		control_points_ids(0, 0) = 0;
		control_points.row(0) << 0, 0;

		size_t max_dist_ind_1 = 1;
		size_t max_dist_ind_2 = 2;
		decimal_t max_dist_1 = (X.row(1) - X.row(0)).norm();
		decimal_t max_dist_2 = (X.row(2) - X.row(0)).norm();
		for (auto i = 3; i < X.rows(); ++i)
		{
			auto dist = (X.row(i) - X.row(0)).norm();
			if (dist > max_dist_1)
			{
				if (max_dist_ind_1 != max_dist_ind_2)
				{
					max_dist_2 = max_dist_1;
					max_dist_ind_2 = max_dist_ind_1;
				}

				max_dist_1 = dist;
				max_dist_ind_1 = i;
			}
		}

		control_points_ids(1, 0) = max_dist_ind_1;
		control_points.row(1) << 0, 1;

		control_points_ids(2, 0) = max_dist_ind_2;
		control_points.row(2) << 1, 0;
	}

	void compute_random_control_points()
	{
		auto n_ctps = 3;
		control_points = Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>(n_ctps, 2);
		control_points_ids = Eigen::Matrix<Eigen::DenseIndex, Eigen::Dynamic, Eigen::Dynamic>(n_ctps, 1);

		std::random_device rd;  //Will be used to obtain a seed for the random number engine
		std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
		std::uniform_real_distribution<decimal_t> dis(0.0, 1.0);

		for (auto i = 0; i < control_points.rows(); ++i)
		{
			control_points_ids(i, 0) = i;

			// transform the random unsigned int generated by gen into a
			// double in [1, 2). Each call to dis(gen) generates a new random double
			control_points(i, 0) = dis(gen);
			control_points(i, 1) = dis(gen);
		}
	}

#if 0
	void control_points_free_mapping()
	{
		const size_t knn = 3;

		Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> D(X.rows(), X.rows());
		
		for (auto i = 0; i < X.rows(); ++i)
		{
			for (auto j = 0; j < X.rows(); ++j)
			{
				D(i, j) = (X.row(i) - X.row(j)).norm();
			}
		}

		Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>::Index i, j;
		decimal_t d01 = D.maxCoeff(&i, &j) + epsilon;

		D(i, j) = 0;
		D(j, i) = 0;

		std::vector<Eigen::DenseIndex> processed;
		processed.push_back(j);
		processed.push_back(i);

		Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> D_processed(2, D.cols());
		D_processed.row(0) = D.row(j);
		D_processed.row(1) = D.row(i);

		D_processed.maxCoeff(&i, &j);
		decimal_t d02 = std::pow(D(processed[0], j), 2) + epsilon;
		decimal_t d12 = std::pow(D(processed[1], j), 2) + epsilon;
		
		processed.push_back(j);

		for (auto i = 0; i < 3; ++i)
		{
			D.row(processed[i]).setZero();
		}

		decimal_t triangle_v1 = (-d12 + d02 + std::pow(d01, decimal_t(2))) / (decimal_t(2) * d01);
		decimal_t triangle_v2 = std::sqrt(d02 - std::pow(triangle_v1, decimal_t(2)));

		Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> proj_tri(3, 2);
		proj_tri.row(0) << 0, 0;
		proj_tri.row(1) << d01, 0;
		proj_tri.row(2) << triangle_v1, triangle_v2;

		auto self_mapped = proj_tri;
		auto max_dist = 1e8;

		D = D + max_dist * Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>::Identity(X.rows(), X.rows());

		for (auto idx_j : processed)
		{
			for (auto j : processed)
			{
				D(j, idx_j) = max_dist;
				D(idx_j, i) = max_dist;
			}
		}

		control_points = Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>(3, 2);
		control_points_ids = Eigen::Matrix<Eigen::DenseIndex, Eigen::Dynamic, Eigen::Dynamic>(3, 1);

		control_points = self_mapped;
		control_points_ids << processed[0], processed[1], processed[2];

#if 0
		D_processed = Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>::Zero(processed.size(), X.rows());

		auto k_weigths = Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>::Zero(knn, 1);

		for (auto i = knn; i < X.rows(); ++i)
		{
			D_processed = D.block(0, 0, processed.size(), X.rows());
			Eigen::DenseIndex idx_i, idx_j;
			D_processed.minCoeff(&idx_i, &idx_j);
			std::cout << idx_i << ' ' << idx_j << std::endl;

		}
#endif
	}
#endif

protected:


	Eigen::Matrix<decimal_t, 1, Eigen::Dynamic> compute_data_centroid()
	{
		Eigen::Matrix<decimal_t, 1, Eigen::Dynamic> center(1, X.cols());
		center.setZero();
		for (auto i = 0; i < X.rows(); ++i)
		{
			center += X.row(i);
		}
		center /= (T)n_samples;

		for (auto i = 0; i < X.rows(); ++i)
		{
			X.row(i) -= center;
		}
		return center;
	}

	void compute_control_points_centroid()
	{
		this->control_points_center = Eigen::Matrix<decimal_t, 1, 2>(this->control_points.col(0).mean(), this->control_points.col(1).mean());
	}

	void center_control_points()
	{
		for (auto i = 0; i < control_points.rows(); ++i)
			control_points.row(i) = control_points.row(i) - control_points_center;
	}


	void setup_control_points_data()
	{
		control_points_data = Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>(control_points.rows(), X.cols());
		for (auto i = 0; i < control_points_data.rows(); ++i)
		{
			auto id = control_points_ids.row(i)[0];
			control_points_data.row(i) = X.row(id);
		}
	}

	void scale_control_points()
	{
		//
		// Compute SVD to scale control points
		//
		Eigen::JacobiSVD<Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>> svd_ctp_data;
		svd_ctp_data.compute(control_points_data, Eigen::ComputeThinU | Eigen::ComputeThinV | Eigen::ComputeEigenvectors);
		auto sing_val_data = svd_ctp_data.singularValues().block<2, 1>(0, 0);
		//
		Eigen::JacobiSVD<Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>> svd_ctp;
		svd_ctp.compute(control_points, Eigen::ComputeThinU | Eigen::ComputeThinV | Eigen::ComputeEigenvectors);
		auto sing_val_ctp = svd_ctp.singularValues();

		Eigen::Matrix<decimal_t, 2, 2> diag_sing_data = Eigen::Matrix<decimal_t, 2, 2>::Zero();
		Eigen::Matrix<decimal_t, 2, 2> diag_sing_ctp = Eigen::Matrix<decimal_t, 2, 2>::Zero();
		for (auto i = 0; i < 2; ++i)
		{
			diag_sing_data(i, i) = sing_val_data(i);
			diag_sing_ctp(i, i) = sing_val_ctp(i);
		}

		const auto V_diag_sing = svd_ctp.matrixV() * diag_sing_data;
		const auto scale = V_diag_sing * svd_ctp.matrixU().transpose();
		control_points = scale.transpose();

		//std::cout << "control_points: \n" << control_points.transpose() << std::endl;

		self_U = svd_ctp.matrixV().transpose();
		self_S = diag_sing_ctp;
		self_So = diag_sing_data;
		self_V = svd_ctp.matrixU().transpose();
	}

	void compute_weights()
	{
		weights = Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>(X.rows(), control_points.rows());

		for (auto i = 0; i < X.rows(); ++i)
		{
			const auto& x = X.row(i);

			for (auto j = 0; j < control_points.rows(); ++j)
			{
				const auto& xi = control_points_data.row(j);

				weights(i, j) = (decimal_t)1.0 / ((xi - x).norm() + epsilon);
			}
		}
	}

	void compute_xy_tilde()
	{
		auto n_samples = X.rows();
		alpha = Eigen::Matrix<decimal_t, Eigen::Dynamic, 1>(n_samples);
		x_tilde = Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>(n_samples, X.cols());
		y_tilde = Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>(n_samples, 2);

		for (auto i = 0; i < n_samples; ++i)
		{
			const auto alpha_i = weights.row(i).sum();
			x_tilde.row(i) = ((control_points_data.transpose() * weights.row(i).transpose()) / alpha_i).transpose();
			y_tilde.row(i) = ((control_points.transpose() * weights.row(i).transpose()) / alpha_i).transpose();
			alpha(i, 0) = alpha_i;
		}

	}

	void compute_M()
	{
		auto n_control_points = control_points.rows();
		Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> A(n_control_points, X.cols());
		Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> B(n_control_points, 2);

		for (auto i = 0; i < n_control_points; ++i)
		{
			const auto x_hat = control_points_data.row(i) - x_tilde.row(i);
			const auto y_hat = control_points.row(i) - y_tilde.row(i);
			const auto ai = alpha(i, 0);

			A.row(i) = std::sqrt(ai) * x_hat;
			B.row(i) = std::sqrt(ai) * y_hat;
		}


		Eigen::JacobiSVD<Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>> svd;
		svd.compute(A.transpose() * B, Eigen::ComputeThinU | Eigen::ComputeThinV | Eigen::ComputeEigenvectors);

		this->M = svd.matrixU() * svd.matrixV();
	}

	void compute_Y()
	{
		auto n_samples = X.rows();
		Y = Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic>(n_samples, 2);
		for (auto i = 0; i < n_samples; ++i)
		{
			Y.row(i) = (X.row(i) - x_tilde.row(i)) * M + y_tilde.row(i);
		}

		auto proj = self_U.transpose() * Y.transpose();

		Eigen::Matrix<decimal_t, 2, 2> Sinv = Eigen::Matrix<decimal_t, 2, 2>::Zero();
		for (auto i = 0; i < 2; ++i)
		{
			Sinv(i, i) = decimal_t(1) / (self_So(i, i) + epsilon);
		}

		const auto proj_unscaled = Sinv * proj;
		const auto self_S_proj_unscaled = self_S * proj_unscaled;
		const auto mapped = self_U * self_S_proj_unscaled;

		Y = mapped.transpose();

		for (auto i = 0; i < n_samples; ++i)
		{
			Y.row(i) += control_points_center;
		}
	}




	Eigen::Matrix<decimal_t, 1, 2> control_points_center;
	Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> control_points_data;

	Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> self_U;
	Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> self_S;
	Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> self_So;
	Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> self_V;

	Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> weights;

	Eigen::Matrix<decimal_t, Eigen::Dynamic, 1> alpha;
	Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> x_tilde;
	Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> y_tilde;

	Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> M;

	

	const decimal_t epsilon = 1e-7;

public:
	Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> X;
	Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> control_points;
	Eigen::Matrix<Eigen::DenseIndex, Eigen::Dynamic, 1> control_points_ids;
	Eigen::Matrix<decimal_t, Eigen::Dynamic, Eigen::Dynamic> Y;

};

#endif // _LAMP_H_