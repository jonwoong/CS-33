//
//  main.c
//  homework2
//
//  Created by Jonathan Woong on 10/27/14.
//  Copyright (c) 2014 Jonathan Woong. All rights reserved.
//

#include <stdio.h>

void loopthrough(int w)
{
    printf("a = ");
    for (int a=-5;a<6;a++)
    {
        printf("%d",a);
        printf(" ");
    }
    printf("\n");
    printf("a/4 = ");
    for (short b=-5;b<6;b++)
    {
        short c = b/4;
        printf("%d",c);
        printf(" ");
    }
    printf("\n");
    printf("a>>2 = ");
    for (short d=-5;d<6;d++)
    {
        int e = d>>2;
        printf("%d",e);
        printf(" ");
    }
    printf("\n");
    printf("(a+3)>>2 = ");
    for (short f=-5;f<6;f++)
    {
        int g = (f+3)>>2;
        printf("%d",g);
        printf(" ");
    }
    
    
}

void to_binary( short n, int w, int *x )
{
    int i ;
    long m,y ;
    
    if ( n >= 0 )
        m = n ;
    else
        m = exp2(w)+n ; /* so that mod works properly */
    
    for( i=0; i<w; i++ )
    {
        x[i] = m%2 ;
        y = m ;
        m = m/2 ;
    }
}

void main()
{
    short a, b, MIN_INT, MAX_INT ;
    int W, w, x[20], y[20], z[20] ;
    
    w = sizeof(a)*8 ;
    W = w-1 ;
    MAX_INT =  exp2( W )-1 ;
    MIN_INT = -exp2( W )   ;
    
    //
    //  put your test code here
    //
    
}

/*int main(int argc, const char * argv[]) {
    int w = 16;
    loopthrough(16);
}*/
