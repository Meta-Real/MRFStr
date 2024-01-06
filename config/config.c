/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <string.h>

#ifdef __AVX512F__
#define mrfstr_avx512_switch(prefix, func)                            \
    _mrfstr_config.prefix ## func ## _sub = __mrfstr_avx512_ ## func; \
    _mrfstr_config.prefix ## func ## _size = 64;                      \
    break

#define mrfstr_avx512_nswitch(func)                              \
    _mrfstr_config.n ## func ## _sub = __mrfstr_avx512_ ## func; \
    _mrfstr_config.n ## func ## _size = 64;                      \
    break
#define mrfstr_avx512_tswitch(func)                               \
    _mrfstr_config.t ## func ## _sub = __mrfstr_avx512_t ## func; \
    _mrfstr_config.t ## func ## _size = 64;                       \
    break
#else
#define mrfstr_avx512_switch(prefix, func)
#define mrfstr_avx512_nswitch(func)
#define mrfstr_avx512_tswitch(func)
#endif

#ifdef __AVX__
#define mrfstr_avx_switch(prefix, func)                            \
    _mrfstr_config.prefix ## func ## _sub = __mrfstr_avx_ ## func; \
    _mrfstr_config.prefix ## func ## _size = 32;                   \
    break

#ifdef __AVX2__
#define mrfstr_avx_nswitch(func)                              \
    _mrfstr_config.n ## func ## _sub = __mrfstr_avx_ ## func; \
    _mrfstr_config.n ## func ## _size = 32;                   \
    break
#define mrfstr_avx_tswitch(func)                               \
    _mrfstr_config.t ## func ## _sub = __mrfstr_avx_t ## func; \
    _mrfstr_config.t ## func ## _size = 32;                    \
    break
#else
#define mrfstr_avx_nswitch(func)
#define mrfstr_avx_tswitch(func)
#endif

#else
#define mrfstr_avx_switch(prefix, func)
#define mrfstr_avx_nswitch(func)
#define mrfstr_avx_tswitch(func)
#endif

#ifdef __SSE2__
#define mrfstr_sse_switch(prefix, func)                            \
    _mrfstr_config.prefix ## func ## _sub = __mrfstr_sse_ ## func; \
    _mrfstr_config.prefix ## func ## _size = 16;                   \
    break

#define mrfstr_sse_nswitch(func)                              \
    _mrfstr_config.n ## func ## _sub = __mrfstr_sse_ ## func; \
    _mrfstr_config.n ## func ## _size = 16;                   \
    break
#define mrfstr_sse_tswitch(func)                               \
    _mrfstr_config.t ## func ## _sub = __mrfstr_sse_t ## func; \
    _mrfstr_config.t ## func ## _size = 16;                    \
    break
#else
#define mrfstr_sse_switch(prefix, func)
#define mrfstr_sse_nswitch(func)
#define mrfstr_sse_tswitch(func)
#endif

#define mrfstr_base_switch(prefix, func)                            \
    _mrfstr_config.prefix ## func ## _sub = __mrfstr_base_ ## func; \
    _mrfstr_config.prefix ## func ## _size = 8;                     \
    break

#define mrfstr_base_nswitch(func)                              \
    _mrfstr_config.n ## func ## _sub = __mrfstr_base_ ## func; \
    _mrfstr_config.n ## func ## _size = 8;                     \
    break
#define mrfstr_base_tswitch(func)                               \
    _mrfstr_config.t ## func ## _sub = __mrfstr_base_t ## func; \
    _mrfstr_config.t ## func ## _size = 8;                      \
    break

#define mrfstr_switch(func)            \
    switch (normal)                    \
    {                                  \
    case MRFSTR_CONFIG_SIMD_AVX512:    \
        mrfstr_avx512_switch(n, func); \
    case MRFSTR_CONFIG_SIMD_AVX:       \
        mrfstr_avx_switch(n, func);    \
    case MRFSTR_CONFIG_SIMD_SSE:       \
        mrfstr_sse_switch(n, func);    \
    case MRFSTR_CONFIG_SIMD_NONE:      \
        mrfstr_base_switch(n, func);   \
    }                                  \
    switch (threaded)                  \
    {                                  \
    case MRFSTR_CONFIG_SIMD_AVX512:    \
        mrfstr_avx512_switch(t, func); \
    case MRFSTR_CONFIG_SIMD_AVX:       \
        mrfstr_avx_switch(t, func);    \
    case MRFSTR_CONFIG_SIMD_SSE:       \
        mrfstr_sse_switch(t, func);    \
    case MRFSTR_CONFIG_SIMD_NONE:      \
        mrfstr_base_switch(t, func);   \
    }                                  \
    break

