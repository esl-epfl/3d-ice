/******************************************************************************
 * This file is part of 3D-ICE, version 1.0 .                                 *
 *                                                                            *
 * 3D-ICE is free software: you can  redistribute it and/or  modify it  under *
 * the terms of the  GNU General  Public  License as  published by  the  Free *
 * Software  Foundation, either  version  3  of  the License,  or  any  later *
 * version.                                                                   *
 *                                                                            *
 * 3D-ICE is  distributed  in the hope  that it will  be useful, but  WITHOUT *
 * ANY  WARRANTY; without  even the  implied warranty  of MERCHANTABILITY  or *
 * FITNESS  FOR A PARTICULAR  PURPOSE. See the GNU General Public License for *
 * more details.                                                              *
 *                                                                            *
 * You should have  received a copy of  the GNU General  Public License along *
 * with 3D-ICe. If not, see <http://www.gnu.org/licenses/>.                   *
 *                                                                            *
 *                             Copyright (C) 2010                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar                                                    *
 *          Alessandro Vincenzi                                               *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *                                                                            *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

%{
#include <string.h>
#include <stdlib.h>

#include "types.h"
#include "die.h"
#include "stack_element.h"
#include "../bison/stack_description_parser.h"
%}

/* Instructs flex to write a C header-file. This file contains function     */
/* prototypes, external variables, and types used by the scanner. Only the  */
/* external API is exported by the header file. Many macros that are usable */
/* from within scanner actions are not exported to the header file.         */

%option header-file="stack_description_scanner.h"

/* Directs flex to write the scanner to outfile instead of lex.yy.c. */

%option outfile="stack_description_scanner.c"

/* Changes the default `yy' prefix used by flex for all globally-visible */
/* variable and function names to instead be `PREFIX'.                   */

%option prefix="stack_description_"

/* Directs flex to generate a scanner that maintains the number of the */
/* current line read from its input in the global variable yylineno.   */

%option yylineno

/* Makes the scanner not call yywrap() upon an end-of-file, but simply */
/* assume that there are no more files to scan.                        */

%option noyywrap

/* Warn about certain things. In particular, if the default rule can be */
/* matched but no default rule has been given, the flex will warn you.  */

%option warn

/* Do not generate the unput() function */

%option nounput

/* Generate the default rule */

%option default

/* Instructs flex to generate a reentrant C scanner. The generated scanner */
/* may safely be used in a multi-threaded environment.                     */

%option reentrant

/* Instructs flex to generate a C scanner that is meant to be called by a GNU */
/* bison parser. The scanner has minor API changes for bison compatibility.   */
/* In particular, the declaration of yylex is modified to take an additional  */
/* parameter, yylval.                                                         */

%option bison-bridge

/* Definitions used to simplify the scanner specification */

unsigned_integer        [[:digit:]]+
signed_integer          [+-]?[[:digit:]]+
exponent                [eE]{signed_integer}
double                  {signed_integer}\.{unsigned_integer}{exponent}?
identifier              [[:alpha:]](\_|[[:alnum:]])*
path_begin              (\.{1,2}\/|\/)?
path                    \"{path_begin}{identifier}(\/|\.{1,2}|{identifier})*\"

/* exclusive start conditions to exclude C/C++ like comments in */
/* the scanned file.                                            */

%x ONE_LINE_COMMENT
%x MULTIPLE_LINE_COMMENT

/* Begin of "rules" section of the flex file. For every token read from the */
/* input file that match one of the rule in the left column the scanner     */
/* executes the action in the right column. If a value which is not the id  */
/* of the token must be passed to the parser, it is given to the shared     */
/* variable yylval. Comments are discarded.                                 */

%%

[[:space:]]*                 ;

"//"                         BEGIN(ONE_LINE_COMMENT) ;
"/*"                         BEGIN(MULTIPLE_LINE_COMMENT) ;

":"                          return yytext[0] ;
","                          return yytext[0] ;
";"                          return yytext[0] ;

"ambient"                    return AMBIENT ;
"bottom"                     return BOTTOM ;
"capacity"                   return CAPACITY ;
"cell"                       return CELL ;
"channel"                    return CHANNEL ;
"chip"                       return CHIP ;
"coefficient"                return COEFFICIENT ;
"conventional"               return CONVENTIONAL ;
"coolant"                    return COOLANT ;
"conductivity"               return CONDUCTIVITY ;
"die"                        return DIE ;
"dimensions"                 return DIMENSIONS ;
"first"                      return FIRST ;
"floorplan"                  return FLOORPLAN ;
"flow"                       return FLOW ;
"heat"                       return HEAT ;
"height"                     return HEIGHT ;
"incoming"                   return INCOMING ;
"layer"                      return LAYER ;
"last"                       return LAST ;
"length"                     return LENGTH ;
"material"                   return MATERIAL ;
"on"                         return ON ;
"rate"                       return RATE ;
"side"                       return SIDE ;
"sink"                       return SINK ;
"source"                     return SOURCE ;
"specific"                   return SPECIFIC ;
"stack"                      return STACK ;
"temperature"                return TEMPERATURE ;
"thermal"                    return THERMAL ;
"top"                        return TOP ;
"transfer"                   return TRANSFER ;
"volumetric"                 return VOLUMETRIC ;
"wall"                       return WALL ;
"width"                      return WIDTH ;

{signed_integer}   {
                     yylval->double_v = atof(yytext) ;
                     return DVALUE ;
                   }

{double}           {
                     yylval->double_v = atof(yytext) ;
                     return DVALUE ;
                   }

{identifier}       {
                     yylval->char_p = strdup(yytext) ;
                     return IDENTIFIER ;
                   }

{path}             {
                     int n = strlen(yytext) - 2 ;
                     yylval->char_p = (char *) malloc (sizeof(char) * n) ;
                     strncpy (yylval->char_p, yytext+1, n) ;
                     return PATH;
                   }

<ONE_LINE_COMMENT>\n              BEGIN(INITIAL);
<ONE_LINE_COMMENT>.               ;
<MULTIPLE_LINE_COMMENT>"*/"       BEGIN(INITIAL);
<MULTIPLE_LINE_COMMENT>[^*\n]+    ;
<MULTIPLE_LINE_COMMENT>"*"[^/]    ;
<MULTIPLE_LINE_COMMENT>\n         ;
