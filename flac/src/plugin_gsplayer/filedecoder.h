#ifndef __FILEDECODER_H__
#define __FILEDECODER_H__

#include <windows.h>
#include <tchar.h>
#include <string.h>
#include "mapplugin.h"

#define FLAC__NO_DLL
#include "FLAC++/decoder.h"

extern "C" { 
#include "../src/plugin_common/dither.h"
}

ULONGLONG FileSize(LPCTSTR pszFileName);

class FileDecoder : public FLAC::Decoder::File {
public:
	FileDecoder(): FLAC::Decoder::File() { InternalBufferHead = NULL; RawInternalBufferHead = NULL; InternalBufferSize = 0; PresumedChannels = 0; PresumedBlockSize = 0; FileSize = 0; }
	~FileDecoder() { }

	LONGLONG FileSize; // set this before calling process_until_end_of_metadata
	MAP_PLUGIN_FILE_INFO MapPluginFileInfo;
	int InternalBufferSize; // this is in bytes
	FLAC__int32* InternalBuffer;
	unsigned TotalSamples;

	BOOL hasFileTag;
	MAP_PLUGIN_FILETAG FileTag;
	
	::FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]);
	void metadata_callback(const ::FLAC__StreamMetadata *metadata);
	void error_callback(::FLAC__StreamDecoderErrorStatus status);

	void CleanUp();

private:
	FLAC__int32* InternalBufferHead;
	FLAC__int32** RawInternalBufferHead;
	unsigned PresumedChannels;
	unsigned PresumedBlockSize;
};

#endif
