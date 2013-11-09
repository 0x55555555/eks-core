import "../EksBuild" as Eks;

Eks.Library {
  name: "EksCore"
  toRoot: "../../"

  property var windowsIncludePaths: [
    "C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\include",
	"C:\\Program Files (x86)\\Windows Kits\\8.1\\Include\\um",
	"C:\\Program Files (x86)\\Windows Kits\\8.1\\Include\\shared"
  ]
  
  property var windoewsLibPaths: [
	"C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\lib\\amd64",
	"C:\\Program Files (x86)\\Windows Kits\\8.1\\Lib\\winv6.3\\umc"
  ]
  
  Depends { name: "Qt.gui" }
  Depends { name: "Qt.widgets" }
  
  Properties {
    condition: debug
    cpp.dynamicLibraries: [ "Dbghelp" ]
  }
  
  cpp.includePaths: base.concat( windowsIncludePaths )
  cpp.dynamicLibraries: base.concat( windoewsLibPaths )

  Export {
    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
	
    cpp.includePaths: [ "./include" ].concat(windowsIncludePaths)
  }
}
