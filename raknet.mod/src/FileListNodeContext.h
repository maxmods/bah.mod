/// \file FileListNodeContext.h
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.

#ifndef __FILE_LIST_NODE_CONTEXT_H
#define __FILE_LIST_NODE_CONTEXT_H

struct FileListNodeContext
{
	FileListNodeContext() {}
	FileListNodeContext(unsigned char o, unsigned int f) : op(o), fileId(f) {}
	~FileListNodeContext() {}

	unsigned char op;
	unsigned int fileId;
};

#endif
