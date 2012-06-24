#include "XArrayMath"
#include "XAssert"
#include "Eigen/Core"
#include "Eigen/Geometry"
#include "QRect"
#include "XProfiler"
#include "XAllocatorBase"

XMathsEngine *g_engine = 0;
XMathsEngine *XMathsEngine::engine()
  {
  return g_engine;
  }

void XMathsEngine::setEngine(XMathsEngine *e)
  {
  g_engine = e;
  }

XMathsResult::XMathsResult(const XMathsOperation &o, XVectorI2D tl, xuint32 invSampleDensity, XMathsOperation::DataType type, void *data)
  {
  XMathsEngine::engine()->evaluateData(&o, (void**)&o._userData, tl, invSampleDensity, type, data);
  }

XMathsOperation::XMathsOperation() : _inputA(0), _inputB(0), _userData(0), _user(0), _nextUser(0)
  {
  xAssert(XMathsEngine::engine());
  _operation = NoOp;
  }

XMathsOperation::~XMathsOperation()
  {
  XMathsEngine::engine()->onCleanUp(this, &_userData);
  while(_user)
    {
    removeUser(_user);
    }
  }

void XMathsOperation::addUser(XMathsOperation *o) const
  {
  xAssert(o->_nextUser == 0);
  o->_nextUser = _user;
  _user = o;
  }

void XMathsOperation::removeUser(XMathsOperation *o) const
  {
  XMathsOperation *i = _user;
  XMathsOperation *prev = 0;
  while(i != o && i)
    {
    prev = i;
    i = i->_nextUser;
    }

  xAssert(i);
  if(prev)
    {
    prev->_nextUser = i->_nextUser;
    }
  i->_nextUser = 0;
  }

void XMathsOperation::setInput(const XMathsOperation **inp, const XMathsOperation *in)
  {
  if(in != (*inp))
    {
    if(*inp)
      {
      (*inp)->removeUser(this);
      }

    *inp = in;
    if(in)
      {
      in->addUser(this);
      }

    setInputDirty();
    }
  }

void XMathsOperation::setOperation(XMathsOperation::OperationType t)
  {
  if(_operation != t)
    {
    _operation = t;
    setOperationDirty();
    }
  }

void XMathsOperation::setValue(const XVector4D &v)
  {
  if(!_vectorData.isApprox(v))
    {
    _vectorData = v;
    setValueDirty();
    }
  }

void XMathsOperation::setValue(xuint32 v)
  {
  if(_integerData != v)
    {
    _integerData = v;
    setValueDirty();
    }
  }

void XMathsOperation::setInputDirty()
  {
  XMathsEngine::engine()->onInputDirty(this, &_userData);

  XMathsOperation *dep = _user;
  while(dep)
    {
    dep->setInputDirty();
    dep = dep->_nextUser;
    }
  }

void XMathsOperation::setOperationDirty()
  {
  XMathsEngine::engine()->onOperationDirty(this, &_userData);

  XMathsOperation *dep = _user;
  while(dep)
    {
    dep->setOperationDirty();
    dep = dep->_nextUser;
    }
  }

void XMathsOperation::setValueDirty()
  {
  XMathsEngine::engine()->onValueDirty(this, &_userData);

  XMathsOperation *dep = _user;
  while(dep)
    {
    dep->setValueDirty();
    dep = dep->_nextUser;
    }
  }

void XMathsOperation::copy(const XMathsOperation &a)
  {
  add(a, XVector4D::Zero());
  }

void XMathsOperation::load(DataType t, void* data, xsize stride, xsize dataWidth, xsize dataHeight, xsize dataChannels, const XVectorI2D &m)
  {
  setInput(&_inputA, 0);
  setInput(&_inputB, 0);

  setOperation(Load);
  setInputDirty();
  setValueDirty();
  _userData = XMathsEngine::engine()->loadData(t, data, stride, dataWidth, dataHeight, dataChannels, m);
  }

void XMathsOperation::add(const XMathsOperation &a, const XMathsOperation &b)
  {
  setInput(&_inputA, &a);
  setInput(&_inputB, &b);

  setOperation(Add);
  }

void XMathsOperation::add(const XMathsOperation &a, const XVector4D &v)
  {
  setInput(&_inputA, &a);
  setInput(&_inputB, 0);

  setOperation(AddConst);
  setValue(v);
  }

void XMathsOperation::multiply(const XMathsOperation &a, const XMathsOperation &b)
  {
  setInput(&_inputA, &a);
  setInput(&_inputB, &b);

  setOperation(Multiply);
  }

