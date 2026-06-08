/* C code produced by gperf version 2.7 */
/* Command-line: gperf -G -p -g -o -t -N is_AFM_keyword -k$,2,4,6,1 AFM.gperf  */
#include "AFM.h"

#define TOTAL_KEYWORDS 65
#define MIN_WORD_LENGTH 1
#define MAX_WORD_LENGTH 18
#define MIN_HASH_VALUE 1
#define MAX_HASH_VALUE 137
/* maximum key range = 137, duplicates = 0 */

#ifdef __GNUC__
__inline
#endif
static unsigned int
hash (str, len)
     register const char *str;
     register unsigned int len;
{
  static unsigned char asso_values[] =
    {
      138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
      138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
      138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
      138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
      138, 138, 138, 138, 138, 138, 138, 138,  15,  25,
      138, 138, 138, 138, 138, 138, 138, 138, 138,  30,
      138, 138, 138, 138, 138,   0,  35,   0,  45,  10,
       10, 138,  40,  85, 138,   0,  20,  10,   5, 138,
       60, 138, 138,   0,  30,  15,  20,  25,  55,   5,
      138, 138, 138, 138, 138, 138, 138,   0, 138,   0,
        0,   0, 138,   0,   0,   0, 138, 138,   0,   0,
        0,  65,   5, 138,   0,   0,   0,   0, 138, 138,
        5,   5, 138, 138, 138, 138, 138, 138, 138, 138,
      138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
      138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
      138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
      138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
      138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
      138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
      138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
      138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
      138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
      138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
      138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
      138, 138, 138, 138, 138, 138, 138, 138, 138, 138,
      138, 138, 138, 138, 138, 138
    };
  register int hval = len;

  switch (hval)
    {
      default:
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      case 5:
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      case 3:
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

static AFM_keyword_t wordlist[] =
  {
    {""},
    {"C", AK_C},
    {"CC", AK_CC},
    {""}, {""}, {""}, {""}, {""},
    {"Ascender", AK_ASCENDER},
    {"CharWidth", AK_CHAR_WIDTH},
    {"Characters", AK_CHARACTERS},
    {"N", AK_N},
    {"CharacterSet", AK_CHARACTER_SET},
    {"StartKernData", AK_START_KERN_DATA},
    {"StartKernPairs", AK_START_KERN_PAIRS},
    {"StartComposites", AK_START_COMPOSITES},
    {"StartCharMetrics", AK_START_CHAR_METRICS},
    {"EscChar", AK_ESC_CHAR},
    {"FullName", AK_FULL_NAME},
    {""},
    {"MetricSets", AK_METRICS_SETS},
    {"EndKernData", AK_END_KERN_DATA},
    {"EndKernPairs", AK_END_KERN_PAIRS},
    {"EndComposites", AK_END_COMPOSITES,},
    {"EndCharMetrics", AK_END_CHAR_METRICS},
    {"FamilyName", AK_FAMILY_NAME},
    {"StartFontMetrics", AK_START_FONT_METRICS},
    {""},
    {"MappingScheme", AK_MAPPING_SCHEME},
    {""},
    {"StartKernPairs0", AK_START_KERN_PAIRS0},
    {"Weight", AK_WEIGHT},
    {"UnderlinePosition", AK_UNDERLINE_POSITION},
    {"UnderlineThickness", AK_UNDERLINE_THICKNESS},
    {"EndFontMetrics", AK_END_FONT_METRICS},
    {""}, {""},
    {"WY", AK_WY},
    {""},
    {"TrackKern", AK_TRACK_KERN},
    {"StartKernPairs1", AK_START_KERN_PAIRS1},
    {"L", AK_L},
    {""}, {""},
    {"StartTrackKern", AK_START_TRACK_KERN},
    {""}, {""}, {""},
    {"W0Y", AK_W0Y},
    {"CapHeight", AK_CAP_HEIGHT},
    {""},
    {"W", AK_W},
    {"EndTrackKern", AK_END_TRACK_KERN},
    {""},
    {"Descender", AK_DESCENDER},
    {""}, {""},
    {"W0", AK_W0},
    {"W1Y", AK_W1Y},
    {"StartDirection", AK_START_DIRECTION},
    {""},
    {";", AK_SEPARATOR},
    {"VV", AK_VV},
    {"PCC", AK_PCC},
    {""}, {""}, {""},
    {"EndDirection", AK_END_DIRECTION},
    {"KPY", AK_KPY},
    {""}, {""},
    {"B", AK_B},
    {"Comment", AK_COMMENT},
    {""}, {""}, {""},
    {"Notice", AK_NOTICE},
    {"W1", AK_W1},
    {""}, {""}, {""}, {""},
    {"CH", AK_CH},
    {"FontName", AK_FONT_NAME},
    {""}, {""}, {""}, {""}, {""},
    {"EncodingScheme", AK_ENCODING_SCHEME},
    {""}, {""},
    {"Version", AK_VERSION},
    {""}, {""},
    {"IsBaseFont", AK_IS_BASE_FONT},
    {"ItalicAngle", AK_ITALIC_ANGLE},
    {"IsFixedPitch", AK_IS_FIXED_PITCH},
    {"W0X", AK_W0X},
    {""}, {""}, {""},
    {"XHeight", AK_X_HEIGHT},
    {"KPH", AK_KPH},
    {""}, {""}, {""}, {""},
    {"W1X", AK_W1X},
    {""}, {""}, {""},
    {"VVector", AK_V_VECTOR},
    {"IsFixedV", AK_IS_FIXED_V},
    {""}, {""}, {""}, {""},
    {"KPX", AK_KPX},
    {""}, {""}, {""},
    {"KP", AK_KP},
    {"FontBBox", AK_FONT_BBOX},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""},
    {"WX", AK_WX}
  };

#ifdef __GNUC__
__inline
#endif
AFM_keyword_t *
is_AFM_keyword (str, len)
     register const char *str;
     register unsigned int len;
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return &unknow_AFM_keyword;
}
