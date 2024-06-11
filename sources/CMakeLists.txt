cmake_minimum_required(VERSION 3.16)
project(Project-KAPE VERSION 0.1.0)

# abilita il supporto per i test, tra cui l'opzione BUILD_TESTING usata sotto
include(CTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# richiedi l'uso di C++17, senza estensioni non-standard offerte dal compilatore usato 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# abilita warning
string(APPEND CMAKE_CXX_FLAGS
      " -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion"
      " -Wshadow -Wimplicit-fallthrough -Wextra-semi -Wold-style-cast ")

      
# abilita asserzioni di debug (in gcc), l'address sanitizer e l'undefined-behaviour sanitizer in debug mode
string(APPEND CMAKE_CXX_FLAGS_DEBUG " -D_GLIBCXX_ASSERTIONS -fsanitize=address,undefined -fno-omit-frame-pointer")
string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG " -fsanitize=address,undefined -fno-omit-frame-pointer")

#for analysis
string(APPEND CMAKE_CXX_FLAGS "-pg")
      
# richiedi il componente graphics della libreria SFML, versione 2.5
#   le dipendenze vengono identificate automaticamente
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

add_executable(project-kape main.cpp geometry.cpp environment.cpp ants.cpp  drawing.cpp simulation.cpp logger.cpp)
target_link_libraries(project-kape PRIVATE sfml-graphics)

# se il testing e' abilitato...
#   per disabilitare il testing, passare -DBUILD_TESTING=OFF a cmake durante la fase di configurazione
if (BUILD_TESTING)

  # aggiungi l'eseguibile all.t
add_executable(geometry_test.t geometry.t.cpp geometry.cpp)
add_executable(environment_test.t geometry.cpp environment.t.cpp environment.cpp logger.cpp)
add_executable(ant_test.t ants.t.cpp ants.cpp geometry.cpp environment.cpp logger.cpp)
  # aggiungi l'eseguibile all.t alla lista dei test
  add_test(NAME geometry_test.t COMMAND geometry_test.t)
  add_test(NAME environment_test.t COMMAND environment_test.t)
  add_test(NAME ant_test.t COMMAND ant_test.t)
endif()