void XMathsOperation::multiply(const XMathsOperation &a, const XVector4D &v)
  {
  setInput(&_inputA, &a);
  setInput(&_inputB, 0);

  setOperation(MultiplyConst);
  setValue(v);
  }

void XMathsOperation::convolve(const XMathsOperation &a, const XMathsOperation &b)
  {
  setInput(&_inputA, &a);
  setInput(&_inputB, &b);

  setOperation(Convolve);
  }

void XMathsOperation::shuffle(const XMathsOperation &a, xuint32 mask)
  {
  setInput(&_inputA, &a);
  setInput(&_inputB, 0);

  setOperation(Shuffle);
  setValue(mask);
  }

void XMathsOperation::splice(const XMathsOperation &a, const XMathsOperation &b, xuint32 mask)
  {
  setInput(&_inputA, &a);
  setInput(&_inputB, &b);

  setOperation(Splice);
  setValue(mask);
  }

template <typename T> struct TypeAccessorsBase
  {
  typedef typename Eigen::Matrix <T, 4, 1> Vec;
  typedef typename Eigen::Array <Vec, Eigen::Dynamic, Eigen::Dynamic> Array;
  };

template <typename T> struct TypeAccessors
  {
  };

template <> struct TypeAccessors <float> : public TypeAccessorsBase<float>
  {
  static float white() { return 1.0f; }
  static float black() { return 0.0f; }
  };

template <> struct TypeAccessors <xuint8> : public TypeAccessorsBase<xuint8>
  {
  static xuint8 white() { return 255; }
  static xuint8 black() { return 0; }
  };

template <typename T> struct ImageData
  {
  typedef TypeAccessors<T> Type;
  typedef typename Type::Vec Vec;
  typedef typename Type::Array Array;

  XVectorI2D _offset;
  Array _data;
  };

