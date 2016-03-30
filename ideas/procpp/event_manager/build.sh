mkdir -p build/python
swig -c++ -python swig/ReactHandler.i
find ./swig -maxdepth 1 -mindepth 1 -not -name ReactHandler.i -exec mv '{}' build/python \;

mkdir -p build/php
swig -c++ -php swig/ReactHandler.i
find ./swig -maxdepth 1 -mindepth 1 -not -name ReactHandler.i -exec mv '{}' build/php \;

export CXX=/usr/bin/g++-5
pushd build/php && cmake -DLAN:STRING=php ../.. && make && popd
pushd build/python && cmake -DLAN:STRING=python ../.. && make && popd
