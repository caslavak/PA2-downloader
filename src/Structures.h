/**
 * @file Structures.h
 * @brief Header file that contains helper structures 
 */

#ifndef STRUCTURES_H
#define	STRUCTURES_H

#include <string>
#include <iostream>


/** 
 * Structure which stores parsed addres.  
 */
struct Address {
    //std::string protocol = "http";       
    std::string host; ///< name of the server
    std::string path = "/"; ///< path to the file

    /** A constructor. */
    Address(void) {
    };

    bool operator<(const Address & x) const {
        return (host + path) < (x.host + x.path);
    }

    /** Overloaded << operator */
    friend std::ostream & operator <<(std::ostream & os, const Address & addr) {
        os << "Address: \nHost: " << addr.host << "\nPath: " << addr.path << std::endl;
        return os;
    }
};

/**
 * Item of download queue.
 */
struct Page {
    Address address;    ///< Address of page
    int depth;          ///< deepth of page
    bool raw = false;   ///< Download wiithout processing?
};

/** 
 * Structure which stores user options.
 */
struct Options {
    int maxDepth = 1; ///< Specifies maximal depth of pages that are processed.
    bool ignoreRedirects = false; ///< Defines whether response headers 3xx should be counted as next-level of download.
    bool verbose = false; ///< Defines whether response headers 3xx should be counted as next-level of download.
    bool downloadImages = false; ///< Defines whether images will be downloaded regardless its depth.
    std::string outputDirectory = "./"; ///< Specifies where to save page.

    /** A constructor. */
    Options() {
    };

    /** A constructor. 
     *  @param tMaxDepth Maximal depth of download.
     *  @param tIgnoreRedirects If true, redirects won't increment page depth.
     *  @param tOutputDirectory Directory where downloaded data will be saved.
     *  @param tDownloadImages If true, images will be downloaded regardless its depth.
     */
    Options(int tMaxDepth, bool tIgnoreRedirects = false, const std::string & tOutputDirectory = "", bool tDownloadImages = false)
    : maxDepth(tMaxDepth), ignoreRedirects(tIgnoreRedirects), downloadImages(tDownloadImages), outputDirectory(tOutputDirectory) {
        if (outputDirectory.back() != '/') outputDirectory += "/";
    };
};


#endif	/* STRUCTURES_H */

