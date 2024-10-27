AUX_SOURCE_DIRECTORY(./data_struct LIB_SRCS)
AUX_SOURCE_DIRECTORY(./event LIB_SRCS)
AUX_SOURCE_DIRECTORY(./file LIB_SRCS)
AUX_SOURCE_DIRECTORY(./log LIB_SRCS)
AUX_SOURCE_DIRECTORY(./thread LIB_SRCS)
AUX_SOURCE_DIRECTORY(./util LIB_SRCS)
AUX_SOURCE_DIRECTORY(./encrypt LIB_SRCS)

# shared library
ADD_LIBRARY(StableEvent SHARED ${LIB_SRCS})

# static library
ADD_LIBRARY (StableEvent_static STATIC ${LIB_SRCS})
SET_TARGET_PROPERTIES(StableEvent_static PROPERTIES OUTPUT_NAME "StableEvent")
SET_TARGET_PROPERTIES(StableEvent PROPERTIES CLEAN_DIRECT_OUTPUT 1)
SET_TARGET_PROPERTIES(StableEvent_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# 设置 -fvisibility=hidden 选项
#SET_TARGET_PROPERTIES(StableEvent PROPERTIES
#    COMPILE_FLAGS "-fvisibility=hidden"
#    CXX_VISIBILITY_PRESET hidden
#)

#SET(OPENSSL_LIBS ${PROJECT_SOURCE_DIR}/lib/libcrypto.a)
#target_link_libraries(StableInfra PRIVATE ${OPENSSL_LIBS})
#target_link_libraries(StableInfra_static PRIVATE ${OPENSSL_LIBS})

#install command is executed when "make install"
INSTALL(TARGETS StableEvent DESTINATION /usr/local/lib)
INSTALL(TARGETS StableEvent_static DESTINATION /usr/local/lib)
INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION /usr/local/include/libStableEvent)
