#include <Godot.hpp>
#include <Reference.hpp>
#include <Node.hpp>
#include <array>
#include <MeshInstance.hpp>
#include <Mesh.hpp>
#include <TriangleMesh.hpp>
#include <MeshInstance.hpp>

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

class CreateCube : public godot::Node{
    constexpr static int TextureHC = 8;
    constexpr static int TextureVC = 8;

    GODOT_CLASS(CreateCube, godot::Node)
    public:
    CreateCube(){}
    void _init() 
    {
        Godot::print("CreateCube _init()");
    }
    void _ready()
    {
        Godot::print("CreateCube _ready()");
        std::array<int,6> arr = {1,2,3,4,5,6};
        create_cube(arr);
    }

    void create_cube(std::array<int,6>& ts)
    {
        Godot::print("create cube!!!");
        MeshInstance* mi = MeshInstance::_new();
        add_child(mi);
    }

    Rect2 get_uv_form_tid(int tid)
    {
        if(tid >= TextureHC * TextureVC || tid < 0)
            return Rect2(0.f,0.f,1.f,1.f);
        int x = tid % TextureHC;
        int y = tid / TextureHC;

        return Rect2(  static_cast<float>(x) / static_cast<float>(TextureHC),static_cast<float>(y) / static_cast<float>(TextureVC) ,
            static_cast<float>(x + 1) / static_cast<float>(TextureHC),static_cast<float>(y + 1) / static_cast<float>(TextureVC)
        );
    }

    void _process(double delta)
    {
        
    }
    static void _register_methods()
    {
        godot::register_method("_ready", &CreateCube::_ready);
        godot::register_method("_process", &CreateCube::_process);
    }
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