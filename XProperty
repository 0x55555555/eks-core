#ifndef XPROPERTY_H
#define XPROPERTY_H

#define XPropertyMember( type, name ) type _##name;

#define XReadProperty( type, name, func ) type func ( ) const { return _##name; }
#define XConstReadProperty( type, name, func ) const type func( ) const { return _##name; }
#define XRefAccessProperty( type, name, func ) type &func ( ) { return _##name; }

#define XWriteProperty( type, name, func ) void func ( type _in ) { _##name = _in; }

#define XSpecificProperty( type, readName, writeName, accessLevel ) \
    private: \
    XPropertyMember( type, readName ); \
    accessLevel: \
    XReadProperty( type, readName, readName ); \
    XWriteProperty( type, readName, writeName )

#define XROSpecificProperty( type, readName, accessLevel, memberLevel ) \
    memberLevel: \
    XPropertyMember( type, readName ); \
    accessLevel: \
    XReadProperty( type, readName, readName )

#define XWOSpecificProperty( type, propName, writeName, accessLevel ) \
    private: \
    XPropertyMember( type, propName ); \
    accessLevel: \
    XWriteProperty( type, propName, writeName )

#define XRefSpecificProperty( type, name, accessLevel ) \
    private: \
    XPropertyMember( type, name ) \
    accessLevel: \
    XConstReadProperty( type &, name, name ) \
    XRefAccessProperty( type, name, name )

#define XRORefSpecificProperty( type, name, accessLevel ) \
    private: \
    XPropertyMember( type, name ) \
    accessLevel: \
    XConstReadProperty( type &, name, name )

#define XByRefSpecificProperty( type, name, writeName, accessLevel ) \
    private: \
    XPropertyMember( type, name ) \
    accessLevel: \
    XConstReadProperty( type &, name, name ) \
    XWriteProperty( const type &, name, writeName )

#define XROByRefSpecificProperty( type, name, accessLevel ) \
    private: \
    XPropertyMember( type, name ) \
    accessLevel: \
    XConstReadProperty( type &, name, name )


#define XInitProperty(name, value) _##name( value )
#define XGetPropertyMember( name ) _##name

#define XProperty( type, readName, writeName ) XSpecificProperty( type, readName, writeName, public )
#define XROProperty( type, readName ) XROSpecificProperty( type, readName, public, private )
#define XWOProperty( type, propName, writeName ) XWOSpecificProperty( type, propName, writeName, public )

#define XRefProperty( type, name ) XRefSpecificProperty( type, name, public )
#define XRORefProperty( type, name ) XRORefSpecificProperty( type, name, public )

#define XByRefProperty(type, name, writeName) XByRefSpecificProperty( type, name, writeName, public )
#define XROByRefProperty(type, name) XROByRefSpecificProperty( type, name, public )

#define XProperties private

#endif // XPROPERTY_H
