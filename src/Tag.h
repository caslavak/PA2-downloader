/**
 * @file Tag.h
 * @brief Header file of classes that handles HTML tags parsing.
 */

#ifndef TAG_H
#define	TAG_H

#include "Downloader.h"

#include <string>
#include <vector>
#include <regex.h>
#include <sys/types.h>
//TEST
#include <iostream>
#include <cstdio>



#define A 0         ///< Constant for a tag
#define LINK 1      ///< Constant for link tag
#define IMG 2       ///< Constant for img tag
#define SCRIPT 3    ///< Constant for script tag

/**
 * Class for common operations with tags.
 */
class Tag : public Downloader {
private:

protected:

    /**
     * Struct that stores boundaries of tag.
     */
    struct Bounds {
        int left; ///< An offset of the start of a tag.
        int right; ///< An offset of the end of a tag.

        /**
         * A constructor
         * @param l An offset of the start of a tag.
         * @param r An offset of the end of a tag.
         */
        Bounds(int l, int r)
        : left(l), right(r) {
        };
    };

    /**
     * Searches for tags.
     * This function search for tags in the HTML page using regular expressions.
     * @param text A HTML page.
     * @param type Type of tag to search for.
     * @return A vector of boundaries of tags.
     */
    std::vector<Bounds> regexer(const std::string & text, char type);
public:

    /**
     * A constructor.
     * @param options Options for downloader.
     * @param syn Synchronization.
     */
    Tag(const Options & options)
    : Downloader(options) {
    };
};

/**
 * Handler for a tags
 */
class TagA : public Tag {
public:

    /**
     * A constructor.
     * @param options Options for downloader.
     * @param syn Synchronization.
     */
    TagA(const Options & options)
    : Tag(options) {
    };

    /**
     * Processes the text.
     * Finds the tags, downloads and process the refered URL and replaces tag's link to new address
     * @param text A HTML page.
     * @param down Reference to downloader class.
     * @param parentAddress Address of parental page.
     * @param currentDepth Depth of parental page.
     */
    void process(std::string & text, Downloader & down, const Address & parentAddress, int currentDepth);
};

/**
 * Handler for img, link and script tags
 */
class TagOther : public Tag {
public:

    /**
     * A constructor.
     * @param options Options for downloader.
     * @param syn Synchronization.
     */
    TagOther(const Options & options)
    : Tag(options) {
    };

    /**
     * Processes the text.
     * Finds the tags, downloads the refered URL as raw file and replaces tag's link to new address
     * @param text A HTML page.
     * @param down Reference to downloader class.
     * @param parentAddress Address of parental page.
     * @param currentDepth Depth of parental page.
     */
    void process(std::string & text, Downloader & down, const Address & parentAddress, int currentDepth);


};
#endif	/* TAG_H */