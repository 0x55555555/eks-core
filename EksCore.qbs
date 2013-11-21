import "../EksBuild" as Eks;

Eks.Library {
  name: "EksCore"
  toRoot: "../../"

  Depends { name: "Qt.gui" }
  Depends { name: "Qt.widgets" }

  property string msvcPath: "C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\"
  property string windowsSDKPath: "C:\\Program Files (x86)\\Windows Kits\\8.1\\"

  property var windowsIncludePaths: [
    msvcPath + "VC\\include",
    windowsSDKPath + "Include\\um",
    windowsSDKPath + "Include\\shared"
  ]

  property var windowsLibPaths: [
    msvcPath + "VC\\lib\\amd64",
    windowsSDKPath + "Lib\\winv6.3\\umc"
  ]

  Properties {
    condition: qbs.targetOS == "windows"

    cpp.includePaths: base.concat( windowsIncludePaths )
    cpp.libraryPaths: base.concat( windowsLibPaths )

   Properties {
      condition: debug
      cpp.dynamicLibraries: base.concat([ "Dbghelp" ])
    }
  }

  Export {
    Depends { name: "cpp" }
    Depends { name: "Qt.core" }

    cpp.includePaths: [
      "./include",
      Qt.core.incPath + "\\QtANGLE" ].concat(windowsIncludePaths)
  }
}
