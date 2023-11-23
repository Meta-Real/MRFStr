/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>
#endif

void mrfstr_import(mrfstr_t str, mrfstr_file_t *file)
{
    if (!file->size)
        return;

    mrfstr_set_nstr(str, file->data, file->size);
}

void mrfstr_export(mrfstr_file_t *file, mrfstr_t str)
{
    if (file->size < str->size)
    {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        if (file->size)
        {
            UnmapViewOfFile(file->data);
            CloseHandle(file->map);
        }

        file->size = str->size;

        LONG hsize = str->size >> 32;
        SetFilePointer(file->handle, str->size & 0xFFFFFFFF, &hsize, FILE_BEGIN);
        SetEndOfFile(file->handle);

        file->map = CreateFileMapping(file->handle, NULL, PAGE_READWRITE, 0, 0, NULL);
        file->data = MapViewOfFile(file->map, FILE_MAP_ALL_ACCESS, 0, 0, str->size);
#endif
    }

    mrfstr_memcpy(file->data, str->data, str->size);
}
