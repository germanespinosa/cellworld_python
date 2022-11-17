#include <pybind11/pybind11.h>
#include <cellworld_python.h>
#include <cell_world/resources.h>
#include <cell_world/coordinates.h>
#include <cell_world/location.h>
#include <cell_world/transformation.h>
#include <cell_world/cell.h>
#include <cell_world/shape.h>
#include <cell_world/world.h>
#include <cell_world/geometry.h>
#include <cell_world/cell_group.h>
#include <cell_world/graph.h>
#include <cell_world/map.h>
#include <cell_world/timer.h>
#include <cell_world/experiment.h>

using namespace pybind11;
using namespace json_cpp;
using namespace std;
using namespace cell_world;

struct Python_json_object :  Json_builder {

    vector<void*> pointers;

    template<typename T>
    T& create_member(T &v){
        void* pointer = new T();
        pointers.push_back(pointer);
        T *t_pointer = (T *)pointer;
        *t_pointer = v;
        return *t_pointer;
    }

    ~Python_json_object(){
        for(auto pointer:pointers){
            free(pointer);
        }
    }
};

PYBIND11_MODULE(cellworld2_core, m)
{

    json_vector_binding<int>(m,"Int_list");
    json_vector_binding<bool>(m,"Bool_list");
    json_vector_binding<unsigned int>(m,"Unsigned_int_list");
    json_vector_binding<float>(m,"Float_list");

    json_object_binding<Location>(m,"Location")
            .def(pybind11::init<float,float>())
            .def("mod",  &Location::mod)
            .def("move",  &Location::move)
            .def("transform",  &Location::transform)
            .def("atan",+[](const Location &l){return l.atan();})
            .def("atan",+[](const Location &l, Location &l2){return l.atan(l2);})
            .def("dist",+[](const Location &l){return l.dist();})
            .def("dist",+[](const Location &l,const Location &l2){return l.dist(l2);})
            .def("dist",+[](const Location &l, const Location &l2,const Location &l3){return l.dist(l2,l3);})
            .def("transformation",+[](const Location &l){return l.dist();})
            .def("transformation",+[](const Location &l,const Location &l2){return l.dist(l2);})
            .def(-self)
            .def(self == self)
            .def(self != self)
            .def(self - self)
            .def(self + self)
            .def(self - self)
            .def(self * self)
            .def(self * float())
            .def(self / float())
            ;
    json_vector_binding<Location>(m,"Location_list");

    json_object_binding<Coordinates>(m,"Coordinates")
            .def(pybind11::init<int,int>());

    json_vector_binding<Coordinates>(m,"Coordinates_list");

    json_object_binding<Transformation>(m,"Transformation")
            .def(init<float, float>())
            .def_readwrite("size", &Transformation::size)
            .def_readwrite("rotation", &Transformation::rotation)
            .def("theta",&Transformation::theta)
            ;

    json_object_binding<Cell>(m,"Cell")
            .def(init<const Coordinates &>())
            .def(init<Coordinates, Location, bool>())
            .def(init<const Cell &>())
            .def_readwrite("id", &Cell::id)
            .def_readwrite("coordinates", &Cell::coordinates)
            .def_readwrite("location", &Cell::location)
            .def_readwrite("occluded", &Cell::occluded)
            .def(self == Cell())
            .def(self != Cell())
            ;

    json_vector_binding<Cell>(m,"Cell_list");

    json_object_binding<Shape>(m,"Shape")
            .def(init<int>())
            .def_readwrite("sides", &Shape::sides)
            ;

    json_object_binding<Polygon>(m,"Polygon")
            .def(init<const Polygon &>())
            .def(init<const Location, unsigned int, float, float >())
            .def(init<const Location &,const Shape &, const Transformation &>())
            .def_readwrite("vertices", &Polygon::vertices)
            .def_readwrite("center", &Polygon::center)
            .def_readwrite("radius", &Polygon::radius)
            .def("contains", +[](const Polygon &p, const Location &l){return p.contains(l);})
            .def("contains", +[](const Polygon &p, const Polygon &l){return p.contains(l);})
            .def("overlaps", &Polygon::overlaps)
            .def("move",  +[](const Polygon &p, const Location &l){return p.move(l);})
            .def("move",  +[](const Polygon &p, float t, float d){return p.move(t,d);})
            .def(self += Location())
            ;

    json_object_binding<Polygon_list>(m,"Polygon_list")
            .def(init<>())
            .def(init<const Location_list &, const Shape &, const Transformation &>())
            .def("__getitem__", &Polygon_list::get_item_at)
            .def("__setitem__", &Polygon_list::set_item_at)
            .def("contains", &Polygon_list::contains)
            ;

    json_object_binding<Space>(m,"Space")
            .def(init<const Location &, const Shape &, const Transformation &>())
            .def_readwrite("center", &Space::center)
            .def_readwrite("shape", &Space::shape)
            .def_readwrite("transformation", &Space::transformation)
            .def("transform", &Space::transform)
            .def("scale", &Space::scale)
            ;

    json_object_binding<World_info>(m,"World_info")
            .def_readwrite("world_configuration", &World_info::world_configuration)
            .def_readwrite("world_implementation", &World_info::world_implementation)
            .def_readwrite("occlusions", &World_info::occlusions)
            ;

    json_object_binding<World_configuration>(m,"World_configuration")
            .def(init<const World_configuration &>())
            .def(init<const Shape &, const Coordinates_list &, const Connection_pattern &>())
            .def_readwrite("cell_shape", &World_configuration::cell_shape)
            .def_readwrite("cell_coordinates", &World_configuration::cell_coordinates)
            .def_readwrite("connection_pattern", &World_configuration::connection_pattern)
            .def_static("get_from_parameters_name",+[](const string &name){
                object requests = module_::import("cellworld2");
                object requests_get = requests.attr("get_resource");
                World_configuration wc;
                str response = requests_get("world_configuration",name);
                wc.from_json(response);
                return wc;
            })
            ;

    json_object_binding<World_implementation>(m,"World_implementation")
            .def(init<const World_implementation &>())
            .def(init<const Location_list &, const Space &, const Transformation &>())
            .def_readwrite("cell_locations", &World_implementation::cell_locations)
            .def_readwrite("space", &World_implementation::space)
            .def_readwrite("cell_transformation", &World_implementation::cell_transformation)
            .def("transform",&World_implementation::transform)
            .def("scale",&World_implementation::scale)
            .def_static("get_from_parameters_name",+[](const string &configuration, const string &name){
                object requests = module_::import("cellworld2");
                object requests_get = requests.attr("get_resource");
                World_implementation wc;
                str response = requests_get("world_implementation", configuration, name);
                wc.from_json(response);
                return wc;
            })
            ;

    json_object_binding<World_statistics>(m,"World_statistics")
            .def_readwrite("spatial_entropy", &World_statistics::spatial_entropy)
            .def_readwrite("spatial_espinometry", &World_statistics::spatial_espinometry)
            .def_readwrite("spatial_connections", &World_statistics::spatial_connections)
            .def_readwrite("spatial_connections_derivative", &World_statistics::spatial_connections_derivative)
            .def_readwrite("spatial_centrality", &World_statistics::spatial_centrality)
            .def_readwrite("spatial_centrality_derivative", &World_statistics::spatial_centrality_derivative)
            .def_readwrite("visual_entropy", &World_statistics::visual_entropy)
            .def_readwrite("visual_espinometry", &World_statistics::visual_espinometry)
            .def_readwrite("visual_connections", &World_statistics::visual_connections)
            .def_readwrite("visual_connections_derivative", &World_statistics::visual_connections_derivative)
            .def_readwrite("visual_centrality", &World_statistics::visual_centrality)
            .def_readwrite("visual_centrality_derivative", &World_statistics::visual_centrality_derivative)
            .def_readwrite("ITOR_potential", &World_statistics::ITOR_potential)
            .def_readwrite("ITOR_direction", &World_statistics::ITOR_direction)
            .def_static("get_from_parameters_name",+[](const string &configuration, const string &name){
                object requests = module_::import("cellworld2");
                object requests_get = requests.attr("get_resource");
                World_statistics wc;
                str response = requests_get("world_statistics", configuration, name);
                wc.from_json(response);
                return wc;
            })
            ;

    m.def("angle_difference", &angle_difference);
    m.def("direction", &direction);
    m.def("to_radians", &to_radians);
    m.def("to_degrees", &to_degrees);
    m.def("normalize", &normalize);
    m.def("normalize_degrees", &normalize_degrees);
    m.def("segments_intersect", &segments_intersect);
    m.def("angle_between", &angle_between);

    json_object_binding<Cell_group_builder>(m,"Cell_group_builder")
            .def("__getitem__", &Cell_group_builder::get_item_at)
            .def("__setitem__", &Cell_group_builder::set_item_at)
            .def_static("get_from_parameters_name",+[](const string &configuration, const string &occlusions, const string &name){
                object requests = module_::import("cellworld2");
                object requests_get = requests.attr("get_resource");
                Cell_group_builder wc;
                str response = requests_get("cell_group", configuration, occlusions, name);
                wc.from_json(response);
                return wc;
            })
            .def_static("get_from_parameters_name",+[](const string &configuration, const string &name){
                object requests = module_::import("cellworld2");
                object requests_get = requests.attr("get_resource");
                Cell_group_builder wc;
                str response = requests_get("cell_group", configuration, name);
                wc.from_json(response);
                return wc;
            })
            ;

    json_object_binding<Cell_group>(m,"Cell_group")
            .def(init<const cell_world::Cell_group&>())
            .def("__getitem__", +[](const Cell_group& cg, size_t i){return cg[i];})
            .def("contains", +[](const Cell_group& cg, unsigned int i){return cg.contains(i);})
            .def("contains", +[](const Cell_group& cg, const Cell &c){return cg.contains(c);})
            .def("clear", &Cell_group::clear)
            .def("find", +[](const Cell_group& cg, unsigned int i){return cg.find(i);})
            .def("find", +[](const Cell_group& cg, const Cell &c){return cg.find(c);})
            .def("find", +[](const Cell_group& cg, const Location &l){return cg.find(l);})
            .def("find", +[](const Cell_group& cg, const Coordinates &c){return cg.find(c);})
            .def("get_distances", &Cell_group::get_distances)
            .def("random_shuffle", &Cell_group::random_shuffle)
            .def("occluded_cells", &Cell_group::occluded_cells)
            .def("free_cells", &Cell_group::free_cells)
            .def("get_builder", &Cell_group::get_builder)
            .def("random_cell", &Cell_group::get_builder)
            .def(self + Cell_group())
            .def(self - Cell_group())
            .def(self - Cell())
            .def(self + Cell())
            .def(self == Cell_group())
            .def(self != Cell_group())
            ;

    json_object_binding<Graph>(m,"Graph")
            .def(init<const cell_world::Cell_group&>())
            .def(init<const cell_world::Graph&>())
            .def("add", +[](Graph& g, const Cell&c){return g.add(c);})
            .def("add", +[](Graph& g, const Cell_group &cg){return g.add(cg);})
            .def("connect",  +[](Graph& g, const Cell& c, const Cell_group &cg){return g.connect(c, cg);})
            .def("connect",  +[](Graph& g, const Graph& cg){return g.connect(cg);})
            .def("get_connectors", &Graph::get_connectors)
            .def("get_entropy", &Graph::get_entropy)
            .def("invert", &Graph::invert)
            .def("get_shortest_path", &Graph::get_shortest_path)
            .def("get_centrality", &Graph::get_centrality)
            .def("is_connected", &Graph::is_connected)
            .def(!self)
            .def(self == Graph())
            .def("__getitem__", +[](const Graph& cg, size_t i){return cg[i];})
            .def("__getitem__", +[](const Graph& cg, const Cell &c){return cg[c.id];})
            ;

    json_object_binding<World>(m,"World")
            .def(init<const World_configuration &>())
            .def(init<const World_configuration &, const World_implementation &>())
            .def(init<const World_configuration &, const World_implementation &, const Cell_group_builder &>())
            .def_readwrite("connection_pattern", &World::connection_pattern)
            .def_readwrite("cells", &World::cells)
            .def_readwrite("cell_shape", &World::cell_shape)
            .def_readwrite("cell_transformation", &World::cell_transformation)
            .def("add", +[](World& g, Cell c){return g.add(c);})
            .def("add", +[](World& g, Coordinates c){return g.add(c);})
            .def("create_cell_group", +[](World& g){return g.create_cell_group();})
            .def("create_cell_group", +[](World& g, const Cell_group_builder &cgb){return g.create_cell_group(cgb);})
            .def("create_cell_group", +[](World& g, const std::string &cgb){return g.create_cell_group(cgb);})
            .def("create_graph", +[](World& g){return g.create_graph();})
            .def("create_graph", +[](World& g, const Graph_builder &cgb){return g.create_graph(cgb);})
            .def("create_paths", &World::create_paths)
            .def("set_occlusions", &World::set_occlusions)
            .def("size", &World::size)
            .def("create_location_visibility", &World::create_location_visibility)
            .def("get_configuration", &World::get_configuration)
            .def("get_implementation", &World::get_implementation)
            .def("get_statistics", &World::get_statistics)
            .def_static("get_from_parameters_name",+[](const string &configuration, const string &implementation){
                object requests = module_::import("cellworld2");
                object requests_get = requests.attr("get_resource");
                World_configuration wc;
                World_implementation wi;
                Cell_group_builder o;
                str response = requests_get("world_configuration", configuration);
                wc.from_json(response);
                response = requests_get("world_implementation", configuration, implementation);
                wi.from_json(response);
                return World(wc, wi);
            })
            .def_static("get_from_parameters_name",+[](const string &configuration, const string &implementation, const string &name){
                object requests = module_::import("cellworld2");
                object requests_get = requests.attr("get_resource");
                World_configuration wc;
                World_implementation wi;
                Cell_group_builder o;
                str response = requests_get("world_configuration", configuration);
                wc.from_json(response);
                response = requests_get("world_implementation", configuration, implementation);
                wi.from_json(response);
                response = requests_get("cell_group", configuration, name, "occlusions");
                o.from_json(response);
                return World(wc, wi, o);
            })
            .def_static("get_from_world_info",+[](const World_info &world_info){
                object requests = module_::import("cellworld2");
                object requests_get = requests.attr("get_resource");
                World_configuration wc;
                World_implementation wi;
                Cell_group_builder o;
                str response = requests_get("world_configuration", world_info.world_configuration);
                wc.from_json(response);
                response = requests_get("world_implementation", world_info.world_configuration, world_info.world_implementation);
                wi.from_json(response);
                response = requests_get("cell_group", world_info.world_configuration, world_info.occlusions);
                o.from_json(response);
                return World(wc, wi, o);
            })
            ;

    class_<Map>(m, "Map")
            .def(init<const Cell_group &>())
            .def("__getitem__", +[](const Map& m, const Coordinates &c){return m[c];})
            .def("__getitem__", +[](const Map& m, int x, int y){return m[Coordinates(x,y)];})
            .def("find", &Map::find)
            .def_readwrite("cells", &Map::cells)
            ;

    json_object_binding<Path_builder>(m,"Path_builder")
            .def_readwrite("moves", &Path_builder::moves)
            .def_readwrite("steps", &Path_builder::steps)
            .def_static("get_from_parameters_name",+[](const string &configuration, const string &occlusions, const string &name){
                object requests = module_::import("cellworld2");
                object requests_get = requests.attr("get_resource");
                Path_builder wc;
                str response = requests_get("paths", configuration, occlusions, name);
                wc.from_json(response);
                return wc;
            })
            ;

    json_object_binding<Paths>(m,"Paths")
            .def(init<const Graph&, Move_list >())
            .def(init<const Graph&, const Path_builder &>())
            .def("get_move", &Paths::get_move)
            .def("get_steps", &Paths::get_steps)
            .def("get_steps", &Paths::get_steps)
            .def("get_path", &Paths::get_path)
            .def("get_moves", &Paths::get_moves)
            .def("set_move", &Paths::set_move)
            .def("__getitem__", +[](const Map& m, const Coordinates &c){return m[c];})
            .def("__getitem__", +[](const Map& m, int x, int y){return m[Coordinates(x,y)];})
            .def_readwrite("cells", &Paths::cells)
            .def_readwrite("moves", &Paths::moves)
            .def_readwrite("steps", &Paths::steps)
            .def("get_euclidean",  &Paths::get_euclidean)
            .def("get_manhattan",  &Paths::get_manhattan)
            .def("get_astar",  &Paths::get_astar)
            ;

    class_<Timer>(m, "Timer")
            .def(init<>())
            .def(init<float>())
            .def("reset", &Timer::reset)
            .def("to_seconds", &Timer::to_seconds)
            .def("time_out", &Timer::time_out)
            .def("wait", &Timer::wait)
            ;

    class_<Coordinates_visibility>(m, "Coordinates_visibility")
            .def("create_graph", &Coordinates_visibility::create_graph)
            .def("invert", &Coordinates_visibility::invert)
            ;

    class_<Coordinates_visibility_cone>(m,"Coordinates_visibility_cone")
            .def(init<const Graph &, float>())
            .def("visible_cells", &Coordinates_visibility_cone::visible_cells)
            .def("is_visible", &Coordinates_visibility_cone::is_visible)
            ;

    class_<Location_visibility>(m, "Location_visibility")
            .def(init<const Shape &, const Transformation &>())
            .def(init<const Cell_group &, const Shape &, const Transformation &>())
            .def("is_visible", +[](const Location_visibility& lv, const Location &src, const Location &dst){ return lv.is_visible(src, dst);})
            .def("is_visible", +[](const Location_visibility& lv, const Location &src, float src_theta, float src_cone, const Location &dst){ return lv.is_visible(src, src_theta, src_cone, dst);})
            .def("is_visible_multi", &Location_visibility::is_visible_multi)
            ;

    json_object_binding<Step>(m,"Step")
            .def_readwrite("time_stamp", &Step::time_stamp)
            .def_readwrite("agent_name", &Step::agent_name)
            .def_readwrite("frame", &Step::frame)
            .def_readwrite("location", &Step::location)
            .def_readwrite("rotation", &Step::rotation)
            .def_readwrite("data", &Step::data)
            .def("convert", &Step::convert)
            .def(self == self)
            ;


    json_vector_binding<Step>(m,"Trajectories");

    json_object_binding<Episode>(m,"Episode")
            .def_readwrite("start_time", &Episode::start_time)
            .def_readwrite("time_stamp", &Episode::time_stamp)
            .def_readwrite("end_time", &Episode::end_time)
            .def_readwrite("trajectories", &Episode::trajectories)
            .def_readwrite("captures", &Episode::captures)
            .def(self == self)
            ;

    json_vector_binding<Episode>(m,"Episodes");

    json_object_binding<Experiment>(m,"Experiment")
            .def_readwrite("name", &Experiment::name)
            .def_readwrite("subject_name", &Experiment::subject_name)
            .def_readwrite("world_configuration_name", &Experiment::world_configuration_name)
            .def_readwrite("world_implementation_name", &Experiment::world_implementation_name)
            .def_readwrite("occlusions", &Experiment::occlusions)
            .def_readwrite("duration", &Experiment::duration)
            .def_readwrite("start_time", &Experiment::start_time)
            .def_readwrite("episodes", &Experiment::episodes)
            .def("set_name", &Experiment::set_name)
            ;

    json_object_binding<Python_json_object>(m, "Json_builder")
            .def("add_member",+[](Python_json_object &jb, const string &name, bool mandatory, int &variable){
                jb.json_add_member(name,mandatory,std::move(Json_wrap_object(jb.create_member(variable)).get_unique_ptr()));
            })
            .def("add_member",+[](Python_json_object &jb, const string &name, bool mandatory, bool &variable){
                jb.json_add_member(name,mandatory,std::move(Json_wrap_object(jb.create_member(variable)).get_unique_ptr()));
            })
            .def("add_member",+[](Python_json_object &jb, const string &name, bool mandatory, float &variable){
                jb.json_add_member(name,mandatory,std::move(Json_wrap_object(jb.create_member(variable)).get_unique_ptr()));
            })
            .def("add_member",+[](Python_json_object &jb, const string &name, bool mandatory, string &variable){
                jb.json_add_member(name,mandatory,std::move(Json_wrap_object(jb.create_member(variable)).get_unique_ptr()));
            })
            ;
}


