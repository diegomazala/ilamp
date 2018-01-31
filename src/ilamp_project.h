#ifndef __ILAMP_PROJECT_H__
#define __ILAMP_PROJECT_H__

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "json.hpp"
using json = nlohmann::json;





struct ilamp_project
{
	std::string projectName = "ProjectName.ilp";
	std::string filename2d = "Filename.2d";
	std::string filenameNd = "Filename.nd";
	std::string outputFolder = "C:/tmp/";
	std::vector<std::string> inputFiles = { std::string("mod1.ply"), std::string("mod2.ply"), std::string("mod3.ply") };

	uint16_t numNeighbours = 4;
	uint16_t kdTreeCount = 4;
	uint16_t knnSearchChecks = 128;

	ilamp_project() 
	{
	}

	ilamp_project(const std::string& json_file)
	{
		std::ifstream ilp(json_file);
		ilp >> *this;
		ilp.close();
	}

	friend void to_json(json& j, const ilamp_project& p)
	{
		j = json
		{ 
			{ "ProjectName", p.projectName },
			{ "FileName2d", p.filename2d },
			{ "FileNameNd", p.filenameNd },
			{ "OutputFolder", p.outputFolder },
			{ "InputFiles", p.inputFiles },
			{ "NumNeighbours", p.numNeighbours },
			{ "KdTreeCount", p.kdTreeCount },
			{ "KnnSearchChecks", p.knnSearchChecks }
		};
	}

	friend void from_json(const json& j, ilamp_project& p)
	{
		p.projectName	= j.at("ProjectName").get<std::string>();
		p.filename2d	= j.at("FileName2d").get<std::string>();
		p.filenameNd	= j.at("FileNameNd").get<std::string>();
		p.outputFolder = j.at("OutputFolder").get<std::string>();
		p.inputFiles	= j.at("InputFiles").get<std::vector<std::string>>();
		p.numNeighbours = j.at("NumNeighbours").get<uint16_t>();
		p.kdTreeCount = j.at("KdTreeCount").get<uint16_t>();
		p.knnSearchChecks = j.at("KnnSearchChecks").get<uint16_t>();
	}

	friend void from_file(const std::string& json_file, ilamp_project& p)
	{
		std::ifstream ilp(json_file);
		ilp >> p;
		ilp.close();
	}

	friend std::ostream& operator << (std::ostream& os, const ilamp_project& p)
	{
		json j;
		to_json(j, p);
		os << j;
		return os;
	}

	friend std::istream& operator >> (std::istream& is, ilamp_project& p)
	{
		json j;
		is >> j;
		from_json(j, p);
		return is;
	}


	void test()
	{
		ilamp_project p1;

		// conversion: person -> json
		json j1 = p1;

		std::cout << std::setw(4) << j1 << std::endl;


		std::ofstream o("ilp.json");
		o << std::setw(4) << j1 << std::endl;
		o.close();

		// conversion: json -> person
		ilamp_project p2 = j1;

		json j2 = p2;
		std::cout << std::setw(4) << j2 << std::endl << std::endl;


		// read a JSON file
		std::ifstream i("ilp.json");
		json j3;
		i >> j3;
		i.close();

		std::cout << std::setw(4) << j3 << std::endl << std::endl;



		ilamp_project p4;
		std::ifstream i2("ilp.json");
		i2 >> p4;
		i2.close();
		std::cout << ">>>>>>>>>>>>>>>>>>>>\n" << std::setw(4) << p4 << std::endl << std::endl;



		ilamp_project py_prj;
		std::ifstream py_ilp("PyProjectName.ilp");
		py_ilp >> py_prj;
		py_ilp.close();
		std::cout << "=====================\n" << std::setw(4) << py_prj << std::endl << std::endl;
	}
};


#endif  //__ILAMP_PROJECT_H__
