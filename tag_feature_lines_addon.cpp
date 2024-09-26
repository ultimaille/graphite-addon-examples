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

	params.help = "Tag feature lines of a surface + polyline model";

	// Add program parameters
	params.add(Parameters::Type::Input, "model", "").description("");

	/* Parse program arguments */
	params.init_from_args(argc, argv);

	// Get args
	std::string model = params["model"];
	std::filesystem::path result_path(params.result_path());

	// Out file
	std::filesystem::path model_path(model);
	
	std::filesystem::path out_filename = result_path / (model_path.filename().string() + "_tagged.geogram");

	// Read surface
	Triangles m;
	read_by_extension(model, m);
	CornerAttribute<bool> feature_line_attr(m);

	// Read poyline
	PolyLine p;
	PolyLineAttributes attributes = read_by_extension(model, p);

	m.connect();

	// Mark halfedge as feature line
	// May have a smarter way ?
	for (auto e : p.iter_edges()) {
		for (auto he : m.iter_halfedges()) {

			if (e.from() == he.from() && e.to() == he.to()) {
				feature_line_attr[he] = true;
				break;
			}
		}
	}

	write_by_extension(out_filename.string(), m, {{}, {}, {{"feature_line", feature_line_attr.ptr}}});
	std::cout << "writed to: " << out_filename << std::endl;

	return 0;
}