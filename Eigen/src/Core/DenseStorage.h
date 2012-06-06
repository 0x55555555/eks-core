// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2008 Gael Guennebaud <gael.guennebaud@inria.fr>
// Copyright (C) 2006-2009 Benoit Jacob <jacob.benoit.1@gmail.com>
// Copyright (C) 2010 Hauke Heibel <hauke.heibel@gmail.com>
//
// Eigen is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Eigen is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// Eigen. If not, see <http://www.gnu.org/licenses/>.

#ifndef EIGEN_MATRIXSTORAGE_H
#define EIGEN_MATRIXSTORAGE_H

#ifdef EIGEN_DENSE_STORAGE_CTOR_PLUGIN
  #define EIGEN_INTERNAL_DENSE_STORAGE_CTOR_PLUGIN EIGEN_DENSE_STORAGE_CTOR_PLUGIN;
#else
  #define EIGEN_INTERNAL_DENSE_STORAGE_CTOR_PLUGIN
#endif

namespace internal {

struct constructor_without_unaligned_array_assert {};

/** \internal
  * Static array. If the MatrixOrArrayOptions require auto-alignment, the array will be automatically aligned:
  * to 16 bytes boundary if the total size is a multiple of 16 bytes.
  */
template <typename T, int Size, int MatrixOrArrayOptions,
          int Alignment = (MatrixOrArrayOptions&DontAlign) ? 0
                        : (((Size*sizeof(T))%16)==0) ? 16
                        : 0 >
struct plain_array
{
  T array[Size];
  plain_array() {}
  plain_array(constructor_without_unaligned_array_assert) {}
};

#ifdef EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
  #define EIGEN_MAKE_UNALIGNED_ARRAY_ASSERT(sizemask)
#else
  #define EIGEN_MAKE_UNALIGNED_ARRAY_ASSERT(sizemask) \
    eigen_assert((reinterpret_cast<size_t>(array) & sizemask) == 0 \
              && "this assertion is explained here: " \
              "http://eigen.tuxfamily.org/dox-devel/TopicUnalignedArrayAssert.html" \
              " **** READ THIS WEB PAGE !!! ****");
#endif

template <typename T, int Size, int MatrixOrArrayOptions>
struct plain_array<T, Size, MatrixOrArrayOptions, 16>
{
  EIGEN_USER_ALIGN16 T array[Size];
  plain_array() { EIGEN_MAKE_UNALIGNED_ARRAY_ASSERT(0xf) }
  plain_array(constructor_without_unaligned_array_assert) {}
};

template <typename T, int MatrixOrArrayOptions, int Alignment>
struct plain_array<T, 0, MatrixOrArrayOptions, Alignment>
{
  EIGEN_USER_ALIGN16 T array[1];
  plain_array() {}
  plain_array(constructor_without_unaligned_array_assert) {}
};

} // end namespace internal

/** \internal
  *
  * \class DenseStorage
  * \ingroup Core_Module
  *
  * \brief Stores the data of a matrix
  *
  * This class stores the data of fixed-size, dynamic-size or mixed matrices
  * in a way as compact as possible.
  *
  * \sa Matrix
  */
template<typename T, int Size, int _Rows, int _Cols, int _Options> class DenseStorage;

// purely fixed-size matrix
template<typename T, int Size, int _Rows, int _Cols, int _Options> class DenseStorage
{
    internal::plain_array<T,Size,_Options> m_data;
  public:
    inline explicit DenseStorage() {}
    inline DenseStorage(internal::constructor_without_unaligned_array_assert)
      : m_data(internal::constructor_without_unaligned_array_assert()) {}
    inline DenseStorage(DenseIndex,DenseIndex,DenseIndex) {}
    inline void swap(DenseStorage& other) { std::swap(m_data,other.m_data); }
    inline static DenseIndex rows(void) {return _Rows;}
    inline static DenseIndex cols(void) {return _Cols;}
    inline void conservativeResize(DenseIndex,DenseIndex,DenseIndex) {}
    inline void resize(DenseIndex,DenseIndex,DenseIndex) {}
    inline const T *data() const { return m_data.array; }
    inline T *data() { return m_data.array; }
};

