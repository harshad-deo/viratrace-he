clang-format -i -style="{BasedOnStyle: llvm, ColumnLimit: 120}" src/*.cpp
clang-format -i -style="{BasedOnStyle: llvm, ColumnLimit: 120}" test/*.cpp
clang-format -i -style="{BasedOnStyle: llvm, ColumnLimit: 120}" bench/src/*.cpp
clang-format -i -style="{BasedOnStyle: llvm, ColumnLimit: 120}" include/*.h