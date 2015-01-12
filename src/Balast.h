/**
 * @file Balast.h
 * @brief Header file that specifications of predefined HTML pages.
 */

#ifndef BALAST_H
#define	BALAST_H

#include <string>

/**
 * Container for pages.
 * Predefined pages are stored in this class.
 */
class Balast {
public:
    /** Predefined 404 page. */
    const std::string get404();

    /** Predefined 500 page. */
    const std::string get500();

    /** Predefined page which signalizes that max depth has been reached. */
    const std::string getMaxDepth();

    /** Predefined which signalizes error during saving of file. */
    const std::string getSaveError();

    /** Predefined homepage that redirects browser to the entry point of download. 
     *  @param link Link to the entry point.
     *  @return Link to the homepage.
     */
    const std::string getHomepage(const std::string & link);
    
    /** Predefined page that redirects browser to another location. 
     *  @param link Link to the new address.
     *  @return Link to the redirected page.
     */
    const std::string getRedirect(const std::string & link);
};

#endif	/* BALAST_H */

