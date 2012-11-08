win32-msvc2008 {
  QMAKE_CXXFLAGS += /arch:SSE2
}

win32-msvc2010 {
  QMAKE_CXXFLAGS += /arch:SSE2
}

win32-arm-msvc2012 {
  DEFINES += EIGEN_DONT_VECTORIZE
}

win32-g++ {
  QMAKE_CXXFLAGS += -msse2
}
