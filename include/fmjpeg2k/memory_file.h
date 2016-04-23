#include "openjpeg.h"

struct DecodeData {
public:
    DecodeData(unsigned char* src_data, OPJ_SIZE_T src_size) :
        src_data(src_data), src_size(src_size), offset(0) {
    }
    unsigned char* src_data;
    OPJ_SIZE_T     src_size;
    OPJ_SIZE_T     offset;
};

OPJ_UINT32 opj_read_from_memory(void * p_buffer, OPJ_UINT32 p_nb_bytes, DecodeData* p_file);
OPJ_UINT32 opj_write_from_memory (void * p_buffer, OPJ_UINT32 p_nb_bytes, DecodeData* p_file);
OPJ_SIZE_T opj_skip_from_memory (OPJ_SIZE_T p_nb_bytes, DecodeData * p_file);
OPJ_BOOL opj_seek_from_memory (OPJ_SIZE_T p_nb_bytes, DecodeData * p_file);
opj_stream_t* OPJ_CALLCONV opj_stream_create_memory_stream(DecodeData* p_mem, OPJ_UINT32 p_size, bool p_is_read_stream);
void msg_callback(const char* msg, void* f);