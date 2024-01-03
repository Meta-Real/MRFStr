# MRFStr Library version 0.1.0
# MetaReal Fast String Library

add_executable(MRFStrSet set.c)
target_link_libraries(MRFStrSet PUBLIC MRFStr)
add_test(MRFStrSet MRFStrSet)

add_executable(MRFStrRepeatChr repeat_chr.c)
target_link_libraries(MRFStrRepeatChr PUBLIC MRFStr)
add_test(MRFStrRepeatChr MRFStrRepeatChr)

add_executable(MRFStrRepeat repeat.c)
target_link_libraries(MRFStrRepeat PUBLIC MRFStr)
add_test(MRFStrRepeat MRFStrRepeat)

add_executable(MRFStrEqual equal.c)
target_link_libraries(MRFStrEqual PUBLIC MRFStr)
add_test(MRFStrEqual MRFStrEqual)
