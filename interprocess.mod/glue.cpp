/*
 Copyright (c) 2009 Bruce A Henderson
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


using namespace boost::interprocess;


extern "C" {

#include "blitz.h"

	BBObject * _bah_interprocess_TInterprocessException__create(BBString * message, int errorCode, int nativeError);

	shared_memory_object * bmx_sharedmemoryobject_create(int access, BBString * name, boost::interprocess::mode_t mode);
	void bmx_sharedmemoryobject_free(shared_memory_object * shm);
	bool bmx_sharedmemoryobject_remove(BBString * name);
	void bmx_sharedmemoryobject_truncate(shared_memory_object * shm, BBInt64 offset);
	BBString * bmx_sharedmemoryobject_getname(shared_memory_object * shm);
	bool bmx_sharedmemoryobject_getsize(shared_memory_object * shm, BBInt64 * size);
	boost::interprocess::mode_t bmx_sharedmemoryobject_getmode(shared_memory_object * shm);

	mapped_region * bmx_mapped_region_createshm(shared_memory_object * mapping, boost::interprocess::mode_t mode, BBInt64 offset, std::size_t size, void * address);
	std::size_t bmx_mapped_region_getsize(mapped_region * region);
	void * bmx_mapped_region_getaddress(mapped_region * region);
	void bmx_mapped_region_getoffset(mapped_region * region, BBInt64 * v);
	boost::interprocess::mode_t bmx_mapped_region_getmode(mapped_region * region);
	bool bmx_mapped_region_flush(mapped_region * region, std::size_t mappingOffset, std::size_t numBytes);
	void bmx_mapped_region_free(mapped_region * region);

}

void bmx_throw_interprocess_exception(interprocess_exception &e) {
	bbExThrow(_bah_interprocess_TInterprocessException__create(bbStringFromCString(e.what()), e.get_error_code(), 		e.get_native_error()));
}

shared_memory_object * bmx_sharedmemoryobject_create(int access, BBString * name, boost::interprocess::mode_t mode) {
	char * p = bbStringToCString(name);
	shared_memory_object * shm = 0;
	
	try{
	
		switch(access) {
			case 0:
				shm = new shared_memory_object(create_only, p, mode);
				break;
			case 1:
				shm = new shared_memory_object(open_or_create, p, mode);
				break;
			case 2:
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

bool bmx_sharedmemoryobject_remove(BBString * name) {
	char * p = bbStringToCString(name);
	bool res = shared_memory_object::remove(p);
	bbMemFree(p);
	return res;
}

void bmx_sharedmemoryobject_truncate(shared_memory_object * shm, BBInt64 offset) {
	shm->truncate(offset);
}

BBString * bmx_sharedmemoryobject_getname(shared_memory_object * shm) {
	return bbStringFromCString(shm->get_name());
}

bool bmx_sharedmemoryobject_getsize(shared_memory_object * shm, BBInt64 * size) {
	offset_t s;
	bool res = shm->get_size(s);
	*size = static_cast<BBInt64>(s);
	return res;
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

void * bmx_mapped_region_getaddress(mapped_region * region) {
	return region->get_address();
}

void bmx_mapped_region_getoffset(mapped_region * region, BBInt64 * v) {
	*v = region->get_offset();
}

boost::interprocess::mode_t bmx_mapped_region_getmode(mapped_region * region) {
	return region->get_mode();
}

bool bmx_mapped_region_flush(mapped_region * region, std::size_t mappingOffset, std::size_t numBytes) {
	return region->flush(mappingOffset, numBytes);
}

void bmx_mapped_region_free(mapped_region * region) {
	delete region;
}


