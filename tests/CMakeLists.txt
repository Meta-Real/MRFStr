# MRFStr Library version 0.1.0
# MetaReal Fast String Library

add_executable(MRFStrSet set.c)
target_link_libraries(MRFStrSet PUBLIC MRFStr)
add_test(TMRFStrSet MRFStrSet)

add_executable(MRFStrConcat concat.c)
target_link_libraries(MRFStrConcat PUBLIC MRFStr)
add_test(TMRFStrConcat MRFStrConcat)

add_executable(MRFStrRepeatChr repeat_chr.c)
target_link_libraries(MRFStrRepeatChr PUBLIC MRFStr)
add_test(TMRFStrRepeatChr MRFStrRepeatChr)

add_executable(MRFStrRepeat repeat.c)
target_link_libraries(MRFStrRepeat PUBLIC MRFStr)
add_test(TMRFStrRepeat MRFStrRepeat)

add_executable(MRFStrReplace replace.c)
target_link_libraries(MRFStrReplace PUBLIC MRFStr)
add_test(TMRFStrReplace MRFStrReplace)

add_executable(MRFStrEqual equal.c)
target_link_libraries(MRFStrEqual PUBLIC MRFStr)
add_test(TMRFStrEqual MRFStrEqual)

add_executable(MRFStrContainChr contain_chr.c)
target_link_libraries(MRFStrContainChr PUBLIC MRFStr)
add_test(TMRFStrContainChr MRFStrContainChr)

add_executable(MRFStrFindChr find_chr.c)
target_link_libraries(MRFStrFindChr PUBLIC MRFStr)
add_test(TMRFStrFindChr MRFStrFindChr)
