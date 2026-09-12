cmake_minimum_required(VERSION 4.3.3)

project(Scopalatro LANGUAGES CXX)
add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME}	PUBLIC cxx_std_26)

#set(CMAKE_CXX_STANDARD 26)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src/Card.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CombatModel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CombatPresenter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CombatTable.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CombatView.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Deck.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/EffectSystem.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Enemy.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Game.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/HandArea.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/HoverMap.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/TaskManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/TaskQueue.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(TetoEngine REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE TetoEngine::TetoEngine)

set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# installation
include(GNUInstallDirs)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ScopalatroTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT ScopalatroTargets
    FILE ScopalatroTargets.cmake
    NAMESPACE TetoEngine::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TetoEngine
)
