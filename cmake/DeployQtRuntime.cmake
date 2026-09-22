if(NOT DEFINED QT_HOME OR NOT DEFINED DEPLOY_DIR)
    message(FATAL_ERROR "QT_HOME and DEPLOY_DIR are required")
endif()

file(REMOVE_RECURSE "${DEPLOY_DIR}/mediaservice")

file(MAKE_DIRECTORY
    "${DEPLOY_DIR}"
    "${DEPLOY_DIR}/audio"
    "${DEPLOY_DIR}/bearer"
    "${DEPLOY_DIR}/iconengines"
    "${DEPLOY_DIR}/imageformats"
    "${DEPLOY_DIR}/mediaservice"
    "${DEPLOY_DIR}/platforms"
    "${DEPLOY_DIR}/playlistformats"
    "${DEPLOY_DIR}/styles")

file(COPY
    "${QT_HOME}/bin/Qt5Core.dll"
    "${QT_HOME}/bin/Qt5Gui.dll"
    "${QT_HOME}/bin/Qt5Multimedia.dll"
    "${QT_HOME}/bin/Qt5MultimediaWidgets.dll"
    "${QT_HOME}/bin/Qt5Network.dll"
    "${QT_HOME}/bin/Qt5OpenGL.dll"
    "${QT_HOME}/bin/Qt5Svg.dll"
    "${QT_HOME}/bin/Qt5Widgets.dll"
    "${QT_HOME}/bin/D3Dcompiler_47.dll"
    "${QT_HOME}/bin/libEGL.dll"
    "${QT_HOME}/bin/libGLESV2.dll"
    "${QT_HOME}/bin/opengl32sw.dll"
    DESTINATION "${DEPLOY_DIR}")

file(COPY
    "${QT_HOME}/plugins/audio/qtaudio_windows.dll"
    "${QT_HOME}/plugins/audio/qtaudio_wasapi.dll"
    DESTINATION "${DEPLOY_DIR}/audio")

file(COPY
    "${QT_HOME}/plugins/bearer/qgenericbearer.dll"
    DESTINATION "${DEPLOY_DIR}/bearer")

file(COPY
    "${QT_HOME}/plugins/iconengines/qsvgicon.dll"
    DESTINATION "${DEPLOY_DIR}/iconengines")

file(COPY
    "${QT_HOME}/plugins/imageformats/qgif.dll"
    "${QT_HOME}/plugins/imageformats/qicns.dll"
    "${QT_HOME}/plugins/imageformats/qico.dll"
    "${QT_HOME}/plugins/imageformats/qjpeg.dll"
    "${QT_HOME}/plugins/imageformats/qsvg.dll"
    "${QT_HOME}/plugins/imageformats/qtga.dll"
    "${QT_HOME}/plugins/imageformats/qtiff.dll"
    "${QT_HOME}/plugins/imageformats/qwbmp.dll"
    "${QT_HOME}/plugins/imageformats/qwebp.dll"
    DESTINATION "${DEPLOY_DIR}/imageformats")

file(COPY
    "${QT_HOME}/plugins/mediaservice/qtmedia_audioengine.dll"
    "${QT_HOME}/plugins/mediaservice/wmfengine.dll"
    DESTINATION "${DEPLOY_DIR}/mediaservice")
file(REMOVE "${DEPLOY_DIR}/mediaservice/dsengine.dll")

file(COPY
    "${QT_HOME}/plugins/platforms/qwindows.dll"
    DESTINATION "${DEPLOY_DIR}/platforms")

file(COPY
    "${QT_HOME}/plugins/playlistformats/qtmultimedia_m3u.dll"
    DESTINATION "${DEPLOY_DIR}/playlistformats")

file(COPY
    "${QT_HOME}/plugins/styles/qwindowsvistastyle.dll"
    DESTINATION "${DEPLOY_DIR}/styles")

message(STATUS "Qt runtime copied to ${DEPLOY_DIR}")
