import "../EksBuild" as Eks;
//import "Utils.js" as Utils

Eks.Library {
    name: "EksCore"
    Depends { name: "Qt.core" }

    cpp.includePaths: [thisRoot + "include"]
}
