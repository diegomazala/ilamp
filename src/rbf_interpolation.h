#include <Eigen/Dense>
#include <iostream>


Eigen::MatrixXd rbfcreate(
	Eigen::MatrixXd x, 
	Eigen::MatrixXd y, 
	Eigen::MatrixXd RBFFunction(Eigen::MatrixXd r, double const_num),
	double RBFConstant, 
	double RBFSmooth);

Eigen::MatrixXd rbfphi_linear(Eigen::MatrixXd r, double const_num);

Eigen::MatrixXd rbfphi_cubic(Eigen::MatrixXd r, double const_num);

Eigen::MatrixXd rbfphi_gaussian(Eigen::MatrixXd r, double const_num);

Eigen::MatrixXd rbfphi_multiquadrics(Eigen::MatrixXd r, double const_num);

Eigen::MatrixXd rbfphi_thinplate(Eigen::MatrixXd r, double const_num);

Eigen::MatrixXd rbfAssemble(
	Eigen::MatrixXd x, 
	Eigen::MatrixXd RBFFunction(Eigen::MatrixXd r, double const_num),
	double RBFConstant, 
	double RBFSmooth);

Eigen::MatrixXd rbfinterp(
	Eigen::MatrixXd nodes, 
	Eigen::MatrixXd rbfcoeff, 
	Eigen::MatrixXd x,
	Eigen::MatrixXd RBFFunction(Eigen::MatrixXd r, double const_num),
	double RBFConstant);

double rbfcheck(
	Eigen::MatrixXd x, 
	Eigen::MatrixXd y, 
	Eigen::MatrixXd rbfcoeff,
	Eigen::MatrixXd RBFFunction(Eigen::MatrixXd r, double const_num),
	double RBFConstant);