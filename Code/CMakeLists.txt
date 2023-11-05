
IF(NO_CURL)
    add_subdirectory(coldfield)
    add_subdirectory(sutton)
ELSE()
    ad_add_all_subdirs()
ENDIF()