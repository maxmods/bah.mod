' Copyright (c) 2008-2009 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the auther nor the names of its contributors may be used to 
'       endorse or promote products derived from this software without specific
'       prior written permission.
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

Import "random.bmx"
Import "common.bmx"


Rem
bbdoc: A 2D Binary Space Partition tree.
about: It can be used to split a rectangular region into non overlapping sub-regions.
End Rem
Type TCODBsp

	Field objectPtr:Byte Ptr
	
	
	Rem
	bbdoc: 
	End Rem
	Field x:Int
	Rem
	bbdoc: 
	End Rem
	Field y:Int
	Rem
	bbdoc: 
	End Rem
	Field w:Int
	Rem
	bbdoc: 
	End Rem
	Field h:Int
	Rem
	bbdoc: The position of splitting.
	End Rem
	Field position:Int
	Rem
	bbdoc: True if has horizontal splitting.
	End Rem
	Field horizontal:Int
	Rem
	bbdoc: The level in the tree.
	End Rem
	Field level:Int
	
	Global cb:Int(node:TCODBsp, userData:Object)
	Global data:Object
	
	Function _create:TCODBsp(objectPtr:Byte Ptr, x:Int, y:Int, w:Int, h:Int, position:Int, horizontal:Int, level:Int)
		If objectPtr Then
			Local this:TCODBsp = New TCODBsp
			this.objectPtr = objectPtr
			this.x = x
			this.y = y
			this.w = w
			this.h = h
			this.position = position
			this.horizontal = horizontal
			this.level = level
			Return this
		End If
	End Function

	Rem
	bbdoc: Creates the root node of the tree.
	about: This node encompasses the whole rectangular region.
	End Rem
	Function CreateBsp:TCODBsp(x:Int, y:Int, w:Int, h:Int)
		Return New TCODBsp.Create(x, y, w, h)
	End Function

	Rem
	bbdoc: Creates the root node of the tree.
	about: This node encompasses the whole rectangular region.
	End Rem
	Method Create:TCODBsp(x:Int, y:Int, w:Int, h:Int)
		objectPtr = bmx_tcodbsp_create(x, y, w, h)
		_update()
		Return Self
	End Method
	
	Method _update()
		bmx_tcodbsp_update(objectPtr, Varptr x, Varptr y, Varptr w, Varptr h, Varptr position, Varptr horizontal, Varptr level)
	End Method
	
	Rem
	bbdoc: Splits the node into two smaller non-overlapping nodes.
	End Rem
	Method SplitOnce(horizontal:Int, position:Int)
		bmx_tcodbsp_splitonce(objectPtr, horizontal, position)
	End Method
	
	Rem
	bbdoc: Recursively splits the bsp.
	about: At each step, a random orientation (horizontal/vertical) and position are choosen
	End Rem
	Method SplitRecursive(randomizer:TCODRandom, nb:Int, minHSize:Int, maxHRatio:Int, minVSize:Int, maxVRatio:Int)
		bmx_tcodbsp_splitrecursive(objectPtr, randomizer.randomPtr, nb, minHSize, maxHRatio, minVSize, maxVRatio)
	End Method
	
	Rem
	bbdoc: Resets the size of the tree nodes without changing the splitting data (orientation/position).
	about: It should be called with the initial region size or a bigger size, else some splitting position may be out of the region.
	<p>
	You can use it if you changed the nodes size and position while using the BSP tree, which happens typically when you use the
	tree to build a dungeon. You create rooms inside the tree leafs, then shrink the leaf to fit the room size. Calling resize on
	the root node with the original region size allows you to reset all nodes to their original size.
	</p>
	End Rem
	Method Resize(x:Int, y:Int, w:Int, h:Int)
		bmx_tcodbsp_resize(objectPtr, x, y, w, h)
	End Method
	
	Rem
	bbdoc: Navigates from the node to its left-child.
	about: Returns Null if the corresponding node does not exist.
	End Rem
	Method GetLeft:TCODBsp()
		Return TCODBsp(bmx_tcodbsp_getleft(objectPtr))
	End Method
	
	Rem
	bbdoc: Navigates from the node to its right-child.
	about: Returns Null if the corresponding node does not exist.
	End Rem
	Method GetRight:TCODBsp()
		Return TCODBsp(bmx_tcodbsp_getright(objectPtr))
	End Method
	
	Rem
	bbdoc: Navigates from the node to its parent node.
	about: Returns Null if the corresponding node does not exist.
	End Rem
	Method GetParent:TCODBsp()
		Return TCODBsp(bmx_tcodbsp_getparent(objectPtr))
	End Method
	
	Rem
	bbdoc: Returns True if the node is split, False otherwise.
	End Rem
	Method IsLeaf:Int()
		Return bmx_tcodbsp_isleaf(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns True if the specified cell is inside the node.
	End Rem
	Method Contains:Int(cx:Int, cy:Int)
		Return bmx_tcodbsp_contains(objectPtr, cx, cy)
	End Method
	
	Rem
	bbdoc: Searches the tree for the smallest node containing the map cell.
	about: If the cell is outside the tree, the function returns Null.
	End Rem
	Method FindNode:TCODBsp(cx:Int, cy:Int)
		Return TCODBsp(bmx_tcodbsp_findnode(objectPtr, cx, cy))
	End Method

	Rem
	bbdoc: Traverses the tree in pre order.
	about: The callback is called for the current node, then for the left child, then for the right child.
	<p>
	The traversal returns False if the traversal has been interrupted (a callback returned false).
	</p>
	End Rem
	Method TraversePreOrder:Int(nodeCallback:Int(node:TCODBsp, userData:Object), userData:Object)
		cb = nodeCallback
		data = userData
		Return bmx_tcodbsp_traversepreorder(objectPtr)
	End Method
	
	Rem
	bbdoc: Traverses the tree in order.
	about: The callback is called for the left child, then for current node, then for the right child.
	<p>
	The traversal returns False if the traversal has been interrupted (a callback returned false).
	</p>
	End Rem
	Method TraverseInOrder:Int(nodeCallback:Int(node:TCODBsp, userData:Object), userData:Object)
		cb = nodeCallback
		data = userData
		Return bmx_tcodbsp_traverseinorder(objectPtr)
	End Method
	
	Rem
	bbdoc: Traverses the tree in post order.
	about: The callback is called for the left child, then for the right child, then for the current node.
	<p>
	The traversal returns False if the traversal has been interrupted (a callback returned false).
	</p>
	End Rem
	Method TraversePostOrder:Int(nodeCallback:Int(node:TCODBsp, userData:Object), userData:Object)
		cb = nodeCallback
		data = userData
		Return bmx_tcodbsp_traversepostorder(objectPtr)
	End Method
	
	Rem
	bbdoc: Traverses the tree in level order.
	about: The callback is called for the nodes level by level, from left to right.
	<p>
	The traversal returns False if the traversal has been interrupted (a callback returned false).
	</p>
	End Rem
	Method TraverseLevelOrder:Int(nodeCallback:Int(node:TCODBsp, userData:Object), userData:Object)
		cb = nodeCallback
		data = userData
		Return bmx_tcodbsp_traverselevelorder(objectPtr)
	End Method
	
	Rem
	bbdoc: Traverses the tree in inverted level order.
	about: The callback is called in the exact inverse order as Level-order.
	<p>
	The traversal returns False if the traversal has been interrupted (a callback returned false).
	</p>
	End Rem
	Method TraverseInvertedLevelOrder:Int(nodeCallback:Int(node:TCODBsp, userData:Object), userData:Object)
		cb = nodeCallback
		data = userData
		Return bmx_tcodbsp_traverseinvertedlevelorder(objectPtr)
	End Method
	
	Function _Callback:Int(userData:TCODBsp)
		Return cb(userdata, data)
	End Function
	
	Rem
	bbdoc: Deletes a part of the tree, releasing resources for all sub nodes.
	End Rem
	Method RemoveChildren()
		bmx_tcodbsp_removechildren(objectPtr)
	End Method
	
	Rem
	bbdoc: Frees the tree, and all its resources.
	End Rem
	Method Free()
		If objectPtr Then
			bmx_tcodbsp_free(objectPtr)
			objectPtr = Null
		End If
	End Method
	
End Type

