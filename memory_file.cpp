#include <string.h>
#include <limits>
#include <algorithm>
#include "fmjpeg2k/memory_file.h"

OPJ_UINT32 opj_read_from_memory(void * p_buffer, OPJ_UINT32 nb_bytes, DecodeData* p_user_data)
{	
	DecodeData* srcData = static_cast<DecodeData*>(p_user_data);
    if (!srcData || !srcData->src_data || srcData->src_size == 0) {
        return -1;
    }
    // Reads at EOF return an error code.
    if (srcData->offset >= srcData->src_size) {
        return -1;
    }
    OPJ_SIZE_T bufferLength = srcData->src_size - srcData->offset;
    OPJ_SIZE_T readlength = nb_bytes < bufferLength ? nb_bytes : bufferLength;
    memcpy(p_buffer, &srcData->src_data[srcData->offset], readlength);
    srcData->offset += readlength;
    return readlength;
}

OPJ_UINT32 opj_write_from_memory (void * p_buffer, OPJ_UINT32 nb_bytes, DecodeData* p_user_data)
{
	DecodeData* srcData = static_cast<DecodeData*>(p_user_data);
    if (!srcData || !srcData->src_data || srcData->src_size == 0) {
        return -1;
    }
    // Writes at EOF return an error code.
    if (srcData->offset >= srcData->src_size) {
        return -1;
    }
    OPJ_SIZE_T bufferLength = srcData->src_size - srcData->offset;
    OPJ_SIZE_T writeLength = nb_bytes < bufferLength ? nb_bytes : bufferLength;
    memcpy(&srcData->src_data[srcData->offset], p_buffer, writeLength);
    srcData->offset += writeLength;
    return writeLength;
}

OPJ_SIZE_T opj_skip_from_memory (OPJ_SIZE_T nb_bytes, DecodeData * p_user_data)
{
	DecodeData* srcData = static_cast<DecodeData*>(p_user_data);
    if (!srcData || !srcData->src_data || srcData->src_size == 0) {
        return -1;
    }
    // Offsets are signed and may indicate a negative skip. Do not support this
    // because of the strange return convention where either bytes skipped or
    // -1 is returned. Following that convention, a successful relative seek of
    // -1 bytes would be required to to give the same result as the error case.
    if (nb_bytes < 0) {
        return -1;
    }
    // FIXME: use std::make_unsigned<OPJ_OFF_T>::type once c++11 lib is OK'd.
    uint64_t unsignedNbBytes = static_cast<uint64_t>(nb_bytes);
    // Additionally, the offset may take us beyond the range of a size_t (e.g.
    // 32-bit platforms). If so, just clamp at EOF.
    if (unsignedNbBytes > std::numeric_limits<OPJ_SIZE_T>::max() - srcData->offset) {
        srcData->offset = srcData->src_size;
    } else {
        OPJ_SIZE_T checkedNbBytes = static_cast<OPJ_SIZE_T>(unsignedNbBytes);
        // Otherwise, mimic fseek() semantics to always succeed, even past EOF,
        // clamping at EOF.  We can get away with this since we don't actually
        // provide negative relative skips from beyond EOF back to inside the
        // data, which would be the only reason to need to know exactly how far
        // beyond EOF we are.
        srcData->offset = std::min(srcData->offset + checkedNbBytes, srcData->src_size);
    }
    return nb_bytes;
}

OPJ_BOOL opj_seek_from_memory (OPJ_SIZE_T nb_bytes, DecodeData * p_user_data)
{
	   DecodeData* srcData = static_cast<DecodeData*>(p_user_data);
    if (!srcData || !srcData->src_data || srcData->src_size == 0) {
        return OPJ_FALSE;
    }
    // Offsets are signed and may indicate a negative position, which would
    // be before the start of the file. Do not support this.
    if (nb_bytes < 0) {
        return OPJ_FALSE;
    }
    // FIXME: use std::make_unsigned<OPJ_OFF_T>::type once c++11 lib is OK'd.
    uint64_t unsignedNbBytes = static_cast<uint64_t>(nb_bytes);
    // Additionally, the offset may take us beyond the range of a size_t (e.g.
    // 32-bit platforms). If so, just clamp at EOF.
    if (unsignedNbBytes > std::numeric_limits<OPJ_SIZE_T>::max()) {
        srcData->offset = srcData->src_size;
    } else {
        OPJ_SIZE_T checkedNbBytes = static_cast<OPJ_SIZE_T>(nb_bytes);
        // Otherwise, mimic fseek() semantics to always succeed, even past EOF,
        // again clamping at EOF.
        srcData->offset = std::min(checkedNbBytes, srcData->src_size);
    }
    return OPJ_TRUE;
}

opj_stream_t* OPJ_CALLCONV opj_stream_create_memory_stream(DecodeData* p_mem,OPJ_UINT32 p_size, bool p_is_read_stream)
{
	opj_stream_t* l_stream = NULL;
	if (! p_mem)
		return NULL;

	l_stream = opj_stream_create(p_size,p_is_read_stream);
	if (!l_stream)
		return NULL;

	opj_stream_set_user_data(l_stream, p_mem, NULL);
	opj_stream_set_user_data_length(l_stream, p_mem->src_size);
	opj_stream_set_read_function(l_stream,(opj_stream_read_fn) opj_read_from_memory);
	opj_stream_set_write_function(l_stream, (opj_stream_write_fn) opj_write_from_memory);
	opj_stream_set_skip_function(l_stream, (opj_stream_skip_fn) opj_skip_from_memory);
	opj_stream_set_seek_function(l_stream, (opj_stream_seek_fn) opj_seek_from_memory);
	return l_stream;
}

void msg_callback(const char* msg, void* f)
{	
}