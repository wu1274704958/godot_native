#include <Godot.hpp>
#include <Reference.hpp>
#include <Node.hpp>
#include <array>
#include <MeshInstance.hpp>
#include <Mesh.hpp>
#include <TriangleMesh.hpp>
#include <MeshInstance.hpp>
#include <SurfaceTool.hpp>
#include <ResourcePreloader.hpp>
#include <ResourceLoader.hpp>
#include <Material.hpp>
#include <ArrayMesh.hpp>
#include <PoolArrays.hpp>
#include <Transform.hpp>
#include <Spatial.hpp>

using namespace godot;

class SimpleClass : public Reference {
    GODOT_CLASS(SimpleClass, Reference);
public:
    SimpleClass() { }

    /** `_init` must exist as it is called by Godot. */
    void _init() {
        Godot::print("call _init()");
    }

    void test_void_method() {
        Godot::print("This is test");
    }
    

    Variant method(Variant arg) {
        Variant ret;
        ret = arg;

        return ret;
    }

    static void _register_methods() {
        register_method("method", &SimpleClass::method);

        /**
         * The line below is equivalent to the following GDScript export:
         *     export var _name = "SimpleClass"
         **/
        register_property<SimpleClass, String>("base/name", &SimpleClass::_name, String("SimpleClass"));

        /** Alternatively, with getter and setter methods: */
        register_property<SimpleClass, int>("base/value", &SimpleClass::set_value, &SimpleClass::get_value, 0);

        /** Registering a signal: **/
        // register_signal<SimpleClass>("signal_name");
        // register_signal<SimpleClass>("signal_name", "string_argument", GODOT_VARIANT_TYPE_STRING)
    }

    String _name;
    int _value;

    void set_value(int p_value) {
        _value = p_value;
    }

    int get_value() const {
        return _value;
    }
};

class CreateCube : public godot::Spatial{
    constexpr static int TextureHC = 8;
    constexpr static int TextureVC = 8;

    GODOT_CLASS(CreateCube, godot::Spatial)
    public:
    CreateCube(){}
    void _init() 
    {
        Godot::print("CreateCube _init()");
    }
    void _ready()
    {
        Godot::print("CreateCube _ready()");
        auto mi = create_cube({3,2,3,0,3,3});
        auto mi2 = create_cube({3,2,3,0,3,3});
        auto zuan = create_cube({22,22,22,22,22,22});
        auto gold = create_cube({16,16,16,16,16,16});
        auto glass = create_cube({29,29,29,29,29,29});
        auto tree = create_cube({18,18,18,18,18,18});
        mi2->set_translation(Vector3(0.0f,1.f,0.f));
        zuan->set_translation(Vector3(0.0f,0.f,-1.f));
        gold->set_translation(Vector3(0.0f,0.f,1.f));
        glass->set_translation(Vector3(1.0f,0.f,0.f));
        tree->set_translation(Vector3(-1.f,0.f,0.f));
        add_child(mi2);
        add_child(mi);
        add_child(zuan);
        add_child(gold);
        add_child(tree);
        add_child(glass);
    }

    inline Variant create_cube(int _1,int _2,int _3,int _4,int _5,int _6)
    {
        return create_cube({_1,_2,_3,_4,_5,_6});
    }

    MeshInstance* create_cube(std::array<int,6>&& ts)
    {
        return create_cube(ts);
    }

    MeshInstance* create_cube(std::array<int,6>& ts)
    {
        Godot::print("create cube!!!");
        SurfaceTool* st = SurfaceTool::_new();
        st->begin(Mesh::PRIMITIVE_TRIANGLES);

        for(int i = 0;i < ts.size();++i)
            create_surface(st,i,ts[i]);

        auto mesh = st->commit();
        MeshInstance* mi = MeshInstance::_new();
        mi->set_mesh(mesh);
        mi->set_material_override(ResourceLoader::get_singleton()->load("res://textures/material.tres"));
        return mi;
    }
    // sid Surface id
    //	0 => front
    //	1 => down
    //	2 => right
    //	3 => up
    //	4 => left
    //	5 => behind  
    void create_surface(SurfaceTool* st,int sid,int tid)
    {
        Vector3 normal = get_normal_form_sid(sid);
        Rect2 uv = get_uv_form_tid(tid);
        PoolVector3Array verices;
        get_pos_form_sid(&verices,sid);

        for(int i = 0;i < verices.size();++i)
        {
            //printf("x = %f y = %f z = %f \n",verices[i].x,verices[i].y,verices[i].z);
            st->add_normal(normal);
            st->add_uv(get_uv_form_vertex(i,&uv));
            st->add_vertex(verices[i]);
        }
        
    }

