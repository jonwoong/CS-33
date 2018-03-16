#include <stdio.h>

#include <math.h>


void to_binary( int n, int w, int *x, int *o )
{
    /* n is an input and is the number to be converted to binary */
    /* w is an input = to the width in bits of x */
    /* x is an output array of w ints, either 0 or 1: the binary representation of n */
    /*           x[0] is the least significant bit   */
    /*           x[w-1] is the most signifcant bit, for a signed number, it is the sign */
    /* o is an output = 1 if an overflow occurred ( n is too large for w bits ) */
    
    // Discriminate against negative sign //
    int Tcomplement = 0;
    if (n<0)
    {
        n *= -1;
        Tcomplement = 1;
    }
    
    // Fill x-array with 0's //
    int r=0;
    while (r!=w)
    {
        x[r] = 0;
        r++;
    }
    
    // Short division of decimal integer //
    int i=0;
    int R = n%2;
    while (n!=1&&n!=0)
    {
        x[i] = R;
        i++;
        n = n/2;
        R = n%2;
    }
    if (n==1)
    {
        x[i] = 1;
    }
    if (i > w)
    {
        o = 1;
    }
    else
    {
        o = 0;
    }
    
    // Twos Complement //
    if (Tcomplement == 1)
    {
        // Change all 0's to 1's //
        for (int n=0; n < w; n++)
        {
            if (x[n] == 0)
            {
                x[n] = 1;
            }
            else if(x[n] == 1)
            {
                x[n] = 0;
            }
        }
        
        // Add 1 //
        if (x[0] == 0)
        {
            x[0] = 1;
        }
        else if (x[0] == 1)
        {
            int t=0;
            while (x[t]!=0)
            {
                x[t] = 0;
                t++;
            }
            x[t] = 1;
        }
    }
}

void from_binary( int *x, int w, int *n )
{
    /* x is an input array of w ints, either 0 or 1: the binary representation of n */
    /*           x[0] is the least significant bit   */
    /*           x[w-1] is the most signifcant bit, for a signed number, it is the sign */
    /* w is an input = to the width in bits of x */
    /* n is an output and will be equal to the value stored in binary in x */
    
    // Discriminate against negative sign //
    int Tcomplement = 0;
    if (x[w-1] == 1)
    {
        Tcomplement = 1;
    }
    
    // Sum of i-positions to the power of 2 //
    int sum = 0;
    for (int i=0; i != w-1; i++)
    {
        if (x[i] == 1)
        {
            int k = pow(2,i);
            sum += k;
        }
    }
    *n = sum;
    if (Tcomplement == 1)
    {
        *n *= -1;
    }
}

void adder( int *x, int *y, int *z, int *o, int w ) ;
void mult ( int *x, int *y, int *z, int *o, int w ) ;

void main()
{
    int a,b,c,d,w,n,o ;
    int x[32], y[32], z[32] ;
    
    o = 0 ;
    w = 9 ; /* word size in bits */
    /* note that x, y, and z must be at least as large as w and may be larger */
    
    b = 1 ; /* add one to each */
    for( c=250;c<260;c++ )
    {
        if ( c > 255 )  /* exceeds 8 bits */
            d = c-512 ;
        else
            d =  c ;
        printf( "%d %d ", d,b ) ;
        to_binary( d,w,x,&o ) ;
        if ( o )
            printf( "%d Overflow in to_binary! ",d ) ;
        to_binary( b,w,y,&o ) ;
        if ( o )
            printf( "%d Overflow in to_binary! ",b ) ;
        adder( x,y,z,&o,w ) ;
        from_binary( z,w,&a ) ;
        printf( "%d ", a ) ;
        if ( o )
            printf( "Overflow in adder! " ) ;
        printf( "\n" ) ;
    }
    
    b = -1 ; /* add -1 to each */
    for( c=250;c<260;c++ )
    {
        if ( c > 255 )
            d = c-512 ;
        else
            d =  c ;
        printf( "%d %d ", d,b ) ;
        to_binary( d,w,x,&o ) ;
        if ( o )
            printf( "%d Overflow in to_binary! ",d ) ;
        to_binary( b,w,y,&o ) ;
        if ( o )
            printf( "%d Overflow in to_binary! ",d ) ;
        adder( x,y,z,&o,w ) ;
        from_binary( z,w,&a ) ;
        printf( "%d ", a ) ;
        if ( o )
            printf( "Overflow in adder! " ) ;
        printf( "\n" ) ;
    }
    
    
    for( w=8; w<13; w=w+4 )          /* word sizes 8 and 12 bits */
        for( a=99; a >-100; a=a-99 )  /* 99, 0, -99 */
            for( b=4; b>-5; b=b-4 )    /* 4, 0, -4 */
            {
                to_binary( a,w,x,&o ) ;
                if ( o )
                    printf( "%d Overflow in to_binary! ",a ) ;
                to_binary( b,w,y,&o ) ;
                if ( o )
                    printf( "%d Overflow in to_binary! ",b ) ;
                mult( x,y,z,&o,w ) ;
                printf( "%d %d ", a,b ) ;
                from_binary( z,w,&n ) ;
                printf( "%d ", n ) ;
                if ( o )
                    printf( "Overflow in multiplier! " ) ;
                printf( "\n" ) ;
            }
}

