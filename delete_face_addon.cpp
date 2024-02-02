#include <ultimaille/all.h>
#include <param_parser/param_parser.h>

using namespace UM;

int main(int argc, char** argv) {

    // Create parameters
    Parameters params;

    // Add program parameters
    params.add("input", "model", "").description("Model to process");
    params.add("int", "facet_index", "0").description("Facet index to remove");

    /* Parse program arguments */
    params.init_from_args(argc, argv);

    // Print
    std::string s = params["model"];
    std::cout << "Input model: " << s << std::endl;

    std::string filename = params["model"];
    int i = params["facet_index"];

    // Open model
    Triangles m;
    read_by_extension(filename, m);

    // Delete facet
    std::vector<bool> deleted_facets(m.nfacets());
    deleted_facets[i] = true;
    m.delete_facets(deleted_facets);

    std::cout << "Deleted facet: " << i << std::endl;

    // Output model
    if (!std::filesystem::is_directory("output"))
        std::filesystem::create_directories("output");

    std::string file = std::filesystem::path(filename).filename().string();
    
    std::string out_filename = "output/" + file;
    write_by_extension(out_filename, m);
    
    return 0;
}
