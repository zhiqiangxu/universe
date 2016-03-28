swig -c++ -python -o swig/ReactHandler_python_wrap.cxx swig/ReactHandler.i
mv swig/ReactHandler.py build/
mv swig/ReactHandler_python_wrap.cxx build/
export CXX=/usr/bin/g++-5
cd build && cmake .. && make
