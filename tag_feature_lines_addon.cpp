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
	CornerAttribute<bool> is_feature_attr(m);

	// Read poyline
	PolyLine p;
	PolyLineAttributes attributes = read_by_extension(model, p);

	// Duplicate edges to create bi-directional edge
	int n_edges = p.nedges();
	int offset = p.create_edges(n_edges);

	for (int i = 0; i < n_edges; i++) {
		int k = offset + i;
		p.vert(k, 0) = p.vert(i, 1);
		p.vert(k, 1) = p.vert(i, 0);
	}

	m.connect();
	p.connect();


	// Mark halfedge as feature line
	// May have a smarter way ?
	for (auto v : m.iter_vertices()) {
		PolyLine::Vertex vp(p, v);

		for (auto he : v.iter_halfedges()) {
			for (auto e : vp.iter_edges()) {
				if (he.to() == e.to())
					is_feature_attr[he] = true;
			}
		}

	}

	write_by_extension(out_filename.string(), m, {{}, {}, {{"is_feature", is_feature_attr.ptr}}});
	std::cout << "writed to: " << out_filename << std::endl;

	return 0;
}