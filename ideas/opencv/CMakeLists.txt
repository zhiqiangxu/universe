project( DisplayImage )
find_package( OpenCV REQUIRED )
add_executable( DisplayImage DisplayImage.cpp )
target_link_libraries( DisplayImage ${OpenCV_LIBS} )

project( Hello )
add_executable( Hello Hello.cpp )
target_link_libraries( Hello ${OpenCV_LIBS} )
target_compile_features(Hello PRIVATE cxx_range_for)

