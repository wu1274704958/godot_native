scons platform=windows generate_bindings=yes target=release bits=32
scons platform=windows generate_bindings=yes target=release bits=64
scons platform=android generate_bindings=yes target=release android_arch=armv7
scons platform=android generate_bindings=yes target=release android_arch=arm64v8