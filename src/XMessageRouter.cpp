#define builtIn
#include "XMessageRouter"
#include "XMessageHandler"
#include "QDebug"
#include "QTextStream"

XList <XMessageHandler *> XMessageRouter::_handlers;

void XMessageRouter::registerMessageHandler( XMessageHandler &in )
{
    registerMessageHandler( &in );
}

void XMessageRouter::registerMessageHandler( XMessageHandler *in )
{
    _handlers << in;
    in->_listeners << &_handlers;
}

void XMessageRouter::unregisterMessageHandler( XMessageHandler &in )
{
    unregisterMessageHandler( &in );
}

void XMessageRouter::unregisterMessageHandler( XMessageHandler *in )
{
    _handlers.removeAll( in );
    in->_listeners.removeAll( &_handlers );
}

QDebug XMessageRouter::stream( int type, const char *file, const char *function, unsigned int line )
{
    foreach( XMessageHandler *handler, _handlers )
    {
        if( handler->canHandle( type ) )
        {
            handler->XGetPropertyMember( triggerFile ) = file;
            handler->XGetPropertyMember( triggerFunction ) = function;
            handler->XGetPropertyMember( triggerLine ) = line;
            handler->XGetPropertyMember( triggerType ) = type;
            return QDebug( handler );
        }
    }

    return qDebug();
}

void XMessageRouter::removeHandler( QObject *in )
    {
    _handlers.removeAll( static_cast <XMessageHandler *> (in) );
    }

void xRegisterMessageHandler( XMessageHandler &in )
    {
    XMessageRouter::registerMessageHandler( &in );
    }

void xRegisterMessageHandler( XMessageHandler *in )
    {
    XMessageRouter::registerMessageHandler( in );
    }

void xUnregisterMessageHandler( XMessageHandler &in )
    {
    XMessageRouter::registerMessageHandler( &in );
    }

void xUnregisterMessageHandler( XMessageHandler *in )
    {
    XMessageRouter::registerMessageHandler( in );
    }

QDebug operator <<( QDebug dbg, std::string str )
    {
    dbg << QString::fromStdString( str );
    return dbg;
    }