#define mrfstr_tnswitch(func)        \
    switch (normal)                  \
    {                                \
    case MRFSTR_CONFIG_SIMD_AVX512:  \
        mrfstr_avx512_nswitch(func); \
    case MRFSTR_CONFIG_SIMD_AVX:     \
        mrfstr_avx_nswitch(func);    \
    case MRFSTR_CONFIG_SIMD_SSE:     \
        mrfstr_sse_nswitch(func);    \
    case MRFSTR_CONFIG_SIMD_NONE:    \
        mrfstr_base_nswitch(func);   \
    }                                \
    switch (threaded)                \
    {                                \
    case MRFSTR_CONFIG_SIMD_AVX512:  \
        mrfstr_avx512_tswitch(func); \
    case MRFSTR_CONFIG_SIMD_AVX:     \
        mrfstr_avx_tswitch(func);    \
    case MRFSTR_CONFIG_SIMD_SSE:     \
        mrfstr_sse_tswitch(func);    \
    case MRFSTR_CONFIG_SIMD_NONE:    \
        mrfstr_base_tswitch(func);   \
    }                                \
    break

#ifdef __AVX512F__
mrfstr_config_t _mrfstr_config =
{
    1,
    __mrfstr_avx512_copy, 64,
    __mrfstr_avx512_copy, 64,
    __mrfstr_avx512_fill, 64,
    __mrfstr_avx512_fill, 64,
#ifdef __AVX512BW__
    __mrfstr_avx512_replace_chr, __mrfstr_avx512_replace_chr2, 64,
    __mrfstr_avx512_replace_chr, __mrfstr_avx512_replace_chr2, 64,
#else
    __mrfstr_avx_replace_chr, __mrfstr_avx_replace_chr2, 32,
    __mrfstr_avx_replace_chr, __mrfstr_avx_replace_chr2, 32,
#endif
    __mrfstr_avx512_equal, 64,
    __mrfstr_avx512_tequal, 64,
    __mrfstr_avx512_contain_chr, 64,
    __mrfstr_avx512_tcontain_chr, 64,
    __mrfstr_avx512_find_chr, 64,
    __mrfstr_avx512_tfind_chr, 64
};
#elif defined(__AVX__)
mrfstr_config_t _mrfstr_config =
{
    1,
    __mrfstr_avx_copy, 32,
    __mrfstr_avx_copy, 32,
    __mrfstr_avx_fill, 32,
    __mrfstr_avx_fill, 32,
#ifdef __AVX2__
    __mrfstr_avx_replace_chr, __mrfstr_avx_replace_chr2, 32,
    __mrfstr_avx_replace_chr, __mrfstr_avx_replace_chr2, 32,
    __mrfstr_avx_equal, 32,
    __mrfstr_avx_tequal, 32,
    __mrfstr_avx_contain_chr, 32,
    __mrfstr_avx_tcontain_chr, 32,
    __mrfstr_avx_find_chr, 32,
    __mrfstr_avx_tfind_chr, 32
#else
    __mrfstr_sse_replace_chr, __mrfstr_sse_replace_chr2, 16,
    __mrfstr_sse_replace_chr, __mrfstr_sse_replace_chr2, 16,
    __mrfstr_sse_equal, 16,
    __mrfstr_sse_tequal, 16,
    __mrfstr_sse_contain_chr, 16,
    __mrfstr_sse_tcontain_chr, 16,
    __mrfstr_sse_find_chr, 16,
    __mrfstr_sse_tfind_chr, 16
#endif
};
#elif defined(__SSE2__)
mrfstr_config_t _mrfstr_config =
{
    1,
    __mrfstr_sse_copy, 16,
    __mrfstr_sse_copy, 16,
    __mrfstr_sse_fill, 16,
    __mrfstr_sse_fill, 16,
#ifdef __SSE4_1__
    __mrfstr_sse_replace_chr, __mrfstr_sse_replace_chr2, 16,
    __mrfstr_sse_replace_chr, __mrfstr_sse_replace_chr2, 16,
#else
    __mrfstr_base_replace_chr, __mrfstr_base_replace_chr2, 8,
    __mrfstr_base_replace_chr, __mrfstr_base_replace_chr2, 8,
#endif
    __mrfstr_sse_equal, 16,
    __mrfstr_sse_tequal, 16,
    __mrfstr_sse_contain_chr, 16,
    __mrfstr_sse_tcontain_chr, 16,
    __mrfstr_sse_find_chr, 16,
    __mrfstr_sse_tfind_chr, 16
};
#else
mrfstr_config_t _mrfstr_config =
{
    1,
    __mrfstr_base_copy, 8,
    __mrfstr_base_copy, 8,
    __mrfstr_base_fill, 8,
    __mrfstr_base_fill, 8,
    __mrfstr_base_replace_chr, __mrfstr_base_replace_chr2, 8,
    __mrfstr_base_replace_chr, __mrfstr_base_replace_chr2, 8,
    __mrfstr_base_equal, 8,
    __mrfstr_base_tequal, 8,
    __mrfstr_base_contain_chr, 8,
    __mrfstr_base_tcontain_chr, 8,
    __mrfstr_base_find_chr, 8,
    __mrfstr_base_tfind_chr, 8
};
#endif

