' Copyright (c) 2009-2012 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Rem
bbdoc: Interprocess Communication
End Rem
Module BaH.Interprocess

ModuleInfo "Version: 1.01"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: Wrapper - 2009-2012 Bruce A Henderson"

ModuleInfo "History: 1.01"
ModuleInfo "History: Updated to boost 1.52"
ModuleInfo "History: Removed region GetOffset(). Added ShrinkBy()."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. (Boost 1.45)"

ModuleInfo "CC_OPTS: -fexceptions"

?win32
ModuleInfo "LD_OPTS: -lole32"
ModuleInfo "LD_OPTS: -loleaut32"
?

Import "common.bmx"



Type TMemoryMappable

	Field objectPtr:Byte Ptr
	
End Type


Rem
bbdoc: A shared memory object.
End Rem
Type TSHMO Extends TMemoryMappable

	Rem
	bbdoc: 
	End Rem
	Function CreateSHMO:TSHMO(access:Int, name:String, Mode:Int)
		Return New TSHMO.Create(access, name, Mode)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TSHMO(access:Int, name:String, Mode:Int)
		objectPtr = bmx_sharedmemoryobject_create(access, name, Mode)
		Return Self
	End Method
	
	Rem
	bbdoc: Sets the size of the shared memory mapping.
	End Rem
	Method Truncate(offset:Long)
		bmx_sharedmemoryobject_truncate(objectPtr, offset)
	End Method
	
	Rem
	bbdoc: Returns the name of the file.
	End Rem
	Method GetName:String()
		Return bmx_sharedmemoryobject_getname(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the size of the shared memory.
	End Rem
	Method GetSize:Int(size:Long Var)
		Return bmx_sharedmemoryobject_getsize(objectPtr, Varptr size)
	End Method
	
	Rem
	bbdoc: Returns the access mode.
	End Rem
	Method GetMode:Int()
		Return bmx_sharedmemoryobject_getmode(objectPtr)
	End Method
	
	Rem
	bbdoc: Frees this shared memory object.
	about: All mapped regions are still valid after destruction. This method will deallocate any system resources allocated by the system for
	use by this process for this resource. The resource can still be opened again by calling constructor. To erase the resource from the
	system use Remove().
	End Rem
	Method Free()
		If objectPtr Then
			bmx_sharedmemoryobject_free(objectPtr)
			objectPtr = Null
		End If
	End Method
	
	Rem
	bbdoc: Erases a shared memory object from the system.
	returns: False on error.
	End Rem
	Function Remove:Int(name:String)
		Return bmx_sharedmemoryobject_remove(name)
	End Function
	
	Method Delete()
		Free()
	End Method

End Type

Rem
bbdoc: Represents a portion or region created from a memory mappable object.
End Rem
Type TMappedRegion
	
	Field objectPtr:Byte Ptr
	
	Rem
	bbdoc: 
	End Rem
	Function CreateMappedRegion:TMappedRegion(mapping:TMemoryMappable, Mode:Int, offset:Long = 0, size:Int = 0, address:Byte Ptr = Null)
		Return New TMappedRegion.Create(mapping, Mode, offset, size, address)
	End Function
	
	Rem
	bbdoc: Creates a mapping region of the mapped memory @mapping, starting in offset @offset, and the mapping's size will be @size.
	about: The mapping can be opened for read only, read-write or copy-on-write.
	If an address is specified, both the offset and the address must be multiples of the page size.
	The OS could allocate more pages than size/page_size(), but GetAddress() will always return the address passed in this method (if not null)
	and GetSize() will return the specified size.
	End Rem
	Method Create:TMappedRegion(mapping:TMemoryMappable, Mode:Int, offset:Long = 0, size:Int = 0, address:Byte Ptr = Null)
		If TSHMO(mapping) Then
			If address Then
				objectPtr = bmx_mapped_region_createshm(mapping.objectPtr, Mode, offset, size, address)
			Else
				objectPtr = bmx_mapped_region_createshm(mapping.objectPtr, Mode, offset, size, Null)
			End If
		End If
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the size of the mapping.
	End Rem
	Method GetSize:Int()
		Return bmx_mapped_region_getsize(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the base address of the mapping.
	End Rem
	Method GetAddress:Byte Ptr()
		Return bmx_mapped_region_getaddress(objectPtr)
	End Method
	
	Rem
	bbdoc: Shrinks current mapped region.
	returns: True on success.
	about: If after shrinking there is no longer need for a previously mapped memory page, accessing that page can trigger a segmentation fault.
	Depending on the OS, this operation might fail (XSI shared memory), it can decommit storage and free a portion of the virtual
	address space (e.g.POSIX) or this method can release some physical memory wihout freeing any virtual address
	space (Windows).
	End Rem
	Method ShrinkBy:Int(bytes:Long, fromBack:Int = True)
		Return bmx_mapped_region_shrinkby(objectPtr, bytes, fromBack)
	End Method
	
	Rem
	bbdoc: Returns the mode of the mapping used to construct the mapped region.
	End Rem
	Method GetMode:Int()
		Return bmx_mapped_region_getmode(objectPtr)
	End Method
	
	Rem
	bbdoc: Flushes to the disk a byte range within the mapped memory.
	returns: False if operation could not be performed.
	about: If @async is True, the method will return before flushing operation is completed. If @async is False, method will return once
	data has been written into the underlying device (i.e., in mapped files OS cached information is
	written to disk)
	End Rem
	Method Flush:Int(mappingOffset:Int = 0, numBytes:Int = 0, async:Int = True)
		Return bmx_mapped_region_flush(objectPtr, mappingOffset, numBytes, async)
	End Method
		
	Rem
	bbdoc: 
	End Rem
	Method Free()
		If objectPtr Then
			bmx_mapped_region_free(objectPtr)
			objectPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type

Rem
bbdoc: A mutex with a global name, so it can be found from different processes.
End Rem
Type TNamedMutex

	Field objectPtr:Byte Ptr
	
	Rem
	bbdoc: 
	End Rem
	Function CreateNamedMutex:TNamedMutex(access:Int, name:String)
		Return New TNamedMutex.Create(access, name)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TNamedMutex(access:Int, name:String)
		objectPtr = bmx_named_mutex_create(access, name)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Unlock()
		bmx_named_mutex_unlock(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Lock()
		bmx_named_mutex_lock(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method TryLock:Int()
		Return bmx_named_mutex_trylock(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method TimedLock:Int(time:Int)
		Return bmx_named_mutex_timedlock(objectPtr, time)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function Remove:Int(name:String)
		Return bmx_named_mutex_remove(name)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Free()
		If objectPtr Then
			bmx_named_mutex_free(objectPtr)
			objectPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method

End Type

Rem
bbdoc: A global condition variable that can be created by name.
End Rem
Type TNamedCondition

	Field objectPtr:Byte Ptr
	
	Rem
	bbdoc: 
	End Rem
	Function CreateNamedSemaphore:TNamedCondition(access:Int, name:String)
		Return New TNamedCondition.Create(access, name)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TNamedCondition(access:Int, name:String)
		objectPtr = bmx_named_condition_create(access, name)
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method NotifyOne()
		bmx_named_condition_notifyone(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method NotifyAll()
		bmx_named_condition_notifyall(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Wait(lock:TScopedLock)
		bmx_named_condition_wait(objectPtr, lock.objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method TimedWait:Int(lock:TScopedLock, time:Int)
		Return bmx_named_condition_timedwait(objectPtr, lock.objectPtr, time)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function Remove:Int(name:String)
		Return bmx_named_condition_remove(name)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Free()
		If objectPtr Then
			bmx_named_condition_free(objectPtr)
			objectPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method

End Type

Rem
bbdoc: A semaphore with a global name, so it can be found from different processes.
about: Allows several resource sharing patterns and efficient acknowledgment mechanisms.
End Rem
Type TNamedSempahore

	Field objectPtr:Byte Ptr

	Rem
	bbdoc: 
	End Rem
	Function CreateNamedSemaphore:TNamedSempahore(access:Int, name:String, initialCount:Int = 1)
		Return New TNamedSempahore.Create(access, name, initialCount)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TNamedSempahore(access:Int, name:String, initialCount:Int = 1)
		objectPtr = bmx_named_semphore_create(access, name, initialCount)
		Return Self
	End Method

	Rem
	bbdoc: Increments the semaphore count.
	about: If there are processes/threads blocked waiting for the semaphore, then one of these
	processes will return successfully from its wait function. If there is an error a
	TInterprocessException is thrown.
	End Rem
	Method Post()
		bmx_named_semaphore_post(objectPtr)
	End Method
	
	Rem
	bbdoc: Decrements the semaphore.
	about: If the semaphore value is not greater than zero, then the calling process/thread blocks
	until it can decrement the counter. If there is an error a TInterprocessException is thrown.
	End Rem
	Method Wait()
		bmx_named_semaphore_wait(objectPtr)
	End Method
	
	Rem
	bbdoc: Decrements the semaphore if the semaphore's value is greater than zero and returns true.
	about: If the value is not greater than zero returns false. If there is an error a
	TInterprocessException is thrown.
	End Rem
	Method TryWait:Int()
		Return bmx_named_semaphore_trywait(objectPtr)
	End Method
	
	Rem
	bbdoc: Decrements the semaphore if the semaphore's value is greater than zero and returns true.
	about: Otherwise, waits for the semaphore to the posted or the timeout expires. If the timeout
	expires, the method returns false. If the semaphore is posted the function returns true.
	If there is an error throws TSemException
	End Rem
	Method TimedWait:Int(time:Int)
		Return bmx_named_semaphore_timedwait(objectPtr, time)
	End Method

	Rem
	bbdoc: Erases a named semaphore from the system.
	returns: False on error.
	End Rem
	Function Remove:Int(name:String)
		Return bmx_named_semaphore_remove(name)
	End Function

	Rem
	bbdoc: 
	End Rem
	Method Free()
		If objectPtr Then
			bmx_named_semaphore_free(objectPtr)
			objectPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type

Rem
bbdoc: A scoped llock is meant to carry out the tasks for locking, unlocking, try-locking and timed-locking (recursive or not) for the Mutex.
about: 
End Rem
Type TScopedLock

	Field objectPtr:Byte Ptr

	Rem
	bbdoc: 
	End Rem
	Function CreateLock:TScopedLock(mutex:TNamedMutex)
		Return New TScopedLock.Create(mutex)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TScopedLock(mutex:TNamedMutex)
		objectPtr = bmx_scoped_lock_create(mutex.objectPtr)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Lock()
		bmx_scoped_lock_lock(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method TryLock:Int()
		Return bmx_scoped_lock_trylock(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method TimedLock:Int(time:Int)
		Return bmx_scoped_lock_timedlock(objectPtr, time)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Unlock()
		bmx_scoped_lock_unlock(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Free()
		If objectPtr Then
			bmx_scoped_lock_free(objectPtr)
			objectPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method

End Type

Rem
bbdoc: A memory bank using shared memory
about: Allocates or connects to existing shared memory of the specified @name.
Note that the memory is not freed from the system when this object is destroyed. You should call either Remove() or #RemoveSharedBank.
End Rem
Type TSharedBank Extends TBank

	Field name:String
	Field shared:TSHMO
	Field region:TMappedRegion
	
	Function CreateShared:TSharedBank(name:String, size:Int)
		Return New TSharedBank._Create(name, size)
	End Function
	
	Method _Create:TSharedBank(name:String, size:Int)
	
		Self.name = name
		
		' create the shared object
		shared = New TSHMO.Create(OPEN_OR_CREATE, name, MODE_RW)
		shared.Truncate(size)
		
		' create a mapped region
		region = New TMappedRegion.Create(shared, MODE_RW)
		
		_buf = region.GetAddress()
		_size=size
		_capacity=-1
		
		Return Self
	End Method
	
	Rem
	bbdoc: Removes the related shared memory from the system.
	about: After calling this, the bank and its data are invalid.
	End Rem
	Method Remove()
		TSHMO.Remove(name)
	End Method
	
End Type

Rem
bbdoc: Creates (or opens) a shared memory bank.
about: Note that to free the shared memory from the system you should call RemoveSharedBank().
End Rem
Function CreateSharedBank:TSharedBank(name:String, size:Int)
	Return TSharedBank.CreateShared(name, size)
End Function

Rem
bbdoc: Removes a shared memory bank from the system.
End Rem
Function RemoveSharedBank(name:String)
	TSHMO.Remove(name)
End Function