/*
void main()
{
    // TESTING TO_BINARY //
    int n = 5;
    int x[8];
    int o=0;
    to_binary(n, 8, x, &o);
    printf("%d", n);
    printf(" to binary is ");
    printf("%d",x[0]);
    printf("%d",x[1]);
    printf("%d",x[2]);
    printf("%d",x[3]);
    printf("%d",x[4]);
    printf("%d",x[5]);
    printf("%d",x[6]);
    printf("%d",x[7]);
    
    printf("\n");
    
    // TESTING FROM_BINARY //
    int m[8];
    m[0] = 0;
    m[1] = 0;
    m[2] = 1;
    m[3] = 1;
    m[4] = 0;
    m[5] = 0;
    m[6] = 0;
    m[7] = 0;
    int r = 0;
    from_binary(m, 8, &r);
    printf("%d",r);
    printf(" is what will be added to the number above.");
    
    // TESTING ADDITION //
    printf("\n");
    int Z[8];
    int k=0;
    adder(x,m,Z,&o,8);
    printf("%d",Z[0]);
    printf("%d",Z[1]);
    printf("%d",Z[2]);
    printf("%d",Z[3]);
    printf("%d",Z[4]);
    printf("%d",Z[5]);
    printf("%d",Z[6]);
    printf("%d",Z[7]);
    printf(" or ");
    from_binary(Z,8,&k);
    printf("%d",k);
    printf(" is the result.");
    
    // TESTING MULTIPLICATION //
    printf("\n");
    printf("%d",n);
    printf(" will be multiplied by ");
    printf("%d",r);
    printf("\n");
    int Y[8];
    int u=0;
    mult(x,m,Y,&o,8);
    from_binary(Y,8,&u);
    printf("%d",u);
    printf(" is the result");
    
}
 */

 void adder( int *x, int *y, int *z, int *o, int w )
{
    /* x is an input array of w ints, either 0 or 1 representing the first addend */
    /* y is an input array of w ints, either 0 or 1 representing the second addend */
    /* z is an output array of w ints, either 0 or 1 representing the sum of x and y */
    /* o is an output = 1 if an overflow occurred */
    /* w is an input = to the width in bits of x, y, z */
    
    int i, c = 0;
    for (i = 0; i < w; i++)
    {
        z[i] = ((x[i] ^ y[i] ^c));
        c = ((x[i] & y[i]) | (x[i] &c)) | (y[i] & c);
    }
    z[i] = c;
}
void mult ( int *x, int *y, int *z, int *o, int w )
{
    /* x is an input array of w ints, either 0 or 1 representing the first multiplicand */
    /* y is an input array of w ints, either 0 or 1 representing the second multicand */
    /* z is an output array of w ints, either 0 or 1 representing the product of x and y */
    /* o is an output = 1 if an overflow occurred */
    /* w is an input = to the width in bits of x, y, z */
    o = 0;
    int A = 0;
    from_binary(x, w, &A);
    int B = 0;
    from_binary(y,w, &B);
    int C = A*B;
    to_binary(C, w, z, o);
}
