/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QATOMIC_AARCH64_H
#define QATOMIC_AARCH64_H

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#define Q_ATOMIC_INT_REFERENCE_COUNTING_IS_ALWAYS_NATIVE

inline bool QBasicAtomicInt::isReferenceCountingNative()
{ return true; }
inline bool QBasicAtomicInt::isReferenceCountingWaitFree()
{ return false; }

#define Q_ATOMIC_INT_TEST_AND_SET_IS_ALWAYS_NATIVE

inline bool QBasicAtomicInt::isTestAndSetNative()
{ return true; }
inline bool QBasicAtomicInt::isTestAndSetWaitFree()
{ return false; }

#define Q_ATOMIC_INT_FETCH_AND_STORE_IS_ALWAYS_NATIVE

inline bool QBasicAtomicInt::isFetchAndStoreNative()
{ return true; }
inline bool QBasicAtomicInt::isFetchAndStoreWaitFree()
{ return false; }

#define Q_ATOMIC_INT_FETCH_AND_ADD_IS_ALWAYS_NATIVE

inline bool QBasicAtomicInt::isFetchAndAddNative()
{ return true; }
inline bool QBasicAtomicInt::isFetchAndAddWaitFree()
{ return false; }

#define Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isTestAndSetNative()
{ return true; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isTestAndSetWaitFree()
{ return false; }

#define Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_ALWAYS_NATIVE

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndStoreNative()
{ return true; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndStoreWaitFree()
{ return false; }

#define Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_ALWAYS_NATIVE

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndAddNative()
{ return true; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndAddWaitFree()
{ return false; }

#ifndef Q_DATA_MEMORY_BARRIER
# define Q_DATA_MEMORY_BARRIER asm volatile("dmb sy\n":::"memory")
#endif
#ifndef Q_COMPILER_MEMORY_BARRIER
# define Q_COMPILER_MEMORY_BARRIER asm volatile("":::"memory")
#endif

inline bool QBasicAtomicInt::ref()
{
    int newValue;

    Q_COMPILER_MEMORY_BARRIER;
    newValue = __atomic_add_fetch(&_q_value, 1, __ATOMIC_ACQ_REL);
    Q_COMPILER_MEMORY_BARRIER;

    return newValue != 0;
}

inline bool QBasicAtomicInt::deref()
{
    int newValue;

    Q_COMPILER_MEMORY_BARRIER;
    newValue = __atomic_sub_fetch(&_q_value, 1, __ATOMIC_ACQ_REL);
    Q_COMPILER_MEMORY_BARRIER;

    return newValue != 0;
}

inline bool QBasicAtomicInt::testAndSetRelaxed(int expectedValue, int newValue)
{
    bool val;

    Q_COMPILER_MEMORY_BARRIER;
    val =  __atomic_compare_exchange_n (&_q_value, &expectedValue, newValue,
                                        false, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
    Q_COMPILER_MEMORY_BARRIER;
    return val;
}

inline int QBasicAtomicInt::fetchAndStoreRelaxed(int newValue)
{
    int val;
    Q_COMPILER_MEMORY_BARRIER;
    val = __atomic_exchange_n(&_q_value, newValue, __ATOMIC_RELAXED);
    Q_COMPILER_MEMORY_BARRIER;
    return val;
}

inline int QBasicAtomicInt::fetchAndAddRelaxed(int valueToAdd)
{
    int val;
    Q_COMPILER_MEMORY_BARRIER;
    val = __atomic_fetch_add(&_q_value, valueToAdd, __ATOMIC_RELAXED);
    Q_COMPILER_MEMORY_BARRIER;
    return val;
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelaxed(T *expectedValue, T *newValue)
{
    bool val;
    Q_COMPILER_MEMORY_BARRIER;
    val =  __atomic_compare_exchange_n (&_q_value, &expectedValue, newValue,
                                        false, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
    Q_COMPILER_MEMORY_BARRIER;
    return val;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelaxed(T *newValue)
{
    T *val;
    Q_COMPILER_MEMORY_BARRIER;
    val = __atomic_exchange_n(&_q_value, newValue, __ATOMIC_RELAXED);
    Q_COMPILER_MEMORY_BARRIER;
    return val;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelaxed(qptrdiff valueToAdd)
{
    T *val;
    Q_COMPILER_MEMORY_BARRIER;
    val = __atomic_fetch_add(&_q_value, valueToAdd, __ATOMIC_RELAXED);
    Q_COMPILER_MEMORY_BARRIER;
    return val;
}

inline bool QBasicAtomicInt::testAndSetAcquire(int expectedValue, int newValue)
{
    bool returnValue = testAndSetRelaxed(expectedValue, newValue);
    Q_DATA_MEMORY_BARRIER;
    return returnValue;
}

inline bool QBasicAtomicInt::testAndSetRelease(int expectedValue, int newValue)
{
    Q_DATA_MEMORY_BARRIER;
    return testAndSetRelaxed(expectedValue, newValue);
}

inline bool QBasicAtomicInt::testAndSetOrdered(int expectedValue, int newValue)
{
    Q_DATA_MEMORY_BARRIER;
    bool returnValue = testAndSetRelaxed(expectedValue, newValue);
    Q_COMPILER_MEMORY_BARRIER;
    return returnValue;
}

inline int QBasicAtomicInt::fetchAndStoreAcquire(int newValue)
{
    int returnValue = fetchAndStoreRelaxed(newValue);
    Q_DATA_MEMORY_BARRIER;
    return returnValue;
}

inline int QBasicAtomicInt::fetchAndStoreRelease(int newValue)
{
    Q_DATA_MEMORY_BARRIER;
    return fetchAndStoreRelaxed(newValue);
}

inline int QBasicAtomicInt::fetchAndStoreOrdered(int newValue)
{
    Q_DATA_MEMORY_BARRIER;
    int returnValue = fetchAndStoreRelaxed(newValue);
    Q_COMPILER_MEMORY_BARRIER;
    return returnValue;
}

inline int QBasicAtomicInt::fetchAndAddAcquire(int valueToAdd)
{
    int returnValue = fetchAndAddRelaxed(valueToAdd);
    Q_DATA_MEMORY_BARRIER;
    return returnValue;
}

inline int QBasicAtomicInt::fetchAndAddRelease(int valueToAdd)
{
    Q_DATA_MEMORY_BARRIER;
    return fetchAndAddRelaxed(valueToAdd);
}

inline int QBasicAtomicInt::fetchAndAddOrdered(int valueToAdd)
{
    Q_DATA_MEMORY_BARRIER;
    int returnValue = fetchAndAddRelaxed(valueToAdd);
    Q_COMPILER_MEMORY_BARRIER;
    return returnValue;
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetAcquire(T *expectedValue, T *newValue)
{
    bool returnValue = testAndSetRelaxed(expectedValue, newValue);
    Q_DATA_MEMORY_BARRIER;
    return returnValue;
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelease(T *expectedValue, T *newValue)
{
    Q_DATA_MEMORY_BARRIER;
    return testAndSetRelaxed(expectedValue, newValue);
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetOrdered(T *expectedValue, T *newValue)
{
    Q_DATA_MEMORY_BARRIER;
    bool returnValue = testAndSetAcquire(expectedValue, newValue);
    Q_COMPILER_MEMORY_BARRIER;
    return returnValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreAcquire(T *newValue)
{
    T *returnValue = fetchAndStoreRelaxed(newValue);
    Q_DATA_MEMORY_BARRIER;
    return returnValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelease(T *newValue)
{
    Q_DATA_MEMORY_BARRIER;
    return fetchAndStoreRelaxed(newValue);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreOrdered(T *newValue)
{
    Q_DATA_MEMORY_BARRIER;
    T *returnValue = fetchAndStoreRelaxed(newValue);
    Q_COMPILER_MEMORY_BARRIER;
    return returnValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddAcquire(qptrdiff valueToAdd)
{
    T *returnValue = fetchAndAddRelaxed(valueToAdd);
    Q_DATA_MEMORY_BARRIER;
    return returnValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelease(qptrdiff valueToAdd)
{
    Q_DATA_MEMORY_BARRIER;
    return fetchAndAddRelaxed(valueToAdd);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddOrdered(qptrdiff valueToAdd)
{
    Q_DATA_MEMORY_BARRIER;
    T *returnValue = fetchAndAddRelaxed(valueToAdd);
    Q_COMPILER_MEMORY_BARRIER;
    return returnValue;
}

#undef Q_DATA_MEMORY_BARRIER
#undef Q_COMPILER_MEMORY_BARRIER

QT_END_NAMESPACE

QT_END_HEADER

#endif // QATOMIC_AARCH64_H
