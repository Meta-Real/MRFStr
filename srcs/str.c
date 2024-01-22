/*
MIT License

Copyright (c) 2023 MetaReal

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
*/

#include <mrfstr-intern.h>
#include <binary.h>

mrfstr_size_t mrfstr_strlen(
    mrfstr_data_ct str)
{
    mrfstr_byte_t align = (uintptr_t)str % _mrfstr_config.strlen_size;
    if (align)
    {
        align = _mrfstr_config.strlen_size - align;
        for (mrfstr_byte_t i = 0; i != align; i++)
            if (*str++ == '\0')
                return i;
    }

    return _mrfstr_config.strlen_sub(str) + align;
}
