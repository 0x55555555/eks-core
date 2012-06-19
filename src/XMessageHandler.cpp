#include "XMessageHandler"
#include "QString"

XMessageHandler::XMessageHandler()
{
    QIODevice::open( WriteOnly );
}

XMessageHandler::~XMessageHandler()
    {
    Q_FOREACH( XList <XMessageHandler *> *listener, _listeners )
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
    Q_EMIT handle( QString( QByteArray( data, size ) ) );
    return size;
}

qint64 XMessageHandler::readData( char *, qint64 )
{
    return 0;
}