// null matrix
template<typename T, int _Rows, int _Cols, int _Options> class DenseStorage<T, 0, _Rows, _Cols, _Options>
{
  public:
    inline explicit DenseStorage() {}
    inline DenseStorage(internal::constructor_without_unaligned_array_assert) {}
    inline DenseStorage(DenseIndex,DenseIndex,DenseIndex) {}
    inline void swap(DenseStorage& ) {}
    inline static DenseIndex rows(void) {return _Rows;}
    inline static DenseIndex cols(void) {return _Cols;}
    inline void conservativeResize(DenseIndex,DenseIndex,DenseIndex) {}
    inline void resize(DenseIndex,DenseIndex,DenseIndex) {}
    inline const T *data() const { return 0; }
    inline T *data() { return 0; }
};

// dynamic-size matrix with fixed-size storage
template<typename T, int Size, int _Options> class DenseStorage<T, Size, Dynamic, Dynamic, _Options>
{
    internal::plain_array<T,Size,_Options> m_data;
    DenseIndex m_rows;
    DenseIndex m_cols;
  public:
    inline explicit DenseStorage() : m_rows(0), m_cols(0) {}
    inline DenseStorage(internal::constructor_without_unaligned_array_assert)
      : m_data(internal::constructor_without_unaligned_array_assert()), m_rows(0), m_cols(0) {}
    inline DenseStorage(DenseIndex, DenseIndex rows, DenseIndex cols) : m_rows(rows), m_cols(cols) {}
    inline void swap(DenseStorage& other)
    { std::swap(m_data,other.m_data); std::swap(m_rows,other.m_rows); std::swap(m_cols,other.m_cols); }
    inline DenseIndex rows(void) const {return m_rows;}
    inline DenseIndex cols(void) const {return m_cols;}
    inline void conservativeResize(DenseIndex, DenseIndex rows, DenseIndex cols) { m_rows = rows; m_cols = cols; }
    inline void resize(DenseIndex, DenseIndex rows, DenseIndex cols) { m_rows = rows; m_cols = cols; }
    inline const T *data() const { return m_data.array; }
    inline T *data() { return m_data.array; }
};

// dynamic-size matrix with fixed-size storage and fixed width
template<typename T, int Size, int _Cols, int _Options> class DenseStorage<T, Size, Dynamic, _Cols, _Options>
{
    internal::plain_array<T,Size,_Options> m_data;
    DenseIndex m_rows;
  public:
    inline explicit DenseStorage() : m_rows(0) {}
    inline DenseStorage(internal::constructor_without_unaligned_array_assert)
      : m_data(internal::constructor_without_unaligned_array_assert()), m_rows(0) {}
    inline DenseStorage(DenseIndex, DenseIndex rows, DenseIndex) : m_rows(rows) {}
    inline void swap(DenseStorage& other) { std::swap(m_data,other.m_data); std::swap(m_rows,other.m_rows); }
    inline DenseIndex rows(void) const {return m_rows;}
    inline DenseIndex cols(void) const {return _Cols;}
    inline void conservativeResize(DenseIndex, DenseIndex rows, DenseIndex) { m_rows = rows; }
    inline void resize(DenseIndex, DenseIndex rows, DenseIndex) { m_rows = rows; }
    inline const T *data() const { return m_data.array; }
    inline T *data() { return m_data.array; }
};

// dynamic-size matrix with fixed-size storage and fixed height
template<typename T, int Size, int _Rows, int _Options> class DenseStorage<T, Size, _Rows, Dynamic, _Options>
{
    internal::plain_array<T,Size,_Options> m_data;
    DenseIndex m_cols;
  public:
    inline explicit DenseStorage() : m_cols(0) {}
    inline DenseStorage(internal::constructor_without_unaligned_array_assert)
      : m_data(internal::constructor_without_unaligned_array_assert()), m_cols(0) {}
    inline DenseStorage(DenseIndex, DenseIndex, DenseIndex cols) : m_cols(cols) {}
    inline void swap(DenseStorage& other) { std::swap(m_data,other.m_data); std::swap(m_cols,other.m_cols); }
    inline DenseIndex rows(void) const {return _Rows;}
    inline DenseIndex cols(void) const {return m_cols;}
    inline void conservativeResize(DenseIndex, DenseIndex, DenseIndex cols) { m_cols = cols; }
    inline void resize(DenseIndex, DenseIndex, DenseIndex cols) { m_cols = cols; }
    inline const T *data() const { return m_data.array; }
    inline T *data() { return m_data.array; }
};

