#ifndef LIBRARY_HANDLER_H
#define LIBRARY_HANDLER_H

#include <fstream>

class LibraryHandler {
public:
    static LibraryHandler *GetLibraryHandler();
    static void Close();

    void Insert(const char *path, bool isDirectory);

private:
    static LibraryHandler *sInstance;

    char *prefPath;

    std::fstream library;

    LibraryHandler();
    ~LibraryHandler();
};

#endif //LIBRARY_HANDLER_H
