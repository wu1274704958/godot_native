#include <Godot.hpp>
#include <Reference.hpp>
#include <Sprite.hpp>
#include <Control.hpp>

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

class RotateSimple : public godot::Sprite{
    GODOT_CLASS(RotateSimple, godot::Sprite)
    public:
    RotateSimple(){}
    void _init() 
    {
        Godot::print("RotateSimple _init()");
    }
    void _ready()
    {
        Godot::print("RotateSimple _ready()");
        auto rect = get_rect();
        
        auto par = static_cast<godot::Control*>(get_parent());
        auto size = par->get_rect().get_size();
        
        this->set_position(Vector2(size.width/2.f,get_position().y));
    }
    void _process(double delta)
    {
        rotate += delta;
        if(rotate > 360.f) rotate = 0.f;
        set_rotation(rotate);
    }
    static void _register_methods()
    {
        godot::register_method("_ready", &RotateSimple::_ready);
        godot::register_method("_process", &RotateSimple::_process);
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
    godot::register_class<RotateSimple>();
}