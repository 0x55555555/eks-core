#include "XMessageHandler"
#include "QString"

XMessageHandler::XMessageHandler()
{
    QIODevice::open( WriteOnly );
}

XMessageHandler::~XMessageHandler()
    {
    foreach( XList <XMessageHandler *> *listener, _listeners )
        {
        listener->removeAll( this );
        }
    }

bool XMessageHandler::canHandle( int ) const
{
    return FALSE;
}

qint64 XMessageHandler::writeData( const char *data, qint64 size )
{
    emit handle( QString( QByteArray( data, size ) ) );
    return size;
}

qint64 XMessageHandler::readData( char *, qint64 )
{
    return 0;
}
