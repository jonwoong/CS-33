/* A note to who ever grades this:
1. Scheduling/time conflicts during thanksgiving week severely limited the amount of time
 I had to work on this
2. I did not attempt to get this code working on the SEAS Linux server (time constraints)
3. The output .txt files are most assuredly incorrect
4. I'm aware that I'm failing this class (due to prioritizing other classes, etc.) but 
want you to know that both the next lab and final will demonstrate a greater 
understanding of the material.

So please, don't work tirelessly to get this code working on the Linux server if it proves
to be difficult. Just know that I tried. I'm willing to take the F.
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Functions to calculate index,offset,tag
int intToOffset(int address)
{
    int result = 0;
    int array[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i=0;
    int R = address%2;
    while (address>1)
    {
        array[i] = R;
        i++;
        address = address/2;
        R = address%2;
    }
    if (address==1)
    {
        array[i]=1;
    }// Convert address to binary
    
    for (int n=0; n!=5; n++)
    {
        if (array[n]==1)
        {
            int addThis = pow(2,n);
            result += addThis;
        }
    }// Calculate offset bits
    return result;
}


int intToTag(int address)
{
    int result = 0;
    int array[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i=0;
    int R = address%2;
    while (address>1)
    {
        array[i] = R;
        i++;
        address = address/2;
        R = address%2;
    }
    if (address==1)
    {
        array[i]=1;
    }// Convert address to binary
    
    for (int n=0; n!=7; n++)
    {
        if (array[n+9]==1)
        {
            int addThis = pow(2,n);
            result += addThis;
        }
    }// Calculate tag bits
    return result;
}


int intToIndex(int address)
{
    int result = 0;
    int array[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i=0;
    int R = address%2;
    while (address>1)
    {
        array[i] = R;
        i++;
        address = address/2;
        R = address%2;
    }
    if (address==1)
    {
        array[i]=1;
    }// Convert address to binary
    
    for (int n=0; n!=4; n++)
    {
        if (array[n+5]==1)
        {
            int addThis = pow(2,n);
            result += addThis;
        }
    }// Calculate index bits
    return result;
}

// end user functions

#define S       16     //  (16 cache sets)
#define E       1      //  (4-way associative cache)
#define B       32     //  (32 elements in each block)
#define T       7      //  (7 tag bits)
#define M       65536  //  (65536 location memory)
#define READ    1
#define WRITE   0

int s ;
int b ;
int m ;

int rhits = 0 ; int rmiss = 0 ; int whits = 0 ; int wmiss = 0 ; int dwrit = 0 ;

struct cache_t
{
    char dirty ;
    char valid ;
    int  tag   ;
    int 	last  ;
    int  *block ;
} cache[S][E] ;

int *memory ;

int callno = 0 ;

int x  =     0 ;
int y  = 16384 ;
int z  = 32768 ;
int ni =    20 ;
int nj =    20 ;

void stats( char *t )
{
    int i,j,k,A ;
    
    for( i=0; i<S; i++ )
        for( j=0; j<E; j++ )
        {
            if( cache[i][j].valid & cache[i][j].dirty )
            {
                A = cache[i][j].tag*exp2(m-T)+i*B ;
                for( k=0; k<B; k++ )
                    memory[A+k] = cache[i][j].block[k] ;
                dwrit = dwrit+1 ;
            }
            cache[i][j].valid = 0 ;
            cache[i][j].dirty = 0 ;
        }
    
    printf( "%8s y=%5d ni=%5d nj=%5d rh=%5d rm=%5d wh=%5d wm= %5d dw=%5d\n", t, y,ni,nj,
           rhits, rmiss, whits, wmiss, dwrit ) ;
    rhits = 0 ; rmiss = 0 ;
    whits = 0 ; wmiss = 0 ;
    dwrit = 0 ;
    
}

void initcache()
{
    //
    // put code to calculate s, b, m here
    //
    s = log2(S);
    b = log2(B);
    m = log2(M);
    
    printf( "S= %d E= %d B= %d T= %d M= %d s= %d b= %d m= %d\n", S,E,B,T,M,s,b,m ) ;
    
    //
    // put code to initialize cache and memory here
    //
    for (int i=0; i<S; i++)
    {
        for (int j=0; j<E; j++)
        {
            cache[S][E].block = NULL;
            cache[S][E].valid = 0;
            cache[S][E].dirty = 0;
            cache[S][E].last = 0;
            cache[S][E].tag = 0;
        }
    } // initialize cache
    
    memory = (int*) malloc(M);
    for (int i=0; i< M; i++)
    {
        memory[i] = i;
    } // initialize memory
}

void readwritecache( int readwrite, int a, int *value, int *hitmiss, int voice )
{
    
    //
    // readwrite = READ (1) for read, WRITE (0) for write
    // a = binary memory address ( 0 <= a < 65535 ) to read or write
    // *value is memory value returned when readwrite = READ
    //           memory value to store when readwrite = WRITE
    // hitmiss is set to 0 for a miss, 1 for a hit, in either read or write case
    // voice is a debugging switch
    //
    //
    //   compute si, ta, and bo from the address a
    int si, ta, bo, li ;
    //	si = stack, index
    //	ta = tag
    //	bo = block offset
    //      increment callno
    //
    si = intToIndex(a);
    ta = intToTag(a);
    bo = intToOffset(a);
    callno++;

    if( voice )
        printf( "%5d rw= %d a= %5d bo= %3d si= %3d ta= %3d", callno,readwrite,a,bo,si,ta ) ;
    //
    //   check each line of the set:
    //	if( cache[si][line#].valid && cache[si][line#].tag = ta )
    //	to find a hit
    //
    //   if no hit, choose a line (not valid or LRU )
    //
    //   if chosen line dirty, copy to memory
    //
    
    int hit = 0;
    for (int i=0; i < E; i++)
    {
        if (cache[si][i].valid == 1 && cache[si][i].tag == ta)
        {
            hit=1;
            li = i;
            break;
        }
        else if (hit == 0 && cache[si][i].valid == 0)
        {
            li = i;
            continue;
        }
    }
    if (cache[si][li].dirty == 1)
    {
        memory[a] = *cache[si][li].block;
    }
    if( voice )
        printf( "li= %d", li ) ;
    //
    //    copy from memory to chosen line
    //
    cache[si][li].block = &memory[a];
    //    if write, copy value into line, set dirty
    //
    if (readwrite == 1)
    {
        cache[si][li].block = value;
        cache[si][li].dirty = 1;
        cache[si][li].valid = 1;
    }
    //    else copy value from line, not dirty
    //
    else if (readwrite == 0)
    {
        value = cache[si][li].block;
        cache[si][li].dirty = 0;
    }
    //    set last for line
    //
    cache[si][li].last = li;
    if( voice )
        printf( " %d %d %d\n", *value, cache[si][li].valid, cache[si][li].dirty ) ;
}

void locationexample()
{
    int i,j,k,hm, r;
    
    for( y=16374; y< 16395; y=y+1 )
    {
        for( i=0; i<nj;i++ )
        {
            readwritecache( READ,   x+i, &r, &hm, 0 ) ;
            readwritecache( WRITE,  y+i, &r, &hm, 0 ) ;                                                }
        stats( "loc copy" ) ;
    }
    
    
    //
    // code for row wise transponse
    //
    for (int i=0; i<nj; i++)
    {
        for (int j=0; j<ni; j++)
        {
            readwritecache( READ,   x+i, &r, &hm, 0 ) ;
            readwritecache( WRITE,  y+i, &r, &hm, 0 ) ;
        }
    }

    stats( "loc rows" ) ;
    
    //
    // code for col wise transponse
    //
    for (int j=0; j<nj; j++)
    {
        for (int i=0; i<ni; i++)
        {
            readwritecache( READ,   x+i, &r, &hm, 0 ) ;
            readwritecache( WRITE,  y+i, &r, &hm, 0 ) ;
        }
    }
    stats( "loc cols" ) ;
    
}

void wsexample()
{
    int i,j,ii,jj,hm,r ;
    
    y = 20000 ;
    for( ni=88; ni<128; ni=ni+8 )
    {
        nj = ni ;
        //
        // code for row wise transpose
        //
        for (i=0; i<ni; i++)
        {
            for (j=0; j<nj; j++)
            {
                readwritecache( READ,   x+i, &r, &hm, 0 ) ;
                readwritecache( WRITE,  y+i, &r, &hm, 0 ) ;
            }
        }
        stats( "ws rows" ) ;
    }
    
    for( ni=88; ni<128; ni=ni+8 )
    {
        nj = ni ;
        //
        // code for row wise transpose with 8x blocking
        //
        for (ii=0; ii<ni; ii=ii+8)
        {
            for (jj=0; jj<nj; jj=jj+8)
            {
                for (i=ii; i<ii+8; i++)
                {
                    for (j=jj; j<jj+8; j++)
                    {
                        readwritecache( READ,   x+i, &r, &hm, 0 ) ;
                        readwritecache( WRITE,  y+i, &r, &hm, 0 ) ;
                    }
                }
            }
        }
        stats( "wsbrows" ) ;
    }
    
    for( ni=88; ni<128; ni=ni+8 )
    {
        nj = ni ;
        //
        // code for col wise transpose 
        //
        for (j=0; j<nj; j++)
        {
            for (i=0; i<ni; i++)
            {
                readwritecache( READ,   x+i, &r, &hm, 0 ) ;
                readwritecache( WRITE,  y+i, &r, &hm, 0 ) ;
            }
        }
        stats( "ws cols" ) ;
    }
    
    for( ni=88; ni<128; ni=ni+8 )
    {
        nj = ni ;
        //
        // code for col wise transpose with 8x blocking
        //
        for (jj=0; jj<ni; jj=jj+8)
        {
            for (ii=0; ii<nj; ii=ii+8)
            {
                for (j=jj; j<jj+8; j++)
                {
                    for (i=ii; i<ii+8; i++)
                    {
                        readwritecache( READ,   x+i, &r, &hm, 0 ) ;
                        readwritecache( WRITE,  y+i, &r, &hm, 0 ) ;
                    }
                }
            }
        }

        stats( "wsbcols" ) ;
    }
    
}

int main()
{
    
    initcache() ;
    locationexample() ;
    wsexample() ;
    
    return 0 ;
}