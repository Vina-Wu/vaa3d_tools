# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build

# Include any dependencies generated for this target.
include Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/depend.make

# Include the progress variables for this target.
include Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/progress.make

# Include the compile flags for this target's objects.
include Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/flags.make

Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.o: Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/flags.make
Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.o: /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Thresholding/KappaSigmaThreshold.cxx
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.o"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.o -c /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Thresholding/KappaSigmaThreshold.cxx

Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.i"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Thresholding/KappaSigmaThreshold.cxx > CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.i

Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.s"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Thresholding/KappaSigmaThreshold.cxx -o CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.s

Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.o.requires:
.PHONY : Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.o.requires

Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.o.provides: Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.o.requires
	$(MAKE) -f Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/build.make Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.o.provides.build
.PHONY : Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.o.provides

Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.o.provides.build: Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.o
.PHONY : Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.o.provides.build

Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.o: Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/flags.make
Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.o: Source/Thresholding/moc_KappaSigmaThreshold.cxx
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.o"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.o -c /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding/moc_KappaSigmaThreshold.cxx

Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.i"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding/moc_KappaSigmaThreshold.cxx > CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.i

Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.s"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding/moc_KappaSigmaThreshold.cxx -o CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.s

Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.o.requires:
.PHONY : Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.o.requires

Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.o.provides: Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.o.requires
	$(MAKE) -f Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/build.make Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.o.provides.build
.PHONY : Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.o.provides

Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.o.provides.build: Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.o
.PHONY : Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.o.provides.build

Source/Thresholding/moc_KappaSigmaThreshold.cxx: /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Thresholding/KappaSigmaThreshold.h
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating moc_KappaSigmaThreshold.cxx"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && /usr/bin/moc-qt4 -I/home/liyun/V3D/v3d_main/basic_c_fun/../common_lib/include -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Review/Statistics -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Review -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/gdcm/src -I/home/liyun/ITK/ITK-bin/Utilities/gdcm -I/home/liyun/ITK/ITK-bin/Utilities/vxl/core -I/home/liyun/ITK/ITK-bin/Utilities/vxl/vcl -I/home/liyun/ITK/ITK-bin/Utilities/vxl/v3p/netlib -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/vxl/core -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/vxl/vcl -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/vxl/v3p/netlib -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities -I/home/liyun/ITK/ITK-bin/Utilities -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/itkExtHdrs -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/nifti/znzlib -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/nifti/niftilib -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/expat -I/home/liyun/ITK/ITK-bin/Utilities/expat -I/home/liyun/ITK/ITK-bin/Utilities/DICOMParser -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/DICOMParser -I/home/liyun/ITK/ITK-bin/Utilities/NrrdIO -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/NrrdIO -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/MetaIO -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/SpatialObject -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Numerics/NeuralNetworks -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Numerics/FEM -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/IO -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Numerics -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Common -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/BasicFilters -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Algorithms -I/home/liyun/ITK/ITK-bin -I/usr/include/qt4 -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtCore -I/home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Common -I/home/liyun/V3D/v3d_main/basic_c_fun -I/home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins -DQT_GUI_LIB -DQT_CORE_LIB -o /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding/moc_KappaSigmaThreshold.cxx /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Thresholding/KappaSigmaThreshold.h

# Object files for target KappaSigmaThreshold
KappaSigmaThreshold_OBJECTS = \
"CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.o" \
"CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.o"

# External object files for target KappaSigmaThreshold
KappaSigmaThreshold_EXTERNAL_OBJECTS =

bin/libKappaSigmaThreshold.so: Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.o
bin/libKappaSigmaThreshold.so: Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.o
bin/libKappaSigmaThreshold.so: bin/libV3DInterface.a
bin/libKappaSigmaThreshold.so: bin/libV3DITKCommon.a
bin/libKappaSigmaThreshold.so: /usr/lib/libQtGui.so
bin/libKappaSigmaThreshold.so: /usr/lib/libQtCore.so
bin/libKappaSigmaThreshold.so: Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/build.make
bin/libKappaSigmaThreshold.so: Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX shared library ../../bin/libKappaSigmaThreshold.so"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/KappaSigmaThreshold.dir/link.txt --verbose=$(VERBOSE)
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && /usr/bin/cmake -E copy /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/bin/libKappaSigmaThreshold.so /home/liyun/V3D/v3d/plugins/ITK//Thresholding/KappaSigmaThreshold/KappaSigmaThreshold.so

# Rule to build all files generated by this target.
Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/build: bin/libKappaSigmaThreshold.so
.PHONY : Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/build

Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/requires: Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/KappaSigmaThreshold.o.requires
Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/requires: Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/moc_KappaSigmaThreshold.o.requires
.PHONY : Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/requires

Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/clean:
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && $(CMAKE_COMMAND) -P CMakeFiles/KappaSigmaThreshold.dir/cmake_clean.cmake
.PHONY : Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/clean

Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/depend: Source/Thresholding/moc_KappaSigmaThreshold.cxx
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Thresholding /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Source/Thresholding/CMakeFiles/KappaSigmaThreshold.dir/depend

