add_executable(TSet set.c)
target_link_libraries(TSet PRIVATE MRFStr)
add_test(TSet TSet)

add_executable(TRepeatChr repeat_chr.c)
target_link_libraries(TRepeatChr PRIVATE MRFStr)
add_test(TRepeatChr TRepeatChr)

add_executable(TRepeat repeat.c)
target_link_libraries(TRepeat PRIVATE MRFStr)
add_test(TRepeat TRepeat)

add_executable(TReverse reverse.c)
target_link_libraries(TReverse PRIVATE MRFStr)
add_test(TReverse TReverse)

add_executable(TReplaceChr replace_chr.c)
target_link_libraries(TReplaceChr PRIVATE MRFStr)
add_test(TReplaceChr TReplaceChr)

#add_executable(TEqual equal.c)
#target_link_libraries(TEqual PRIVATE MRFStr)
#add_test(TEqual TEqual)

#add_executable(TContainChr contain_chr.c)
#target_link_libraries(TContainChr PRIVATE MRFStr)
#add_test(TContainChr TContainChr)

#add_executable(TFindChr find_chr.c)
#target_link_libraries(TFindChr PRIVATE MRFStr)
#add_test(TFindChr TFindChr)

#add_executable(TCountChr count_chr.c)
#target_link_libraries(TCountChr PRIVATE MRFStr)
#add_test(TCountChr TCountChr)
