#include <stdio.h>
#include <iostream>
using namespace std;

char *min_ptr = NULL , *max_ptr ;


void spray_paint( char *x, int sz, char tok, char *t )
{
    printf( "%p %3d %x %s\n", x,sz,(unsigned char) tok,t ) ;
    
    if ((min_ptr == NULL) || (x<min_ptr))
    {
        min_ptr = x;
    } // Set min_ptr
    
    for (int i=0; i<sz; i++)
    {
        *x = tok;
        x++;
    } // Overwrite previous value with token
    
    if ((max_ptr == NULL)||(x > max_ptr))
    {
        max_ptr = x;
    } // Set max_ptr
}

void dumper( char *x, int n )
{
    long long temp = (long long)x; // Cast x to long long
    temp = temp - (temp%16); // Make x divisible by 16
    for (int i=0; i<n; i++)
    {
        printf("%p",temp + (16*i));
        cout << " ";
        printf("%0.8x", *x); // +x00
        cout << " ";
        printf("%0.8x",*(x + 4)); // +x04
        cout << " ";
        printf("%0.8x",*(x + 8)); // +x08
        cout << " ";
        printf("%0.8x",*(x + 12)); // +x0c
        x++;
        cout<<endl;
    }
}


void sub2()
{
    int i ;
    char* a ;
    char x[20] ;
    
    spray_paint( (char *) &i, sizeof(i), 0xf1, "sub2.i" ) ;
    spray_paint( (char *) &a, sizeof(a), 0xf2, "sub2.a" ) ;
    spray_paint( (char *) &x, sizeof(x), 0xf3, "sub2.x" ) ;
    printf ( "Min= %p Max= %p\n", min_ptr, max_ptr ) ;
    dumper( min_ptr,(int) (max_ptr-min_ptr)/16+1 ) ;
    
    //Save Stack
    char *originalMinPtr = min_ptr; // Remember position of original min_ptr
    int n=0;
    long arraySize = max_ptr-min_ptr; // Size of external array
    char savedStack[arraySize];
    while (n < arraySize)
    {
        savedStack[n] = *min_ptr;
        n++;
        min_ptr++;
    } // Save each element
    min_ptr = originalMinPtr; // Reset min_ptr
    
    //Destroy Stack
    spray_paint(min_ptr, arraySize, 0xf4, "destroy_stack");
    
    printf( " destroyed stack\n" ) ;
    dumper( min_ptr,(int) (max_ptr-min_ptr)/16+1 ) ;
    //Restore Stack
    int m=0;
    while (m < arraySize)
    {
        *min_ptr = savedStack[m];
        m++;
        min_ptr++;
    } // Restore each element
    min_ptr = originalMinPtr; // Reset min_ptr
    printf( " restored stack\n" ) ;
    dumper( min_ptr,(int) (max_ptr-min_ptr)/16+1 ) ;
}

void sub1()
{
    float i ;
    short a ;
    int x[20] ;
    
    spray_paint( (char *) &i, sizeof(i), 0xe1, "sub1.i" ) ;
    spray_paint( (char *) &a, sizeof(a), 0xe2, "sub1.a" ) ;
    spray_paint( (char *) &x, sizeof(x), 0xe3, "sub1.x" ) ;
    sub2() ;
}
int main()
{
    struct mine {
        char* a ;
        char x[20] ;
        float z ;
    } i;
    
    union crazy {
        float a ;
        char b ;
        int s ;
    } a ;
    
    char x[50] ;
    
    spray_paint( (char *) &i, sizeof(i), 0xd1, "main.i" ) ;
    spray_paint( (char *) &a, sizeof(a), 0xd2, "main.a" ) ;
    spray_paint( (char *) &x, sizeof(x), 0xd3, "main.x" ) ;
    
    sub1() ;
    
    return 0 ;
}

