#!/usr/bin/env python
import os
import sys
import subprocess

if sys.version_info < (3,):
    def decode_utf8(x):
        return x
else:
    import codecs
    def decode_utf8(x):
        return codecs.utf_8_decode(x)[0]

if (os.name=="nt"):
    import subprocess

    def mySubProcess(cmdline,env):
        #print "SPAWNED : " + cmdline
        startupinfo = subprocess.STARTUPINFO()
        startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        proc = subprocess.Popen(cmdline, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
            stderr=subprocess.PIPE, startupinfo=startupinfo, shell = False, env = env)
        data, err = proc.communicate()
        rv = proc.wait()
        if rv:
            print("=====")
            print(err.decode("utf-8"))
            print("=====")
        return rv

    def mySpawn(sh, escape, cmd, args, env):

        newargs = ' '.join(args[1:])
        cmdline = cmd + " " + newargs

        rv=0
        if len(cmdline) > 32000 and cmd.endswith("ar") :
            cmdline = cmd + " " + args[1] + " " + args[2] + " "
            for i in range(3,len(args)) :
                rv = mySubProcess( cmdline + args[i], env )
                if rv :
                    break
        else:
            rv = mySubProcess( cmdline, env )

        return rv

# platform= makes it in line with Godots scons file, keeping p for backwards compatibility

# This makes sure to keep the session environment variables on windows,
# that way you can run scons in a vs 2017 prompt and it will find all the required tools


godot_headers_path = ARGUMENTS.get("headers", os.getenv("GODOT_HEADERS", "godot-cpp/godot_headers"))
godot_bindings_path = ARGUMENTS.get("cpp_bindings", os.getenv("CPP_BINDINGS", "godot-cpp"))

# default to debug build, must be same setting as used for cpp_bindings
target = ARGUMENTS.get("target", "debug")


if sys.platform.startswith('linux'):
    host_platform = 'linux'
elif sys.platform == 'darwin':
    host_platform = 'osx'
elif sys.platform == 'win32' or sys.platform == 'msys':
    host_platform = 'windows'
else:
    raise ValueError(
        'Could not detect platform automatically, please specify with '
        'platform=<platform>'
    )

env = Environment(ENV = os.environ)

is64 = sys.maxsize > 2**32
if (
    env['TARGET_ARCH'] == 'amd64' or
    env['TARGET_ARCH'] == 'emt64' or
    env['TARGET_ARCH'] == 'x86_64' or
    env['TARGET_ARCH'] == 'arm64-v8a'
):
    is64 = True


opts = Variables([], ARGUMENTS)
opts.Add(EnumVariable(
    'platform',
    'Target platform',
    host_platform,
    allowed_values=('linux', 'osx', 'windows', 'android'),
    ignorecase=2
))

opts.Add(EnumVariable(
    'bits',
    'Target platform bits',
    '64' if is64 else '32',
    ('32', '64')
))
opts.Add(BoolVariable(
    'use_llvm',
    'Use the LLVM compiler - only effective when targeting Linux',
    False
))

opts.Add(EnumVariable(
    'target',
    'Compilation target',
    'debug',
    allowed_values=('debug', 'release'),
    ignorecase=2
))

opts.Add(EnumVariable(
    'android_arch',
    'Target Android architecture',
    'armv7',
    ['armv7','arm64v8','x86','x86_64']
))

opts.Add(
    'android_api_level',
    'Target Android API level',
    '18' if ARGUMENTS.get("android_arch", 'armv7') in ['armv7', 'x86'] else '21'
)

opts.Add(
    'ANDROID_NDK_ROOT',
    'Path to your Android NDK installation. By default, uses ANDROID_NDK_ROOT from your defined environment variables.',
    os.environ.get("ANDROID_NDK_ROOT", None)
)

opts.Add(
    'project',
    'project for dir'
)


def add_sources(sources, dir):
    for f in os.listdir(dir):
        if f.endswith(".cpp"):
            sources.append(dir + "/" + f)

opts.Update(env)
Help(opts.GenerateHelpText(env))

project = ARGUMENTS.get("project","null")
if project == "null":
    print("project not input!")
    exit(-1)

if host_platform == 'windows' and env['platform'] != 'android':
    if env['bits'] == '64':
        env = Environment(TARGET_ARCH='amd64')
    elif env['bits'] == '32':
        env = Environment(TARGET_ARCH='x86')

    opts.Update(env)

if ARGUMENTS.get("use_llvm", "no") == "yes":
    env["CXX"] = "clang++"

# put stuff that is the same for all first, saves duplication
if env["platform"] == "osx":
    env.Append(CCFLAGS=["-g", "-O3", "-std=c++17", "-arch", "x86_64"])
    env.Append(LINKFLAGS=["-arch", "x86_64", "-framework", "Cocoa", "-Wl,-undefined,dynamic_lookup"])
elif env["platform"] == "linux":
    env.Append(CCFLAGS=["-g", "-O3", "-std=c++17", "-Wno-writable-strings"])
    env.Append(LINKFLAGS=["-Wl,-R,'$$ORIGIN'"])
elif env["platform"] == "windows":
    # need to add detection of msvc vs mingw, this is for msvc...
    env.Append(LINKFLAGS=["/WX"])
    if target == "debug":
        env.Append(CCFLAGS=["-EHsc", "-D_DEBUG", "/MDd","/std:c++17"])
    else:
        env.Append(CCFLAGS=["-O2", "-EHsc", "-DNDEBUG", "/MD","/std:c++17"])
