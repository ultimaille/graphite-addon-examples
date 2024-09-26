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

using namespace UM;

int main(int argc, char** argv) {
    
    // Create parameters
    Parameters params;

	params.help = "Load a step file and convert to surface or volume mesh";

    // Add program parameters
    params.add(Parameters::Type::File, "gmsh_path", "gmsh").description("GMSH path / name (please add GMSH to you PATH !)");
    params.add(Parameters::Type::File, "step_path", "").description("Step file to load");
    params.add(Parameters::Type::Bool, "volume", "false").description("Convert to volume mesh");
    params.add(Parameters::Type::Double, "size_factor", "1").description("Size factor");
    params.add(Parameters::Type::Bool, "to_polyline", "false").description("Extract polyline");

    /* Parse program arguments */
    params.init_from_args(argc, argv);

    // Get args
	std::string gmsh_path = params["gmsh_path"];
    std::string step_path = params["step_path"];
	bool volume = params["volume"];
	bool to_polyline = params["to_polyline"];
	double size_factor = params["size_factor"];

    std::filesystem::path result_path(params.result_path());


	// Out file
	std::filesystem::path out_filename = result_path / "out.mesh";
	// std::filesystem::path out_filename = result_path / "out.vtk";

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

	std::cout << "to polyline ? " << to_polyline << std::endl;

	if (!to_polyline) 
		return 0;


	PolyLine p;
	PolyLineAttributes attributes = read_by_extension(out_filename.string(), p);
	EdgeAttribute<int> edge_regions_attr("region", attributes, p);
	PointAttribute<int> point_regions_attr("region", attributes, p);

	// Duplicate edge to create bi-directional edge
	int n_edges = p.nedges();
	int offset = p.create_edges(n_edges);

	EdgeAttribute<int> new_edge_regions_attr(p);


	for (int i = 0; i < n_edges; i++) {
		int k = offset + i;
		p.vert(k, 0) = p.vert(i, 1);
		p.vert(k, 1) = p.vert(i, 0);
		new_edge_regions_attr[i] = edge_regions_attr[i];
		new_edge_regions_attr[k] = edge_regions_attr[i];
	}


	std::string geogram_out_filename = out_filename.string() + ".geogram";
	
	write_by_extension(geogram_out_filename, p, {{{"region", point_regions_attr.ptr}}, {{"region", new_edge_regions_attr.ptr}}});
	// write_by_extension(geogram_out_filename, p, attributes);
	std::cout << "writed to: " << geogram_out_filename << std::endl;

	// std::remove(out_filename.c_str());
	// std::cout << "remove: " << out_filename.string() << std::endl;

    return 0;
}