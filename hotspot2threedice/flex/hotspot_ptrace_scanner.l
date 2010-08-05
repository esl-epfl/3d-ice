/******************************************************************************
 *                                                                            *
 * Source file "flex/hotspot_ptrace_scanner.l"                                     *
 *                                                                            *
 * Compile this file with "flex filename.l"                                   *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

%{
#include <string.h>
#include <stdlib.h>

#include "../bison/hotspot_ptrace_parser.h"
%}

/* Instructs flex to write a C header-file. This file contains function     */
/* prototypes, external variables, and types used by the scanner. Only the  */
/* external API is exported by the header file. Many macros that are usable */
/* from within scanner actions are not exported to the header file.         */

%option header-file="hotspot_ptrace_scanner.h"

/* Directs flex to write the scanner to outfile instead of lex.yy.c. */

%option outfile="hotspot_ptrace_scanner.c"

/* Changes the default `yy' prefix used by flex for all globally-visible */
/* variable and function names to instead be `PREFIX'.                   */

%option prefix="hotspot_ptrace_"

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

"#"                   BEGIN(ONE_LINE_COMMENT) ;

{identifier}          {
                        yylval->string = strdup(yytext) ;
                        return IDENTIFIER ;
                      }

{double}              {
                        yylval->d_value = atof(yytext) ;
                        return DVALUE ;
                      }

<ONE_LINE_COMMENT>\n              BEGIN(INITIAL);
<ONE_LINE_COMMENT>.               ;
