mkdir -p build/python
swig -c++ -python swig/ReactHandler.i
find ./swig -maxdepth 1 -mindepth 1 -not -name *.i -exec mv '{}' build/python \;

mkdir -p build/php
swig -c++ -php swig/ReactHandler.i
find ./swig -maxdepth 1 -mindepth 1 -not -name *.i -exec mv '{}' build/php \;

export CXX=/usr/bin/g++-5
pushd build/php && cmake -DLAN:STRING=php ../.. && make -j4 && popd
#pushd build/python && cmake -DLAN:STRING=python ../.. && make -j4 && popd
mkdir -p build/native
pushd build/native && cmake -DLAN:STRING=native ../.. && make -j4 && popd
