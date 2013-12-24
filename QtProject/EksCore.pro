# -------------------------------------------------
# Project created by QtCreator 2010-01-16T14:33:07
# -------------------------------------------------
TARGET = EksCore
TEMPLATE = lib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
    ../src/XAsyncTask.cpp \
    ../src/XMathHelpers.cpp \
    ../src/XStringSimple.cpp \
    ../src/XGlobalAllocator.cpp \
    ../src/XTemporaryAllocator.cpp \
    ../src/XLoggingAllocator.cpp \
    ../src/XCore.cpp \
    ../src/XJsonWriter.cpp

HEADERS += \
    ../include/Math/Eigen/SVD \
    ../include/Math/Eigen/StdVector \
    ../include/Math/Eigen/StdList \
    ../include/Math/Eigen/StdDeque \
    ../include/Math/Eigen/Sparse \
    ../include/Math/Eigen/QtAlignedMalloc \
    ../include/Math/Eigen/QR \
    ../include/Math/Eigen/LU \
    ../include/Math/Eigen/LeastSquares \
    ../include/Math/Eigen/Jacobi \
    ../include/Math/Eigen/Householder \
    ../include/Math/Eigen/Geometry \
    ../include/Math/Eigen/Eigenvalues \
    ../include/Math/Eigen/Eigen2Support \
    ../include/Math/Eigen/Eigen \
    ../include/Math/Eigen/Dense \
    ../include/Math/Eigen/Core \
    ../include/Math/Eigen/Cholesky \
    ../include/Math/Eigen/Array \
    ../include/Math/Eigen/src/Core/Visitor.h \
    ../include/Math/Eigen/src/Core/VectorwiseOp.h \
    ../include/Math/Eigen/src/Core/VectorBlock.h \
    ../include/Math/Eigen/src/Core/TriangularMatrix.h \
    ../include/Math/Eigen/src/Core/Transpositions.h \
    ../include/Math/Eigen/src/Core/Transpose.h \
    ../include/Math/Eigen/src/Core/Swap.h \
    ../include/Math/Eigen/src/Core/Stride.h \
    ../include/Math/Eigen/src/Core/StableNorm.h \
    ../include/Math/Eigen/src/Core/SolveTriangular.h \
    ../include/Math/Eigen/src/Core/SelfCwiseBinaryOp.h \
    ../include/Math/Eigen/src/Core/SelfAdjointView.h \
    ../include/Math/Eigen/src/Core/Select.h \
    ../include/Math/Eigen/src/Core/Reverse.h \
    ../include/Math/Eigen/src/Core/ReturnByValue.h \
    ../include/Math/Eigen/src/Core/Replicate.h \
    ../include/Math/Eigen/src/Core/Redux.h \
    ../include/Math/Eigen/src/Core/Random.h \
    ../include/Math/Eigen/src/Core/ProductBase.h \
    ../include/Math/Eigen/src/Core/Product.h \
    ../include/Math/Eigen/src/Core/PlainObjectBase.h \
    ../include/Math/Eigen/src/Core/PermutationMatrix.h \
    ../include/Math/Eigen/src/Core/NumTraits.h \
    ../include/Math/Eigen/src/Core/NoAlias.h \
    ../include/Math/Eigen/src/Core/NestByValue.h \
    ../include/Math/Eigen/src/Core/MatrixBase.h \
    ../include/Math/Eigen/src/Core/Matrix.h \
    ../include/Math/Eigen/src/Core/MathFunctions.h \
    ../include/Math/Eigen/src/Core/MapBase.h \
    ../include/Math/Eigen/src/Core/Map.h \
    ../include/Math/Eigen/src/Core/IO.h \
    ../include/Math/Eigen/src/Core/GlobalFunctions.h \
    ../include/Math/Eigen/src/Core/GenericPacketMath.h \
    ../include/Math/Eigen/src/Core/Fuzzy.h \
    ../include/Math/Eigen/src/Core/Functors.h \
    ../include/Math/Eigen/src/Core/ForceAlignedAccess.h \
    ../include/Math/Eigen/src/Core/Flagged.h \
    ../include/Math/Eigen/src/Core/EigenBase.h \
    ../include/Math/Eigen/src/Core/Dot.h \
    ../include/Math/Eigen/src/Core/DiagonalProduct.h \
    ../include/Math/Eigen/src/Core/DiagonalMatrix.h \
    ../include/Math/Eigen/src/Core/Diagonal.h \
    ../include/Math/Eigen/src/Core/DenseStorage.h \
    ../include/Math/Eigen/src/Core/DenseCoeffsBase.h \
    ../include/Math/Eigen/src/Core/DenseBase.h \
    ../include/Math/Eigen/src/Core/CwiseUnaryView.h \
    ../include/Math/Eigen/src/Core/CwiseUnaryOp.h \
    ../include/Math/Eigen/src/Core/CwiseNullaryOp.h \
    ../include/Math/Eigen/src/Core/CwiseBinaryOp.h \
    ../include/Math/Eigen/src/Core/CommaInitializer.h \
    ../include/Math/Eigen/src/Core/BooleanRedux.h \
    ../include/Math/Eigen/src/Core/Block.h \
    ../include/Math/Eigen/src/Core/BandMatrix.h \
    ../include/Math/Eigen/src/Core/Assign.h \
    ../include/Math/Eigen/src/Core/ArrayWrapper.h \
    ../include/Math/Eigen/src/Core/ArrayBase.h \
    ../include/Math/Eigen/src/Core/Array.h \
    ../include/Async/XAsyncTaskImpl.h \
    ../include/Async/XAsyncTask.h \
    ../include/Containers/XVector.h \
    ../include/Containers/XUnorderedMap.h \
    ../include/Containers/XStringSimple.h \
    ../include/Containers/XStringBuffer.h \
    ../include/Containers/XIntrusiveLinkedList.h \
    ../include/Math/XQuaternion.h \
    ../include/Math/XMathVector.h \
    ../include/Math/XMathMatrix.h \
    ../include/Math/XMathHelpers.h \
    ../include/Math/XMathCurve.h \
    ../include/Math/XFrame.h \
    ../include/Math/XColour.h \
    ../include/Memory/XWeakShared.h \
    ../include/Memory/XUniquePointer.h \
    ../include/Memory/XTypedAllocator.h \
    ../include/Memory/XThreadSafeAllocator.h \
    ../include/Memory/XTemporaryAllocator.h \
    ../include/Memory/XSharedPointer.h \
    ../include/Memory/XResourceDescription.h \
    ../include/Memory/XLoggingAllocator.h \
    ../include/Memory/XGlobalAllocator.h \
    ../include/Memory/XBucketAllocator.h \
    ../include/Memory/XAllocatorBase.h \
    ../include/QtHelper/XQWidgets.h \
    ../include/QtHelper/XQWidget.h \
    ../include/QtHelper/XQApplication.h \
    ../include/Utilities/XTime.h \
    ../include/Utilities/XTemplateHelpers.h \
    ../include/Utilities/XProperty.h \
    ../include/Utilities/XPrivateImpl.h \
    ../include/Utilities/XOptional.h \
    ../include/Utilities/XMacroHelpers.h \
    ../include/Utilities/XFlags.h \
    ../include/Utilities/XFileSequence.h \
    ../include/Utilities/XEventLogger.h \
    ../include/Utilities/XCodeLocation.h \
    ../include/Utilities/XAssert.h \
    ../include/XGlobal.h \
    ../include/XCore.h \
    ../include/Utilities/XJsonWriter.h

























