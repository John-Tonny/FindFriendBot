/****************************************************************************
MIT License

Copyright (c) 2019 TOK

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
****************************************************************************/
/*
 * implement some lock operator, support multi platform, include mutex/rw/spin lock;
 */
#ifndef __LOCK_H
#define __LOCK_H
#include <cstddef>
#ifdef WIN32
#ifndef __WINDOWS
#define __WINDOWS
#endif //__WINDOWS
#endif //WIN32

#if (defined __LINUX || defined __MACX) && !defined __NO_THREAD
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#define	__TOK_PTHREAD 
#elif defined __WINDOWS && !defined __NO_THREAD
#include <Windows.h>
#include <condition_variable>	//	rw_lock for windows version
#define	__TOK_WIN_THREAD
#elif defined __NO_THREAD
#define	__TOK_NO_THREAD
#endif //__LINUX

namespace tok
{
	//	reference count, thread safe
	struct _Refcount_Base
	{
#ifdef __TOK_WIN_THREAD	
		typedef long	_RC_t;
#else
		typedef size_t	_RC_t;
#endif //__TOK_WIN_THREAD
		//	data member reference count
		volatile		_RC_t		ref_count_;

#ifdef __TOK_PTHREAD	
		pthread_mutex_t	ref_count_lock_;
		_Refcount_Base(_RC_t __n) : ref_count_(__n) {  pthread_mutex_init(&ref_count_lock_, 0);}
#else
		_Refcount_Base(_RC_t __n) : ref_count_(__n) { }
#endif //__TOK_PTHREAD

#ifdef __TOK_PTHREAD	
		void incr()	
		{ 
			pthread_mutex_lock(&ref_count_lock_);
			++ref_count_;
			pthread_mutex_unlock(&ref_count_lock_);
		}
		_RC_t decr() 
		{ 
			pthread_mutex_lock(&ref_count_lock_);
			volatile _RC_t __tmp = --ref_count_;
			pthread_mutex_unlock(&ref_count_lock_);
			return __tmp;
		}
#elif defined __TOK_WIN_THREAD
		void incr()	{ ::InterlockedIncrement((_RC_t*)&ref_count_); }
		_RC_t decr() { return ::InterlockedDecrement((_RC_t*)&ref_count_); }
#else	//	no use thread
		void incr()	{  ++ref_count_;}
		_RC_t decr() { return --ref_count_; }
#endif //__TOK_WIN_THREAD
	};

	//	atomic swap on unsigned long
#ifdef __TOK_PTHREAD
	// we use a template here only to get a unique initialized instance.
	template<int __dummy>
	struct _Swap_lock_struct 
	{
		static pthread_mutex_t _S_swap_lock;
	};
	template<int __dummy>
	pthread_mutex_t _Swap_lock_struct<__dummy>::_S_swap_lock = PTHREAD_MUTEX_INITIALIZER;
	// this should be portable, but performance is expected
	// to be quite awful.  This really needs platform specific
	// code.
	
	inline unsigned long _Atomic_swap(unsigned long * __p, unsigned long __q) 
	{
		pthread_mutex_lock(&_Swap_lock_struct<0>::_S_swap_lock);
		unsigned long __result = *__p;
		*__p = __q;
		pthread_mutex_unlock(&_Swap_lock_struct<0>::_S_swap_lock);
		return __result;
	}
#elif defined __TOK_WIN_THREAD
	inline unsigned long _Atomic_swap(unsigned long* __p,unsigned long __q)
	{
		return (unsigned long) ::InterlockedExchange((LPLONG)__p,(LONG)__q);
	}
#else 
	static inline unsigned long _Atomic_swap(unsigned long * __p, unsigned long __q) {
		unsigned long __result = *__p;
		*__p = __q;
		return __result;
	}
#endif //__TOK_WIN_THREAD/

