# Generated from linboGUI.pro.

cmake_minimum_required(VERSION 3.16)
project(linbo_gui LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
  set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "/usr/bin")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Widgets)
find_package(Qt6 COMPONENTS Svg)
find_package(Qt6 COMPONENTS SvgWidgets)
find_package(Qt6 COMPONENTS Xml)
find_package(Qt6 COMPONENTS LinguistTools)

qt_add_executable(linbo_gui "")

set(SOURCE_FILES
    headers/backend/linbobackend.h
    headers/backend/linbocmd.h
    headers/backend/linboconfig.h
    headers/backend/linboconfigreader.h
    headers/backend/linbodiskpartition.h
    headers/backend/linboimage.h
    headers/backend/linbologger.h
    headers/backend/linboos.h
    headers/backend/linbopostprocessactions.h
    headers/backend/linbotheme.h
    headers/frontend/components/linbocheckbox.h
    headers/frontend/components/linbocombobox.h
    headers/frontend/components/linbodialog.h
    headers/frontend/components/linbolineedit.h
    headers/frontend/components/linboprogressbar.h
    headers/frontend/components/linbopushbutton.h
    headers/frontend/components/linbopushbuttonoverlay.h
    headers/frontend/components/linboradiobutton.h
    headers/frontend/components/linbostackedwidget.h
    headers/frontend/components/linbosvgwidget.h
    headers/frontend/components/linboterminal.h
    headers/frontend/components/linbotextbrowser.h
    headers/frontend/components/linbotoolbutton.h
    headers/frontend/dialogs/linboconfirmationdialog.h
    headers/frontend/dialogs/linboimagecreationdialog.h
    headers/frontend/dialogs/linboimageuploaddialog.h
    headers/frontend/dialogs/linbologindialog.h
    headers/frontend/dialogs/linboregisterdialog.h
    headers/frontend/dialogs/linboterminaldialog.h
    headers/frontend/dialogs/linboupdatecachedialog.h
    headers/frontend/linboclientinfo.h
    headers/frontend/linboguitheme.h
    headers/frontend/linbomainactions.h
    headers/frontend/linbomainpage.h
    headers/frontend/linboosselectbutton.h
    headers/frontend/linboosselectionrow.h
    headers/linbogui.h
    sources/backend/linbobackend.cpp
    sources/backend/linbocmd.cpp
    sources/backend/linboconfig.cpp
    sources/backend/linboconfigreader.cpp
    sources/backend/linbodiskpartition.cpp
    sources/backend/linboimage.cpp
    sources/backend/linbologger.cpp
    sources/backend/linboos.cpp
    sources/backend/linbotheme.cpp
    sources/frontend/components/linbocheckbox.cpp
    sources/frontend/components/linbocombobox.cpp
    sources/frontend/components/linbodialog.cpp
    sources/frontend/components/linbolineedit.cpp
    sources/frontend/components/linboprogressbar.cpp
    sources/frontend/components/linbopushbutton.cpp
    sources/frontend/components/linbopushbuttonoverlay.cpp
    sources/frontend/components/linboradiobutton.cpp
    sources/frontend/components/linbostackedwidget.cpp
    sources/frontend/components/linbosvgwidget.cpp
    sources/frontend/components/linboterminal.cpp
    sources/frontend/components/linbotextbrowser.cpp
    sources/frontend/components/linbotoolbutton.cpp
    sources/frontend/dialogs/linboconfirmationdialog.cpp
    sources/frontend/dialogs/linboimagecreationdialog.cpp
    sources/frontend/dialogs/linboimageuploaddialog.cpp
    sources/frontend/dialogs/linbologindialog.cpp
    sources/frontend/dialogs/linboregisterdialog.cpp
    sources/frontend/dialogs/linboterminaldialog.cpp
    sources/frontend/dialogs/linboupdatecachedialog.cpp
    sources/frontend/linboclientinfo.cpp
    sources/frontend/linboguitheme.cpp
    sources/frontend/linbomainactions.cpp
    sources/frontend/linbomainpage.cpp
    sources/frontend/linboosselectbutton.cpp
    sources/frontend/linboosselectionrow.cpp
    sources/linbogui.cpp
    sources/main.cpp
)

target_sources(linbo_gui PRIVATE ${SOURCE_FILES})

target_include_directories(linbo_gui PUBLIC
    headers
    headers/backend
    headers/frontend
    headers/frontend/components
    headers/frontend/dialogs
)

target_link_libraries(linbo_gui PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Svg
    Qt::SvgWidgets
    Qt::Widgets
    Qt::Xml
)


