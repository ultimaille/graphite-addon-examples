#include <ultimaille/all.h>
#include <param_parser/param_parser.h>
#include <optional>
#include <iterator>
#include <vector>

using namespace UM;

void _propagate(CellAttribute<bool> & visited, Volume::Halfedge & h, void (*f)(Volume::Halfedge h)) {

    if (f != nullptr)
        f(h);
    
    visited[h.cell()] = true;
    std::cout << h.cell() << "->";

    auto opp0 = h.opposite_f().opposite_c();
    auto opp1 = h.next().next().opposite_f().opposite_c();
    auto opp2 = h.opposite_c();
    auto opp3 = h.opposite_f().next().next().opposite_f().opposite_c();

    auto h0 = opp0.opposite_f().next().next();
    auto h1 = opp1.opposite_f();
    auto h2 = opp2.opposite_f().next().next().opposite_f();
    auto h3 = opp3;

    if (opp0.active() && !visited[h0.cell()])
        _propagate(visited, h0, f);
    if (opp1.active() && !visited[h1.cell()])
        _propagate(visited, h1, f);
    if (opp2.active() && !visited[h2.cell()])
        _propagate(visited, h2, f);
    if (opp3.active() && !visited[h3.cell()])
        _propagate(visited, h3, f);
}

void propagate(Hexahedra & m, Volume::Halfedge & start_h, void (*f)(Volume::Halfedge h)) {
    CellAttribute<bool> visited(m, false);
    _propagate(visited, start_h, f);
}

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

    /* Parse program arguments */
    params.init_from_args(argc, argv);

    // Print
    std::string s = params["model"];
    std::cout << "Input model: " << s << std::endl;

    std::string filename = params["model"];
    int e_idx = params["edge"];

    // Open model
    Hexahedra m;
    VolumeAttributes attr = read_by_extension(filename, m);

    // Connect mesh
    m.connect();
    
    CellAttribute<bool> layer_attr(m, false);
    CellAttribute<int> layers_0(m, false);

    // Generate indirection table
    // int deduce_c_idx = 23616;
    // he, vert
    // int a[12][2] = {{37351,37353},{37353,37352},{37352,37350},{37350,37351},{2732,6275},{6275,6276},{6276,5791},{5791,2732},{37351,2732},{37353,6275},{37352,6276},{37350,5791}};
    // Volume::Cell deduce_c(m, deduce_c_idx);

    // std::vector<int> bob(12);
    // for (int hi = 0; hi < deduce_c.nhalfedges(); hi ++) {
    //     auto h = deduce_c.halfedge(hi);
    //     for (int i = 0; i < 12; i++) {
    //         if ((a[i][0] == h.from() && a[i][1] == h.to()) || (a[i][1] == h.from() && a[i][0] == h.to())) {
    //             std::cout << a[i][0] << "," << a[i][1] << ", geogram local h:" << i << "ultimaille he:" << hi << std::endl;

    //             bob[i] = hi;
    //         }
    //     }
    // }

    // std::cout << "{";
    // for (int i = 0; i < bob.size(); i++) {
    //     std::cout << bob[i] << ", ";
    // }
    // std::cout << "}" << std::endl;


    // Get edge
    auto h = Volume::Halfedge(m, e_idx);

    // Extract layer
    propagate_layer(layer_attr, h);


    // Find all layers
    const int nhalfedges = m.ncells() * 24; 
    DisjointSet ds(nhalfedges);
    get_layer(ds, m);

    std::vector<int> setIds(nhalfedges);

    ds.get_sets_id(setIds);


    std::cout << ds.nsets() << std::endl;

    bool ok[ds.nsets()] = {false};

    
    int ngroup = 0;
    for (int i = 0; i < setIds.size(); i++) {
        int groupId = setIds[i];
        if (ok[groupId])
            continue;
        
        ngroup++;
        ok[groupId] = true;

        for (int j = 0; j < setIds.size(); j++) {
            if (setIds[j] == groupId) {
                Volume::Halfedge h(m, j);
                layers_0[h.cell()] = groupId;
            }
        }
    }

    std::cout << "n group: " << ngroup << std::endl;
    
    for (int groupId = 0; groupId < ds.nsets(); groupId++) {
        for (int j = 0; j < setIds.size(); j++) {
            if (setIds[j] == groupId) {
                Volume::Halfedge h(m, j);
                layers_0[h.cell()] = groupId;
            }
        }
    }





    std::cout << "end." << std::endl;


    // Output model
    if (!std::filesystem::is_directory("output"))
        std::filesystem::create_directories("output");

    std::string file = std::filesystem::path(filename).filename().string();
    
    std::string out_filename = "output/" + file;
    write_by_extension(out_filename, m, {{}, {{"layer", layer_attr.ptr}, {"layer_0", layers_0.ptr}}, {}, {}});
    
    std::cout << "save model to " << out_filename << std::endl;


    return 0;
}