namespace cell_world {
    Resources Resources::from(const string resource) {
        Resources wr;
        wr._resource = resource;
        return wr;
    }

    Resources &Resources::key(const string &k) {
        _keys.push_back(k);
        return *this;
    }

    Resources &Resources::key(int k) {
        return key(std::to_string(k));
    }

    Resources &Resources::key(unsigned int k) {
        return key(std::to_string(k));
    }

    Resources &Resources::key(const char *k) {
        return key(string(k));
    }

    string _cache_invalidation(){
        stringstream ss;
        using namespace std::chrono;
        ss << "?r=" << duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        return ss.str();
    }

    std::istream &Resources::get() {
        return _resource_stream = ifstream("cfn");
    }

    std::string Resources::url() {
        string url = "https://raw.githubusercontent.com/germanespinosa/cellworld_data/master/" + _resource + "/" +
                     _file_name();
        return url;
    }

    std::string Resources::_file_name() {
        string fn;
        bool first = true;
        for (auto &k : _keys) {
            if (!first) fn += '.';
            first = false;
            fn += k;
        }
        return fn;
    }

    std::string &Resources::cache_folder() {
        static string cache_folder="./cellworld_cache";
        return cache_folder;
    }

    std::string Resources::to_string() {
        stringstream ss;
        ss << get().rdbuf();
        return ss.str();
    }

    void Resources::remove_cache() {
    }
}