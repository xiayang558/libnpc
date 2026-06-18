#include "array.h"
#include <string.h>

/**
 * np_typename  --  Return human-readable name corresponding to a type character (like numpy.typename)
 *
 * Single character type code to description name mapping:
 *   'f' → "single precision"      'd' → "double precision"
 *   'i' → "integer"                'b' → "signed char"
 *   'h' → "short"                  'l' → "long integer"
 *   'q' → "long long integer"      'g' → "long precision"
 *   'B' → "unsigned char"          'H' → "unsigned short"
 *   'L' → "unsigned long integer"  'Q' → "unsigned long long integer"
 *   'F' → "complex single precision"
 *   'D' → "complex double precision"
 *   'G' → "complex long double precision"
 *   '?' → "bool"                   'S' → "string"
 *   'U' → "unicode"                'O' → "object"
 *
 * @param typechar Single character type code (such as "f", "d", "i")
 * @return Corresponding description string, returns "unknown" for unknown types
 */
const char* np_typename(const char *typechar) {
    if (typechar == NULL || typechar[0] == '\0') return "unknown";
    char c = typechar[0];
    switch (c) {
        case '?': return "bool";
        case 'b': return "signed char";
        case 'B': return "unsigned char";
        case 'h': return "short";
        case 'H': return "unsigned short";
        case 'i': return "integer";
        case 'l': return "long integer";
        case 'L': return "unsigned long integer";
        case 'q': return "long long integer";
        case 'Q': return "unsigned long long integer";
        case 'f': return "single precision";
        case 'd': return "double precision";
        case 'g': return "long precision";
        case 'F': return "complex single precision";
        case 'D': return "complex double precision";
        case 'G': return "complex long double precision";
        case 'S': return "string";
        case 'U': return "unicode";
        case 'O': return "object";
        default:  return "unknown";
    }
}
