#ifndef LIBRARY_HANDLER_H
#define LIBRARY_HANDLER_H

class LibraryHandler {
public:
    static LibraryHandler *GetLibraryHandler();
    static void Close();

private:
    static LibraryHandler *sInstance;

    void *prefPath;

    LibraryHandler();
    ~LibraryHandler();
};

#endif //LIBRARY_HANDLER_H
