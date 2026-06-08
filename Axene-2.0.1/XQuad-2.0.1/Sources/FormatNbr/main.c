/*
** main.c for XQuad in FormatNbr/
** programme to test TimeFuncs
**
** Copyright (C) 1995-2000 Axene.
** Authors: Stéphane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
** Email: xcalibur@axene.org
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program; if not, write to the Free Software
**    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Started on  Thr Dec  7 18:58:18 1995 Emmanuel Paris
** Last update Thr Dec  7 18:58:18 1995 Emmanuel Paris
*/

#include "TimeFuncs.h"

static char *day_in_week[8] = { 
  "", "lundi", "mardi", "mercredi", "jeudi",
  "vendredi", "samedi", "dimanche" };

static char *month_in_year[13] = {
  "", "janvier", "février", "mars", "avril", "mai", "juin", "juillet",
  "aout", "septembre", "octobre", "novembre", "décembre" };

int main(argc, argv)
int argc;
char *argv[];
{
  xc_date_time_t	xc_date_time;
  xc_date_t		xc_date;
  xc_time_t		xc_time;
  int			i, j;
  double	val;
  boolean	bexit = FALSE, exit = FALSE, valid;
  char		choice[200], input[200], inpt2[200];
  
  printf("date type: xc -> date or time\n\t1- date & time.\n\t2- date.\n\t3- time.\n"); 
  printf("date type: date or time -> xc\n\t4- date & time.\n\t5- date.\n\t6- time.\n");
  
  while(!exit)
  {
    printf("date type: ");
    scanf("%s", choice);
    
    bexit = FALSE;
    while(!bexit)
    {
      switch(choice[0] - '0')
      {
      case 1:
	printf("date and time value: ");
	scanf("%s", input);
	
	if (input[0] == 'q')
	{
	  bexit = TRUE;
	  break;
	}
	val = atof(input);
	convert_to_date_and_time(val, &xc_date_time);
	printf("%f -> %s %2d %s %d", val,
	       day_in_week[xc_date_time.date.day_in_week],
	       xc_date_time.date.day, 
	       month_in_year[xc_date_time.date.month],
	       xc_date_time.date.year);
	
	printf(" -- %2d:%02d'%02d\".%03d\n",
	       xc_date_time.time.hour, xc_date_time.time.minute, 
	       xc_date_time.time.second, xc_date_time.time.millisecond);
	break;
      case 2:
	printf("date value: ");
	scanf("%s", input);
	
	if (input[0] == 'q')
	{
	  bexit = TRUE;
	  break;
	}
	j = atoi(input);
       	val = (double)j;
	
	convert_to_date(val, &xc_date);
	printf("%d -> %s %2d %s %d\n", j, day_in_week[xc_date.day_in_week],
	       xc_date.day, month_in_year[xc_date.month], xc_date.year);
	printf("year: %d %s -- week num: %d\n", 
	       xc_date.after_jc ? xc_date.year : -xc_date.year,
	       xc_date.after_jc ? "ap JC" : "av JC",
	       xc_date.week);
	break;
      case 3:
	printf("time value: ");
	scanf("%s", input);
	
	if (input[0] == 'q')
	{
	  bexit = TRUE;
	  break;
	}
	val = (double)atof(input);
	
	convert_to_time(val, &xc_time);
	printf("%f -> %2d:%02d'%02d\".%03d -- %2d:%02d'%02d\".%03d %s\n", val,
	       xc_time.hour, xc_time.minute, 
	       xc_time.second,xc_time.millisecond,
	       xc_time.hour12, xc_time.minute, 
	       xc_time.second,xc_time.millisecond,
	       xc_time.am_pm?"PM":"AM" );
	break;
      case 4:
	printf("date and time: XX/XX/[-]XXXX XX:XX:XX[.XXXX] ");
	scanf("%s %s", input, inpt2);
	
	if (input[0] == 'q')
	{
	  bexit = TRUE;
	  break;
	}
	
	valid = FALSE;
	if (isdigit(input[0]) && isdigit(input[1]) && input[2] == '/' &&
	    isdigit(input[3]) && isdigit(input[4]) && input[5] == '/')
	{
	  int	i, minus;
	
	  xc_date_time.date.day = (input[0] - '0') * 10 + input[1] - '0';
	  xc_date_time.date.month = (input[3] - '0') * 10 + input[4] - '0';
	  
	  if (input[6] == '-') {  minus = -1; i = 7; }
	  else { minus = 1; i = 6; }
	  
	  if (isdigit(input[i]) && isdigit(input[i + 1]) &&
	      isdigit(input[i + 2]) && isdigit(input[i + 3]))
	  {
	    xc_date_time.date.year = ((input[i] - '0') * 1000 + 
				      (input[i + 1] - '0') * 100 + 
				      (input[i + 2] - '0') * 10 +
				      (input[i + 3] - '0')) * minus;
	    
	    if (isdigit(inpt2[0]) && isdigit(inpt2[1]) && inpt2[2] == ':' &&
		isdigit(inpt2[3]) && isdigit(inpt2[4]) && inpt2[5] == ':' && 
		isdigit(inpt2[6]) && isdigit(inpt2[7]))
	    {
	      xc_date_time.time.hour = (inpt2[0] - '0') * 10 + inpt2[1]-'0';
	      xc_date_time.time.minute = (inpt2[3]-'0') * 10 + inpt2[4]-'0';
	      xc_date_time.time.second = (inpt2[6]-'0') * 10 + inpt2[7]-'0';
	      
	      xc_date_time.time.millisecond = 0;
	      if (inpt2[8] == '.' && isdigit(inpt2[9])
		  && isdigit(inpt2[10]) && isdigit(inpt2[11]))
	      {
		xc_date_time.time.millisecond = (inpt2[9] - '0') * 100 + 
		  (inpt2[10] - '0') * 10 + inpt2[11] - '0';
	      }
	    
	      val = convert_to_xc_date_and_time(&xc_date_time, &valid);
	      if (valid)
		printf("%s -> %f\n", input, val);
	    }
	  }
	}
	if (!valid)
	  printf("%s -> Date and Time not VALID\n");
	break;
      case 5:
	printf("date: XX/XX/[-]XXXX ");
	scanf("%s", input);
	
	if (input[0] == 'q')
	{
	  bexit = TRUE;
	  break;
	}
	
	valid = FALSE;
	if (isdigit(input[0]) && isdigit(input[1]) && input[2] == '/' &&
	    isdigit(input[3]) && isdigit(input[4]) && input[5] == '/')
	{
	  int	i, minus;
	
	  xc_date.day = (input[0] - '0') * 10 + input[1] - '0';
	  xc_date.month = (input[3] - '0') * 10 + input[4] - '0';
	  
	  if (input[6] == '-') {  minus = -1; i = 7; }
	  else { minus = 1; i = 6; }
	  
	  if (isdigit(input[i]) && isdigit(input[i + 1]) &&
	      isdigit(input[i + 2]) && isdigit(input[i + 3]))
	  {
	    xc_date.year = ((input[i] - '0') * 1000 + 
				      (input[i + 1] - '0') * 100 + 
				      (input[i + 2] - '0') * 10 +
				      (input[i + 3] - '0')) * minus;
	    val = convert_to_xc_date(&xc_date, &valid);
	    if (valid)
	      printf("%s -> %f\n", input, val);
	  }
	}
	if (!valid)
	  printf("%s -> Date not VALID\n");
	break;
      case 6:
	printf("time: XX:XX:XX[.XXXX] ");
	scanf("%s", input);
	
	if (input[0] == 'q')
	{
	  bexit = TRUE;
	  break;
	}
	
	valid = FALSE;
	if (isdigit(input[0]) && isdigit(input[1]) && input[2] == ':' &&
	    isdigit(input[3]) && isdigit(input[4]) && input[5] == ':' && 
	    isdigit(input[6]) && isdigit(input[7]))
	{
	  xc_time.hour = (input[0] - '0') * 10 + input[1] - '0';
	  xc_time.minute = (input[3] - '0') * 10 + input[4] - '0';
	  xc_time.second = (input[6] - '0') * 10 + input[7] - '0';
	  
	  xc_time.millisecond = 0;
	  if (input[8] == '.' && isdigit(input[9])
	      && isdigit(input[10]) && isdigit(input[11]))
	  {
	    xc_time.millisecond = (input[9] - '0') * 100 + 
	      (input[10] - '0') * 10 + input[11] - '0';
	  }
	  val = convert_to_xc_time(&xc_time, &valid);
	  if (valid)
	    printf("%s -> %f\n", input, val);
	}
	if (!valid)
	  printf("%s -> Time not VALID\n");
	break;
      default:
	bexit = exit = TRUE;
	break;
      }
    }
  }
  return 0;
}


