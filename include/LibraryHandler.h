#ifndef LIBRARY_HANDLER_H
#define LIBRARY_HANDLER_H

#include <fstream>
#include <vector>
#include <string>

typedef struct ScanContext {
    std::vector<std::string> files;
    std::vector<std::string> titles;
    int count = 0;
} ScanContext;

class LibraryHandler {
public:
    static LibraryHandler *GetLibraryHandler();
    static void Close();

    ScanContext *GetContext();

    void QueueTrack(int index) const;

    void ScanLibrary();

private:
    static LibraryHandler *sInstance;

    char *prefPath;

    std::fstream library;

    ScanContext ctx;

    LibraryHandler();
    ~LibraryHandler();

    std::vector<std::string> ScanDirectory(const char *root);

    bool CheckCount();
};

#endif //LIBRARY_HANDLER_H
