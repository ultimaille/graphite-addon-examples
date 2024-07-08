#include <ultimaille/all.h>
#include <param_parser/param_parser.h>

using namespace UM;

int main(int argc, char** argv) {

    // Create parameters
    Parameters params;

    params.help = "This addon delete a facet";

    // Add program parameters
    params.add("input", "model", "").description("Model to process");
    params.add("int", "facet_index", "0").description("Facet index to remove");
    // Just an example on how to use enum
    params.add("string", "example", "x").possible_values("x,y,z").visible("true").type_of_param("system");

    /* Parse program arguments */
    params.init_from_args(argc, argv);

    if (params.has_run_from())
        std::cout << "Run from: " << params.run_from() << std::endl;
    else 
        std::cout << "Standalone mode." << std::endl;

    // Get parameters
    std::string s = params["model"];
    std::string filename = params["model"];
    int i = params["facet_index"];

    // Print info
    std::cout << "Input model: " << s << std::endl;

    std::filesystem::path result_path(params.result_path());

    // Open model
    Triangles m;
    read_by_extension(filename, m);

    // Delete facet
    std::vector<bool> deleted_facets(m.nfacets());
    deleted_facets[i] = true;
    m.delete_facets(deleted_facets);

    std::cout << "Deleted facet: " << i << std::endl;

    // Output model to output directory at working dir if no result_path given
    if (result_path.empty() && !std::filesystem::is_directory("output")) {
        std::filesystem::create_directories("output");
        result_path = "output";
    }

    // Get file name and output path
    std::string file = std::filesystem::path(filename).filename().string();
    std::string out_filename = (result_path / file).string();

    write_by_extension(out_filename, m);
    
    return 0;
}