template <typename T> struct OperationQueue
  {
  typedef TypeAccessors<T> Type;
  typedef typename Type::Vec Vec;
  typedef typename Type::Array Array;

  typedef void (*OperationFunction)(const XVectorI2D&, const XMathsOperation* o, Vec &arr, const Vec &);

  static void nullOp(const XVectorI2D&, const XMathsOperation*, Vec& arr, const Vec&)
    {
    arr = Vec::Zero();
    }

  static void load(const XVectorI2D& pt, const XMathsOperation* o, Vec& arr, const Vec&);

  static void add(const XVectorI2D&, const XMathsOperation*, Vec& arr, const Vec& b)
    {
    XProfileFunction
    arr = arr + b;
    }

  static void addConst(const XVectorI2D&, const XMathsOperation* o, Vec& arr, const Vec&)
    {
    XProfileFunction
    arr = arr + o->vectorData().cast<T>();
    }

  static void shuffle(const XVectorI2D&, const XMathsOperation* o, Vec& arr, const Vec&)
    {
    XProfileFunction
    xuint8 comp[4];
    XMathsOperation::shuffleComponents(o->integerData(), comp);

    Vec in = arr;

    for(xsize i = 0; i < 4; ++i)
      {
      if(comp[i] >= 4)
        {
        if(comp[i] == XMathsOperation::ShuffleOne)
          {
          arr(i) = Type::white();
          }
        else
          {
          arr(i) = Type::black();
          }
        }
      else
        {
        arr(i) = in(comp[i]);
        }
      }
    }

  struct Operation
    {
    OperationFunction _function;
    const XMathsOperation *_operation;
    xsize _writePoint;
    xsize _inputBPoint;
    };

  OperationQueue(XAllocatorBase *all, xsize s)
    {
    _storageLocationCount = 0;
    _allocator = all;
    _size = s;
    }

  static OperationQueue* create(XAllocatorBase *all, xsize count)
    {
    void *ptr = all->alloc(sizeof(OperationQueue) + (sizeof(Operation)*(count-1)));
    return new(ptr) OperationQueue(all, count);
    }

  static void destroy(OperationQueue *q)
    {
    q->_allocator->free(q);
    }

  static xsize buildQueueSize(const XMathsOperation *op)
    {
    const XMathsOperation *a = op->inputA();
    const XMathsOperation *b = op->inputB();

    xsize count = 1;
    if(a)
      {
      count += buildQueueSize(a);
      }

    if(b)
      {
      count += buildQueueSize(b);
      }

    return count;
    }

  static OperationQueue *buildQueue(const XMathsOperation *op, XAllocatorBase *all)
    {
    XProfileFunction
    xsize operationSize = buildQueueSize(op);

    OperationQueue<T> *q = create(all, operationSize);

    q->buildQueue(op, operationSize-1, q->_storageLocationCount++);

    return q;
    }

  void selectOperation(Operation &opData, const XMathsOperation *op, xsize writePoint, xsize inputBPoint)
    {
    /*xuint8 mode = kFastMethod;
    if(a && b)
      {
      if(a.offset != b->offset)
        {
        mode = kLongMethod;
        }
      }*/


    static OperationFunction g_sampleFunctions[] =
    {
      nullOp,   // NoOp,
      load,     // Load,
      add,      // Add,
      addConst, // AddConst,
      0,        // Multiply,
      0,        // MultiplyConst,
      0,        // Convolve,
      shuffle,  // Shuffle,
      0,        // Splice
    };

    opData._writePoint = writePoint;
    opData._inputBPoint = inputBPoint;
    opData._function = g_sampleFunctions[op->operation()];//[mode];
    opData._operation = op;
    xAssert(opData._function);
    }

  xsize buildQueue(const XMathsOperation *op, xsize writePoint, xsize storageLocation)
    {
    const XMathsOperation *a = op->inputA();
    const XMathsOperation *b = op->inputB();
    xsize inputsWritePoint = writePoint-1;
    xsize inputBPoint = 0;
    if(b)
      {
      inputBPoint = _storageLocationCount++;
      inputsWritePoint = buildQueue(b, inputsWritePoint, inputBPoint);
      }
    if(a)
      {
      inputsWritePoint = buildQueue(a, inputsWritePoint, storageLocation);
      }

    selectOperation(_operation[writePoint], op, storageLocation, inputBPoint);
    return inputsWritePoint;
    }

  void runQueue(XVectorI2D sampleStart, xuint32 invSampleFreq, Array &arr)
    {
    XProfileFunction
    xAssert(_storageLocationCount > 0);
    Vec *writePoints = new Vec[_storageLocationCount];

    for(xsize y = 0, h = arr.cols(); y < h; ++y)
      {
      for(xsize x = 0, w = arr.rows(); x < w; ++x)
        {
        XVectorI2D pt = sampleStart + XVectorI2D(x, y) * invSampleFreq;

        for(xsize o = 0; o < _size; ++o)
          {
          const Operation &op = _operation[o];
          op._function(pt, op._operation, writePoints[op._writePoint], writePoints[op._inputBPoint]);
          }
        arr(x, y) = writePoints[0];
        }
      }
    }

  static void loadData(void **input,
                       const void *data,
                       xsize stride,
                       xsize width,
                       xsize height,
                       xsize channels,
                       const XVectorI2D &m)
    {
    ImageData<T> *&image = *(ImageData<T>**)input;
    if(!image)
      {
      image = new ImageData<T>;
      }
    image->_offset = m;

    T *dataPtr = (T*)data;
    image->_data.resize(width, height);

    typename Type::Vec wP(Type::white(),Type::white(), Type::white(), Type::white());

    for(int y = 0; y < image->_data.cols(); ++y)
      {
      for(int x = 0; x < image->_data.rows(); ++x)
        {
        typename Type::Vec& v = image->_data(x, y);
        v = wP;
        memcpy(v.data(), dataPtr, sizeof(T)*channels);
        dataPtr += stride;
        }
      }
    }

  xsize _storageLocationCount;
  XAllocatorBase *_allocator;
  xsize _size;
  Operation _operation[1];
  };

struct QueueHolder
  {
  ~QueueHolder()
    {
    clearQueue();
    clearImage();
    }

  void clearQueue()
    {
    if(_queue)
      {
      if(_type == XMathsOperation::Float)
        {
        OperationQueue<float> *data = (OperationQueue<float> *)_queue;
        OperationQueue<float>::destroy(data);
        }
      else if(_type == XMathsOperation::Byte)
        {
        OperationQueue<xuint8> *data = (OperationQueue<xuint8> *)_queue;
        OperationQueue<xuint8>::destroy(data);
        }
      _queue = 0;
      }
    }

  void clearImage()
  {
    if(_imageData)
      {
      if(_type == XMathsOperation::Float)
        {
        ImageData<float> *imData = (ImageData<float> *)_imageData;
        delete imData;
        }
      else if(_type == XMathsOperation::Byte)
        {
        ImageData<xuint8> *imData = (ImageData<xuint8> *)_imageData;
        delete imData;
        }
      _imageData = 0;
      }
    }

  void *_queue;
  void *_imageData;
  XMathsOperation::DataType _type;
  };

