add_executable(MRFStrSet set.c)
target_link_libraries(MRFStrSet PRIVATE MRFStr)
add_test(MRFStrSet MRFStrSet)

add_executable(MRFStrRepeatChr repeat_chr.c)
target_link_libraries(MRFStrRepeatChr PRIVATE MRFStr)
add_test(MRFStrRepeatChr MRFStrRepeatChr)

add_executable(MRFStrRepeat repeat.c)
target_link_libraries(MRFStrRepeat PRIVATE MRFStr)
add_test(MRFStrRepeat MRFStrRepeat)

add_executable(MRFStrReverse reverse.c)
target_link_libraries(MRFStrReverse PRIVATE MRFStr)
add_test(MRFStrReverse MRFStrReverse)

add_executable(MRFStrReplaceChr replace_chr.c)
target_link_libraries(MRFStrReplaceChr PRIVATE MRFStr)
add_test(MRFStrReplaceChr MRFStrReplaceChr)

add_executable(MRFStrEqual equal.c)
target_link_libraries(MRFStrEqual PRIVATE MRFStr)
add_test(MRFStrEqual MRFStrEqual)

add_executable(MRFStrContainChr contain_chr.c)
target_link_libraries(MRFStrContainChr PRIVATE MRFStr)
add_test(MRFStrContainChr MRFStrContainChr)

add_executable(MRFStrFindChr find_chr.c)
target_link_libraries(MRFStrFindChr PRIVATE MRFStr)
add_test(MRFStrFindChr MRFStrFindChr)

add_executable(MRFStrCountChr count_chr.c)
target_link_libraries(MRFStrCountChr PRIVATE MRFStr)
add_test(MRFStrCountChr MRFStrCountChr)
