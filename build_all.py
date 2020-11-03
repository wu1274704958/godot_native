import json
import os
import sys

def compile(platform,mod,release):
    target = "debug"
    if release:
        target = "release"
    if platform == "windows":
        os.system(str.format("scons platform={} project={} target={} bits={}",platform,mod,target,32))
        os.system(str.format("scons platform={} project={} target={} bits={}",platform,mod,target,64))
    elif platform == "android":
        os.system(str.format("scons platform={} project={} target={} android_arch={}",platform,mod,target,"armv7"))
        os.system(str.format("scons platform={} project={} target={} android_arch={}",platform,mod,target,"arm64v8"))

def copy_to(platform,mod,release,project_dir,bin_dir):
    target = "debug"
    if release:
        target = "release"
    arr = []
    if platform == "windows":
        arr.append(str.format("lib{}_{}_{}_{}{}",mod,platform,target,32,".dll"))
        arr.append(str.format("lib{}_{}_{}_{}{}",mod,platform,target,64,".dll"))
    elif platform == "android":
        arr.append(str.format("lib{}_{}_{}_{}{}",mod,platform,target,"armv7",".so"))
        arr.append(str.format("lib{}_{}_{}_{}{}",mod,platform,target,"arm64v8",".so"))
    for f in arr:
        os.system(str.format("copy bin\\{} {}\\{} /Y",f,project_dir,bin_dir))

conf = "cnf"
if len(sys.argv) > 1:
    conf = sys.argv[1]

cnf = json.load( open(conf+".json","r"))
debug = True
if "debug" in cnf:
    debug = cnf["debug"]
# compile
mod_arr = []
if "mod" in cnf:
    mod_arr = cnf["mod"]
    for n in mod_arr:
        compile("windows",n,not debug)
        compile("android",n,not debug)
# copy to project
    if "project" in cnf and "bin_dir" in cnf:
        for n in mod_arr:
            copy_to("windows",n,not debug,cnf["project"],cnf["bin_dir"])
            copy_to("android",n,not debug,cnf["project"],cnf["bin_dir"])
    