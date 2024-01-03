# Performance Order

* **C**: Standard C functions
* **64**: NoSIMD Instructions
* **128**: SSE
* **256**: AVX
* **512**: AVX512

## mrfstr_memcpy

threading on:
64 > 512 > 256 > 128 > C \
threading off:
128 > 512 > 256 > 64 = C

## mrfstr_memset

threading on:
64 > 512 > C > 256 > 128 \
threading off:
512 = 256 > 128 > 64 = C

## mrfstr_memcmp

threading on:
512 > 256 > 128 = 64 > C \
threading off:
512 > 256 > 128 = 64 = C

## mrfstr_memchr

threading on:
256 > 512 > 128 = 64 > C \
threading off:
512 > 256 > 64 > 128 > C
