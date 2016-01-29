/*!
 * \file utils.h
 * \author Peng Fei
 * \brief typedefs, enums, helper functions, and other common utilities
 */

#ifndef BOOKSHELF_UTILS_H
#define BOOKSHELF_UTILS_H

#include <string>

namespace thueda {

    /*!
     * \typedef std::string Id
     * \brief Id type
     */
    typedef std::string  Id;

    /*!
     * \typedef unsigned int Length
     * \brief Length type
     */
    typedef unsigned int Length;

    /*!
     * \typedef unsigned int Coordinate 
     * \brief Coordinate type
     */
    typedef double Coordinate;

    /*!
     * \typedef double Distance 
     * \brief Distance type
     */
    typedef double Distance;

    /*!
     * \typedef double Direction 
     * \brief Direction type
     */
    typedef double Direction;

    /*!
     * \enum IOType {Output, Input};
     * \brief Pin's IO type
     */
    enum IOType {Output, Input};

}

#endif// BOOKSHELF_UTILS_H

