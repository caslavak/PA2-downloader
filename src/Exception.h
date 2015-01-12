/**
 * @file Exception.h
 * @brief Header file that contains exceptions
 */

#ifndef EXCEPTION_H
#define	EXCEPTION_H
#include <iostream>
#include "Structures.h"

/**
 * A parental exception.
 * Parental class of all exceptions.
 */
class Exception {
public:

    /**
     * A constructor.
     * It writes handed message to standard error output.
     * @param message Error message.
     */
    Exception(const char * message) {
        std::cerr << message << std::endl;
    };
};

/**
 * An exception.
 * This exception is thrown when the program was unable to establish a connection.
 */
class ConnectionErrorException : public Exception {
public:

    /**
     * A constructor.
     * @param message Error message.
     */
    ConnectionErrorException(const char * message)
    : Exception(message) {
    };
};

/**
 * An exception.
 * This exception is thrown when invalid address has been received.
 */
class InvalidAddressException : public Exception {
public:

    /**
     * A constructor.
     * @param message Error message.
     */
    InvalidAddressException(const char * message)
    : Exception(message) {
    };
};

/**
 * An exception.
 * This exception is thrown when the program receives bad header.
 */
class BadHeaderException : public Exception {
public:

    /**
     * A constructor.
     * @param message Error message.
     */
    BadHeaderException(const char * message)
    : Exception(message) {
    };
};

/**
 * An exception.
 * This exception is thrown when the program can't make directory.
 */
class UnableToMakeDirectoryException : public Exception {
public:

    /**
     * A constructor.
     * @param message Error message.
     */
    UnableToMakeDirectoryException(const char * message)
    : Exception(message) {
    };
};

/**
 * An exception.
 * This exception is thrown when the program can't create file.
 */
class UnableToAccessFileException : public Exception {
public:

    /**
     * A constructor.
     * @param message Error message.
     */
    UnableToAccessFileException(const char * message)
    : Exception(message) {
    };
};

/**
 * An exception.
 * This exception is thrown when something really strange has happened.
 */
class WTFException : public Exception {
public:

    /**
     * A constructor.
     * @param message Error message.
     */
    WTFException(const char * message)
    : Exception(message) {
    };
};

#endif	/* EXCEPTION_H */

