#include "ilamp.h"
#include "ilamp_project.h"
#include "ilamp_utils.h"

#include <vector>
#include <fstream>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#define DllExport extern "C" __declspec (dllexport)



static std::unique_ptr<ILamp<float>> ilamp;
static std::vector<float> output_verts;
static std::unique_ptr<std::ofstream> ilamp_log;


DllExport void ILamp_BuildNdFile(const char* ilamp_project_filename)
{
	//
	// Creating nd file
	//
	ilamp_project ilp_prj(ilamp_project_filename);
	build_nd_file(ilp_prj.inputFiles, ilp_prj.filenameNd);
}

DllExport bool ILamp_RunLamp(const char* input_filename_nd, const char* output_filename_2d)
{
	//
	// Running lamp in order to generate 2d file from nd file
	//
	std::string lamp_script = std::getenv("ILAMP_LAMP_SCRIPT");

	if (!fs::exists(lamp_script))
	{
		std::cerr << "Lamp python script not found" << std::endl;
		return false;
	}

	std::stringstream lamp_cmd;
	lamp_cmd << "python " << lamp_script << ' ' << input_filename_nd << ' ' << output_filename_2d << " > lamp.log";
	std::system(lamp_cmd.str().c_str());
#if _DEBUG
	std::cout << std::ifstream("lamp.log").rdbuf();
#endif

	return true;
}

DllExport bool ILamp_LoadInputFiles(const char* filename_2d, const char* filename_Nd)
{
	ilamp.reset(new ILamp<float>());
	ilamp_log.reset(new std::ofstream("ilamp.log"));
	
	(*ilamp_log) << "Info : Ilamp dll initilized" << std::endl;

	if (!(ilamp->load_data_2d(filename_2d) && ilamp->load_data_Nd(filename_Nd)))
		return false;

	if (ilamp->verts_Nd.size() != ilamp->verts_2d.size())
	{
		(*ilamp_log) << "Error: <ILamp_LoadInputFiles> Vertex arrays do not have the same size. Abort" << std::endl;
		return false;
	}

	return true;
}



DllExport void ILamp_BuildKdTree(uint16_t kdtree_count)
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_BuildKdTree> ilamp not initilized" << std::endl;
		return;
	}
	
	try
	{
		ilamp->build_kdtree(kdtree_count);
	}
	catch (const std::exception& ex)
	{
		(*ilamp_log) << ex.what() << std::endl;
	}
}


DllExport bool ILamp_RunILamp(float x, float y, int num_neighbours, int knn_search_checks)
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_RunILamp> ilamp not initilized" << std::endl;
		return false;
	}

	try
	{
		//(*ilamp_log) << "Info : <ILamp_RunILamp> " << x << ' ' << y << ' ' << num_neighbours << ' ' << knn_search_checks << std::endl;
		ilamp->execute(x, y, num_neighbours, knn_search_checks);
		return true;
	}
	catch (const std::exception& ex)
	{
		(*ilamp_log) << "Error: <ILamp_RunILamp> " << x << ' ' << y << ' ' << num_neighbours << ' ' << knn_search_checks << std::endl
			<< ex.what() << std::endl;
		return false;
	}
}

DllExport size_t ILamp_QRows()
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_QRows> ilamp not initilized" << std::endl;
		return 0;
	}
	return ilamp->q.rows();
}

DllExport size_t ILamp_QCols()
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_QCols> ilamp not initilized" << std::endl;
		return 0;
	}
	return ilamp->q.cols();
}


DllExport void ILamp_CopyQ(void* p_array_float_N)
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_CopyQ> ilamp not initilized" << std::endl;
		return;
	}

	float* p_array_float = (float*)p_array_float_N;

	// safeguard - pointer must be not null
	if (!p_array_float)
		return;

	size_t r = ilamp->q.rows();
	size_t c = ilamp->q.cols();
	size_t coords_count = ilamp->q.rows() * ilamp->q.cols();

	std::memcpy(p_array_float, ilamp->q.data(), coords_count * sizeof(float));
}

DllExport float ILamp_MinX()
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_MinX> ilamp not initilized" << std::endl;
		return 0;
	}
	return ilamp->min_x;
}