# Resources:
set(linbo_resource_files
    "resources/fonts/PTSans-Bold.ttf"
    "resources/fonts/PTSans-BoldItalic.ttf"
    "resources/fonts/PTSans-Italic.ttf"
    "resources/fonts/PTSans-Regular.ttf"
    "resources/fonts/UbuntuMono-B.ttf"
    "resources/fonts/UbuntuMono-BI.ttf"
    "resources/fonts/UbuntuMono-R.ttf"
    "resources/fonts/UbuntuMono-RI.ttf"
    "resources/icons/dark/back.svg"
    "resources/icons/dark/brandBottom.svg"
    "resources/icons/dark/brandTop.svg"
    "resources/icons/dark/cancel.svg"
    "resources/icons/dark/checkBoxChecked.svg"
    "resources/icons/dark/checkBoxUnchecked.svg"
    "resources/icons/dark/desktop.svg"
    "resources/icons/dark/image.svg"
    "resources/icons/dark/logout.svg"
    "resources/icons/dark/network.svg"
    "resources/icons/dark/overlayChecked.svg"
    "resources/icons/dark/overlayHovered.svg"
    "resources/icons/dark/overlayPressed.svg"
    "resources/icons/dark/partition.svg"
    "resources/icons/dark/radioButtonChecked.svg"
    "resources/icons/dark/radioButtonDisabled.svg"
    "resources/icons/dark/radioButtonUnchecked.svg"
    "resources/icons/dark/reboot.svg"
    "resources/icons/dark/register.svg"
    "resources/icons/dark/reinstall.svg"
    "resources/icons/dark/settings.svg"
    "resources/icons/dark/shutdown.svg"
    "resources/icons/dark/start.svg"
    "resources/icons/dark/sync.svg"
    "resources/icons/dark/terminal.svg"
    "resources/icons/dark/upload.svg"
    "resources/icons/light/back.svg"
    "resources/icons/light/brandBottom.svg"
    "resources/icons/light/brandTop.svg"
    "resources/icons/light/cancel.svg"
    "resources/icons/light/checkBoxChecked.svg"
    "resources/icons/light/checkBoxUnchecked.svg"
    "resources/icons/light/desktop.svg"
    "resources/icons/light/image.svg"
    "resources/icons/light/logout.svg"
    "resources/icons/light/network.svg"
    "resources/icons/light/overlayChecked.svg"
    "resources/icons/light/overlayHovered.svg"
    "resources/icons/light/overlayPressed.svg"
    "resources/icons/light/partition.svg"
    "resources/icons/light/radioButtonChecked.svg"
    "resources/icons/light/radioButtonDisabled.svg"
    "resources/icons/light/radioButtonUnchecked.svg"
    "resources/icons/light/reboot.svg"
    "resources/icons/light/register.svg"
    "resources/icons/light/reinstall.svg"
    "resources/icons/light/settings.svg"
    "resources/icons/light/shutdown.svg"
    "resources/icons/light/start.svg"
    "resources/icons/light/sync.svg"
    "resources/icons/light/terminal.svg"
    "resources/icons/light/upload.svg"
    "resources/icons/universal/defaultOs.svg"
    "resources/icons/universal/overlayImage.svg"
    "resources/icons/universal/overlayKeyboardFocus.svg"
    "resources/icons/universal/overlayReinstall.svg"
    "resources/icons/universal/overlayStart.svg"
    "resources/icons/universal/overlaySync.svg"
    "resources/icons/universal/reinstallLegacy.svg"
    "resources/icons/universal/startLegacy.svg"
    "resources/icons/universal/syncLegacy.svg"
    "resources/icons/universal/uploadLegacy.svg"
    "resources/images/linbo_logo_small.svg"
)

qt6_add_resources(linbo_gui "linbo"
    PREFIX
        "/"
    BASE
        "resources"
    FILES
        ${linbo_resource_files}
)

qt_add_translations(linbo_gui TS_FILES
    "resources/translations/linbo-gui/blank.ts"
    "resources/translations/linbo-gui/af-ZA.ts"
    "resources/translations/linbo-gui/ar-SA.ts"
    "resources/translations/linbo-gui/ca-ES.ts"
    "resources/translations/linbo-gui/cs-CZ.ts"
    "resources/translations/linbo-gui/da-DK.ts"
    "resources/translations/linbo-gui/de-DE.ts"
    "resources/translations/linbo-gui/el-GR.ts"
    "resources/translations/linbo-gui/en-GB.ts"
    "resources/translations/linbo-gui/es-ES.ts"
    "resources/translations/linbo-gui/fi-FI.ts"
    "resources/translations/linbo-gui/fr-FR.ts"
    "resources/translations/linbo-gui/he-IL.ts"
    "resources/translations/linbo-gui/hu-HU.ts"
    "resources/translations/linbo-gui/it-IT.ts"
    "resources/translations/linbo-gui/ja-JP.ts"
    "resources/translations/linbo-gui/ko-KR.ts"
    "resources/translations/linbo-gui/nl-NL.ts"
    "resources/translations/linbo-gui/no-NO.ts"
    "resources/translations/linbo-gui/pl-PL.ts"
    "resources/translations/linbo-gui/pt-BR.ts"
    "resources/translations/linbo-gui/pt-PT.ts"
    "resources/translations/linbo-gui/ro-RO.ts"
    "resources/translations/linbo-gui/ru-RU.ts"
    "resources/translations/linbo-gui/sr-SP.ts"
    "resources/translations/linbo-gui/sv-SE.ts"
    "resources/translations/linbo-gui/tr-TR.ts"
    "resources/translations/linbo-gui/uk-UA.ts"
    "resources/translations/linbo-gui/vi-VN.ts"
    "resources/translations/linbo-gui/zh-CN.ts"
    "resources/translations/linbo-gui/zh-TW.ts"
)

qt6_add_resources(linbo_gui "translations"
    PREFIX
        "/"
    BASE
        "resources/translations/linbo-gui"
    FILES
        ${translations_resource_files}
)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(linbo_gui PUBLIC
        TEST_ENV=\"${CMAKE_CURRENT_SOURCE_DIR}/fakeroot\"
    )
endif()

install(TARGETS linbo_gui
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)

target_compile_definitions(linbo_gui PUBLIC
    GUI_VERSION=\"\\\"7.1.0~rc0\\\"\"
)

qt_import_plugins(linbo_gui
    INCLUDE Qt6::QLinuxFbIntegrationPlugin)
