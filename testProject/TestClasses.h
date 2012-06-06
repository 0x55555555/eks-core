#ifndef TESTCLASSES_H
#define TESTCLASSES_H

#include "QObject"
#include "XObject"
#include "XSignal"

class TestA : public QObject, public XObject
    {
    Q_OBJECT
    X_OBJECT( TestA, XObject, 2 )
public:

    TestA()
        {
        i=0;
        }
    int fn()
        {
        i = i*5;
        return i;
        }
    int i;
    };
class TestB : public TestA
    {
    Q_OBJECT
    X_OBJECT( TestB, TestA, 3 )
public:

    TestB()
        {
        i=0;
        }
    int fn()
        {
        i = i*7;
        return i;
        }
    int i;
    };
class TestC : public QObject, public XObject
    {
    Q_OBJECT
    X_OBJECT( TestC, XObject, 4 )
public:

    TestC()
        {
        i=0;
        }
    int fn()
        {
        i = i*6;
        return i;
        }
    int i;
    };

#endif // TESTCLASSES_H
