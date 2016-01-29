/*!
 * \file utils.h
 * \author Peng Fei
 * \brief typedefs, enums, helper functions, and other common utilities
 */

#ifndef UTILS_H
#define UTILS_H

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
typedef unsigned int Coordinate;

/*!
 * \typedef double Distance 
 * \brief Distance type
 */
typedef double Distance;

/*!
 * \enum IOType {Output, Input};
 * \brief Pin's IO type
 */
enum IOType {Output, Input};
}

#endif// UTILS_H

