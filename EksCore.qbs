import "../EksBuild" as Eks;

Eks.Library {
    name: "EksCore"
    toRoot: "../../"

    //cpp.includePaths: base.concat( [ "include" ] )

    Depends { name: "Qt.gui" }
    Depends { name: "Qt.widgets" }

    Properties {
        condition: debug
        cpp.dynamicLibraries: [ "Dbghelp" ]
    }

    Export {
        Depends { name: "Qt.core" }
        cpp.includePaths: [ "C:/Users/Jorj/SpaceGamez/EksCore/include" ]
    }
}
