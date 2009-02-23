' Copyright (c) 2009 Bruce A Henderson
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

ModuleInfo "Version: 1.00"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: Wrapper - 2009 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

ModuleInfo "CC_OPTS: -fexceptions"

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
	Function CreateSHMO:TSHMO(access:Int, name:String, mode:Int)
		Return New TSHMO.Create(access, name, mode)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TSHMO(access:Int, name:String, mode:Int)
		objectPtr = bmx_sharedmemoryobject_create(access, name, mode)
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
	Function CreateMappedRegion:TMappedRegion(mapping:TMemoryMappable, mode:Int, offset:Long = 0, size:Int = 0, address:Byte Ptr = Null)
		Return New TMappedRegion.Create(mapping, mode, offset, size, address)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TMappedRegion(mapping:TMemoryMappable, mode:Int, offset:Long = 0, size:Int = 0, address:Byte Ptr = Null)
		If TSHMO(mapping) Then
			If address Then
				objectPtr = bmx_mapped_region_createshm(mapping.objectPtr, mode, offset, size, address)
			Else
				objectPtr = bmx_mapped_region_createshm(mapping.objectPtr, mode, offset, size, Null)
			End If
		End If
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetSize:Int()
		Return bmx_mapped_region_getsize(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetAddress:Byte Ptr()
		Return bmx_mapped_region_getaddress(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetOffset:Long()
		Local v:Long
		bmx_mapped_region_getoffset(objectPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetMode:Int()
		Return bmx_mapped_region_getmode(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Flush:Int(mappingOffset:Int = 0, numBytes:Int = 0)
		Return bmx_mapped_region_flush(objectPtr, mappingOffset, numBytes)
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
	
	Method Unlock()
	End Method
	
	Method Lock()
	End Method
	
	Method TryLock:Int()
	End Method
	
	Method TimedLock:Int(time:Int)
	End Method
	
	Function Remove:Int(name:String)
	End Function
	
	Method Free()
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
	
	Method NotifyOne()
	End Method
	
	Method NotifyAll()
	End Method
	
	Method Wait(mutex:TNamedMutex)
	End Method
	
	Method TimedWait:Int(mutex:TNamedMutex, time:Int)
	End Method
	
	Function Remove:Int(name:String)
	End Function

End Type

Rem
bbdoc: A semaphore with a global name, so it can be found from different processes.
about: Allows several resource sharing patterns and efficient acknowledgment mechanisms.
End Rem
Type TNamedSempahore

	Field objectPtr:Byte Ptr
	
	Rem
	bbdoc: Increments the semaphore count.
	about: If there are processes/threads blocked waiting for the semaphore, then one of these
	processes will return successfully from its wait function. If there is an error a
	TInterprocessException is thrown.
	End Rem
	Method Post()
	End Method
	
	Rem
	bbdoc: Decrements the semaphore.
	about: If the semaphore value is not greater than zero, then the calling process/thread blocks
	until it can decrement the counter. If there is an error a TInterprocessException is thrown.
	End Rem
	Method Wait()
	End Method
	
	Rem
	bbdoc: Decrements the semaphore if the semaphore's value is greater than zero and returns true.
	about: If the value is not greater than zero returns false. If there is an error a
	TInterprocessException is thrown.
	End Rem
	Method TryWait:Int()
	End Method
	
	Rem
	bbdoc: Decrements the semaphore if the semaphore's value is greater than zero and returns true.
	about: Otherwise, waits for the semaphore to the posted or the timeout expires. If the timeout
	expires, the method returns false. If the semaphore is posted the function returns true.
	If there is an error throws TSemException
	End Rem
	Method TimedWait:Int(time:Int)
	End Method

	Function Remove:Int(name:String)
	End Function

	Method Free()
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type



