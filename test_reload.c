#include "steve.h"
#include "steves_reloader.h"

#include "test_reload.h"

#include <unistd.h>

int main() {
    Foo *foo;

    FunctionSpec function = {.function_name = "foo", .function_ptr = (void*)&foo};

    LibrarySpec spec = {
        .library_file = "lib_test_reload.dylib",
        .num_functions = 1,
        .functions = &function
    };

    Library library = libraryInit(spec);

    for (;;) {
        if (libraryReload(&library)) {
            logInfo("foo returns: %i", foo());
        }
        logInfo("sleeping");
        sleep(1);
    }
}