void mrfstr_config_tcount(
    mrfstr_byte_t tcount)
{
    _mrfstr_config.tcount = tcount + 1;
}

void mrfstr_config(
    mrfstr_config_type_enum_t type,
    mrfstr_config_simd_enum_t normal,
    mrfstr_config_simd_enum_t threaded)
{
    switch (type)
    {
    case MRFSTR_CONFIG_TYPE_COPY:
        mrfstr_switch(copy);
    case MRFSTR_CONFIG_TYPE_FILL:
        mrfstr_switch(fill);
    case MRFSTR_CONFIG_TYPE_EQUAL:
        mrfstr_tnswitch(equal);
    case MRFSTR_CONFIG_TYPE_CONTAIN_CHR:
        mrfstr_tnswitch(contain_chr);
    case MRFSTR_CONFIG_TYPE_FIND_CHR:
        mrfstr_tnswitch(find_chr);
    case MRFSTR_CONFIG_TYPE_REPLACE_CHR:
        switch (normal)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
#ifdef __AVX512BW__
            _mrfstr_config.nreplace_chr_sub = __mrfstr_avx512_replace_chr;
            _mrfstr_config.nreplace_chr2_sub = __mrfstr_avx512_replace_chr2;
            _mrfstr_config.nreplace_chr_size = 64;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_AVX:
#ifdef __AVX2__
            _mrfstr_config.nreplace_chr_sub = __mrfstr_avx_replace_chr;
            _mrfstr_config.nreplace_chr2_sub = __mrfstr_avx_replace_chr2;
            _mrfstr_config.nreplace_chr_size = 32;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_SSE:
#ifdef __SSE4_1__
            _mrfstr_config.nreplace_chr_sub = __mrfstr_sse_replace_chr;
            _mrfstr_config.nreplace_chr2_sub = __mrfstr_sse_replace_chr2;
            _mrfstr_config.nreplace_chr_size = 16;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_NONE:
            _mrfstr_config.nreplace_chr_sub = __mrfstr_base_replace_chr;
            _mrfstr_config.nreplace_chr2_sub = __mrfstr_base_replace_chr2;
            _mrfstr_config.nreplace_chr_size = 8;
            break;
        }
        switch (threaded)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
#ifdef __AVX512BW__
            _mrfstr_config.treplace_chr_sub = __mrfstr_avx512_replace_chr;
            _mrfstr_config.treplace_chr2_sub = __mrfstr_avx512_replace_chr2;
            _mrfstr_config.treplace_chr_size = 64;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_AVX:
#ifdef __AVX2__
            _mrfstr_config.treplace_chr_sub = __mrfstr_avx_replace_chr;
            _mrfstr_config.treplace_chr2_sub = __mrfstr_avx_replace_chr2;
            _mrfstr_config.treplace_chr_size = 32;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_SSE:
#ifdef __SSE4_1__
            _mrfstr_config.treplace_chr_sub = __mrfstr_sse_replace_chr;
            _mrfstr_config.treplace_chr2_sub = __mrfstr_sse_replace_chr2;
            _mrfstr_config.treplace_chr_size = 16;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_NONE:
            _mrfstr_config.treplace_chr_sub = __mrfstr_base_replace_chr;
            _mrfstr_config.treplace_chr2_sub = __mrfstr_base_replace_chr2;
            _mrfstr_config.treplace_chr_size = 8;
            break;
        }
        break;
    }
}
