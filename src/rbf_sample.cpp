#include <iostream>
#include <Eigen/Dense>
#include "rbf_interpolation.h"

void sample_test_1()
{
	std::cout.precision(4);
	Eigen::MatrixXd m1(9, 1);
	m1 << -2.0,
		-1.5,
		-1.0,
		-0.5,
		0,
		0.5,
		1.0,
		1.5,
		2.0;

	Eigen::MatrixXd m2(9, 1);
	m2 << -0.0366,
		-0.1581,
		-0.3679,
		-0.3894,
		0,
		0.3894,
		0.3679,
		0.1581,
		0.0366;

	const size_t rows = 5;
	Eigen::MatrixXd m3(rows, 1);
	for (int i = 0; i < rows; i++)
	{
		m3(i) = -2 + (double)i*0.01;
	}

	std::cout << std::fixed << "m1     : " << std::endl << m1.transpose() << std::endl;
	std::cout << std::fixed << "m2     : " << std::endl << m2.transpose() << std::endl << std::endl;

	Eigen::MatrixXd rbfcoeff = rbfcreate(m1, m2, rbfphi_multiquadrics, 0.4444, 0);
	//Eigen::MatrixXd result = rbfinterp(m1, rbfcoeff, m3, rbfphi_multiquadrics, 0.4444);
	Eigen::MatrixXd result = rbfinterp(m1, rbfcoeff, m1, rbfphi_multiquadrics, 0.4444);

	std::cout << "rbfcoeff: " << rbfcoeff.rows() << ", " << rbfcoeff.cols() << std::endl << rbfcoeff.transpose() << std::endl << std::endl;
	std::cout << std::fixed << "result : " << result.rows() << ", " << result.cols() << std::endl << result.transpose() << std::endl << std::endl;

	std::cout << std::fixed << "check    : " << rbfcheck(m1, m2, rbfcoeff, rbfphi_multiquadrics, 0.4444) << std::endl;

	std::cout << std::fixed << "max_coeff: " << (m1 - m2).cwiseAbs().maxCoeff() << std::endl << std::endl << std::endl;

}


void sample_test_2()
{
	std::cout.precision(4);
	Eigen::MatrixXd m1(9, 1);
	m1 << 
		0.0,
		1.25,
		2.5,
		3.75,
		5.0,
		6.25,
		7.5,
		8.75,
		10;

	Eigen::MatrixXd m2(9, 1);
	m2 << 
		0.0,
		0.94898462,
		0.59847214,
		-0.57156132,
		-0.95892427,
		-0.03317922,
		0.93799998,
		0.62472395,
		-0.54402111;


	Eigen::MatrixXd rbfcoeff;
	rbfcoeff = rbfcreate(m1, m2, rbfphi_multiquadrics, 0.4444, 0);
	
	Eigen::MatrixXd result = rbfinterp(m1, rbfcoeff, m1, rbfphi_multiquadrics, 0.4444);

	std::cout << std::fixed << "rbfcoeff : " << rbfcoeff.rows() << ", " << rbfcoeff.cols() << std::endl << rbfcoeff.transpose() << std::endl << std::endl;
	std::cout << std::fixed << "result   : " << result.rows() << ", " << result.cols() << std::endl << result.transpose() << std::endl << std::endl;

	std::cout << std::fixed << "check    : " << rbfcheck(m1, m2, rbfcoeff, rbfphi_multiquadrics, 0.4444) << std::endl;
	std::cout << std::fixed << "max_coeff: " << (m1 - m2).cwiseAbs().maxCoeff() << std::endl << std::endl << std::endl;
}





int main(int argc, char *argv[])
{
	sample_test_2();

	return EXIT_SUCCESS;
}