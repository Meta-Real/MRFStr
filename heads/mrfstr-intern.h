/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#ifndef __MRFSTR_INTERN__
#define __MRFSTR_INTERN__

#include <mrfstr.h>

#ifdef _MSC_VER
#include <simddef.h>
#endif

#include <avx512.h>
#include <avx.h>
#include <sse.h>

struct __MRFSTR_CONFIG_T
{
    mrfstr_byte_t thread_count;

    void (*ncopy_sub)(
        mrfstr_ptr_t, mrfstr_ptr_ct, mrfstr_size_t);
    mrfstr_byte_t ncopy_size;
    void (*tcopy_sub)(
        mrfstr_ptr_t, mrfstr_ptr_ct, mrfstr_size_t);
    mrfstr_byte_t tcopy_size;

    void (*nfill_sub)(
        mrfstr_ptr_t, mrfstr_chr_t, mrfstr_size_t);
    mrfstr_byte_t nfill_size;
    void (*tfill_sub)(
        mrfstr_ptr_t, mrfstr_chr_t, mrfstr_size_t);
    mrfstr_byte_t tfill_size;

    mrfstr_bool_t (*ncmp_sub)(
        mrfstr_ptr_ct, mrfstr_ptr_ct, mrfstr_size_t);
    mrfstr_byte_t ncmp_size;
    void (*tcmp_sub)(
        volatile mrfstr_bool_t*,
        mrfstr_ptr_ct, mrfstr_ptr_ct, mrfstr_size_t);
    mrfstr_byte_t tcmp_size;
};
typedef struct __MRFSTR_CONFIG_T mrfstr_config_t;
extern mrfstr_config_t mrfstr_config;

#ifdef __AVX512F__

typedef __m512i mrfstr_simd_t;
#define MRFSTR_SIMD_SIZE 64
#define MRFSTR_SIMD_SHIFT 6

#define mrfstr_simd_set _mm512_set1_epi8
#define mrfstr_simd_loadu _mm512_loadu_si512
#define mrfstr_simd_storeu _mm512_storeu_si512
#define mrfstr_simd_stream _mm512_stream_si512

#elif defined(__AVX__)

typedef __m256i mrfstr_simd_t;
#define MRFSTR_SIMD_SIZE 32
#define MRFSTR_SIMD_SHIFT 5

#define mrfstr_simd_set _mm256_set1_epi8
#define mrfstr_simd_loadu _mm256_loadu_si256
#define mrfstr_simd_storeu _mm256_storeu_si256
#define mrfstr_simd_stream _mm256_stream_si256

#elif defined(__SSE2__)

typedef __m128i mrfstr_simd_t;
#define MRFSTR_SIMD_SIZE 16
#define MRFSTR_SIMD_SHIFT 4

#define mrfstr_simd_set _mm_set1_epi8
#define mrfstr_simd_loadu _mm_loadu_si128
#define mrfstr_simd_storeu _mm_storeu_si128
#define mrfstr_simd_stream _mm_stream_si128

#else
#define MRFSTR_NOSIMD

typedef uint64_t mrfstr_simd_t;
#define MRFSTR_SIMD_SIZE 8
#define MRFSTR_SIMD_SHIFT 3

#define mrfstr_simd_loadu(x) (*(x))
#define mrfstr_simd_storeu(x, y) (*(x) = (y))
#define mrfstr_simd_stream(x, y) (*(x) = (y))

#endif

#define MRFSTR_SLIMIT 0x10000ULL

#define MRFSTR_TSIZE 0x1000000ULL
#define MRFSTR_TLIMIT (MRFSTR_TSIZE << 1)

#define MRFSTR_SIMD_MASK (MRFSTR_SIMD_SIZE - 1)
#define MRFSTR_SIMD_TCHK (MRFSTR_SIMD_SIZE * MRFSTR_THREAD_COUNT)

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <pthread.h>

typedef pthread_t mrfstr_thread_t;
typedef pthread_mutex_t mrfstr_mutex_t;
typedef mrfstr_mutex_t *mrfstr_mutex_p;

#define MRFSTR_TFUNC_RET NULL
#define MRFSTR_CAST_MUTEX(m) &(m)

#define mrfstr_create_thread(f) \
    if (pthread_create(threads + i, NULL, f, data))

#define mrfstr_close_threads \
    while (i)                \
        pthread_join(threads[--i], NULL)

#define mrfstr_create_mutex(m) \
    if (pthread_mutex_init(&(m), NULL))

#define mrfstr_close_mutex(m) pthread_mutex_destroy(&(m))
#define mrfstr_lock_mutex(m) pthread_mutex_lock(m)
#define mrfstr_unlock_mutex(m) pthread_mutex_unlock(m)

#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef HANDLE mrfstr_thread_t;
typedef HANDLE mrfstr_mutex_t;
typedef HANDLE mrfstr_mutex_p;

#define MRFSTR_TFUNC_RET TRUE
#define MRFSTR_CAST_MUTEX(m) m

#define mrfstr_create_thread(f)                           \
    threads[i] = CreateThread(NULL, 0, f, data, 0, NULL); \
    if (!threads[i])

#define mrfstr_close_threads                                \
    do                                                      \
    {                                                       \
        WaitForMultipleObjects(i, threads, TRUE, INFINITE); \
        while (i)                                           \
            CloseHandle(threads[--i]);                      \
    } while (0)

#define mrfstr_create_mutex(m)          \
    m = CreateMutex(NULL, FALSE, NULL); \
    if (!(m))

#define mrfstr_close_mutex CloseHandle
#define mrfstr_lock_mutex(m) WaitForSingleObject(m, INFINITE)
#define mrfstr_unlock_mutex ReleaseMutex

#else
#error Your os is not yet supported for multithreading
#endif

void __mrfstr_copy(
    mrfstr_data_t dst, mrfstr_data_ct src, mrfstr_size_t size);
void __mrfstr_fill(
    mrfstr_data_t res, mrfstr_chr_t chr, mrfstr_size_t size);

mrfstr_bool_t __mrfstr_equal(
    mrfstr_data_ct str1, mrfstr_data_ct str2, mrfstr_size_t size);

mrfstr_bool_t mrfstr_memchr(mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size);
mrfstr_idx_t mrfstr_memchr2(mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size);

#endif
