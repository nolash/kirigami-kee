add_definitions(-D__STRICT_ANSI__)
find_package(PkgConfig)
pkg_check_modules(GRAPHICSMAGICK REQUIRED GraphicsMagick)
add_library(testutils testutil.c)
set_property(TARGET testutils PROPERTY C_VISIBILITY_PRESET default)
set_property(TARGET testutils PROPERTY VISIBILITY_INLINES_HIDDEN OFF)
add_executable(test_gpg test_gpg.cpp ../gpg.cpp ../digest.c ../term_debug.c)
add_executable(test_gpg_nokey test_gpg_nokey.cpp ../gpg.cpp ../digest.c ../term_debug.c)
add_executable(test_gpg_sym test_gpg_sym.cpp ../gpg.cpp ../digest.c ../term_debug.c)
add_executable(test_export test_export.cpp ../export.cpp ../term_debug.c)
add_executable(test_credit test_credit.cpp ../credit.cpp ../export.cpp ../term_debug.c)
add_executable(test_db test_db.cpp ../db.cpp ../endian.c ../term_debug.c)
add_executable(test_transport test_transport.cpp ../transport.c ../qr.c ../term_debug.c)
add_executable(test_rpc test_rpc.cpp ../transport.c ../rpc.cpp ../backend.cpp ../export.cpp ../credit.cpp ../settings.cpp ../gpg.cpp ../command.cpp ../qr.c ../digest.c ../term_debug.c)
target_link_libraries(test_gpg gcrypt utils)
target_link_libraries(test_gpg_nokey gcrypt utils)
target_link_libraries(test_gpg_sym gcrypt utils testutils)
target_link_libraries(test_export varint testutils Qt5::Core)
target_link_libraries(test_credit varint testutils Qt5::Core)
target_link_libraries(test_db testutils gcrypt lmdb)
target_link_libraries(test_transport testutils z b64 qrencode zbar ${GRAPHICSMAGICK_LIBRARIES})
target_link_libraries(test_rpc testutils Qt5::Core Qt5::Network Qt5::Gui varint gcrypt utils z b64 qrencode zbar ${GRAPHICSMAGICK_LIBRARIES})
target_include_directories(test_gpg PRIVATE ..)
target_include_directories(test_gpg_nokey PRIVATE ..)
target_include_directories(test_gpg_sym PRIVATE .. .)
target_include_directories(test_export PRIVATE .. .)
target_include_directories(test_credit  PRIVATE .. .)
target_include_directories(test_db PRIVATE .. .)
target_include_directories(test_transport PRIVATE .. . ${GRAPHICSMAGICK_INCLUDE_DIRS})
target_include_directories(test_rpc PRIVATE .. . ${GRAPHICSMAGICK_INCLUDE_DIRS})
target_compile_definitions(test_rpc PUBLIC ISTESTING)
add_test(NAME gpg COMMAND test_gpg)
add_test(NAME gpg_nokey COMMAND test_gpg_nokey)
add_test(NAME gpg_sym COMMAND test_gpg_sym)
add_test(NAME export COMMAND test_export)
add_test(NAME credit COMMAND test_credit)
add_test(NAME db COMMAND test_db)
add_test(NAME transport COMMAND test_transport)
add_test(NAME rpc COMMAND test_rpc)
target_compile_options(test_db PUBLIC -fexceptions)
target_compile_options(test_transport PUBLIC -fexceptions)
configure_file(key.bin ${CMAKE_CURRENT_BINARY_DIR}/key.bin COPYONLY)
configure_file(credit.bin ${CMAKE_CURRENT_BINARY_DIR}/credit.bin COPYONLY)
configure_file(qr.png ${CMAKE_CURRENT_BINARY_DIR}/qr.png COPYONLY)
file(COPY testdata_mdb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
