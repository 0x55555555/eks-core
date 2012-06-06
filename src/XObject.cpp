#include "XObject"

XObject::XObject( )
    {
    }

XObject::~XObject()
    {
    trigger(destroyed());
    }
