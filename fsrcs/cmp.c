/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <string.h>

mrfstr_bool_t mrfstr_equal(
    mrfstr_ct str1, mrfstr_ct str2)
{
    if (str1->size != str2->size)
        return MRFSTR_FALSE;

    if (!str1->size)
        return MRFSTR_TRUE;

    return mrfstr_memcmp(str1->data, str2->data, str1->size);
}

mrfstr_bool_t mrfstr_n_equal(
    mrfstr_ct str1, mrfstr_ct str2,
    mrfstr_size_t size)
{
    if (str1->size < size)
    {
        if (str2->size != str1->size)
            return MRFSTR_FALSE;

        size = str1->size;
    }
    else if (str2->size < size)
        return MRFSTR_FALSE;

    if (!size)
        return MRFSTR_TRUE;

    return mrfstr_memcmp(str1->data, str2->data, size);
}

mrfstr_bool_t mrfstr_equal_str(
    mrfstr_ct str1, mrfstr_data_ct str2)
{
    if (str1->size != strlen(str2))
        return MRFSTR_FALSE;

    if (!str1->size)
        return MRFSTR_TRUE;

    return mrfstr_memcmp(str1->data, str2, str1->size);
}

mrfstr_bool_t mrfstr_n_equal_str(
    mrfstr_ct str1, mrfstr_data_ct str2,
    mrfstr_size_t size)
{
    if (str1->size < size)
    {
        if (strlen(str2) != str1->size)
            return MRFSTR_FALSE;

        size = str1->size;
    }
    else if (strlen(str2) < size)
        return MRFSTR_FALSE;

    if (!size)
        return MRFSTR_TRUE;

    return mrfstr_memcmp(str1->data, str2, size);
}

mrfstr_bool_t mrfstr_equal_nstr(
    mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size)
{
    if (str1->size != size)
        return MRFSTR_FALSE;

    if (!size)
        return MRFSTR_TRUE;

    return mrfstr_memcmp(str1->data, str2, size);
}

mrfstr_bool_t mrfstr_n_equal_nstr(
    mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size2,
    mrfstr_size_t size)
{
    if (str1->size < size)
    {
        if (size2 != str1->size)
            return MRFSTR_FALSE;

        size = size2;
    }
    else if (size2 < size)
        return MRFSTR_FALSE;

    if (!size)
        return MRFSTR_TRUE;

    return mrfstr_memcmp(str1->data, str2, size);
}
