#if 0

#include "XFileSequence"
#include "XObject"
#include "XSignal"
#include "XDebug"
#include "XChildEvent"
#include "timeNow.h"

class TestSlave : public XObject
    {
public:
    void fn()
        {
        xDebug << "No Arguments baby!";
        }
    void fn1( int in )
        {
        xDebug << "Slot 1" << in << sender();
        }
    void fn2( int in1, int in2 )
        {
        xDebug << "Slot 2" << in1 << in2 << sender();
        }
    void fn3( int in1, int in2, int in3 )
        {
        xDebug << "Slot 3" << in1 << in2 << in3 << sender();
        }
    void fn4( int in1, int in2, int in3, int in4 )
        {
        xDebug << "Slot 4" << in1 << in2 << in3 << in4 << sender();
        }
    void fn5( int in1, int in2, int in3, int in4, int in5 )
        {
        xDebug << "Slot 5" << in1 << in2 << in3 << in4 << in5 << sender();
        }
    void fn6( int in1, int in2, int in3, int in4, int in5, int in6 )
        {
        xDebug << "Slot 6" << in1 << in2 << in3 << in4 << in5 << in6 << sender();
        }
    void fn7( int in1, int in2, int in3, int in4, int in5, int in6, int in7 )
        {
        xDebug << "Slot 7" << in1 << in2 << in3 << in4 << in5 << in6 << in7 << sender();
        }
    void fn8( int in1, int in2, int in3, int in4, int in5, int in6, int in7, int in8 )
        {
        xDebug << "Slot 8" << in1 << in2 << in3 << in4 << in5 << in6 << in7 << in8 << sender();
        }
    void fn9( int &in1, int *in2, int in3, int in4, int in5, int in6, int in7, int in8, int in9 )
        {
        xDebug << "Slot 9" << ++in1 << ++*in2 << in3 << in4 << in5 << in6 << in7 << in8 << in9 << sender();
        }
    };

class TestSignaller : public XObject
    {
public:
    X_SIGNAL( signal );
    X_SIGNAL( signal1, int );
    X_SIGNAL( signal2, int, int );
    X_SIGNAL( signal3, int, int, int );
    X_SIGNAL( signal4, int, int, int, int );
    X_SIGNAL( signal5, int, int, int, int, int );
    X_SIGNAL( signal6, int, int, int, int, int, int );
    X_SIGNAL( signal7, int, int, int, int, int, int, int );
    X_SIGNAL( signal8, int, int, int, int, int, int, int, int );
    X_SIGNAL( signal9, int &, int *, int, int, int, int, int, int, int );

    void test( TestSlave *obj )
        {
        int tmp2 = 0;
        int tmp = 0;

        double begin( timeNow() );

        for( int x=0; x<10000; x++ )
            {
            obj->fn( );
            obj->fn1( 1 );
            obj->fn2( 1, 2 );
            obj->fn3( 1, 2, 3 );
            obj->fn4( 1, 2, 3, 4 );
            obj->fn5( 1, 2, 3, 4, 5 );
            obj->fn6( 1, 2, 3, 4, 5, 6 );
            obj->fn7( 1, 2, 3, 4, 5, 6, 7 );
            obj->fn8( 1, 2, 3, 4, 5, 6, 7, 8 );

            obj->fn9( tmp, &tmp2, 3, 4, 5, 6, 7, 8, 9 );
            }

        double middle( timeNow() );

        for( int x=0; x<10000; x++ )
            {
            trigger( signal( ) );
            trigger( signal1( 1 ) );
            trigger( signal2( 1, 2 ) );
            trigger( signal3( 1, 2, 3 ) );
            trigger( signal4( 1, 2, 3, 4 ) );
            trigger( signal5( 1, 2, 3, 4, 5 ) );
            trigger( signal6( 1, 2, 3, 4, 5, 6 ) );
            trigger( signal7( 1, 2, 3, 4, 5, 6, 7 ) );
            trigger( signal8( 1, 2, 3, 4, 5, 6, 7, 8 ) );

            trigger( signal9( tmp, &tmp2, 3, 4, 5, 6, 7, 8, 9 ) );
            }

        double end( timeNow() );

        xDebug << "Signal Emitter" << this;
        xDebug << "Signals took " << ( end - middle ) << "seconds to complete";
        xDebug << "Pure function calls took " << ( middle - begin ) << "seconds to complete";
        xDebug << "Signals took " << ( end - middle ) / ( middle - begin ) << "times longer to complete than pure function calls.";
        }
    };

// 1.11x slower than pure function calls.
// 1.07x slower than pure function calls.
int testSignalsMain( )
    {
    TestSignaller *sig( new TestSignaller );
    TestSlave *sla( new TestSlave );

    X_CONNECT( sig, signal, sla, fn );
    X_CONNECT( sig, signal1, sla, fn1 );
    X_CONNECT( sig, signal2, sla, fn2 );
    X_CONNECT( sig, signal3, sla, fn3 );
    X_CONNECT( sig, signal4, sla, fn4 );
    X_CONNECT( sig, signal5, sla, fn5 );
    X_CONNECT( sig, signal6, sla, fn6 );
    X_CONNECT( sig, signal7, sla, fn7 );
    X_CONNECT( sig, signal8, sla, fn8 );
    X_CONNECT( sig, signal9, sla, fn9 );

    sig->test( sla );
    }

#endif