template <typename T, int _Options> struct DynamicStorageData
  {
  DenseIndex m_refCount;
  DenseIndex m_rows;
  DenseIndex m_cols;
  DenseIndex m_allocatedCount;
  T m_data[1];

  static DynamicStorageData<T, _Options> *create(DenseIndex size, DenseIndex rows, DenseIndex cols)
    {
    DynamicStorageData<T, _Options> *ret = (DynamicStorageData<T, _Options> *)internal::aligned_malloc(sizeof(DynamicStorageData<T, _Options>) + size*sizeof(T) - sizeof(T));

    ret->m_refCount = 1;
    ret->m_rows = rows;
    ret->m_cols = cols;
    ret->m_allocatedCount = size;

    return ret;
    }

  static DynamicStorageData<T, _Options> *resize(DynamicStorageData<T, _Options> *oldData, DenseIndex size, DenseIndex rows, DenseIndex cols)
    {
    if(oldData && oldData->m_refCount == 1 && oldData->m_allocatedCount >= size)
      {
      oldData->m_rows = rows;
      oldData->m_cols = cols;
      return oldData;
      }

    DynamicStorageData<T, _Options> *data = create(size, rows, cols);

    if(oldData)
      {
      memcpy(data->m_data, oldData->m_data, sizeof(T)*std::min(oldData->m_cols*oldData->m_rows, cols*rows));
      }

    deref(oldData);
    return data;
    }

  static DynamicStorageData<T, _Options> *detach(DynamicStorageData<T, _Options> *oldData)
    {
    if(!oldData || oldData->m_refCount == 1)
      {
      return oldData;
      }

    // todo, dont resize if smaller?
    DynamicStorageData<T, _Options> *data = create(oldData->m_cols*oldData->m_rows, oldData->m_rows, oldData->m_cols);

    memcpy(data->m_data, oldData->m_data, sizeof(T)*oldData->m_cols*oldData->m_rows);

    deref(oldData);
    return data;
    }

  static void ref(DynamicStorageData<T, _Options> *data)
    {
    if(!data)
      {
      return;
      }

    data->m_refCount++;
    }

  static void deref(DynamicStorageData<T, _Options> *data)
    {
    if(!data)
      {
      return;
      }

    DenseIndex ref = --data->m_refCount;
    if(ref == 0)
      {
      internal::aligned_free(data);
      }
    }
  };

// purely dynamic matrix.
template<typename T, int _Options> class DenseStorage<T, Dynamic, Dynamic, Dynamic, _Options>
{
    DynamicStorageData<T, _Options> *m_data;
  public:
    inline explicit DenseStorage() : m_data(0) {}
    inline DenseStorage(internal::constructor_without_unaligned_array_assert)
       : m_data(0) {}
    inline DenseStorage(DenseIndex size, DenseIndex rows, DenseIndex cols)
      : m_data(DynamicStorageData<T, _Options>::create(size, rows, cols))
    { EIGEN_INTERNAL_DENSE_STORAGE_CTOR_PLUGIN }
    inline ~DenseStorage() { DynamicStorageData<T, _Options>::deref(m_data); }
    inline void swap(DenseStorage& other) { std::swap(m_data,other.m_data); }
    template <int OS, int OR, int OC, int OO>
      inline void copy(const DenseStorage<T, OS, OR, OC, OO>& other) { DynamicStorageData<T, _Options>::ref(other.m_data); DynamicStorageData<T, _Options>::deref(m_data); m_data = other.m_data; }
    inline DenseIndex rows(void) const {return m_data ? m_data->m_rows : 0;}
    inline DenseIndex cols(void) const {return m_data ? m_data->m_cols : 0;}
    inline void conservativeResize(DenseIndex size, DenseIndex rows, DenseIndex cols)
    {
      m_data = DynamicStorageData<T, _Options>::resize(m_data, size, rows, cols);
    }
    void resize(DenseIndex size, DenseIndex rows, DenseIndex cols)
    {
      m_data = DynamicStorageData<T, _Options>::resize(m_data, size, rows, cols);
    }
    inline const T *data() const { return m_data ? m_data->m_data : 0; }
    inline T *data() { m_data = DynamicStorageData<T, _Options>::detach(m_data); return m_data ? m_data->m_data : 0; }
};

