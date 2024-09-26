#include <ultimaille/all.h>
#include <param_parser/param_parser.h>

using namespace UM;

int main(int argc, char** argv) {
    
    // Add program parameters
    // params.add("input", "model", "").description("Model to process");

    // params.add(Parameters::Type::EdgesInt(1), "edge_attribute", "").description("Edge");
    // params.add(Parameters::Type::FacetCornersInt(1), "facet_attribute", "").description("Facet");

    // /* Parse program arguments */
    // params.init_from_args(argc, argv);

    // // Get parameters
    // std::string filename = params["model"];
    // std::string facet_attribute = params["facet_attribute"];
    
    // std::filesystem::path result_path(params.result_path());

    // // Load mesh and read attributes
    // Quads m;
    // SurfaceAttributes attributes = read_by_extension(filename, m);
    // // Load attribute
    // CornerAttribute<int> ca(facet_attribute, attributes, m);

    // std::cout << "n corners: " << m.ncorners() << std::endl;

    // for (int i = 0; i < m.ncorners(); i++) {
    //     std::cout << "ca: " << ca[i] << std::endl;
    // }

    // // Save

    // // Output model to output directory at working dir if no result_path given
    // if (result_path.empty() && !std::filesystem::is_directory("output")) {
    //     std::filesystem::create_directories("output");
    //     result_path = "output";
    // }

    // // Get file name and output path
    // std::string file = std::filesystem::path(filename).filename().string();
    // std::string out_filename = (result_path / file).string();

    // write_by_extension(out_filename, m, {{}, {}, {{"ca", ca.ptr}}});
    
    return 0;
}