template <typename T>
    void OperationQueue<T>::load(const XVectorI2D& pt, const XMathsOperation* o, Vec& arr, const Vec&)
  {
  XProfileFunction
  const void *u = o->userData();
  const QueueHolder *h = (const QueueHolder *)u;

  if(h->_imageData)
    {
    if(h->_type == XMathsOperation::Float)
      {
      const ImageData<float> *imData = (const ImageData<float> *)h->_imageData;
      XVectorI2D sampleAt = pt - imData->_offset;

      sampleAt.x() = xMin(imData->_data.rows(), xMax(sampleAt.x(), 0));
      sampleAt.y() = xMin(imData->_data.cols(), xMax(sampleAt.y(), 0));

      ImageData<float>::Vec v = imData->_data(sampleAt.x(), sampleAt.y());
      arr = v.cast<T>();
      }
    else if(h->_type == XMathsOperation::Byte)
      {
      const ImageData<xuint8> *imData = (const ImageData<xuint8> *)h->_imageData;
      XVectorI2D sampleAt = pt - imData->_offset;

      sampleAt.x() = xMin(imData->_data.rows(), xMax(sampleAt.x(), 0));
      sampleAt.y() = xMin(imData->_data.cols(), xMax(sampleAt.y(), 0));

      ImageData<xuint8>::Vec v = imData->_data(sampleAt.x(), sampleAt.y());
      arr = v.cast<T>();
      }
    }
  }

void XReferenceMathsEngine::evaluateData(const XMathsOperation *op,
                                         void **userData,
                                         const XVectorI2D &sampleOffset,
                                         xuint32 invSampleDensity,
                                         XMathsOperation::DataType type,
                                         void *data)
  {
  QueueHolder *&result = *(QueueHolder **)userData;

  if(!result)
    {
    result = new QueueHolder;
    result->_type = type;
    result->_imageData = 0;
    result->_queue = 0;
    }
  xAssert(!result->_imageData);

  if(result->_type == XMathsOperation::Float)
    {
    OperationQueue<float> **queue = (OperationQueue<float> **)&result->_queue;
    if(!*queue)
      {
      *queue = OperationQueue<float>::buildQueue(op, XGlobalAllocator::instance());
      }

    (*queue)->runQueue(sampleOffset, invSampleDensity, *(OperationQueue<float>::Array*)data);
    }
  else if(result->_type == XMathsOperation::Byte)
    {
    OperationQueue<xuint8> **queue = (OperationQueue<xuint8> **)&result->_queue;
    if(!*queue)
      {
      *queue = OperationQueue<xuint8>::buildQueue(op, XGlobalAllocator::instance());
      }

    (*queue)->runQueue(sampleOffset, invSampleDensity, *(OperationQueue<xuint8>::Array*)data);
    }
  }

void *XReferenceMathsEngine::loadData(XMathsOperation::DataType type,
                                      void* data,
                                      xsize stride,
                                      xsize width,
                                      xsize height,
                                      xuint8 channels,
                                      const XVectorI2D &m)
  {
  QueueHolder *ret = new QueueHolder;
  ret->_type = type;

  ret->_queue = 0;
  ret->_imageData = 0;
  if(!data)
    {
    xAssert(type == XMathsOperation::None);
    return ret;
    }

  xAssert(stride);
  if(type == XMathsOperation::Float)
    {
    OperationQueue<float>::loadData(&ret->_imageData, data, stride, width, height, channels, m);
    }
  else if(type == XMathsOperation::Byte)
    {
    OperationQueue<xuint8>::loadData(&ret->_imageData, data, stride, width, height, channels, m);
    }
  else
    {
    xAssertFail();
    }

  return ret;
  }

void XReferenceMathsEngine::onOperationDirty(const XMathsOperation *, void **userData)
  {
  QueueHolder *&res = *(QueueHolder**)userData;
  if(res)
    {
    res->clearQueue();
    res->clearImage();
    }
  }

void XReferenceMathsEngine::onValueDirty(const XMathsOperation *, void **)
  {
  }

void XReferenceMathsEngine::onInputDirty(const XMathsOperation *o, void **userData)
  {
  onOperationDirty(o, userData);
  }

void XReferenceMathsEngine::onCleanUp(const XMathsOperation *, void **userData)
  {
  QueueHolder *&res = *(QueueHolder**)userData;
  delete res;
  res = 0;
  }
