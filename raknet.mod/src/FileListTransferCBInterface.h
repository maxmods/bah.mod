/// \file FileListTransferCBInterface.h
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.

#ifndef __FILE_LIST_TRANSFER_CALLBACK_INTERFACE_H
#define __FILE_LIST_TRANSFER_CALLBACK_INTERFACE_H

#include "RakMemoryOverride.h"
#include "FileListNodeContext.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

/// \brief Used by FileListTransfer plugin as a callback for when we get a file.
/// \details You get the last file when fileIndex==numberOfFilesInThisSet
/// \sa FileListTransfer
class FileListTransferCBInterface
{
public:
	struct OnFileStruct
	{
		/// \brief The index into the set of files, from 0 to numberOfFilesInThisSet
		unsigned fileIndex;

		/// \brief The name of the file
		char fileName[512];

		/// \brief The data pointed to by the file
		char *fileData;

		/// \brief The actual length of this file.
		BitSize_t byteLengthOfThisFile;

		/// \brief How many bytes of this file has been downloaded
		BitSize_t bytesDownloadedForThisFile;

		/// \brief Files are transmitted in sets, where more than one set of files can be transmitted at the same time.
		/// \details This is the identifier for the set, which is returned by FileListTransfer::SetupReceive
		unsigned short setID;

		/// \brief The number of files that are in this set.
		unsigned numberOfFilesInThisSet;

		/// \brief The total length of the transmitted files for this set, after being uncompressed
		unsigned byteLengthOfThisSet;

		/// \brief The total length, in bytes, downloaded for this set.
		unsigned bytesDownloadedForThisSet;

		/// \brief User data passed to one of the functions in the FileList class.
		/// \details However, on error, this is instead changed to one of the enumerations in the PatchContext structure.
		FileListNodeContext context;
	};

	FileListTransferCBInterface() {}
	virtual ~FileListTransferCBInterface() {}

	/// \brief Got a file.
	/// \details This structure is only valid for the duration of this function call.
	/// \return Return true to have RakNet delete the memory allocated to hold this file for this function call.
	virtual bool OnFile(OnFileStruct *onFileStruct)=0;

	/// \brief Got part of a big file.
	/// \details You can get these notifications by calling RakPeer::SetSplitMessageProgressInterval
	/// Otherwise you will only get complete files.
	/// \param[in] onFileStruct General information about this file, such as the filename and the first \a partLength bytes. You do NOT need to save this data yourself. The complete file will arrive normally.
	/// \param[in] partCount The zero based index into partTotal. The percentage complete done of this file is 100 * (partCount+1)/partTotal
	/// \param[in] partTotal The total number of parts this file was split into. Each part will be roughly the MTU size, minus the UDP header and RakNet headers
	/// \param[in] partLength How many bytes long firstDataChunk is
	/// \param[in] firstDataChunk The first \a partLength of the final file. If you store identifying information about the file in the first \a partLength bytes, you can read them while the download is taking place. If this hasn't arrived yet, firstDataChunk will be 0
	virtual void OnFileProgress(OnFileStruct *onFileStruct,unsigned int partCount,unsigned int partTotal,unsigned int dataChunkLength, char *firstDataChunk) {
		(void) onFileStruct;
		(void) partCount;
		(void) partTotal;
		(void) dataChunkLength;
		(void) firstDataChunk;
	}

	/// \brief Called while the handler is active by FileListTransfer
	/// \details Return false when you are done with the class.
	/// At that point OnDereference will be called and the class will no longer be maintained by the FileListTransfer plugin.
	virtual bool Update(void) {return true;}

	/// \brief Called when the download is completed.
	/// \details If you are finished with this class, return false.
	/// At that point OnDereference will be called and the class will no longer be maintained by the FileListTransfer plugin.
	/// Otherwise return true, and Update will continue to be called.
	virtual bool OnDownloadComplete(void) {return false;}

	/// \brief This function is called when this instance is about to be dereferenced by the FileListTransfer plugin.
	/// \details Update will no longer be called.
	/// It will will be deleted automatically if true was passed to FileListTransfer::SetupReceive::deleteHandler
	/// Otherwise it is up to you to delete it yourself.
	virtual void OnDereference(void) {}
};

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif

