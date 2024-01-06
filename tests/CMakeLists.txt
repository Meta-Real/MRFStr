# MRFStr Library version 0.1.0
# MetaReal Fast String Library

add_executable(MRFStrSet set.c)
target_link_libraries(MRFStrSet PUBLIC MRFStr)
add_test(MRFStrSet MRFStrSet)

add_executable(MRFStrConcat concat.c)
target_link_libraries(MRFStrConcat PUBLIC MRFStr)
add_test(MRFStrConcat MRFStrConcat)

add_executable(MRFStrRepeatChr repeat_chr.c)
target_link_libraries(MRFStrRepeatChr PUBLIC MRFStr)
add_test(MRFStrRepeatChr MRFStrRepeatChr)

add_executable(MRFStrRepeat repeat.c)
target_link_libraries(MRFStrRepeat PUBLIC MRFStr)
add_test(MRFStrRepeat MRFStrRepeat)

add_executable(MRFStrReplaceChr replace_chr.c)
target_link_libraries(MRFStrReplaceChr PUBLIC MRFStr)
add_test(MRFStrReplaceChr MRFStrReplaceChr)

add_executable(MRFStrEqual equal.c)
target_link_libraries(MRFStrEqual PUBLIC MRFStr)
add_test(MRFStrEqual MRFStrEqual)

add_executable(MRFStrContainChr contain_chr.c)
target_link_libraries(MRFStrContainChr PUBLIC MRFStr)
add_test(MRFStrContainChr MRFStrContainChr)

add_executable(MRFStrFindChr find_chr.c)
target_link_libraries(MRFStrFindChr PUBLIC MRFStr)
add_test(MRFStrFindChr MRFStrFindChr)
