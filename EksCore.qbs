import qbs 1.0
import "Utils.js" as Utils

DynamicLibrary {
    name: "Core"
    files: [ "include/*.h", "src/*.cpp" ]
    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
}
