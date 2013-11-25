# -------------------------------------------------
# Project created by QtCreator 2010-01-16T14:33:07
# -------------------------------------------------
TARGET = EksCore
TEMPLATE = lib

QT += widgets

INCLUDEPATH += ../include

include("../GeneralOptions.pri")

win32 {
  CONFIG(debug) {
    LIBS += -lDbghelp
  }
}

SOURCES += \
    ../src/XFileSequence.cpp \
    ../src/XTime.cpp \
    ../src/XUnorderedMap.cpp \
    ../src/XEventLogger.cpp \
    ../src/XBucketAllocator.cpp \
    ../src/XAssert.cpp \
    ../src/XCodeLocation.cpp \
    ../src/XArrayMath.cpp \
    ../src/XAsyncTask.cpp \
    ../src/XMathHelpers.cpp \
    ../src/XStringSimple.cpp \
    ../src/XGlobalAllocator.cpp \
    ../src/XTemporaryAllocator.cpp \
    ../src/XLoggingAllocator.cpp \
    ../src/XCore.cpp

HEADERS += \
    ../include/XGlobal \
    ../include/XVector \
    ../include/XProperty \
    ../include/XColour \
    ../include/XQuaternion \
    ../include/XFileSequence \
    ../include/XFlags \
    ../include/XWeakShared \
    ../include/XTime \
    ../include/Eigen/SVD \
    ../include/Eigen/StdVector \
    ../include/Eigen/StdList \
    ../include/Eigen/StdDeque \
    ../include/Eigen/Sparse \
    ../include/Eigen/QtAlignedMalloc \
    ../include/Eigen/QR \
    ../include/Eigen/LU \
    ../include/Eigen/LeastSquares \
    ../include/Eigen/Jacobi \
    ../include/Eigen/Householder \
    ../include/Eigen/Geometry \
    ../include/Eigen/Eigenvalues \
    ../include/Eigen/Eigen2Support \
    ../include/Eigen/Eigen \
    ../include/Eigen/Dense \
    ../include/Eigen/Core \
    ../include/Eigen/Cholesky \
    ../include/Eigen/Array \
    ../include/Eigen/src/Core/Visitor.h \
    ../include/Eigen/src/Core/VectorwiseOp.h \
    ../include/Eigen/src/Core/VectorBlock.h \
    ../include/Eigen/src/Core/TriangularMatrix.h \
    ../include/Eigen/src/Core/Transpositions.h \
    ../include/Eigen/src/Core/Transpose.h \
    ../include/Eigen/src/Core/Swap.h \
    ../include/Eigen/src/Core/Stride.h \
    ../include/Eigen/src/Core/StableNorm.h \
    ../include/Eigen/src/Core/SolveTriangular.h \
    ../include/Eigen/src/Core/SelfCwiseBinaryOp.h \
    ../include/Eigen/src/Core/SelfAdjointView.h \
    ../include/Eigen/src/Core/Select.h \
    ../include/Eigen/src/Core/Reverse.h \
    ../include/Eigen/src/Core/ReturnByValue.h \
    ../include/Eigen/src/Core/Replicate.h \
    ../include/Eigen/src/Core/Redux.h \
    ../include/Eigen/src/Core/Random.h \
    ../include/Eigen/src/Core/ProductBase.h \
    ../include/Eigen/src/Core/Product.h \
    ../include/Eigen/src/Core/PlainObjectBase.h \
    ../include/Eigen/src/Core/PermutationMatrix.h \
    ../include/Eigen/src/Core/NumTraits.h \
    ../include/Eigen/src/Core/NoAlias.h \
    ../include/Eigen/src/Core/NestByValue.h \
    ../include/Eigen/src/Core/MatrixBase.h \
    ../include/Eigen/src/Core/Matrix.h \
    ../include/Eigen/src/Core/MathFunctions.h \
    ../include/Eigen/src/Core/MapBase.h \
    ../include/Eigen/src/Core/Map.h \
    ../include/Eigen/src/Core/IO.h \
    ../include/Eigen/src/Core/GlobalFunctions.h \
    ../include/Eigen/src/Core/GenericPacketMath.h \
    ../include/Eigen/src/Core/Fuzzy.h \
    ../include/Eigen/src/Core/Functors.h \
    ../include/Eigen/src/Core/ForceAlignedAccess.h \
    ../include/Eigen/src/Core/Flagged.h \
    ../include/Eigen/src/Core/EigenBase.h \
    ../include/Eigen/src/Core/Dot.h \
    ../include/Eigen/src/Core/DiagonalProduct.h \
    ../include/Eigen/src/Core/DiagonalMatrix.h \
    ../include/Eigen/src/Core/Diagonal.h \
    ../include/Eigen/src/Core/DenseStorage.h \
    ../include/Eigen/src/Core/DenseCoeffsBase.h \
    ../include/Eigen/src/Core/DenseBase.h \
    ../include/Eigen/src/Core/CwiseUnaryView.h \
    ../include/Eigen/src/Core/CwiseUnaryOp.h \
    ../include/Eigen/src/Core/CwiseNullaryOp.h \
    ../include/Eigen/src/Core/CwiseBinaryOp.h \
    ../include/Eigen/src/Core/CommaInitializer.h \
    ../include/Eigen/src/Core/BooleanRedux.h \
    ../include/Eigen/src/Core/Block.h \
    ../include/Eigen/src/Core/BandMatrix.h \
    ../include/Eigen/src/Core/Assign.h \
    ../include/Eigen/src/Core/ArrayWrapper.h \
    ../include/Eigen/src/Core/ArrayBase.h \
    ../include/Eigen/src/Core/Array.h \
    ../include/XAllocatorBase \
    ../include/XBucketAllocator \
    ../include/XUnorderedMap \
    ../include/XEventLogger \
    ../include/XAssert \
    ../include/XMacroHelpers \
    ../include/XMathHelpers \
    ../include/XCodeLocation \
    ../include/XArrayMath \
    ../include/XTemplateHelpers \
    ../include/XOptional \
    ../include/Eigen/src/Core/util/XprHelper.h \
    ../include/Eigen/src/Core/util/StaticAssert.h \
    ../include/Eigen/src/Core/util/ReenableStupidWarnings.h \
    ../include/Eigen/src/Core/util/NonMPL2.h \
    ../include/Eigen/src/Core/util/MKL_support.h \
    ../include/Eigen/src/Core/util/Meta.h \
    ../include/Eigen/src/Core/util/Memory.h \
    ../include/Eigen/src/Core/util/Macros.h \
    ../include/Eigen/src/Core/util/ForwardDeclarations.h \
    ../include/Eigen/src/Core/util/DisableStupidWarnings.h \
    ../include/Eigen/src/Core/util/Constants.h \
    ../include/Eigen/src/Core/util/BlasUtil.h \
    ../include/XPrivateImpl \
    ../include/XAsyncTask \
    ../include/XAsyncTaskImpl \
    ../include/XUniquePointer \
    ../include/XTypedAllocator \
    ../include/XStringBuffer \
    ../include/XMathVector \
    ../include/XMathMatrix \
    ../include/XStringSimple \
    ../include/XMathCurve \
    ../include/XResourceDescription \
    ../include/XThreadSafeAllocator \
    ../include/XGlobalAllocator \
    ../include/XTemporaryAllocator \
    ../include/XLoggingAllocator \
    ../include/XCore \
    ../include/QtHelper/XQWidget \
    ../include/QtHelper/XQApplication \
    ../include/QtHelper/XQWidgets \
    ../include/XFrame \
    ../include/XSharedPointer \
    ../include/XIntrusiveLinkedList


























