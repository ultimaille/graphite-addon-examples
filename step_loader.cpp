#include <iostream>
#include <fstream>

#include <ultimaille/all.h>
#include <param_parser/param_parser.h>

inline std::string separator()
{
#ifdef _WIN32
	return "\\";
#else
	return "/";
#endif
}

int main(int argc, char** argv) {
    
    // Create parameters
    Parameters params;

	params.help = "Load a step file and convert to surface or volume mesh";

    // Add program parameters
    params.add(Parameters::Type::File, "gmsh_path", "gmsh").description("GMSH path / name (please add GMSH to you PATH !)");
    params.add(Parameters::Type::File, "step_path", "").description("Step file to load");
    params.add(Parameters::Type::Bool, "volume", "true").description("Convert to volume mesh");
    params.add(Parameters::Type::Double, "size_factor", "1").description("Size factor");
    params.add(Parameters::Type::Double, "result_path", "").type_of_param("system");

    /* Parse program arguments */
    params.init_from_args(argc, argv);

    // Get args
	std::string gmsh_path = params["gmsh_path"];
    std::string step_path = params["step_path"];
	bool volume = params["volume"];
	double size_factor = params["size_factor"];

    std::filesystem::path result_path(params["result_path"]);


	// Out file
	std::filesystem::path out_filename = result_path / "out.mesh";

	// Write config file for GMSH
	std::ofstream conf_file;
	conf_file.open("conf.geo");
	if (conf_file.is_open()) { 
		std::string mesh_type = volume ? "3" : "2";
		conf_file << "General.Terminal = 1;\nMesh.MeshSizeFactor = " + std::to_string(size_factor) + ";\nMesh " + mesh_type + ";\nSave \"" + out_filename.string() + "\";";
		conf_file.close();
	} else {
		std::cout << "Unable to write file: conf.geo at the Graphite root directory." << std::endl;
	}

	// Run GMSH giving the input model & config file as arguments
	std::string command = gmsh_path + " " + std::string(step_path) + " conf.geo -0";
	std::cout << "call: " << command << std::endl;
	// Execute
	int result = system(command.c_str());

	std::cout << "call result: " + std::to_string(result) << std::endl;
    
    return 0;
}