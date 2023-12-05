/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <string.h>

mrfstr_bool_t mrfstr_equal(mrfstr_ct str1, mrfstr_ct str2)
{
    if (str1->size != str2->size)
        return MRFSTR_FALSE;

    if (!str1->size || str1->data == str2->data)
        return MRFSTR_TRUE;

    return mrfstr_memcmp(str1->data, str2->data, str1->size);
}

mrfstr_bool_t mrfstr_equal_str(mrfstr_ct str1, mrfstr_data_ct str2)
{
    if (str1->size != strlen(str2))
        return MRFSTR_FALSE;

    if (!str1->size || str1->data == str2)
        return MRFSTR_TRUE;

    return mrfstr_memcmp(str1->data, str2, str1->size);
}

mrfstr_bool_t mrfstr_equal_nstr(mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size)
{
    if (str1->size != size)
        return MRFSTR_FALSE;

    if (!size || str1->data == str2)
        return MRFSTR_TRUE;

    return mrfstr_memcmp(str1->data, str2, size);
}
