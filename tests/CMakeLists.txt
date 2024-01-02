# MRFStr Library version 0.1.0
# MetaReal Fast String Library

add_executable(MRFStrSet mrfstr_set.c)
target_link_libraries(MRFStrSet PUBLIC MRFStr)
add_test(MRFStrSet MRFStrSet)
