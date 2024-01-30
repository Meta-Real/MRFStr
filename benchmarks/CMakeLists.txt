add_executable(BSet set.c)
target_link_libraries(BSet PRIVATE MRFStr)

add_executable(BRepeatChr repeat_chr.c)
target_link_libraries(BRepeatChr PRIVATE MRFStr)

add_executable(BEqual equal.c)
target_link_libraries(BEqual PRIVATE MRFStr)
