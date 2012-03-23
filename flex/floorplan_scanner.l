/******************************************************************************
 * This file is part of 3D-ICE, version 2.1 .                                 *
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
 * with 3D-ICE. If not, see <http://www.gnu.org/licenses/>.                   *
 *                                                                            *
 *                             Copyright (C) 2010                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar                                                    *
 *          Alessandro Vincenzi                                               *
 *          Giseong Bak                                                       *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

%{
#include "../bison/floorplan_parser.h"
%}

/* Instructs flex to write a C header-file. This file contains function     */
/* prototypes, external variables, and types used by the scanner. Only the  */
/* external API is exported by the header file. Many macros that are usable */
/* from within scanner actions are not exported to the header file.         */

%option header-file="floorplan_scanner.h"

/* Directs flex to write the scanner to outfile instead of lex.yy.c. */

%option outfile="floorplan_scanner.c"

/* Changes the default `yy' prefix used by flex for all globally-visible */
/* variable and function names to instead be `PREFIX'.                   */

%option prefix="floorplan_"

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

u_integer [[:digit:]]+
s_integer [+-]?[[:digit:]]+
exponent  [eE]{s_integer}
double    {s_integer}\.{u_integer}{exponent}?

identifier     [[:alpha:]](\_|[[:alnum:]])*

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



[[:space:]]*          ;

"//"                  BEGIN(ONE_LINE_COMMENT) ;
"/*"                  BEGIN(MULTIPLE_LINE_COMMENT) ;

"("                   return yytext[0] ;
")"                   return yytext[0] ;
":"                   return yytext[0] ;
","                   return yytext[0] ;
";"                   return yytext[0] ;

"dimension"           return DIMENSION ;
"position"            return POSITION ;
"power"               return POWER ;
"rectangle"           return RECTANGLE ;
"values"              return VALUES ;

{identifier}          {
                        yylval->identifier = strdup(yytext) ;
                        return IDENTIFIER ;
                      }

{s_integer}           {
                        yylval->power_value = atof(yytext) ;
                        return DVALUE ;
                      }

{double}              {
                        yylval->power_value = atof(yytext) ;
                        return DVALUE ;
                      }

<ONE_LINE_COMMENT>\n              BEGIN(INITIAL) ;
<ONE_LINE_COMMENT>.               ;
<MULTIPLE_LINE_COMMENT>"*/"       BEGIN(INITIAL) ;
<MULTIPLE_LINE_COMMENT>[^*\n]+    ;
<MULTIPLE_LINE_COMMENT>"*"[^/]    ;
<MULTIPLE_LINE_COMMENT>\n         ;
