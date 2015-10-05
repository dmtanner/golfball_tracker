import os

env = Environment()
env['CXXFLAGS'] = "-Wall -O0 -g -Wfatal-errors -std=c++14 -fPIC"
env['CPPPATH'] = ['include']
env['LIBS'] = ['opencv_highgui', 'opencv_core', 'opencv_imgproc', 'opencv_calib3d', 'point_grey_camera' ]

non_main_cpp = ['src/main.cpp', 'src/Processor.cpp']  

trugolf = env.Program("bin/trugolf",  [non_main_cpp])
