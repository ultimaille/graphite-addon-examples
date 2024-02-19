#include <ultimaille/all.h>
#include <param_parser/param_parser.h>
#include <optional>
#include <iterator>
#include <vector>
#include <queue>

using namespace UM;

// Propagate an hex layer perpendicular to a given halfedge
void bfs_cell_propagate(Hexahedra & m, Volume::Halfedge start_h, std::function<void(Volume::Halfedge, int)> f) {

    um_assert(m.connected());
    CellAttribute<bool> visited(m, false);

    std::queue<int> q;
    std::queue<int> d;
    q.push(start_h);
    d.push(0);
    visited[start_h.cell()] = true;

    while (!q.empty()) {

        int depth = d.front();
        d.pop();        
        
        Volume::Halfedge h(m, q.front());
        q.pop();
        f(h, depth);

        auto opp0 = h.opposite_f().opposite_c();
        auto opp1 = h.next().next().opposite_f().opposite_c();
        auto opp2 = h.opposite_c();
        auto opp3 = h.opposite_f().next().next().opposite_f().opposite_c();

        auto h0 = opp0.opposite_f().next().next();
        auto h1 = opp1.opposite_f();
        auto h2 = opp2.opposite_f().next().next().opposite_f();
        auto h3 = opp3;

        if (opp0.active() && !visited[h0.cell()]) {
            q.push(h0);
            d.push(depth + 1);
            visited[h0.cell()] = true;
        }
        if (opp1.active() && !visited[h1.cell()]) {
            q.push(h1);
            d.push(depth + 1);
            visited[h1.cell()] = true;
        }
        if (opp2.active() && !visited[h2.cell()]) {
            q.push(h2);
            d.push(depth + 1);
            visited[h2.cell()] = true;
        }
        if (opp3.active() && !visited[h3.cell()]) {
            q.push(h3);
            d.push(depth + 1);
            visited[h3.cell()] = true;
        }

    }
}

void _propagate(CellAttribute<bool> & visited, Volume::Halfedge & h, std::function<void(Volume::Halfedge&)> f) {

    if (f != nullptr)
        f(h);
    
    visited[h.cell()] = true;

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

void propagate(Hexahedra & m, Volume::Halfedge & start_h, std::function<void(Volume::Halfedge&)> f) {
    um_assert(m.connected());
    CellAttribute<bool> visited(m, false);
    _propagate(visited, start_h, f);
}

vec3 middle(vec3 & a, vec3 & b) {
    return a + (b - a) * .5;
}

int main(int argc, char** argv) {

    // Create parameters
    Parameters params;

    // Add program parameters
    params.add("input", "model", "").description("Model to process");
    params.add(Parameters::Type::CellsBool(1), "layer", "layer").description("Layer attribute");

    /* Parse program arguments */
    params.init_from_args(argc, argv);

    // Print
    std::string s = params["model"];
    std::cout << "Input model: " << s << std::endl;

    std::string filename = params["model"];
    std::string layer_attr_name = params["layer"];

    // Open model
    Hexahedra m;
    VolumeAttributes attr = read_by_extension(filename, m);
    // Read layer attribute
    CellAttribute<bool> layer_attr(layer_attr_name, attr, m);

    Volume::Halfedge selected_h(m, 10);



    m.connect();


    int n_cells = 0;
    bfs_cell_propagate(m, selected_h, [&n_cells](auto h, int _) {
        n_cells++;
    });

    // Split !

    std::cout << "ncells: " << m.ncells() << std::endl;
    
    const int n_new_cells = n_cells * 2;
    const int off_c = m.create_cells(n_new_cells);
    const int off_v = m.points.create_points(n_new_cells * 8);
    int off_v_l = n_new_cells * 8 / 2;

    std::cout << "ncells: " << m.ncells() << std::endl;
    
    int v_id = 0, c_id = 0;
    std::vector<bool> to_kill(m.ncells());



    bfs_cell_propagate(m, selected_h, [&](Volume::Halfedge h, int depth) {

        // Get half-edges mids
        auto h1 = h.next().next();
        auto h2 = h1.opposite_f().next().next().opposite_f();
        auto h3 = h2.next().next();
        

        std::array ha = {h, h1, h2, h3};

        auto c = h.cell();
        
        for (int lh = 0; lh < ha.size(); lh++) {
            // auto cur_h = c.halfedge(lh);
            
            auto cur_h = ha[lh];
            vec3 mid = middle(cur_h.from().pos(), cur_h.to().pos());
            
            int flc = cur_h.from_corner().id_in_cell();
            int tlc = cur_h.to_corner().id_in_cell();

            auto a = cur_h.from().pos();
            auto b = mid;
            if (lh % 2 == 1) { 
                a = mid;
                b = cur_h.to().pos();
            }

            auto c = cur_h.to().pos();
            auto d = mid;
            if (lh % 2 == 1) { 
                c = mid;
                d = cur_h.from().pos();
            }

            // First layer
            m.points[off_v + (c_id * 8) + flc] = a;
            m.points[off_v + (c_id * 8) + tlc] = b;

            m.vert(off_c + c_id, flc) = off_v + (c_id * 8) + flc;
            m.vert(off_c + c_id, tlc) = off_v + (c_id * 8) + tlc;

            // Second layer
            
            m.points[off_v + off_v_l + (c_id * 8) + flc] = c;
            m.points[off_v + off_v_l + (c_id * 8) + tlc] = d;

            m.vert(off_c + c_id + n_cells, flc) = off_v + off_v_l + (c_id * 8) + flc;
            m.vert(off_c + c_id + n_cells, tlc) = off_v + off_v_l + (c_id * 8) + tlc;       
        }
        c_id++;

        to_kill[c] = true;
    });

    m.delete_cells(to_kill);

    m.disconnect();
    m.connect();
    CellAttribute<int> depth_attr(m);
    bfs_cell_propagate(m, selected_h, [&](Volume::Halfedge h, int depth) {
        depth_attr[h.cell()] = depth;
    });

    // Output model
    if (!std::filesystem::is_directory("output"))
        std::filesystem::create_directories("output");

    std::string file = std::filesystem::path(filename).filename().string();
    
    std::string out_filename = "output/" + file;
    write_by_extension(out_filename, m, {{}, {{"layer", layer_attr.ptr}, {"depth", depth_attr.ptr}}, {}, {}});
    
    std::cout << "save model to " << out_filename << std::endl;


    return 0;
}
