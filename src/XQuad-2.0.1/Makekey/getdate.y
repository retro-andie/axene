%{
/* this are the c decl */

#include <time.h>
#include <ctype.h>
#include <string.h>

#if defined(___hpux9) || defined(___hpux10)
#define const 
#endif

static int yylex ();
static int yyerror ();

/*
**  An entry in the lexical lookup table.
*/
typedef struct _TABLE {
    const char	*name;
    int		type;
    time_t	value;
} TABLE;

char	*yyInput;
time_t	yyMonth;
time_t	yyDay;
time_t	yyYear;
%}

%union {
    time_t		Number;
}


%token	tID tDAY tMONTH tSNUMBER tUNUMBER

%type	<Number>	tMONTH tSNUMBER tUNUMBER

%%

date	: tUNUMBER '/' tUNUMBER {
	    yyMonth = $1;
	    yyDay = $3;
	}
	| tUNUMBER '/' tUNUMBER '/' tUNUMBER {
	    yyMonth = $1;
	    yyDay = $3;
	    yyYear = $5;
	}
	| tUNUMBER tSNUMBER tSNUMBER {
	    /* ISO 8601 format.  yyyy-mm-dd.  */
	    yyYear = $1;
	    yyMonth = -$2;
	    yyDay = -$3;
	}
	| tUNUMBER tMONTH tSNUMBER {
	    /* e.g. 17-JUN-1992.  */
	    yyDay = $1;
	    yyMonth = $2;
	    yyYear = -$3;
	}
	| tMONTH tUNUMBER {
	    yyMonth = $1;
	    yyDay = $2;
	}
	| tMONTH tUNUMBER ',' tUNUMBER {
	    yyMonth = $1;
	    yyDay = $2;
	    yyYear = $4;
	}
	| tUNUMBER tMONTH {
	    yyMonth = $2;
	    yyDay = $1;
	}
	| tUNUMBER tMONTH tUNUMBER {
	    yyMonth = $2;
	    yyDay = $1;
	    yyYear = $3;
	}
	;

%%

/* Month and day table. */
static TABLE const MonthDayTable[] = {
    { "january",	tMONTH,  1 },
    { "february",	tMONTH,  2 },
    { "march",		tMONTH,  3 },
    { "april",		tMONTH,  4 },
    { "may",		tMONTH,  5 },
    { "june",		tMONTH,  6 },
    { "july",		tMONTH,  7 },
    { "august",		tMONTH,  8 },
    { "september",	tMONTH,  9 },
    { "sept",		tMONTH,  9 },
    { "october",	tMONTH, 10 },
    { "november",	tMONTH, 11 },
    { "december",	tMONTH, 12 },
    { NULL }
};

/* ARGSUSED */
static int
yyerror (s)
    char	*s;
{
  return 0;
}

static int
LookupWord (buff)
    char		*buff;
{
  register char	*p;
  register const TABLE	*tp;
  int			abbrev;

  /* Make it lowercase. */
  for (p = buff; *p; p++)
    if (isupper (*p))
      *p = tolower (*p);

  /* See if we have an abbreviation for a month. */
  if (strlen (buff) == 3)
    abbrev = 1;
  else if (strlen (buff) == 4 && buff[3] == '.') {
    abbrev = 1;
    buff[3] = '\0';
  }
  else
    abbrev = 0;

  for (tp = MonthDayTable; tp->name; tp++) {
    if (abbrev) {
      if (strncmp (buff, tp->name, 3) == 0) {
	yylval.Number = tp->value;
	return tp->type;
      }
    }
    else if (strcmp (buff, tp->name) == 0) {
      yylval.Number = tp->value;
      return tp->type;
    }
  }

  return tID;
}

static int
yylex ()
{
  register char	c;
  register char	*p;
  char		buff[20];
  int			sign;

  for ( ; ; ) {
    if ((*yyInput == '\n') || (*yyInput == '\r'))
      return 0;

    while (isspace (*yyInput))
      yyInput++;

    if (isdigit (c = *yyInput) || c == '-' || c == '+') {
      if (c == '-' || c == '+') {
	sign = c == '-' ? -1 : 1;
	if (!isdigit (*++yyInput))
	  /* skip the '-' sign */
	  continue;
      }
      else
	sign = 0;
      for (yylval.Number = 0; isdigit (c = *yyInput++); )
	yylval.Number = 10 * yylval.Number + c - '0';
      yyInput--;
      if (sign < 0)
	yylval.Number = -yylval.Number;
      return sign ? tSNUMBER : tUNUMBER;
    }
    if (isalpha (c)) {
      for (p = buff; isalpha (c = *yyInput++) || c == '.'; )
	if (p < &buff[sizeof buff - 1])
	  *p++ = c;
      *p = '\0';
      yyInput--;
      return LookupWord (buff);
    }
    return *yyInput++;
  }
}