	struct mutex_lock
	{
		mutex_lock() { initialize(); }
		~mutex_lock() { uninitialize(); }
#ifdef __TOK_WIN_THREAD
		//	we also use windows  CRITICAL SECTION and spin lock to implement it.
		CRITICAL_SECTION lock_;
		//	set the spin count times is 4000,that means after check 4000 times, if we can not visit the 
		//	resource also, just switch the kernel mode. we also can use SetCriticalSectionSpinCount set the spin count times.
#pragma warning(suppress: 6031)
		void initialize() { if (0){ InitializeCriticalSectionAndSpinCount(&lock_,0x000004000);} else {InitializeCriticalSection(&lock_);} }
		int acquire_lock() { EnterCriticalSection(&lock_); return 0;}
		int tryacquire_lock() { TryEnterCriticalSection(&lock_); return 0; }
		int release_lock() { LeaveCriticalSection(&lock_); return 0; }
		void uninitialize() { DeleteCriticalSection(&lock_); }
#elif defined __TOK_PTHREAD
		pthread_mutex_t lock_;
		void initialize()   { pthread_mutex_init(&lock_, NULL); }
		int acquire_lock() { return pthread_mutex_lock(&lock_); }
		int tryacquire_lock() { return pthread_mutex_trylock(&lock_); }
		int release_lock() { return pthread_mutex_unlock(&lock_); }
		void uninitialize() { }
#else
		void initialize()   {  }
		int acquire_lock() {  return 0; }
		int tryacquire_lock() {  return 0; }
		int release_lock() {  return 0; }
		void uninitialize() { }
#endif //__TOK_PTHREAD
	};

	struct auto_lock
	{
		mutex_lock&	lock_;
		auto_lock(mutex_lock& __lock) : lock_(__lock) { lock_.acquire_lock(); }
		~auto_lock() { lock_.release_lock(); }
	private:
		void operator=(const auto_lock&);
		auto_lock(const auto_lock&);
	};

	struct spin_lock
	{
		spin_lock() { initialize(); }
		~spin_lock() { uninitialize(); }
#ifdef __TOK_WIN_THREAD
		void initialize()   {  }
		int acquire_lock() {  return 0; }
		int tryacquire_lock() {  return 0; }
		int release_lock() {  return 0; }
		void uninitialize() { }
#elif defined __TOK_PTHREAD
#if defined __LINUX
		pthread_spinlock_t spinlock_;
		void initialize()   {  pthread_spin_init(&spinlock_, 0); }
		int acquire_lock() {  return pthread_spin_lock(&spinlock_); }
		int release_lock() {  return pthread_spin_unlock(&spinlock_); }
		void uninitialize() { pthread_spin_destroy(&spinlock_); }
#elif defined __MACX
        void initialize()   {  }
        int acquire_lock() {  return 0; }
        int tryacquire_lock() {  return 0; }
        int release_lock() {  return 0; }
        void uninitialize() { }
#endif //__LINUX
#else
		void initialize()   {  }
		int acquire_lock() {  return 0; }
		int tryacquire_lock() {  return 0; }
		int release_lock() {  return 0; }
		void uninitialize() { }
#endif //__TOK_WIN_THREAD
	};

	struct rw_lock
	{
		rw_lock() { initialize(); }
		~rw_lock() { uninitialize(); }
#ifdef __TOK_WIN_THREAD
		SRWLOCK	rwlock_;
		void initialize()   {  InitializeSRWLock(&rwlock_); }
		int acquire_r_lock() {   AcquireSRWLockShared(&rwlock_); return 0; }
		int release_r_lock() {   ReleaseSRWLockShared(&rwlock_); return 0; }
		int acquire_w_lock() {   AcquireSRWLockExclusive(&rwlock_); return 0; }
		int release_w_lock() {   ReleaseSRWLockExclusive(&rwlock_); return 0; }
		void uninitialize() { }
#elif defined __TOK_PTHREAD
		pthread_rwlock_t rwlock_;
		void initialize()   {  pthread_rwlock_init(&rwlock_, 0); }
		int acquire_r_lock() {  return pthread_rwlock_rdlock(&rwlock_); }
		int release_r_lock() {  return pthread_rwlock_unlock(&rwlock_); }
		int acquire_w_lock() {  return pthread_rwlock_wrlock(&rwlock_); }
		int release_w_lock() {  return pthread_rwlock_unlock(&rwlock_); }
		void uninitialize() { pthread_rwlock_destroy(&rwlock_); }
#else
		 void initialize()   { } 
		 void uninitialize() { }  
#endif //__TOK_WIN_THREAD
	};
}
#endif // __LOCK_H
