#include <ultimaille/all.h>
#include <param_parser/param_parser.h>
#include <optional>
#include <iterator>
#include <vector>

using namespace UM;

// Propagate an hex lace following normal of a given facet
void _lace_propagate(CellAttribute<bool> & visited, Volume::Facet start_f, std::optional<std::function<void(Volume::Facet, int)>> f) {

    int i = -1;
    auto cur_f = start_f;

    while (cur_f.cell().active() && !visited[cur_f.cell()]) {
        visited[cur_f.cell()] = true;
        if (f.has_value())
            f.value()(cur_f, i++);
        
        if (!cur_f.opposite().active())
            break;
        
        // Next face
        cur_f = cur_f.opposite().halfedge(0).opposite_f().next().next().opposite_f().facet();
    }

    cur_f = start_f.halfedge(0).opposite_f().next().next().opposite_f().opposite_c().facet();

    while (cur_f.active() && cur_f.cell().active() && !visited[cur_f.cell()]) {
        visited[cur_f.cell()] = true;
        if (f.has_value())
            f.value()(cur_f, i++);
        
        if (!cur_f.opposite().active())
            break;
        
        // Next face
        cur_f = cur_f.halfedge(0).opposite_f().next().next().opposite_f().facet().opposite();
    }
}

// Propagate an hex lace following normal of a given facet
void lace_propagate(Hexahedra & m, Volume::Facet start_f, std::function<void(Volume::Facet, int)> f) {

    um_assert(m.connected());
    CellAttribute<bool> visited(m, false);
    _lace_propagate(visited, start_f, f);
}

int main(int argc, char** argv) {

    // Create parameters
    Parameters params;

    // Add program parameters
    params.add("input", "model", "").description("Model to process");
    params.add("int", "edge", "").description("Edge global index");

    /* Parse program arguments */
    params.init_from_args(argc, argv);

    std::string filename = params["model"];
    int he_idx = params["edge"];

    // Print
    std::cout << "Input model: " << filename << std::endl;    

    // Open model
    Hexahedra m;
    VolumeAttributes attr = read_by_extension(filename, m);

    // Connect mesh
    m.connect();
    
    CellAttribute<bool> lace_attr(m, false);

    // Get facet from he
    auto he = Volume::Halfedge(m, he_idx);
    auto f = he.facet();

    // Extract lace
    _lace_propagate(lace_attr, f, [](auto f, int _) {
        std::cout << f << "->";
    });

    std::cout << "end." << std::endl;

    // Keep old cell attributes except filter & lace
    std::vector<UM::NamedContainer> cells_attr;
    for (auto a : attr.cells) {
        if (a.first != "lace" && a.first != "filter")
            cells_attr.push_back(a);
    }
    cells_attr.push_back({"lace", lace_attr.ptr});
    cells_attr.push_back({"filter", lace_attr.ptr});
    attr.cells = cells_attr;

    // Output model
    if (!std::filesystem::is_directory("output"))
        std::filesystem::create_directories("output");

    std::string file = std::filesystem::path(filename).filename().string();
    
    std::string out_filename = "output/" + file;
    write_by_extension(out_filename, m, attr);
    
    std::cout << "save model to " << out_filename << std::endl;

    return 0;
}