// matrix with dynamic width and fixed height (so that matrix has dynamic size).
template<typename T, int _Rows, int _Options> class DenseStorage<T, Dynamic, _Rows, Dynamic, _Options>
{
    DynamicStorageData<T, _Options> *m_data;
  public:
    inline explicit DenseStorage() : m_data(0) {}
    inline DenseStorage(internal::constructor_without_unaligned_array_assert) : m_data(0) {}
    inline DenseStorage(DenseIndex size, DenseIndex, DenseIndex cols) : m_data(DynamicStorageData<T, _Options>::create(size, _Rows, cols))
    { EIGEN_INTERNAL_DENSE_STORAGE_CTOR_PLUGIN }
    inline ~DenseStorage() { DynamicStorageData<T, _Options>::deref(m_data); }
    inline void swap(DenseStorage& other) { std::swap(m_data,other.m_data); }
    template <int OS, int OR, int OC, int OO>
      inline void copy(const DenseStorage<T, OS, OR, OC, OO>& other) { DynamicStorageData<T, _Options>::ref(other.m_data); DynamicStorageData<T, _Options>::deref(m_data); m_data = other.m_data; }
    inline static DenseIndex rows(void) {return _Rows;}
    inline DenseIndex cols(void) const {return m_data ? m_data->m_cols : 0;}
    inline void conservativeResize(DenseIndex size, DenseIndex, DenseIndex cols)
    {
      m_data = DynamicStorageData<T, _Options>::resize(m_data, size, _Rows, cols);
    }
    EIGEN_STRONG_INLINE void resize(DenseIndex size, DenseIndex, DenseIndex cols)
    {
      m_data = DynamicStorageData<T, _Options>::resize(m_data, size, rows, cols);
    }
    inline const T *data() const { return m_data ? m_data->m_data : 0; }
    inline T *data() { m_data = DynamicStorageData<T, _Options>::detach(m_data); return m_data ? m_data->m_data : 0; }
};

// matrix with dynamic height and fixed width (so that matrix has dynamic size).
template<typename T, int _Cols, int _Options> class DenseStorage<T, Dynamic, Dynamic, _Cols, _Options>
{
public:
    DynamicStorageData<T, _Options> *m_data;

    inline explicit DenseStorage() : m_data(0) {}
    inline DenseStorage(internal::constructor_without_unaligned_array_assert) : m_data(0) {}
    inline DenseStorage(DenseIndex size, DenseIndex rows, DenseIndex) : m_data(DynamicStorageData<T, _Options>::create(size, rows, _Cols))
    { EIGEN_INTERNAL_DENSE_STORAGE_CTOR_PLUGIN }
    inline ~DenseStorage() { DynamicStorageData<T, _Options>::deref(m_data); }
    inline void swap(DenseStorage& other) { std::swap(m_data,other.m_data); }
    template <int OS, int OR, int OC, int OO>
      inline void copy(const DenseStorage<T, OS, OR, OC, OO>& other) { DynamicStorageData<T, _Options>::ref(other.m_data); DynamicStorageData<T, _Options>::deref(m_data); m_data = other.m_data; }
    inline DenseIndex rows(void) const {return m_data ? m_data->m_rows : 0;}
    inline static DenseIndex cols(void) {return _Cols;}
    inline void conservativeResize(DenseIndex size, DenseIndex rows, DenseIndex)
    {
      m_data = DynamicStorageData<T, _Options>::resize(m_data, size, rows, _Cols);
    }
    EIGEN_STRONG_INLINE void resize(DenseIndex size, DenseIndex rows, DenseIndex)
    {
      m_data = DynamicStorageData<T, _Options>::resize(m_data, size, rows, _Cols);
    }
    inline const T *data() const { return m_data ? m_data->m_data : 0; }
    inline T *data() { m_data = DynamicStorageData<T, _Options>::detach(m_data); return m_data ? m_data->m_data : 0; }
};

#endif // EIGEN_MATRIX_H
