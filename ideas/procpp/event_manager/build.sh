swig -c++ -python -o swig/ReactHandler_python_wrap.cxx swig/ReactHandler.i
mv swig/ReactHandler.py build/
mv swig/ReactHandler_python_wrap.cxx build/

swig -c++ -php -o swig/ReactHandler_php_wrap.cxx swig/ReactHandler.i
mv swig/ReactHandler.php build/
mv swig/php_ReactHandler.h build/
mv swig/ReactHandler_php_wrap.cxx build/

export CXX=/usr/bin/g++-5
cd build && cmake .. && make
