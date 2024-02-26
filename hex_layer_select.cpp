#include <ultimaille/all.h>
#include <param_parser/param_parser.h>
#include <optional>
#include <iterator>
#include <vector>

using namespace UM;

// Recursive method
void propagate_layer(CellAttribute<bool> &attr, Volume::Halfedge h) {

    attr[h.cell()] = true;
    std::cout << h.cell() << "->";

    auto opp0 = h.opposite_f().opposite_c();
    auto opp1 = h.next().next().opposite_f().opposite_c();
    auto opp2 = h.opposite_c();
    auto opp3 = h.opposite_f().next().next().opposite_f().opposite_c();

    auto h0 = opp0.opposite_f().next().next();
    auto h1 = opp1.opposite_f();
    auto h2 = opp2.opposite_f().next().next().opposite_f();
    auto h3 = opp3;

    if (opp0.active() && !attr[h0.cell()])
        propagate_layer(attr, h0);
    if (opp1.active() && !attr[h1.cell()])
        propagate_layer(attr, h1);
    if (opp2.active() && !attr[h2.cell()])
        propagate_layer(attr, h2);
    if (opp3.active() && !attr[h3.cell()])
        propagate_layer(attr, h3);
}

// Disjoint set method
void get_layer(DisjointSet & ds, Hexahedra & m) {
    for (auto h : m.iter_halfedges()) {
        ds.root(h);
    }

    for (auto h : m.iter_halfedges()) {
        if (h.opposite_c().active())
            ds.merge(h, h.opposite_c().opposite_f().next().next().opposite_f());

        if (h.next().next().opposite_f().opposite_c().active())
            ds.merge(h, h.next().next().opposite_f().opposite_c().opposite_f());
    }
}


int main(int argc, char** argv) {

    // Create parameters
    Parameters params;

    // Add program parameters
    params.add("input", "model", "").description("Model to process");
    params.add("int", "edge", "").description("Edge local index");
    params.add("string", "layer_attr_name", "layer").description("Output layer attribute name");

    /* Parse program arguments */
    params.init_from_args(argc, argv);

    std::string filename = params["model"];
    int e_idx = params["edge"];
    std::string layer_attr_name = params["layer_attr_name"];

    // Print
    std::cout << "Input model: " << filename << std::endl;

    // Open model
    Hexahedra m;
    VolumeAttributes attr = read_by_extension(filename, m);
    
    // Connect mesh
    m.connect();
    
    CellAttribute<bool> layer_attr(m, false);
    CellAttribute<bool> layers_attr_ds(m, false);

    // Get edge
    auto h = Volume::Halfedge(m, e_idx);

    // Extract layer
    propagate_layer(layer_attr, h);

    // // Find all layers
    // const int nhalfedges = m.ncells() * 24; 
    // DisjointSet ds(nhalfedges);
    // get_layer(ds, m);

    // std::vector<int> setIds(nhalfedges);
    // ds.get_sets_id(setIds);

    // const int groupId = setIds[h];

    // for (int j = 0; j < setIds.size(); j++) {
    //     if (setIds[j] == groupId) {
    //         Volume::Halfedge h(m, j);
    //         layers_attr_ds[h.cell()] = true;
    //     }
    // }

    std::cout << "end." << std::endl;

    // Keep old cell attributes except filter & lace
    std::vector<UM::NamedContainer> cells_attr;
    for (auto a : attr.cells) {
        if (a.first != layer_attr_name && a.first != "filter" && a.first != "selection")
            cells_attr.push_back(a);
    }
    cells_attr.push_back({layer_attr_name, layer_attr.ptr});
    cells_attr.push_back({"filter", layer_attr.ptr});
    cells_attr.push_back({"selection", layer_attr.ptr});
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
