// This is a small lib to allow dynamically reloading libraries.
// Create an library that contains your main app logic.
// Implement the main loop of your program in another program that calls
// functions in the main loop.
// With this lib you can then reload the library each time it is recompiled which
// makes it possible to livecode things from c.
// It just supports OSX to start with because that's what i'm on right now but the same
// thing can be done on the big three desktop platforms.
//
// @TODO: implement the windows and linux versions in here.
// @TODO: Make it easier to find the library when the program isn't run from the
//        same directory as the exe.
// @Q: How do I get the directory the exe is in?

#ifndef _steves_reloader_h
#define _steves_reloader_h

#include "steve.h"

#include <sys/stat.h>
#include <dlfcn.h>

typedef struct {
    char *function_name;
    void **function_ptr;
} FunctionSpec;

typedef struct {
    char *library_file;
    int num_functions;
    FunctionSpec *functions;
} LibrarySpec;

typedef struct {
    LibrarySpec spec;

    void *handle;
    struct timespec mtime;
} Library;

Library
libraryInit(LibrarySpec spec)
{
    Library library = {0};
    library.spec = spec;

    return library;
}

// Checks if the library has been updated and if it has reloads it.
int
libraryReload(Library *library)
{
    struct stat attr;
    if ((stat(library->spec.library_file, &attr) == 0) &&
        (library->mtime.tv_nsec != attr.st_mtimespec.tv_nsec ||
         library->mtime.tv_sec != attr.st_mtimespec.tv_sec)) {
        logInfo("New library to load");
        
        if (library->handle) {
            dlclose(library->handle);
        }
        
        void* handle = dlopen(library->spec.library_file, RTLD_NOW);

        if (handle) {
            library->handle = handle;
            library->mtime = attr.st_mtimespec;

            for (int i=0; i<library->spec.num_functions; i++) {
                FunctionSpec spec = library->spec.functions[i];
                void *fn = dlsym(handle, spec.function_name);
                if (fn != NULL) {
                    *spec.function_ptr = fn;
                } else {
                    logError("Error loading api symbol %s", spec.function_name);
                    library->handle = NULL;
                    library->mtime.tv_nsec = 0;
                    library->mtime.tv_sec = 0;
                    return 0;
                }
            }

            logInfo("Library reloaded");
            return 1;
            
        } else {
            logError("Error loading game library");
            library->handle = NULL;
            library->mtime.tv_nsec = 0;
            library->mtime.tv_sec = 0;
            return 0;
        }
    }
    
    return 0;
}

// Useful if you want to unload manually to do something like load another one.
void
libraryUnload(Library *library)
{
    if (library->handle) {
        dlclose(library->handle);
    }
}

#endif