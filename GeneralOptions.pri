include($$PWD"/SIMD.pri")
include($$PWD"/C++0x.pri")

win32-g++ {
  QMAKE_CXXFLAGS += -Wall
}

release {
  DEFINES += QT_NO_DEBUG
}

win32-msvc2008 {
  debug {
    QMAKE_CXXFLAGS += /MDd
  }
  release {
    QMAKE_CXXFLAGS += /MD /DEBUG
  }
}

win32-msvc2010 {
  INCLUDEPATH += "C:\\Program Files (x86)\\Microsoft Visual Studio 10.0\\VC\\include" \
                "C:\\Program Files\\Microsoft SDKs\\Windows\\v7.1\\Include"
  LIBS += -L"C:\\Program Files (x86)\\Microsoft Visual Studio 10.0\\VC\\lib" \
          -L"C:\\Program Files\\Microsoft SDKs\\Windows\\v7.1\\Lib"

  release {
    QMAKE_CFLAGS += /Zi
    QMAKE_CXXFLAGS += /Zi
    QMAKE_LFLAGS += /DEBUG
  }
}

ROOT = $$PWD"/../../"
BINFOLDER = $$ROOT"/bin/"

export (ROOT)

DESTDIR = $$BINFOLDER

LIBS += -L$$BINFOLDER

DEFINES += $$upper($$TARGET)_BUILD
