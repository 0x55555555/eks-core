import "../EksBuild" as Eks;

Eks.Library {
    name: "EksCore"
    toRoot: "../../"

    Depends { name: "Qt.gui" }
    Depends { name: "Qt.widgets" }

    Properties {
        condition: debug
        cpp.dynamicLibraries: [ "Dbghelp" ]
    }

    Export {
        Depends { name: "Qt.core" }
        cpp.includePaths: [ toRoot + "EksCore/include" ]
    }
}
