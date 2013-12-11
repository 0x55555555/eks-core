include($$PWD"/SIMD.pri")
include($$PWD"/C++0x.pri")

win32-g++ {
  QMAKE_CXXFLAGS += -Wall
}

CONFIG( debug, debug|release ) {
  DEFINES += NDEBUG _DEBUG X_DEBUG
} else {
  DEFINES += QT_NO_DEBUG
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

win32-msvc2012 {
  LIBS += -L"C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\lib\amd64"

  QMAKE_CXXFLAGS += /WX # /FS

  release {
    QMAKE_CFLAGS += /Zi
    QMAKE_CXXFLAGS += /Zi
    QMAKE_LFLAGS += /DEBUG
  }
}

win32-arm-msvc2012 {
  CONFIG += staticlib

  INCLUDEPATH += "C:\\QtSDK\\Desktop\\Qt\\4.8.1\\msvc2010\\include" \
                "C:\\QtSDK\\Desktop\\Qt\\4.8.1\\msvc2010\\include\\QtCore" \
                "C:\\QtSDK\\Desktop\\Qt\\4.8.1\\msvc2010\\include\\QtGui" \
                "C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\include" \
                "C:\\Program Files (x86)\\Windows Kits\\8.0\\Include\\um" \
                "C:\\Program Files (x86)\\Windows Kits\\8.0\\Include\\shared" \
                "C:\\Program Files (x86)\\Windows Kits\\8.0\\Include\\winrt"

  #QMAKE_CXXFLAGS += /AI\"C:/Program Files (x86)/Microsoft Visual Studio 11.0/VC/vcpackages\" \
  #          /AI\"C:/Program Files (x86)/Windows Kits/8.0/References/CommonConfiguration/Neutral\"
}

CONFIG(staticlib) {
  DEFINES += $$upper($$TARGET)_STATIC
}

ROOT = $$PWD"/../../"
BINFOLDER = $$ROOT"/bin/"

export (ROOT)

DESTDIR = $$BINFOLDER

LIBS += -L$$BINFOLDER

DEFINES += $$upper($$TARGET)_BUILD
