add_executable(BSet set.c)
target_link_libraries(BSet PRIVATE MRFStr)

add_executable(BRepeat repeat.c)
target_link_libraries(BRepeat PRIVATE MRFStr)

add_executable(BRepeatChr repeat_chr.c)
target_link_libraries(BRepeatChr PRIVATE MRFStr)

add_executable(BReverse reverse.c)
target_link_libraries(BReverse PRIVATE MRFStr)

add_executable(BReplaceChr replace_chr.c)
target_link_libraries(BReplaceChr PRIVATE MRFStr)

add_executable(BEqual equal.c)
target_link_libraries(BEqual PRIVATE MRFStr)

#add_executable(BContainChr contain_chr.c)
#target_link_libraries(BContainChr PRIVATE MRFStr)

#add_executable(BCountChr count_chr.c)
#target_link_libraries(BCountChr PRIVATE MRFStr)

#add_executable(BStrlen strlen.c)
#target_link_libraries(BStrlen PRIVATE MRFStr)
