win32-msvc2008 {
  QMAKE_CXXFLAGS += /arch:SSE2
}

win32-msvc2010 {
  QMAKE_CXXFLAGS += /arch:SSE2
}

win32-g++ {
  QMAKE_CXXFLAGS += -msse2
}
