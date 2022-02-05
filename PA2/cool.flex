/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */


%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */



int str_count = 0; 





%}

/*
 * Define names for regular expressions here.
 */





%x COMMENT
%S STRING_CONST

DARROW          =>
ASSIGN          <-
LE              <=

OTHER           [+-*/{}()*/.,;:]

DIGIT          [0-9]
INTEGER        [0-9]+



IDENTIFIRE    [a-zA-Z][a-zA-Z0-9_]*

TYPEID	    [A-Z]([A-Za-z_0-9])*
OBJECTID    [a-z]([A-Za-z_0-9])*


/* COMMENTS */
BEGIN_COMMENT		\(\*
END_COMMENT		\*\)
LINE_COMMENT		--.*

/* KEYWORDS*/
CLASS		(?i:class)
ELSE		(?i:else)
FI		(?i:fi)
IN		(?i:in)
INHERITS	(?i:inherits)
ISVOID		(?i:isvoid)
LET		(?i:let)
LOOP		(?i:loop)
POOL		(?i:pool)
THEN		(?i:then)
CASE		(?i:case)
ESAC		(?i:esac)
NEW		(?i:new)
OF		(?i:of)
NOT		(?i:not)









%%

 /*
  *  Nested comments
  */


 /*
  *  The multiple-character operators.
  */
{DARROW}		{ return (DARROW); }

 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  */


 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */


 /*
  *  KEYWORDS
  */



{CLASS}	    {return CLASS;}
{ELSE}	    {return ELSE;}
{FI}	    {return FI;}
{IN}	    {return IN;}
{INHERITS}  {return INHERITS;}
{ISVOID}    {return ISVOID;}
{LET}	    {return LET;}
{LOOP}      {return LOOP;}
{POOL}      {return POOL;}
{THEN}      {return THEN;}
{CASE}      {return CASE;}
{ESAC}      {return ESAC;}
{NEW}       {return NEW;}
{OF}        {return OF;}
{NOT}       {return NOT;}
{DARROW}    {return DARROW;}
{ASSIGN}    {return ASSIGN;}
{LE}        {return LE;}


 /*
  *  INTEGER
  */

{INTEGER}   {
		cool_yylval.symbol = new IntEntry(yytext, MAX_STR_CONST, str_count++);
		return (INT_CONST);
	    }

%%
