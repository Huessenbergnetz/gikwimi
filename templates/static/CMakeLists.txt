# SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: AGPL-3.0-or-later

#find_program(NPX_EXECUTABLE npx)

#if (NOT ${NPX_EXECUTBALE})
#    message(FATAL_ERROR "Can not find npx executable")
#endif ()

#add_custom_target(generalstatic ALL
#    COMMAND ${NPX_EXECUTABLE} mix $<$<NOT:$<CONFIG:Debug>>:--production>
#    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#    SOURCES
#        assets/js/gikwimi.js
#)

add_custom_target(gikwimistatic
    SOURCES
        package.json
        webpack.mix.js
        favicon.ico
)

add_subdirectory(assets)

install(FILES favicon.ico
    DESTINATION ${GIKWIMI_APPDIR}/static
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

install(DIRECTORY js
    DESTINATION ${GIKWIMI_APPDIR}/static
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
    DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
