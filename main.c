#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "strchr_brk.h"


#define LF      '\n'
#define SP      ' '
#define HT      '\t'
#define COLON   ':'

static const char HEADER_NAME_CHR[256] = {
//  0  1  2  3  4  5  6  7  8 HT LF 11 12 CR 14 15 16 17 18 19 20 21 22 23 24
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//  25 26 27 28 29 30 31 
    0, 0, 0, 0, 0, 0, 0, 
//  SP !  "  #  $  %  &  '  (  )  *  +  ,       .  /
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 
//  digit
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
//  :  ;  <  =  >  ?  @
    0, 0, 0, 0, 0, 0, 0, 
// convert alpha-upper to alpha-lower
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
//  [  \  ]  ^       `
    0, 0, 0, 0, '_', 0, 
//  alpha-lower
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
//  {  |  }  ~
    0, 0, 0, 0
};


static const char HEADER_VAL_CHR[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, HT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 
    SP, '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', 
    '/', 
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
    ':', ';', '<', '=', '>', '?', '@', 
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 
    '[', '\\', ']', '^', '_', '`', 
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
    '{', '|', '}', '~', 0, 
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 
    143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 
    158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 
    173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 
    188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 
    203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 
    218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 
    233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 
    248, 249, 250, 251, 252, 253, 254, 255
};


static void test_illegal( void )
{
    char buf[] = "KEY-1: KEY1 value\n" 
                 "KEY-2: KEY2 value";
    size_t len = sizeof( buf );
    // lookup "X"
    char *ptr = strchr_brkrep( buf, len, 'X', HEADER_NAME_CHR );
    
    // reached to unacceptable charactor
    assert( errno == EILSEQ );
}


static void test_lookup_n_replace_key( char *buf, size_t len );

static void test_lookup_val( char *buf, size_t len )
{
    char *token = buf;
    // lookup ":"
    char *ptr = strchr_brkrep( buf, len, LF, HEADER_VAL_CHR );
    size_t tlen = 0;
    
    // should not error
    assert( errno == 0 );
    *ptr = 0;
    tlen = (ptrdiff_t)ptr - (ptrdiff_t)token;
    printf("val[%zu]: %s\n", tlen, token );
    
    // lookup next key
    if( ptr[1] ){
        test_lookup_n_replace_key( ptr + 1, len - ( tlen + 1 ) );
    }
}


static void test_lookup_n_replace_key( char *buf, size_t len )
{
    char *token = buf;
    // lookup ":"
    char *ptr = strchr_brkrep( buf, len, COLON, HEADER_NAME_CHR );
    size_t tlen = 0;
    
    // should not error
    assert( errno == 0 );
    // set terminator
    *ptr = 0;
    tlen = (ptrdiff_t)ptr - (ptrdiff_t)token;
    printf("key[%zu]: %s\n", tlen, token );
    
    // lookup next value
    test_lookup_val( ptr + 1, len - ( tlen + 1 ) );
}


static void test_lookup_n_replace_key_value( void )
{
    char buf[] = "KEY-1: KEY1 value\n" 
                 "KEY-2: KEY2 value\n"
                 "KEY-3: KEY3 value\n";
    size_t len = sizeof( buf );
    
    test_lookup_n_replace_key( buf, len );
}


int main (int argc, const char * argv[])
{
    test_illegal();
    test_lookup_n_replace_key_value();
    
    return 0;
}