    Rect2 get_uv_form_tid(int tid)
    {
        if(tid >= TextureHC * TextureVC || tid < 0)
            return Rect2(0.f,0.f,1.f,1.f);
        int x = tid % TextureHC;
        int y = tid / TextureHC;

        return Rect2(  static_cast<float>(x) / static_cast<float>(TextureHC),static_cast<float>(y + 1) / static_cast<float>(TextureVC) ,
            static_cast<float>(x + 1) / static_cast<float>(TextureHC),static_cast<float>(y) / static_cast<float>(TextureVC)
        );
    }

    Vector2 get_uv_form_vertex(int vid,Rect2* rect)
    {
        if(vid == 0 || vid == 3)
            return Vector2( rect->position.x,rect->position.y);
        if(vid == 1)
            return Vector2( rect->position.x,rect->size.y);
        if(vid == 2 || vid == 4)
            return Vector2( rect->size.x,rect->size.y);
        if(vid == 5)
            return Vector2( rect->size.x,rect->position.y);
    }

    Vector3 get_normal_form_sid(int sid)
    {
        switch (sid)
        {
            case 0:
            return Vector3(0.f,0.f,1.f);
            case 1:
            return Vector3(0.f,-1.f,0.f);
            case 2:
            return Vector3(1.f,0.f,0.f);
            case 3:
            return Vector3(0.f,1.f,0.f);
            case 4:
            return Vector3(-1.f,0.f,0.f);
            case 5:
            return Vector3(0.f,0.f,-1.f);
        }
    }

    void get_pos_form_sid(PoolVector3Array* out,int sid,float scale = 1.f)
    {
        Vector3 luf(-0.5f * scale,-0.5f * scale,0.5f * scale );
        Vector3 ruf(0.5f * scale,-0.5f * scale,0.5f * scale );
        Vector3 rdf(0.5f * scale,0.5f * scale,0.5f * scale );
        Vector3 ldf(-0.5f * scale,0.5f * scale,0.5f * scale );

        Vector3 lub(-0.5f * scale,-0.5f * scale,-0.5f * scale );
        Vector3 rub(0.5f * scale,-0.5f * scale,-0.5f * scale );
        Vector3 rdb(0.5f * scale,0.5f * scale,-0.5f * scale );
        Vector3 ldb(-0.5f * scale,0.5f * scale,-0.5f * scale );

        switch (sid)
        {
        case 0:
            out->append(luf);
            out->append(ldf);   
            out->append(rdf);

            out->append(luf);
            out->append(rdf);   
            out->append(ruf);
        break;
        case 1:
            out->append(lub);
            out->append(luf);   
            out->append(ruf);

            out->append(lub);
            out->append(ruf);   
            out->append(rub);
        break;
        case 2:
            out->append(ruf);
            out->append(rdf);   
            out->append(rdb);

            out->append(ruf);
            out->append(rdb);   
            out->append(rub);
        break;
        case 3:
            out->append(ldf);
            out->append(ldb);   
            out->append(rdb);

            out->append(ldf);
            out->append(rdb);   
            out->append(rdf);
        break;
        case 4:
            out->append(lub);
            out->append(ldb);   
            out->append(ldf);

            out->append(lub);
            out->append(ldf);   
            out->append(luf);
        break;
        case 5:
            out->append(rub);
            out->append(rdb);   
            out->append(ldb);

            out->append(rub);
            out->append(ldb);   
            out->append(lub);
        break;
        }

    }

    void _process(double delta)
    {
        rotate += delta;
        if(rotate > 360.f) rotate = 0.f;
        set_rotation(Vector3(0.f,rotate,0.f));
    }
    static void _register_methods()
    {
        godot::register_method("_ready", &CreateCube::_ready);
        godot::register_method("_process", &CreateCube::_process);
        Variant(CreateCube::*func)(int,int,int,int,int,int) = &CreateCube::create_cube;
        godot::register_method("create_cube",func);
    }
    float rotate = 0.f;
};

/** GDNative Initialize **/
extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o) {
    godot::Godot::gdnative_init(o);
}

/** GDNative Terminate **/
extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o) {
    godot::Godot::gdnative_terminate(o);
}

/** NativeScript Initialize **/
extern "C" void GDN_EXPORT godot_nativescript_init(void *handle) {
    godot::Godot::nativescript_init(handle);

    godot::register_class<SimpleClass>();
    godot::register_class<CreateCube>();
}