SET(fw_test "${fw_name}-test")
PROJECT(bt_unit_test_gatt C)

INCLUDE(FindPkgConfig)
pkg_check_modules(${fw_test} REQUIRED glib-2.0)
FOREACH(flag ${${fw_test}_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -Wall")

SET(SOURCES
bt_unit_test_gatt.c
bt_unit_test_gatt_hr_profile.c
bt_unit_test_gatt_time_profile_client.c
bt_unit_test_gatt_time_profile_server.c
bt_unit_test_gatt_custom_profile_server.c
)

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${fw_name} ${${fw_test}_LDFLAGS})

INSTALL(TARGETS bt_unit_test_gatt DESTINATION bin)
INSTALL(FILES capi.network.bluetooth.test.gatt.rule DESTINATION /etc/smack/accesses2.d/)

