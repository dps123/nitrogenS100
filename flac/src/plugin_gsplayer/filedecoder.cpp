// gspflac.cpp
// November 29, 2005
//
// by Jeff Huang <keltusflac@gmail.com>
// AIM: keltuskeltus
// ICQ: 46531632
// MSN: keltuskeltus@hotmail.com
//

#include "filedecoder.h"

#include <stdio.h>

::FLAC__StreamDecoderWriteStatus FileDecoder::write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[])
{
	//assert(InternalBufferSize == 0);

	//blocksize is usually 4608, channels is usually 2

	if (PresumedChannels != frame->header.channels || PresumedBlockSize != frame->header.blocksize)
		CleanUp(); // things have changed so let's remake the internal buffer

	if (InternalBufferHead == NULL) // if NULL, then it's the "first time" this function is run
	{
		PresumedChannels = frame->header.channels;
		PresumedBlockSize = frame->header.blocksize;

		RawInternalBufferHead = new FLAC__int32*[PresumedChannels];
		for (unsigned i = 0; i < PresumedChannels; i++)
			RawInternalBufferHead[i] = new FLAC__int32[PresumedBlockSize];
		InternalBufferHead = new FLAC__int32[PresumedBlockSize]; // not sure why we don't need to account for PresumedChannels. Does FLAC__plugin_common__pack_pcm_signed_little_endian reduce the size of the data by PresumedChannels?
	}

	for (unsigned channel = 0; channel < PresumedChannels; channel++)
		//assert(sizeof(buffer[0][0]) == 4);
		memcpy(RawInternalBufferHead[channel], buffer[channel], sizeof(FLAC__int32) * PresumedBlockSize);

	FLAC__plugin_common__pack_pcm_signed_little_endian((FLAC__byte*)InternalBufferHead, (FLAC__int32**)RawInternalBufferHead, PresumedBlockSize, PresumedChannels, MapPluginFileInfo.nBitsPerSample, MapPluginFileInfo.nBitsPerSample);

	InternalBufferSize = sizeof(FLAC__int32) * PresumedBlockSize; // not sure why we don't need to account for PresumedChannels. Does FLAC__plugin_common__pack_pcm_signed_little_endian reduce the size of the data by PresumedChannels?
	InternalBuffer = InternalBufferHead; // reset the position of InternalBuffer

	return ::FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void FileDecoder::CleanUp()
{
	if (InternalBufferHead != NULL)
	{
		delete[] InternalBufferHead;
		InternalBufferHead = NULL;
	}

	if (RawInternalBufferHead != NULL)
	{
		for (unsigned i = 0; i < PresumedChannels; i++)
			delete[] RawInternalBufferHead[i];
		delete[] RawInternalBufferHead;
		RawInternalBufferHead = NULL;
	}

	InternalBufferSize = 0;
}

void FileDecoder::metadata_callback(const ::FLAC__StreamMetadata *metadata)
{
	// assert(FileSize != 0);

	if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
	{
		TotalSamples = (unsigned)metadata->data.stream_info.total_samples & 0xfffffffful;
		MapPluginFileInfo.nBitsPerSample = metadata->data.stream_info.bits_per_sample;
		MapPluginFileInfo.nChannels = metadata->data.stream_info.channels;
		MapPluginFileInfo.nSampleRate = metadata->data.stream_info.sample_rate;
		MapPluginFileInfo.nAvgBitrate = (unsigned)(FileSize / (125. * TotalSamples / MapPluginFileInfo.nSampleRate));
		MapPluginFileInfo.nDuration = (unsigned)((double)TotalSamples / (double)MapPluginFileInfo.nSampleRate * 1000.0 + 0.5);
	}
	else if (metadata->type == FLAC__METADATA_TYPE_VORBIS_COMMENT)
	{
		hasFileTag=TRUE;

		for (unsigned i = 0; i < metadata->data.vorbis_comment.num_comments; ++i)
		{
			char buffer[MAX_PLUGIN_TAG_STR];
			wchar_t wbuffer[MAX_PLUGIN_TAG_STR]={0};
			memcpy(buffer, metadata->data.vorbis_comment.comments[i].entry, min(MAX_PLUGIN_TAG_STR, metadata->data.vorbis_comment.comments[i].length + 1));
			MultiByteToWideChar(CP_UTF8, 0, buffer, strlen(buffer), wbuffer, sizeof(wbuffer) / sizeof(wchar_t));

			wchar_t* wchr = wcschr(wbuffer, L'=');
			if (wchr == NULL)
				continue;

			*wchr = L'\0'; // NULL terminate the string
			wchr++; // Move to the value (it moves 1 wchar_t, aka 2 bytes)
			_wcslwr(wbuffer); // Make the field name lowercase for easy comparison

			if (wcscmp(wbuffer, L"title") == 0)
				wcscpy(FileTag.szTrack, wchr);
			else if (wcscmp(wbuffer, L"artist") == 0)
				wcscpy(FileTag.szArtist, wchr);
			else if (wcscmp(wbuffer, L"album") == 0)
				wcscpy(FileTag.szAlbum, wchr);
			else if (wcscmp(wbuffer, L"comment") == 0)
				wcscpy(FileTag.szComment, wchr);
			else if (wcscmp(wbuffer, L"genre") == 0)
				wcscpy(FileTag.szGenre, wchr);
			else if (wcscmp(wbuffer, L"tracknumber") == 0)
				FileTag.nTrackNum = _wtoi(wchr);
			else if (wcscmp(wbuffer, L"date") == 0)
			{
				*(wchr + 4) = L'\0';
				FileTag.nYear = _wtoi(wchr);
			}
		}
		int a = 5;
	}
}

void FileDecoder::error_callback(::FLAC__StreamDecoderErrorStatus status)
{

}

ULONGLONG FileSize(LPCTSTR pszFileName)
{
	LARGE_INTEGER res;
	HANDLE hFile = CreateFile(pszFileName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) return 0;
	
	DWORD dwHiPart;
	res.LowPart = GetFileSize(hFile, &dwHiPart);
	res.HighPart = dwHiPart;
	CloseHandle(hFile);
	return res.QuadPart;
}
