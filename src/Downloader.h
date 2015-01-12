/**
 *  @file Downloader.h
 *  @brief Main header of the whole program.
 */

#ifndef DOWNLOADER_H
#define	DOWNLOADER_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fstream>
#include <algorithm>    // std::fill_n
#include <vector>
#include <fcntl.h>
#include <list>
#include <set>
#include <regex.h>
#include <sys/types.h>
#include "Structures.h"
#include "Exception.h"
#include "Filemanager.h"

/**
 * Class that handles downloading of pages.
 */
class Downloader {
protected:

    Options options; ///< User options.

    Filemanager manager; ///< Filemanager

    std::list<Page> queue; ///< Download queue

    std::set<Address> tabu; ///< Already downloaded or queued pages

    /**
     * Connects to the server.
     * @param address Address of the server.
     * @return Socket descriptor.
     */
    int newConnection(const char * address) const;



    /**
     * Reads a response from the server.
     * @param sd Socket descriptor.
     * @param response Output parameter in which is response stored.
     * @return HTTP status code.
     */
    int getResponse(int sd, std::string & response) const;

    /**
     * Extracts HTTP status code from header. 
     * @param header HTTP header.
     * @return HTTP status code.
     */
    int getHeaderType(const std::string & header) const;

    /**
     * Get a new address from 3xx header.
     * @param header HTTP header.
     * @return New address.
     */
    std::string getNewLocation(const std::string & header) const;

    /**
     * Translates absolute address to relative.
     * @param root Reference address.
     * @param toConvert Address to be converted.
     * @return Converted relative address.
     */
    std::string getRelativeFromAbsolute(const std::string & root, const std::string & toConvert) const;

public:

    /**
     * A constructor
     * @param opts Container with options.
     * @param s Container with synchronization
     */
    Downloader(const Options & opts);

    /**
     * A constructor
     */
    Downloader(void) {
    };


    /**
     * Downloads webpage.
     * It connects to the server, downloads page, parse it and saves it.
     * @param addr URL of the page.
     * @param currentDepth Current depth of page.
     * @return Path to the downloaded file.
     */
    void getPage(const Address & addr, int currentDepth = 0);

    /**
     * Downloads raw file.
     * It connects to the server, downloads file and saves it without change.
     * @param addr URL of the page.
     * @param currentDepth Current depth of page.
     * @return Path to the downloaded file.
     */
    void getRawPage(const Address & addr, int currentDepth = 0);

    /**
     * Parses address.
     * Sanitizes and splits URL into several pieces. 
     * @param address Address to be parsed.
     * @param parentAddress Address of parent (for relative addresses).
     * @return Structured address.
     */
    const Address parseAddress(const std::string & address, const Address & parentAddress = Address()) const;

    /**
     * Inserts new entry to download queue.
     * @param addr Address to be inserted.
     * @param currentDepth Depth of new page.
     */
    inline void addToQueue(const Address & addr, int currentDepth = 0) {
        Page p;
        if (tabu.find(addr) == tabu.end()) {
            p.address = addr;
            p.depth = currentDepth;
            queue.push_back(p);
            tabu.insert(addr);
        }
    }

    /**
     * Inserts new entry to download queue.
     * @param p Page to be inserted.
     */
    inline void addToQueue(const Page & p) {
        queue.push_back(p);
    }

    /**
     * Inserts page to tabu list.
     * @param a Address of page to be inserted
     */
    inline void addToTabu(const Address & a) {
        tabu.insert(a);
    }

    /**
     * Return if the page is in tabu list.
     * @param addr Address of page.
     * @return True if tage is tabued, false otherwise.
     */
    inline bool isInTabu(const Address & addr) {
        return tabu.find(addr) != tabu.end();

    }

    /**
     * Retrieves page from queue and.
     * @return The page.
     */
    inline Page getFromQueue() {
        Page p = queue.front();
        queue.pop_front();
        return p;
    }

    /**
     * Chcecks whether queue is empty.
     * @return True if queue is empty, false otherwise.
     */
    inline bool empty() {
        return queue.empty();
    }
};

#include "Tag.h"    //there was problem with inheritace when it was at the top (incomplete type Downloader))

#endif	/* DOWNLOADER_H */

