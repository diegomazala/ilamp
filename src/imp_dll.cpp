#include "ilamp.h"
#include "rbf_imp.h"

#include <vector>
#include <fstream>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#define DllExport extern "C" __declspec (dllexport)



static std::unique_ptr<Imp<float>> imp_ptr;
static std::unique_ptr<std::ofstream> imp_log;



DllExport void Imp_Initialize_ILamp()
{
	imp_ptr.reset(new ILamp<float>());
	imp_log.reset(new std::ofstream("imp_ilamp.log"));
	(*imp_log) << "Info : Imp dll initilized as ILamp" << std::endl;
}


DllExport void Imp_Initialize_Rbf()
{
	imp_ptr.reset(new RbfImp<float>());
	imp_log.reset(new std::ofstream("imp_rbf.log"));
	(*imp_log) << "Info : Imp dll initilized as Rbf" << std::endl;
}



DllExport bool Imp_LoadInputFiles(const char* filename_2d, const char* filename_Nd)
{
	if (!imp_ptr)
	{
		(*imp_log) << "Error: <Imp_SetKdTree> ilamp not initilized" << std::endl;
		return false;
	}
	

	if (!(imp_ptr->load_data_2d(filename_2d) && imp_ptr->load_data_Nd(filename_Nd)))
	{
		(*imp_log) << "Error: <Imp_LoadInputFiles> Could not load input files" << std::endl;
		return false;
	}


	if (imp_ptr->verts_Nd.size() != imp_ptr->verts_2d.size())
	{
		(*imp_log) << "Error: <Imp_LoadInputFiles> Vertex arrays do not have the same size. Abort" << std::endl;
		return false;
	}

	return true;
}



DllExport void Imp_ILamp_Setup(uint16_t _kdtree_count, uint16_t _num_neighbours, uint16_t _knn_search_checks)
{
	auto ilamp_ptr = (ILamp<float>*)(imp_ptr.get());

	if (!ilamp_ptr)
	{
		(*imp_log) << "Error: <Imp_ILamp_SetKdTree> ilamp not initilized" << std::endl;
		return;
	}
	
	ilamp_ptr->set_kdtree(_kdtree_count, _num_neighbours, _knn_search_checks);
}


DllExport void Imp_Rbf_Setup(uint8_t function, float constant)
{
	auto rbf_ptr = (RbfImp<float>*)(imp_ptr.get());

	if (!rbf_ptr)
	{
		(*imp_log) << "Error: <Imp_ILamp_SetKdTree> ilamp not initilized" << std::endl;
		return;
	}

	rbf_ptr->setup(RbfFunctionEnum(function), constant);
}



DllExport bool Imp_Build()
{
	if (!imp_ptr)
	{
		(*imp_log) << "Error: <Imp_Build> Imp not initilized" << std::endl;
		return false;
	}

	try
	{
		imp_ptr->build();
		return true;
	}
	catch (const std::exception& ex)
	{
		(*imp_log) << ex.what() << std::endl;
		return false;
	}
}


DllExport bool Imp_Execute(float x, float y)
{
	if (!imp_ptr)
	{
		(*imp_log) << "Error: <Imp_Execute> ilamp not initilized" << std::endl;
		return false;
	}

	try
	{
		imp_ptr->execute(x, y);
		return true;
	}
	catch (const std::exception& ex)
	{
		(*imp_log) << "Error: <ILamp_RunILamp> " << x << ' ' << y << std::endl
			<< ex.what() << std::endl;
		return false;
	}
}

DllExport size_t Imp_QRows()
{
	if (!imp_ptr)
	{
		(*imp_log) << "Error: <Imp_QRows> ilamp not initilized" << std::endl;
		return 0;
	}
	return imp_ptr->q.rows();
}

DllExport size_t Imp_QCols()
{
	if (!imp_ptr)
	{
		(*imp_log) << "Error: <ILamp_QCols> ilamp not initilized" << std::endl;
		return 0;
	}
	return imp_ptr->q.cols();
}


DllExport bool Imp_CopyQ(void* p_array_float_N)
{
	if (!imp_ptr)
	{
		(*imp_log) << "Error: <Imp_CopyQ> ilamp not initilized" << std::endl;
		return false;
	}

	float* p_array_float = (float*)p_array_float_N;

	// safeguard - pointer must be not null
	if (!p_array_float)
		return false;

	size_t r = imp_ptr->q.rows();
	size_t c = imp_ptr->q.cols();
	size_t coords_count = imp_ptr->q.rows() * imp_ptr->q.cols();

	std::memcpy(p_array_float, imp_ptr->q.data(), coords_count * sizeof(float));

	return true;
}

DllExport float Imp_MinX()
{
	if (!imp_ptr)
	{
		(*imp_log) << "Error: <Imp_MinX> Imp not initilized" << std::endl;
		return 0;
	}
	return imp_ptr->min_x;
}

DllExport float Imp_MaxX()
{
	if (!imp_ptr)
	{
		(*imp_log) << "Error: <ILamp_MaxX> Imp not initilized" << std::endl;
		return 0;
	}
	return imp_ptr->max_x;
}

DllExport float Imp_MinY()
{
	if (!imp_ptr)
	{
		(*imp_log) << "Error: <ILamp_MinY> Imp not initilized" << std::endl;
		return 0;
	}
	return imp_ptr->min_y;
}

DllExport float Imp_MaxY()
{
	if (!imp_ptr)
	{
		(*imp_log) << "Error: <ILamp_MaxY> ilamp not initilized" << std::endl;
		return 0;
	}
	return imp_ptr->max_y;
}


DllExport bool Imp_ExecuteLamp(const char* input_filename_nd, const char* output_filename_2d)
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