DllExport float ILamp_MaxX()
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_MaxX> ilamp not initilized" << std::endl;
		return 0;
	}
	return ilamp->max_x;
}

DllExport float ILamp_MinY()
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_MinY> ilamp not initilized" << std::endl;
		return 0;
	}
	return ilamp->min_y;
}

DllExport float ILamp_MaxY()
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_MaxY> ilamp not initilized" << std::endl;
		return 0;
	}
	return ilamp->max_y;
}

DllExport void ILamp_WritePly(size_t verts_count, void* p_verts_array, void* p_normals_array, void* p_uv_array, void* p_faces_array)
{
	float* p_verts_float = (float*)p_verts_array;
	float* p_normals_float = (float*)p_normals_array;
	float* p_uv_float = (float*)p_uv_array;
	int* p_faces_float = (int*)p_faces_array;

	// safeguard - pointer must be not null
	if (!p_verts_float)
		return;

	std::vector<float> verts(p_verts_float, p_verts_float + verts_count * 3);

	write_ply_file("C:/tmp/point_cloud.ply", verts);
	write_ply_file("C:/tmp/mesh.ply", verts, "C:/Users/diego/Google Drive/Data/Test/Heads/Head01.ply");
}


#include "rbf_interpolation.hpp"
DllExport void ILamp_RbfTest()
{

	if (ilamp->verts_Nd.size() != ilamp->verts_2d.size())
	{
		(*ilamp_log) << "Error: <ILamp_LoadInputFiles> Vertex arrays do not have the same size. Abort" << std::endl;
	}

	const auto& y = ilamp->verts_2d;
	const auto& x = ilamp->verts_Nd;

	std::size_t N = x.size();
	std::size_t m = x[0].size();

	Eigen::MatrixXd verts_2d(N, 2);
	Eigen::MatrixXd verts_Nd(N, m);
	
	for (int i = 0; i < N; ++i)
	{
		verts_2d(i, 0) = y[i].x();
		verts_2d(i, 1) = y[i].y();

		for (std::size_t j = 0; j < verts_2d.cols(); ++j)
		{
			verts_Nd(i, j) = (double)x[i][j];
		}
	}


	//auto coeff = rbfcreate(verts_2d, verts_Nd, 'RBFConstant');
}

float phi_function(float r)
{
	return 0;
}

DllExport void ILamp_RbfAlgorithm()
{
	if (ilamp->verts_Nd.size() != ilamp->verts_2d.size())
	{
		(*ilamp_log) << "Error: <ILamp_LoadInputFiles> Vertex arrays do not have the same size. Abort" << std::endl;
	}
	const auto& y = ilamp->verts_2d;
	const auto& x = ilamp->verts_Nd;

	//
	// Queries
	//
	auto p = y[0];
	auto q = x[0];
	
	std::size_t N = x.size();
	std::size_t m = x[0].size();
	Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> phi(N, N);
	Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> b(N, m);

	//
	// b tem que ser (b(N, 1)
	// o sistema será resolvido k vezes
	//



	// 
	// Asssemble phi matrix
	//
	for (std::size_t i = 0; i < N; ++i)
	{
		for (std::size_t j = 0; j < N; ++j)
		{
			const auto phi_func = (y[j] - y[i]).norm();

			phi(i, j) = phi(j, i) = phi_func;
		}
	}

	// 
	// Assemble b matrix
	//
	for (std::size_t i = 0; i < N; ++i)
	{
		for (std::size_t j = 0; j < m; ++j)
		{
			b(i, j) = x[i][j];
		}
	}

	auto lambda = phi.lu().solve(b);

	auto r = lambda.rows();	// 8
	auto c = lambda.cols();	// 67k

	
	Eigen::Matrix<float, Eigen::Dynamic, 1> s_p(m);
	for (std::size_t k = 0; k < m; ++k)
	{
		float sk_p = 0;
		for (std::size_t i = 0; i < N; ++i)
		{
			float lki = lambda(i, k);
			float phi = 1;

			sk_p += lki * phi * (y[i] - p).norm();
		}
	}

}