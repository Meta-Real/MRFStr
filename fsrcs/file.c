/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>
#endif

mrfstr_file_t mrfstr_openfile_read(mrfstr_data_ct path)
{
    mrfstr_file_t file;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    file.handle = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    DWORD hsize;
    file.size = GetFileSize(file.handle, &hsize);
    file.size |= (mrfstr_size_t)hsize << 32;

    if (!file.size)
        return file;

    file.map = CreateFileMapping(file.handle, NULL, PAGE_READONLY, 0, 0, NULL);
    file.data = MapViewOfFile(file.map, FILE_MAP_READ, 0, 0, file.size);
#endif

    return file;
}

mrfstr_file_t mrfstr_openfile_write(mrfstr_data_ct path, mrfstr_size_t size)
{
    mrfstr_file_t file;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    file.handle = CreateFile(path, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    file.size = size;
    if (!size)
        return file;

    LONG hsize = size >> 32;
    SetFilePointer(file.handle, size & 0xFFFFFFFF, &hsize, FILE_BEGIN);
    SetEndOfFile(file.handle);

    file.map = CreateFileMapping(file.handle, NULL, PAGE_READWRITE, 0, 0, NULL);
    file.data = MapViewOfFile(file.map, FILE_MAP_ALL_ACCESS, 0, 0, size);
#endif

    return file;
}

void mrfstr_closefile(mrfstr_file_t *file)
{
    if (!file->size)
        return;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    UnmapViewOfFile(file->data);
    CloseHandle(file->map);
    CloseHandle(file->handle);
#endif
}

void mrfstr_transfer(mrfstr_file_t *dst, mrfstr_file_t *src)
{
    mrfstr_memcpy(dst->data, src->data, src->size);
}
