
#include "Tag.h"
#include "Exception.h"

using namespace std;

vector<Tag::Bounds> Tag::regexer(const string & input, char type) {
    string primaryRegexPattern;
    string secondaryRegexPattern;
    regex_t primary;
    regex_t secondary;
    regmatch_t match;
    regmatch_t miniMatch;
    vector<Bounds> results;
    switch (type) {
        case A:
            primaryRegexPattern = "<[[:blank:]]*a[^>]*href=[\"'][^\"']*[\"'][^>]*>";
            secondaryRegexPattern = "href=[\"'][^\"']*[\"']";
            break;
        case LINK:
            primaryRegexPattern = "<[[:blank:]]*link[^>]*href=[\"'][^\"']*[\"'][^>]*>";
            secondaryRegexPattern = "href=[\"'][^\"']*[\"']";
            break;
        case IMG:
            primaryRegexPattern = "<[[:blank:]]*img[^>]*src=[\"'][^\"']*[\"'][^>]*>";
            secondaryRegexPattern = "src=[\"'][^\"']*[\"']";
            break;
        case SCRIPT:
            primaryRegexPattern = "<[[:blank:]]*script[^>]*src=[\"'][^\"']*[\"'][^>]*>";
            secondaryRegexPattern = "src=[\"'][^\"']*[\"']";
            break;
    }

    const char * text = input.c_str();

    if (regcomp(&primary, primaryRegexPattern.c_str(), REG_EXTENDED | REG_ICASE) != 0) exit(1);
    if (regcomp(&secondary, secondaryRegexPattern.c_str(), REG_EXTENDED | REG_ICASE) != 0) exit(1);

    int leftOffset = 0;
    while (regexec(&primary, text, 1, &match, 0) == 0) {
        // cout << input.substr(match.rm_so + leftOffset, match.rm_eo - match.rm_so) << endl;
        string tmp = input.substr(match.rm_so + leftOffset, match.rm_eo - match.rm_so);
        if (regexec(&secondary, tmp.c_str(), 1, &miniMatch, 0) != 0) throw WTFException("REGEX: Secondary pattern doesn't match. WTF?");
        // cout << "*******************************" << endl;
        if (type == A || type == LINK) {
            results.push_back(Bounds(leftOffset + match.rm_so + miniMatch.rm_so + 6, leftOffset + match.rm_so + miniMatch.rm_eo - 1));
             //cout << "Extracted " << (int)type << ": " << input.substr(leftOffset + match.rm_so + miniMatch.rm_so + 6, match.rm_so + miniMatch.rm_eo - 1 - (match.rm_so + miniMatch.rm_so + 6)) << " - " << match.rm_eo << endl;
            // if(type == 1) cout << text << endl;
        } else {
            results.push_back(Bounds(leftOffset + match.rm_so + miniMatch.rm_so + 5, leftOffset + match.rm_so + miniMatch.rm_eo - 1));
            // cout << "Extracted " << (int)type << ": " <<  input.substr(leftOffset + match.rm_so + miniMatch.rm_so + 5, match.rm_so + miniMatch.rm_eo - 1 - (match.rm_so + miniMatch.rm_so + 5)) << endl;
        }

        text += match.rm_eo;
        leftOffset += match.rm_eo;
    }
    regfree(&primary);
    regfree(&secondary);
    return results;

}

void TagA::process(std::string & text, Downloader & down, const Address & parentAddress, int currentDepth) {
    vector<Bounds> occurrences = regexer(text, A); //get positions of addresses inside <a> tags
    int correction = 0;
    regex_t reg;
    regmatch_t match;
    regcomp(&reg, ".*\\.(pdf|tex|txt|ps|doc|doc[x]?|ppt[x]?|odt|swf|avi|mov|mkv|mp4|mp3|css|js|jpg|png|gif)$", REG_EXTENDED | REG_ICASE);

    for (vector<Bounds>::const_iterator it = occurrences.begin(); it != occurrences.end(); it++) {
        string sub = text.substr(it->left + correction, it->right - it->left);
        //cout << "Exp: " << sub << endl;
        string addr;

        if (sub.length() > 6) //skip mailto
            if (sub.substr(0, 6) == "mailto") continue;
        if(sub.front() == '#') continue;
        try {
            Page p;
            Address address = parseAddress(sub, parentAddress);
            if (regexec(&reg, sub.c_str(), 1, &match, 0) == 0) {
                p.raw = true;
            }
            if (!down.isInTabu(address) && currentDepth >= options.maxDepth) {
                if (!(p.raw && options.downloadImages)) addr = manager.createPageMaxDepth();
            } else {

                if (!down.isInTabu(address)) {
                    p.address = address;
                    p.depth = currentDepth + 1;
                    down.addToQueue(p);
                    down.addToTabu(address);
                }
            }
            if (addr.empty()) {
                if (address.path.back() == '/') {
                    if (p.raw) addr = getRelativeFromAbsolute(parentAddress.host + parentAddress.path, address.host + address.path + "auto_index");
                    else addr = getRelativeFromAbsolute(parentAddress.host + parentAddress.path, address.host + address.path + "auto_index.html");
                } else addr = getRelativeFromAbsolute(parentAddress.host + parentAddress.path, address.host + address.path);
            } else {
                addr = getRelativeFromAbsolute(parentAddress.host + parentAddress.path, addr);
            }
            text.erase(it->left + correction, it->right - it->left); //replace an old address with the new one
            text.insert(it->left + correction, addr);
            correction += addr.length() - (it->right - it->left); //correction for position inside string after replace
        } catch (...) {
        }
    }
    regfree(&reg);
}

void TagOther::process(std::string & text, Downloader & down, const Address & parentAddress, int currentDepth) {
    for (char i = 1; i < 4; i++) { //for IMG, LINK, SCRIPT
        vector<Bounds> occurrences = regexer(text, i); //get positions of addresses inside <a> tags
        int correction = 0;
        for (vector<Bounds>::const_iterator it = occurrences.begin(); it != occurrences.end(); it++) {
            try {
                string addr;
                Page p;
                //cout << "Exp: " << text.substr(it->left + correction, it->right - it->left) << endl;
                Address address = parseAddress(text.substr(it->left + correction, it->right - it->left), parentAddress);
                p.raw = true;
                if (!down.isInTabu(address) && currentDepth >= options.maxDepth && !options.downloadImages) {
                    addr = manager.createPageMaxDepth();
                } else {
                    if (!down.isInTabu(address)) {
                        p.address = address;
                        p.depth = currentDepth + 1;
                        down.addToQueue(p);
                        down.addToTabu(address);
                    }
                }
                if (addr.empty()) {
                    if (address.path.back() == '/') {
                        addr = getRelativeFromAbsolute(parentAddress.host + parentAddress.path, address.host + address.path + "auto_index");
                    } else addr = getRelativeFromAbsolute(parentAddress.host + parentAddress.path, address.host + address.path);
                } else {
                    addr = getRelativeFromAbsolute(parentAddress.host + parentAddress.path, addr);
                }
                text.erase(it->left + correction, it->right - it->left); //replace an old address with the new one
                text.insert(it->left + correction, addr);
                correction += addr.length() - (it->right - it->left); //correction for position inside string after replace            
            } catch (...) {
            }
        }
    }
}