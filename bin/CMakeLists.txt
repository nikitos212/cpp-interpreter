add_executable( ${PROJECT_NAME} 
    main.cpp
    MainWindow.cpp
    ItmoWrapper.cpp
    CodeEditor.cpp
    ConsoleWidget.cpp
    ItmoHighlighter.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE itmoscript)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})