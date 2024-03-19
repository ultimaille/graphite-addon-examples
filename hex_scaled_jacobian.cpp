#include <ultimaille/all.h>
#include <param_parser/param_parser.h>
#include <optional>
#include <iterator>
#include <vector>

using namespace UM;

int main(int argc, char** argv) {

    // Create parameters
    Parameters params;

    // Add program parameters
    params.add("input", "model", "").description("Model to process");

    /* Parse program arguments */
    params.init_from_args(argc, argv);

    // Print
    std::string filename = params["model"];
    std::cout << "Input model: " << filename << std::endl;

    // Open model
    Hexahedra m;
    VolumeAttributes attr = read_by_extension(filename, m);

    CellAttribute<double> scaled_jacobian_attr(m, 0.);

    for (auto c : m.iter_cells()) {
        scaled_jacobian_attr[c] = c.geom<Hexahedron>().scaled_jacobian();
    }

    std::cout << "end." << std::endl;

    // Output model
    if (!std::filesystem::is_directory("output"))
        std::filesystem::create_directories("output");

    std::string file = std::filesystem::path(filename).filename().string();
    
    std::string out_filename = "output/" + file;
    write_by_extension(out_filename, m, {{}, {{"scaled_jacobian", scaled_jacobian_attr.ptr}}, {}, {}});
    
    std::cout << "save model to " << out_filename << std::endl;

    return 0;
}
