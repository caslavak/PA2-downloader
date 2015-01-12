/**
 * @file Filemanager.h
 * @brief Functions that operate with filesystem. 
 */

#ifndef FILEMANAGER_H
#define	FILEMANAGER_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <vector>
#include "Exception.h"
#include "Structures.h"
#include "Balast.h"

/**
 * Class that handles operations with filesystem.
 */
class Filemanager {
private:

    std::string rootPath; ///< Path that points to the place where files will be stored.
    bool verbose; ///< Verbose mode indicator.

    /**
     * Makes directory.
     * Makes specified directory and all of its parrents, if neccessary. 
     * @param path Path to create.
     */
    void makeDirectory(const std::string & path) const;

public:
    /**
     * A constructor.
     */
    Filemanager(void);

    /**
     * A constructor.
     * @param tRootPath Path that points to the place where files will be stored.
     * @param tVerbose  If true, verbose mode is will be on.
     */
    Filemanager(const std::string & tRootPath, bool tVerbose);

    /**
     * Creates 404 page. 
     * @return Address of the page.
     */
    inline std::string createPage404(const Address & addr) const {
        return createPage(addr.host, addr.path, Balast().get404());
    }

    /**
     * Creates Max depth page. 
     * @return Address of the page.
     */
    inline std::string createPageMaxDepth() const {
        return createPage("balast", "/max_depth.html", Balast().getMaxDepth());
    }

    /**
     * Creates 500 page. 
     * @return Address of the page.
     */
    inline std::string createPage500(const Address & addr) const {
        return createPage(addr.host, addr.path, Balast().get500());
    }

    /**
     * Creates Save error page. 
     * @return Address of the page.
     */
    inline std::string createPageSaveError(const Address & addr) const {
        return createPage(addr.host, addr.path, Balast().getSaveError());
    }

    /**
     * Creates homepage. 
     * @return Address of the page.
     */
    inline std::string createHomepage(const std::string & link) const {
        return createPage("", "downloaded_page.html", Balast().getHomepage(link));
    }

    /**
     * Creates redirect page. 
     * @param addr Address of the page
     * @param link Link to new location 
     * @return Address of the redirect page.
     */
    inline std::string createRedirect(const Address & addr, const std::string & link) const {
        return createPage(addr.host, addr.path, Balast().getRedirect(link));
    }

    /**
     * Checks for existence of file.
     * @param path Path of the file.
     * @return True if file exists, false otherwise.
     */
    inline bool fileExists(const std::string & path) const {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
    }

    /**
     * Creates page.
     * Creates page of content specified by "content".
     * @param host Server name.
     * @param path Path to the page.
     * @param content Content of the page.
     * @return Address of the page.
     */
    std::string createPage(const std::string & host, std::string path, const std::string & content) const;

    /**
     * Creates raw file.
     * Creates raw file where the content is stored as is.
     * @param host Server name.
     * @param path Path to the page.
     * @param content Content of the file.
     * @return Address of the page.
     */
    std::string createRawPage(const std::string & host, std::string path, const std::vector<char> & content) const;

};

#endif	/* FILEMANAGER_H */