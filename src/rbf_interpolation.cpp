#include "rbf_interpolation.h"


Eigen::MatrixXd rbfcreate(
	Eigen::MatrixXd x,							
	Eigen::MatrixXd y,											
	Eigen::MatrixXd RBFFunction(Eigen::MatrixXd r, double const_num),
	double RBFConstant,									
	double RBFSmooth = 0)								
														
{
	std::cout << x.rows() << ' ' << x.cols() << std::endl;
	std::cout << y.rows() << ' ' << y.cols() << std::endl;

	int nXDim = x.cols();
	int nYDim = y.cols();
	int nXCount = x.rows();
	int nYCount = y.rows();
	if (nXCount != nYCount)
		std::cerr << "x and y should have the same number of rows" << std::endl;
	if (nYDim != 1)
		std::cerr << "y should be n by 1 vector" << std::endl;
	Eigen::MatrixXd A;
	A = rbfAssemble(x, RBFFunction, RBFConstant, RBFSmooth);

	std::cout << A.rows() << ' ' << A.cols() << std::endl;

	Eigen::MatrixXd b = Eigen::MatrixXd::Zero(y.rows() + nXDim + 1, 1);
	std::cout << b.rows() << ' ' << b.cols() << std::endl;
	b.topRows(y.rows()) = y;
	std::cout << b.rows() << ' ' << b.cols() << std::endl;
	Eigen::MatrixXd rbfcoeff;
	rbfcoeff = A.lu().solve(b);
	std::cout << rbfcoeff.rows() << ' ' << rbfcoeff.cols() << std::endl;
	return rbfcoeff;
}


Eigen::MatrixXd rbfAssemble(
	Eigen::MatrixXd x,						
	Eigen::MatrixXd RBFFunction(Eigen::MatrixXd r, double const_num),
	double RBFConstant,									
	double RBFSmooth)									
														
{
	int dim = x.cols();
	int n = x.rows();
	Eigen::MatrixXd r = Eigen::MatrixXd::Zero(n, n);
	Eigen::MatrixXd temp_A;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < i; j++)
		{
			r(i, j) = (x.row(i) - x.row(j)).norm();
			r(j, i) = r(i, j);
		}
	}
	temp_A = RBFFunction(r, RBFConstant);

	std::cout << "temp_A " << temp_A.rows() << ' ' << temp_A.cols() << std::endl;
	
	for (int i = 0; i < n; i++)
	{
		temp_A(i, i) = temp_A(i, i) - RBFSmooth;
	}
	Eigen::MatrixXd P(x.rows(), x.cols() + 1);
	P.leftCols(1) = Eigen::MatrixXd::Ones(n, 1);
	P.rightCols(x.cols()) = x;

	std::cout << "P " << P.rows() << ' ' << P.cols() << std::endl;

	Eigen::MatrixXd A = Eigen::MatrixXd::Zero(temp_A.rows() + P.cols(), temp_A.cols() + P.cols());
	A.topLeftCorner(temp_A.rows(), temp_A.cols()) = temp_A;
	A.topRightCorner(P.rows(), P.cols()) = P;
	A.bottomLeftCorner(P.cols(), P.rows()) = P.transpose();
	return A;
}


Eigen::MatrixXd rbfphi_linear(Eigen::MatrixXd r, double const_num)
//Linear
{
	Eigen::MatrixXd u(r.rows(), r.cols());
	u = r;
	return u;
}


Eigen::MatrixXd rbfphi_cubic(Eigen::MatrixXd r, double const_num)
//Cubic
{
	Eigen::MatrixXd u(r.rows(), r.cols());
	u = r.array().pow(3);
	return u;
}


Eigen::MatrixXd rbfphi_gaussian(Eigen::MatrixXd r, double const_num)
//Gaussian
{
	Eigen::MatrixXd u(r.rows(), r.cols());
	u = (-0.5*r.array().square() / (const_num*const_num)).array().exp();
	return u;
}


Eigen::MatrixXd rbfphi_multiquadrics(Eigen::MatrixXd r, double const_num)
//Multiquadrics
{
	Eigen::MatrixXd u(r.rows(), r.cols());
	u = (1.0 + r.array().square() / (const_num*const_num)).array().sqrt();
	return u;
}


Eigen::MatrixXd rbfphi_thinplate(Eigen::MatrixXd r, double const_num)
//Thinplate
{
	Eigen::MatrixXd u(r.rows(), r.cols());
	u = (r.array().square()).cwiseProduct((r.array() + 1).log());
	return u;
}


Eigen::MatrixXd rbfinterp(
	Eigen::MatrixXd nodes,						
	Eigen::MatrixXd rbfcoeff,									
	Eigen::MatrixXd x,											
	Eigen::MatrixXd RBFFunction(Eigen::MatrixXd r, double const_num),	
	double RBFConstant)									
														
{
	int dim = nodes.cols();
	int n = nodes.rows();
	int dimPoints = x.cols();
	int nPoints = x.rows();
	if (dim != dimPoints)
		std::cerr << "x should have the same number of rows as an array used to create RBF interpolation" << std::endl;
	Eigen::MatrixXd r = Eigen::MatrixXd::Zero(nPoints, n);
	Eigen::MatrixXd temp_A;
	for (int i = 0; i < nPoints; i++)
	{
		for (int j = 0; j < n; j++)
		{
			r(i, j) = (x.row(i) - nodes.row(j)).norm();
		}
	}
	temp_A = RBFFunction(r, RBFConstant);
	Eigen::MatrixXd P(x.rows(), x.cols() + 1);
	P.leftCols(1) = Eigen::MatrixXd::Ones(x.rows(), 1);
	P.rightCols(x.cols()) = x;
	Eigen::MatrixXd A(nPoints, n + x.cols() + 1);
	A.topLeftCorner(temp_A.rows(), temp_A.cols()) = temp_A;
	A.topRightCorner(P.rows(), P.cols()) = P;
	Eigen::MatrixXd f;
	f = A * rbfcoeff;
	return f;
}


double rbfcheck(
	Eigen::MatrixXd x,
	Eigen::MatrixXd y,
	Eigen::MatrixXd rbfcoeff,
	Eigen::MatrixXd RBFFunction(Eigen::MatrixXd r, double const_num),
	double RBFConstant)
{
	Eigen::MatrixXd S;
	S = rbfinterp(x, rbfcoeff, x, RBFFunction, RBFConstant);
	double maxdiff;
	maxdiff = (S - y).cwiseAbs().maxCoeff();
	return maxdiff;
}