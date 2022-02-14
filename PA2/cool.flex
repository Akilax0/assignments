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
 * 
 *   #define ADD_STRING(c)
 */

/**vaariable to keep track of nested comments**/
int comment_depth;


/**function to check length of string**/

bool len_check(){
    return (string_buf_ptr+1 < &string_buf[MAX_STR_CONST-1]);
}

%}

/*
 * Define names for regular expressions here.
 */



/* Exclusive start  condition COMMENT */

%x COMMENT STRING INVALID_STR

/* composite notations */
DARROW          =>
ASSIGN          <-
LE              <=

/* INTEGERS */
DIGIT          [0-9]
INTEGER        [0-9]+


IDENTIFIRE    [a-zA-Z][a-zA-Z0-9_]*

/* Type and object identifier */
TYPEID	    [A-Z]([A-Za-z_0-9])*
OBJECTID    [a-z]([A-Za-z_0-9])*


/* COMMENTS */
BEGIN_COMMENT		\(\*
END_COMMENT		\*\)
LINE_COMMENT		--.*



NEW_LINE		\n



/* STRINGS */
QUOTE			\"
NULL_CHARACTERS		\\0

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


/* True or False with always lower case first letter*/
FALSE		f(?i:alse)
TRUE		t(?i:rue)



%%

 /*
  *  comments
  */

 /* rule to increment line on newline */

{NEW_LINE}  {curr_lineno++;}

 /* ignore comment strings */
{LINE_COMMENT}	{}

 /* on multiline comment begin start state COMMENT and increment depth */
{BEGIN_COMMENT}	{
    BEGIN(COMMENT);
    comment_depth++;
}		

 /* Check for unmatched closing comment */
{END_COMMENT} {
    cool_yylval.error_msg = "Unmatched *)";
    return ERROR;
}


 /* When in COMMENT state */
<COMMENT>{

    /* Increment comment_depth on new comment open */
    {BEGIN_COMMENT}	{
			    ++comment_depth;
			}
    /* Decrement comment depth on close and exit COMMENT state on depth==0*/
    {END_COMMENT}   {
		       if(--comment_depth==0)
			       BEGIN(INITIAL);
		   }

    /* Error when EOF in comment */
    <<EOF>> {
	BEGIN(INITIAL);
	cool_yylval.error_msg = "EOF in comment";
	return ERROR;
    }


    /* increment on new line */
    {NEW_LINE}  {curr_lineno++;}

    /* reject all other characters inside comment  */
    . {}

}



 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */

 /*Check string begin with ". If so assign pointer ro string buffer and start
 * STING state*/

{QUOTE} {
	    string_buf_ptr = string_buf;
	    BEGIN(STRING);
	}



<STRING>{
    /* Check for closing " if so append endofline and create entry on string
     * table */

	{QUOTE} {
		    *string_buf_ptr++ = '\0';
		    BEGIN(INITIAL);
		    cool_yylval.symbol = stringtable.add_string(string_buf);
		    return STR_CONST;
		}


	/* Error on null characters in string */
	{NULL_CHARACTERS} {
			cool_yylval.error_msg = "String contains null character";
    			return ERROR;
			}

	/* increment line on new line but return ERROR */
	{NEW_LINE} {
			curr_lineno++;
			cool_yylval.error_msg = "Unterminated string constant";
			BEGIN(INITIAL);
			return ERROR;			
	    	}

	/* return EOF in string error  */
	<<EOF>>	{
		   BEGIN(INITIAL);
		   cool_yylval.error_msg = "EOF in string constant";
		   return ERROR;
	}

	/* For the following rules before each append string length is checked
	 * to not exceed max string length */

	/* check character escape in string if so take as c and append to string */
	\\c {
	    if(len_check()){
		*string_buf_ptr++ = 'c';
	    }else{
		   BEGIN(INITIAL);
		   cool_yylval.error_msg = "string too long";
		   return ERROR;
	    }
	}
	
	/* check tab in string if so take as \t and append to string */
	\\t {
	    if(len_check()){
		*string_buf_ptr++ = '\t';
	    }else{
		   BEGIN(INITIAL);
		   cool_yylval.error_msg = "string too long";
		   return ERROR;
	    }
	}

	/* check backspace in string if so take as \b and append to string */
	\\b {
	    if(len_check()){
		*string_buf_ptr++ = '\b';
	    }else{
		   BEGIN(INITIAL);
		   cool_yylval.error_msg = "string too long";
		   return ERROR;
	    }
	}

	/* check newline in string if so take as \n and append to string */
	\\n {
	    if(len_check()){
		*string_buf_ptr++ = '\n';
	    }else{
		   BEGIN(INITIAL);
		   cool_yylval.error_msg = "string too long";
		   return ERROR;
	    }
	}

	/* For all other characters append directly to string */
	. {
	    if(len_check()){
		*string_buf_ptr++ = yytext[0];
	    }else{
		   BEGIN(INITIAL);
		   cool_yylval.error_msg = "string too long";
		   return ERROR;
	    }
	}
}


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

 /*
  *  EXTRA TOKENS
  */

"("		{return '(';}
")"		{return ')';}
"."		{return '.';}
"@"		{return '@';}
"~"		{return '~';}
"*"		{return '*';}
"/"		{return '/';}
"+"		{return '+';}
"-"		{return '-';}
{LE}	    	{return LE;}
"<"		{return '<';}
"="		{return '=';}
{ASSIGN}	{return ASSIGN;}
"{"		{return '{';}
"}"		{return '}';}
":"		{return ':';}
","	 	{return ',';}
";"		{return ';';}
{DARROW}	{return DARROW;}


 /*
  *  BOOLEAN
  */

{TRUE}	{
	    cool_yylval.boolean = true;
	    return (BOOL_CONST);
      
	}

{FALSE}	{
	    cool_yylval.boolean = false;
	    return (BOOL_CONST);
      
	}

 /*
  *  INTEGER
  */

{INTEGER}   {
		cool_yylval.symbol = inttable.add_string(yytext);
		return (INT_CONST);
	    }


 /*
  *  TYPE IDENTIFIER
  */

{TYPEID}   {
		cool_yylval.symbol = idtable.add_string(yytext); 
		return (TYPEID);
	    }
 /*
  *  OBJECT IDENTIFIER
  */

{OBJECTID}   {
		cool_yylval.symbol = idtable.add_string(yytext); 
		return (OBJECTID);
	    }
%%
