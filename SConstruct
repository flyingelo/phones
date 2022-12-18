
def getCxxFlags():
    return ["/EHsc", "/W4", "/WX", "/std:c++20"]

env = Environment()

for cxxFlag in getCxxFlags():
    env.Append(CXXFLAGS = cxxFlag)

env.Replace(VARIANT_DIR = "Build")
env.Replace(SOURCE_DIR = ".")

VariantDir(
    variant_dir = env['VARIANT_DIR'],
    src_dir = env['SOURCE_DIR'],
    duplicate = 0)

phoneSorter = env.Program(
    'Build/PhoneSorter',
    source = ['Build/phones.cpp'])

env.Install('bin', phoneSorter)
