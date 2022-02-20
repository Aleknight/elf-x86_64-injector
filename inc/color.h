/*
 * =====================================================================================
 *
 *       Filename:  color.h
 *
 *    Description: Contains all macros for color manipulation 
 *
 *        Version:  1.0
 *        Created:  10/05/2020 10:26:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aleknight 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef _COLOR_H_
#define _COLOR_H_

/* Basic colors definitions */
#define COLOR_RED "\033[31m"
#define COLOR_BLUE "\033[34m"
#define COLOR_CYAN "\033[36m"
#define COLOR_LIGHT_GREEN "\033[92m"

/* Complex color definitions */
#define COLOR_ORANGE "\033[38;05;226m"

// Effect definitions
#define COLOR_RESET "\033[0m"
#define COLOR_INVISIBLE "\033[8m"

#endif
