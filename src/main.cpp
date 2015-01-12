/**
 * @file main.cpp
 * @brief Entry point of the program.
 */

#include <cstdlib>
#include <sstream>
#include <getopt.h>
#include <string.h>
#include <string>
#include <regex.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>

#include "Downloader.h"
#include "Filemanager.h"
#include "Structures.h"

#define THREADS 1

using namespace std;

int main(int argc, char** argv) {
    const char *optString = "id:o:vhp?";

    const struct option longOpts[] = {
        { "verbose", no_argument, NULL, 'v'},
        { "ignore-redirect", no_argument, NULL, 'i'},
        { "output", required_argument, NULL, 'o'},
        { "max-depth", required_argument, NULL, 'd'},
        { "help", no_argument, NULL, 'h'},
        { "images", no_argument, NULL, 'p'},
        { NULL, no_argument, NULL, 0}
    };

    Options opts;
    string path;

    int opt = getopt_long(argc, argv, optString, longOpts, NULL);
    while (opt != -1) {
        switch (opt) {
            case 'v':
                opts.verbose = true;
                break;

            case 'i':
                opts.ignoreRedirects = true;
                break;

            case 'p':
                opts.downloadImages = true;
                break;

            case 'o':
                opts.outputDirectory = optarg;
                if (opts.outputDirectory.back() != '/') opts.outputDirectory += "/";
                break;

            case 'd':
                opts.maxDepth = stoi(optarg);
                break;
            case 'h':
            case '?':
                cout << "Usage: " << argv[0] << " [-o <directory>] [-v][-i][-p][-m <number>]" << endl
                        << "Description" << endl
                        << "-o, --output <directory> -> directory where to download pages" << endl
                        << "-d, --max-depth <number> -> maximal depth of downloaded pages (default: 1)" << endl
                        << "-i, --ignore-redirects -> redirects won't be counted do depth" << endl
                        << "-v, --verbose -> enable verbose mode" << endl
                        << "-p, --images -> always download images, regardless its depth" << endl;
                return 1;

            default:
                break;
        }

        opt = getopt_long(argc, argv, optString, longOpts, NULL);
    }
    string address;
    int tries = 0;
    regex_t reg;
    regmatch_t match;
    cout << "Please, enter the URL of your desired target." << endl;
    if (regcomp(&reg, "^(http[s]?://|ftp://|//)", REG_EXTENDED | REG_ICASE) != 0) exit(2);
    while (1) {
        cin >> address;
        if (regexec(&reg, address.c_str(), 1, &match, 0) == 0) break;
        else {
            tries++;
            fflush(stdin);
            if (tries == 1) {
                cout << "Invalid URL. Try it another one..." << endl;
            } else if (tries == 2) {
                cout << "Again? Alright, just one more try." << endl;
            } else if (tries == 3) {
                cout << "OK, that's enough! Godbye!" << endl;
                regfree(&reg);
                return 1;
            }
        }
    }
    regfree(&reg);

    Downloader d(opts);

    Filemanager fm(opts.outputDirectory, opts.verbose);
    Address ad = d.parseAddress(address);
    d.addToQueue(ad); //enqueue the new address
    string tmpPath = ad.host + ad.path;
    if (tmpPath.back() == '/') {
        tmpPath += "auto_index.html";
    }
    fm.createHomepage(tmpPath);

    while (1) {
        if (d.empty()) break;
        else {
            Page p = d.getFromQueue();
            try {
                if (p.raw) d.getRawPage(p.address, p.depth);
                else d.getPage(p.address, p.depth);
            } catch (...) {
            }
        }
    }

    //cout << endl << "------- " << d.inTabu() << " files has been downloaded." << " ------- " << endl;
    return 0;
}