elif env["platform"] == 'android':
    if host_platform == 'windows':
        print("gogogo")
        env = env.Clone(tools=['mingw'])
        env["SPAWN"] = mySpawn
    # Verify NDK root
    if not 'ANDROID_NDK_ROOT' in env:
        raise ValueError("To build for Android, ANDROID_NDK_ROOT must be defined. Please set ANDROID_NDK_ROOT to the root folder of your Android NDK installation.")

    # Validate API level
    api_level = int(env['android_api_level'])
    
    if env['android_arch'] in ['x86_64', 'arm64v8'] and api_level < 21:
        print("WARN: 64-bit Android architectures require an API level of at least 21; setting android_api_level=21")
        env['android_api_level'] = '21'
        api_level = 21

    # Setup toolchain
    toolchain = env['ANDROID_NDK_ROOT'] + "/toolchains/llvm/prebuilt/"
    if host_platform == "windows":
        toolchain += "windows"
        import platform as pltfm
        if pltfm.machine().endswith("64"):
            toolchain += "-x86_64"
    elif host_platform == "linux":
        toolchain += "linux-x86_64"
    elif host_platform == "osx":
        toolchain += "darwin-x86_64"
    env.PrependENVPath('PATH', toolchain + "/bin") # This does nothing half of the time, but we'll put it here anyways

    # Get architecture info
    arch_info_table = {
        "armv7" : {
            "march":"armv7-a", "target":"armv7a-linux-androideabi", "tool_path":"arm-linux-androideabi", "compiler_path":"armv7a-linux-androideabi",
            "ccflags" : ['-mfpu=neon']
            },
        "arm64v8" : {
            "march":"armv8-a", "target":"aarch64-linux-android", "tool_path":"aarch64-linux-android", "compiler_path":"aarch64-linux-android",
            "ccflags" : []
            },
        "x86" : {
            "march":"i686", "target":"i686-linux-android", "tool_path":"i686-linux-android", "compiler_path":"i686-linux-android",
            "ccflags" : ['-mstackrealign']
            },
        "x86_64" : {"march":"x86-64", "target":"x86_64-linux-android", "tool_path":"x86_64-linux-android", "compiler_path":"x86_64-linux-android",
            "ccflags" : []
        }
    }
    arch_info = arch_info_table[env['android_arch']]

    # Setup tools
    env['CC'] = toolchain + "/bin/clang"
    env['CXX'] = toolchain + "/bin/clang++"
    env['AR'] = toolchain + "/bin/" + arch_info['tool_path'] + "-ar"

    env.Append(CCFLAGS=['--target=' + arch_info['target'] + env['android_api_level'], '-march=' + arch_info['march'], '-fPIC', '-std=c++17'])#, '-fPIE', '-fno-addrsig', '-Oz'])
    env.Append(CCFLAGS=arch_info['ccflags'])

    cc_dir = env['ANDROID_NDK_ROOT'] + "/toolchains/llvm/prebuilt/windows-x86_64/bin/"
    print("cc_dir = " + cc_dir) 

    cxx = ""
    if env['android_arch'] == "armv7":
        cxx += "armv7a-linux-"
        cxx += str.format("androideabi{}",api_level)
    elif env['android_arch'] == "arm64v8":
        cxx += "aarch64-linux-"
        cxx += str.format("android{}",api_level)#("android"+api_level)
    cc = cxx + "-clang"
    cxx += "-clang++"

    sour = []
    objs = []
    so_list = ""

    add_sources(sour,project)
    if not os.path.exists("temp"):
        os.makedirs("temp")

    for f in sour:
        o = f.replace("/","_")
        o = o.replace(".cpp",".o")
        oo = "temp/"+o
        so_list += oo+" "
        objs.append(oo)
        cmd = cc_dir + cxx+" -fPIC -o "+oo +" -c "+ f +" -g -std=c++17 -Igodot-cpp/include -Igodot-cpp/include/core -Igodot-cpp/include/gen -Igodot-cpp/godot_headers"
        if env["target"] == "debug":
            cmd += " -O3"
        print(cmd)
        os.system(cmd)
        
    lib_pre = "godot-cpp"
    
    bin_name= str.format("bin/lib{}_{}_{}_{}.so",project,env["platform"],target,env['android_arch'])
    lib_name = lib_pre+"."+env["platform"]+"."+target+"."+env['android_arch']
    cmd = cc_dir + cxx+ str.format(" -o {} -shared {} -Lgodot-cpp/bin -l{}",bin_name,so_list,lib_name)
    print(cmd)
    os.system(cmd)
    
    
    #-fPIC -o src/init.o -c src/init.cpp -g -O3 -std=c++14 -Igodot-cpp/include -Igodot-cpp/include/core -Igodot-cpp/include/gen -Igodot-cpp/godot_headers
    #-o bin/libtest_arm64v8.so -shared src/init.o -Lgodot-cpp/bin -lgodot-cpp.android.release.armv7
    exit(0)






env.Append(
    CPPPATH=[
        godot_headers_path,
        godot_bindings_path + "/include/gen/",
        godot_bindings_path + "/include/core/",
        godot_bindings_path + "/include/",
    ]
)

lib_pre = "libgodot-cpp"

arch_suffix = env['bits']
if env['platform'] == 'android':
    arch_suffix = env['android_arch']

env.Append(LIBS=[lib_pre+"."+env["platform"]+"."+target+"."+arch_suffix])

env.Append(LIBPATH=[godot_bindings_path + "/bin/"])

sources = []
add_sources(sources, project)

suffix = ".so"
if env["platform"] == "windows":
    suffix = ".dll"
elif env["platform"] == "osx":
    suffix = ".dylib"

bin_name= str.format("lib{}_{}_{}_{}{}",project,env["platform"],target,arch_suffix,suffix)
print(bin_name)
library = env.SharedLibrary(target="bin/"+bin_name, source=sources)
Default(library)