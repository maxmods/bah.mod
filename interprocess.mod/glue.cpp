/*
 Copyright (c) 2009-2012 Bruce A Henderson
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of Bruce A Henderson nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define CREATE_ONLY    0
#define OPEN_OR_CREATE 1
#define OPEN_ONLY      2


using namespace boost::interprocess;
using namespace boost::posix_time;

class MaxScopedLock;
class MaxNamedMutex;

extern "C" {

#include "blitz.h"

	BBObject * _bah_interprocess_TInterprocessException__create(BBString * message, int errorCode, int nativeError);

	shared_memory_object * bmx_sharedmemoryobject_create(int access, BBString * name, boost::interprocess::mode_t mode);
	void bmx_sharedmemoryobject_free(shared_memory_object * shm);
	int bmx_sharedmemoryobject_remove(BBString * name);
	void bmx_sharedmemoryobject_truncate(shared_memory_object * shm, BBInt64 offset);
	BBString * bmx_sharedmemoryobject_getname(shared_memory_object * shm);
	int bmx_sharedmemoryobject_getsize(shared_memory_object * shm, BBInt64 * size);
	boost::interprocess::mode_t bmx_sharedmemoryobject_getmode(shared_memory_object * shm);

	mapped_region * bmx_mapped_region_createshm(shared_memory_object * mapping, boost::interprocess::mode_t mode, BBInt64 offset, std::size_t size, void * address);
	std::size_t bmx_mapped_region_getsize(mapped_region * region);
	void * bmx_mapped_region_getaddress(mapped_region * region);
	int bmx_mapped_region_shrinkby(mapped_region * region, BBInt64 bytes, int fromBack);
	boost::interprocess::mode_t bmx_mapped_region_getmode(mapped_region * region);
	int bmx_mapped_region_flush(mapped_region * region, std::size_t mappingOffset, std::size_t numBytes, int async);
	void bmx_mapped_region_free(mapped_region * region);

	named_semaphore * bmx_named_semphore_create(int access, BBString * name, int initialCount);
	void bmx_named_semaphore_post(named_semaphore * semaphore);
	void bmx_named_semaphore_wait(named_semaphore * semaphore);
	int bmx_named_semaphore_trywait(named_semaphore * semaphore);
	int bmx_named_semaphore_timedwait(named_semaphore * semaphore, int time);
	int bmx_named_semaphore_remove(BBString * name);
	void bmx_named_semaphore_free(named_semaphore * semaphore);

	named_condition * bmx_named_condition_create(int access, BBString * name);
	void bmx_named_condition_notifyone(named_condition * cond);
	void bmx_named_condition_notifyall(named_condition * cond);
	void bmx_named_condition_wait(named_condition * cond, MaxScopedLock * lock);
	int bmx_named_condition_timedwait(named_condition * cond, MaxScopedLock * lock, int time);
	int bmx_named_condition_remove(BBString * name);
	void bmx_named_condition_free(named_condition * cond);

	MaxScopedLock * bmx_scoped_lock_create(MaxNamedMutex * mutex);
	void bmx_scoped_lock_free(MaxScopedLock * lock);
	void bmx_scoped_lock_lock(MaxScopedLock * lock);
	int bmx_scoped_lock_trylock(MaxScopedLock * lock);
	int bmx_scoped_lock_timedlock(MaxScopedLock * lock, int time);
	void bmx_scoped_lock_unlock(MaxScopedLock * lock);

	MaxNamedMutex * bmx_named_mutex_create(int access, BBString * name);
	void bmx_named_mutex_unlock(MaxNamedMutex * mutex);
	void bmx_named_mutex_lock(MaxNamedMutex * mutex);
	int bmx_named_mutex_trylock(MaxNamedMutex * mutex);
	int bmx_named_mutex_timedlock(MaxNamedMutex * mutex, int time);
	int bmx_named_mutex_remove(BBString * name);
	void bmx_named_mutex_free(MaxNamedMutex * mutex);

}

class MaxNamedMutex
{
public:
	MaxNamedMutex(create_only_t c, const char *name)
		: mutex(c, name)
	{}

	MaxNamedMutex(open_or_create_t c, const char *name)
		: mutex(c, name)
	{}

	MaxNamedMutex(open_only_t c, const char *name)
		: mutex(c, name)
	{}
	
	named_mutex & GetMutex() {
		return mutex;
	}

private:
	named_mutex mutex;
};

class MaxScopedLock
{
public:
	MaxScopedLock(named_mutex & mutex)
	{
		_lock = new scoped_lock<named_mutex>(mutex);
	}
	
	~MaxScopedLock()
	{
		delete _lock;
	}
	
	scoped_lock<named_mutex> * GetLock() {
		return _lock;
	}
	
	void lock() {
		_lock->lock();
	}

private:
	scoped_lock<named_mutex> * _lock;
};

// ********************************************

void bmx_throw_interprocess_exception(interprocess_exception &e) {
	bbExThrow(_bah_interprocess_TInterprocessException__create(bbStringFromCString(e.what()), e.get_error_code(), 		e.get_native_error()));
}

shared_memory_object * bmx_sharedmemoryobject_create(int access, BBString * name, boost::interprocess::mode_t mode) {
	char * p = bbStringToCString(name);
	shared_memory_object * shm = 0;
	
	try{
	
		switch(access) {
			case CREATE_ONLY:
				shm = new shared_memory_object(create_only, p, mode);
				break;
			case OPEN_OR_CREATE:
				shm = new shared_memory_object(open_or_create, p, mode);
				break;
			case OPEN_ONLY:
				shm = new shared_memory_object(open_only, p, mode);
				break;
		}
		
	} catch(interprocess_exception &e){
		bbMemFree(p);
		bmx_throw_interprocess_exception(e);
	}
	
	bbMemFree(p);
	return shm;
}

void bmx_sharedmemoryobject_free(shared_memory_object * shm) {
	delete shm;
}

int bmx_sharedmemoryobject_remove(BBString * name) {
	char * p = bbStringToCString(name);
	bool res = shared_memory_object::remove(p);
	bbMemFree(p);
	return static_cast<int>(res);
}

void bmx_sharedmemoryobject_truncate(shared_memory_object * shm, BBInt64 offset) {
	shm->truncate(offset);
}

BBString * bmx_sharedmemoryobject_getname(shared_memory_object * shm) {
	return bbStringFromCString(shm->get_name());
}

int bmx_sharedmemoryobject_getsize(shared_memory_object * shm, BBInt64 * size) {
	offset_t s;
	bool res = shm->get_size(s);
	*size = static_cast<BBInt64>(s);
	return static_cast<int>(res);
}

boost::interprocess::mode_t bmx_sharedmemoryobject_getmode(shared_memory_object * shm) {
	return shm->get_mode();
}


// ********************************************

mapped_region * bmx_mapped_region_createshm(shared_memory_object * mapping, boost::interprocess::mode_t mode, BBInt64 offset, std::size_t size, void * address) {
	try{
		return new mapped_region(*mapping, mode, offset, size, address);
	} catch(interprocess_exception &e){
		bmx_throw_interprocess_exception(e);
	}
}

std::size_t bmx_mapped_region_getsize(mapped_region * region) {
	return region->get_size();
}

int bmx_mapped_region_shrinkby(mapped_region * region, BBInt64 bytes, int fromBack) {
	static_cast<int>(region->shrink_by(bytes, static_cast<bool>(fromBack)));
}

void * bmx_mapped_region_getaddress(mapped_region * region) {
	return region->get_address();
}

boost::interprocess::mode_t bmx_mapped_region_getmode(mapped_region * region) {
	return region->get_mode();
}

int bmx_mapped_region_flush(mapped_region * region, std::size_t mappingOffset, std::size_t numBytes, int async) {
	return static_cast<int>(region->flush(mappingOffset, numBytes, static_cast<bool>(async)));
}

void bmx_mapped_region_free(mapped_region * region) {
	delete region;
}


// ********************************************

named_semaphore * bmx_named_semphore_create(int access, BBString * name, int initialCount) {
	char * p = bbStringToCString(name);
	named_semaphore * sem = 0;
	
	try{
	
		switch(access) {
			case CREATE_ONLY:
				sem = new named_semaphore(create_only, p, initialCount);
				break;
			case OPEN_OR_CREATE:
				sem = new named_semaphore(open_or_create, p, initialCount);
				break;
			case OPEN_ONLY:
				sem = new named_semaphore(open_only, p);
				break;
		}
		
	} catch(interprocess_exception &e){
		bbMemFree(p);
		bmx_throw_interprocess_exception(e);
	}
	
	bbMemFree(p);
	return sem;
}

void bmx_named_semaphore_post(named_semaphore * semaphore) {
	semaphore->post();
}

void bmx_named_semaphore_wait(named_semaphore * semaphore) {
	semaphore->wait();
}

int bmx_named_semaphore_trywait(named_semaphore * semaphore) {
	return static_cast<int>(semaphore->try_wait());
}

int bmx_named_semaphore_timedwait(named_semaphore * semaphore, int time) {
	std::time_t tt(time);
	return static_cast<int>(semaphore->timed_wait(from_time_t(tt)));
}

int bmx_named_semaphore_remove(BBString * name) {
	char * p = bbStringToCString(name);
	bool res = named_semaphore::remove(p);
	bbMemFree(p);
	return static_cast<int>(res);
}

void bmx_named_semaphore_free(named_semaphore * semaphore) {
	delete semaphore;
}

// ********************************************

named_condition * bmx_named_condition_create(int access, BBString * name) {
	char * p = bbStringToCString(name);
	named_condition * cond = 0;
	
	try{
	
		switch(access) {
			case CREATE_ONLY:
				cond = new named_condition(create_only, p);
				break;
			case OPEN_OR_CREATE:
				cond = new named_condition(open_or_create, p);
				break;
			case OPEN_ONLY:
				cond = new named_condition(open_only, p);
				break;
		}
		
	} catch(interprocess_exception &e){
		bbMemFree(p);
		bmx_throw_interprocess_exception(e);
	}
	
	bbMemFree(p);
	return cond;
}

void bmx_named_condition_notifyone(named_condition * cond) {
	cond->notify_one();
}

void bmx_named_condition_notifyall(named_condition * cond) {
	cond->notify_all();
}

void bmx_named_condition_wait(named_condition * cond, MaxScopedLock * lock) {
	cond->wait(*lock->GetLock());
}

int bmx_named_condition_timedwait(named_condition * cond, MaxScopedLock * lock, int time) {
	std::time_t tt(time);
	return static_cast<int>(cond->timed_wait(*lock->GetLock(), from_time_t(tt)));
}

int bmx_named_condition_remove(BBString * name) {
	char * p = bbStringToCString(name);
	bool res = named_condition::remove(p);
	bbMemFree(p);
	return static_cast<int>(res);
}

void bmx_named_condition_free(named_condition * cond) {
	delete cond;
}


// ********************************************

MaxScopedLock * bmx_scoped_lock_create(MaxNamedMutex * mutex) {
	return new MaxScopedLock(mutex->GetMutex());
}

void bmx_scoped_lock_free(MaxScopedLock * lock) {
	delete lock;
}

void bmx_scoped_lock_lock(MaxScopedLock * lock) {
	lock->GetLock()->lock();
}

int bmx_scoped_lock_trylock(MaxScopedLock * lock) {
	return static_cast<int>(lock->GetLock()->try_lock());
}

int bmx_scoped_lock_timedlock(MaxScopedLock * lock, int time) {
	std::time_t tt(time);
	return static_cast<int>(lock->GetLock()->timed_lock(from_time_t(tt)));
}

void bmx_scoped_lock_unlock(MaxScopedLock * lock) {
	lock->GetLock()->unlock();
}

// ********************************************

MaxNamedMutex * bmx_named_mutex_create(int access, BBString * name) {
	char * p = bbStringToCString(name);
	MaxNamedMutex * mutex = 0;
	try{
	
		switch(access) {
			case CREATE_ONLY:
				mutex = new MaxNamedMutex(create_only, p);
				break;
			case OPEN_OR_CREATE:
				mutex = new MaxNamedMutex(open_or_create, p);
				break;
			case OPEN_ONLY:
				mutex = new MaxNamedMutex(open_only, p);
				break;
		}
		
	} catch(interprocess_exception &e){
		bbMemFree(p);
		bmx_throw_interprocess_exception(e);
	}

	bbMemFree(p);
	return mutex;
}

void bmx_named_mutex_unlock(MaxNamedMutex * mutex) {
	mutex->GetMutex().unlock();
}

void bmx_named_mutex_lock(MaxNamedMutex * mutex) {
	mutex->GetMutex().lock();
}

int bmx_named_mutex_trylock(MaxNamedMutex * mutex) {
	return static_cast<int>(mutex->GetMutex().try_lock());
}

int bmx_named_mutex_timedlock(MaxNamedMutex * mutex, int time) {
	std::time_t tt(time);
	return static_cast<int>(mutex->GetMutex().timed_lock(from_time_t(tt)));
}

int bmx_named_mutex_remove(BBString * name) {
	char * p = bbStringToCString(name);
	bool res = named_mutex::remove(p);
	bbMemFree(p);
	return static_cast<int>(res);
}

void bmx_named_mutex_free(MaxNamedMutex * mutex) {
	delete mutex;
}


