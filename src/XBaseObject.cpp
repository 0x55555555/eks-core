#include "XBaseObject"
#include "XObject"
#include "XChildEvent"
#include "QDebug"

XBaseObject::XBaseObject( ) : _parent( 0 ), _sender( 0 )
    {
    }

XBaseObject::~XBaseObject( )
    {
    foreach( XBaseObject *child, _baseChildren )
        {
        delete child;
        }

    foreach( XBaseObject *child, _children )
        {
        delete child;
        }

    if( _parent )
        {
        _parent->removeChild( static_cast<XObject*>(this) );
        }
    }

XObjectList XBaseObject::children( ) const
    {
    return _children;
    }

XObject *XBaseObject::parent( ) const
    {
    return _parent;
    }

void XBaseObject::childEvent( XChildEvent * )
    {
    }

void XBaseObject::addChild( XObject *in )
    {
    if( in->_parent )
        {
        in->_parent->removeChild( in );
        }
    in->_parent = static_cast<XObject*>(this);

    _children << in;
    XChildEvent ev( XChildEvent::Add, in );
    childEvent( &ev );
    }

void XBaseObject::removeChild( XObject *in )
    {
    in->_parent = 0;
    _children.removeAll( static_cast<XObject*>(in) );
    XChildEvent ev( XChildEvent::Remove, in );
    childEvent( &ev );
    }

XObject *XBaseObject::sender() const
    {
    return static_cast<XObject*>(_sender);
    }

void XBaseObject::addChild( XBaseObject *in )
    {
    if( in->_parent )
        {
        in->_parent->removeChild( in );
        }
    in->_parent = static_cast<XObject*>(this);

    _baseChildren << in;
    }

void XBaseObject::removeChild( XBaseObject *in )
    {
    in->_parent = 0;
    _baseChildren.removeAll( in );
    }
