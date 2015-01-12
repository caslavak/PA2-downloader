#include "Balast.h"

const std::string Balast::get404() {
    std::string text;
    text = "<!DOCTYPE html>\n";
    text += "<html>\n";
    text += "\t<head>\n";
    text += "\t\t<title > Error 404 </title>\n";
    text += "\t\t<meta charset = \"UTF-8\" >\n";
    text += "\t\t<meta name = \"author\" content = \"Jakub Cech\" >\n";
    text += "\t</head>\n";
    text += "\t<body style=\"background-color: white;\">\n";
    text += "\t\t<h1> Error 404 </h1>\n";
    text += "\t\t<h2> This is not the page you're looking for.<br />You can go about your business.</h2> \n";
    text += "\t</body>\n";
    text += "</html >\n";
    return text;
}

const std::string Balast::get500() {
    std::string text;
    text = "<!DOCTYPE html>\n";
    text += "<html>\n";
    text += "\t<head>\n";
    text += "\t\t<title > Error 500 </title>\n";
    text += "\t\t<meta charset = \"UTF-8\" >\n";
    text += "\t\t<meta name = \"author\" content = \"Jakub Cech\" >\n";
    text += "\t</head>\n";
    text += "\t<body style=\"background-color: white;\">\n";
    text += "\t\t<h1> Error 500 </h1>\n";
    text += "\t\t<h2> Server has encountered error during request.</h2> \n";
    text += "\t</body>\n";
    text += "</html >\n";
    return text;
}

const std::string Balast::getMaxDepth() {
    std::string text;
    text = "<!DOCTYPE html>\n";
    text += "<html>\n";
    text += "\t<head>\n";
    text += "\t\t<title > Max Depth </title>\n";
    text += "\t\t<meta charset = \"UTF-8\" >\n";
    text += "\t\t<meta name = \"author\" content = \"Jakub Cech\" >\n";
    text += "\t</head>\n";
    text += "\t<body style=\"background-color: white;\">\n";
    text += "\t\t<h1> Max depth has been reached. </h1>\n";
    text += "\t\t<h2> Max depth of download has been reached.<br />Do you need this page? Try to increase max_depth parameter next time! </h2> \n";
    text += "\t</body>\n";
    text += "</html >\n";
    return text;
}

const std::string Balast::getSaveError() {
    std::string text;
    text = "<!DOCTYPE html>\n";
    text += "<html>\n";
    text += "\t<head>\n";
    text += "\t\t<title > Save Error </title>\n";
    text += "\t\t<meta charset = \"UTF-8\" >\n";
    text += "\t\t<meta name = \"author\" content = \"Jakub Cech\" >\n";
    text += "\t</head>\n";
    text += "\t<body style=\"background-color: white;\">\n";
    text += "\t\t<h1> Error while saving a file. </h1>\n";
    text += "\t\t<h2> There was a problem writing downloaded data. Are paths OK? </h2> \n";
    text += "\t</body>\n";
    text += "</html >\n";
    return text;
}

const std::string Balast::getHomepage(const std::string & link) {
    std::string text;
    text = "<!DOCTYPE html>\n";
    text += "<html>\n";
    text += "\t<head>\n";
    text += "\t\t<title > Downloaded page </title>\n";
    text += "\t\t<meta charset = \"UTF-8\" >\n";
    text += "\t\t<meta name = \"author\" content = \"Jakub Cech\" >\n";
    text += "<script>";
    text += "window.location.href=\"" + link + "\"";
    text += "</script>";
    text += "\t</head>\n";
    text += "\t<body style=\"background-color: white;\">\n";
    text += "\t\t<h1> Proceed to downloaded page </h1>\n";
    text += "\t\t<h2> Congrats, your page has been downloaded! Now, proceed to the page by clicking this link <a href=\"" + link + "\">" + link + "</a></h2> \n";
    text += "\t</body>\n";
    text += "</html >\n";
    return text;
}

const std::string Balast::getRedirect(const std::string & link) {
    std::string text;
    text = "<!DOCTYPE html>\n";
    text += "<html>\n";
    text += "\t<head>\n";
    text += "\t\t<title > Redirected page </title>\n";
    text += "\t\t<meta charset = \"UTF-8\" >\n";
    text += "\t\t<meta name = \"author\" content = \"Jakub Cech\" >\n";
    text += "<script>";
    text += "window.location.href=\"" + link + "\"";
    text += "</script>";
    text += "\t</head>\n";
    text += "\t<body style=\"background-color: white;\">\n";
    text += "\t\t<h1> Proceed to new page </h1>\n";
    text += "\t\t<h2> Page has been redirected to <a href=\"" + link + "\">" + link + "</a></h2> \n";
    text += "\t</body>\n";
    text += "</html >\n";
    return text;
}