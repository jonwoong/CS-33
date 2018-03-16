#include <stdio.h>
#include <stdlib.h>

#define HEAPSIZE 32768

#define hdr_payload(i)  ((struct HDR *) &heap[i])->payload
#define hdr_freeall(i)  ((struct HDR *) &heap[i])->freeall
#define hdr_succesr(i)  ((struct HDR *) &heap[i])->succesr
#define hdr_previus(i)  ((struct HDR *) &heap[i])->previus

char *heap ;
int  heapsize ;

int sizehdr ;

struct HDR       // free block header/footer/linked list
{
    int  payload ;    // size of block (excluding headers)
    char freeall ;    // is this block allocated? 0=free/1=allocated
    int  succesr ;    // successor free block
    int  previus ;    // previous free block
} anchor ;

void dump_heap( char *t )   // prints all of the headers/footers on the heap
{
    int i ;
    
    int j=0 ;
    printf( "%s anchor= %d\n", t, anchor.succesr ) ;
    for( i=0; i<heapsize; i=i+sizehdr  )
        if ( hdr_payload(i) != 0 )
        {
            printf( "%6d %6d %6d %6d %6d\n", i, hdr_payload(i), hdr_freeall(i), hdr_previus(i), hdr_succesr(i) ) ;
            if ( j++%2==1 ) printf( "\n" ) ;
        }
}

void init_heap()   // initialize the heap
{
    heap = (char *) calloc( HEAPSIZE, 1 ) ;
    sizehdr =  16 ;
    heapsize = HEAPSIZE ;
    
    anchor.payload =  0 ;   // anchor
    anchor.freeall =  1 ;
    anchor.succesr =  0 ;
    anchor.previus = -1 ;
    
    hdr_payload(0) = heapsize-2*sizehdr ;   // header
    hdr_freeall(0) = 0 ;
    hdr_succesr(0) = heapsize ;
    hdr_previus(0) = -1 ;
    hdr_payload(heapsize-sizehdr) = heapsize-2*sizehdr ; // footer
    hdr_freeall(heapsize-sizehdr) = 0 ;
}

void wipeBlock(int position) // wipe block at position i
{
    hdr_payload(position) = 0;
    hdr_succesr(position) = 0;
    hdr_previus(position) = 0;
}

void lfree( int *aa )   // frees the block at address aa
{
    sizehdr = 16;
    
    int B2header = (*aa-sizehdr);
    int B2footer = (B2header) + hdr_payload(B2header) + sizehdr;
    int B1footer = (B2header-sizehdr);
    int B1header = (B1footer)-hdr_payload(B1footer)-sizehdr;
    int B3header = (B2footer+sizehdr);
    
    /*(Before freeing block 2)
           1 2 3
    case 1:A A A
    case 2:F A A
    case 3:A A F
    case 4:F A F
    */
    
    /// CASE 1 ///
    if (hdr_freeall(B1footer) == 1 && hdr_freeall(B2header) == 1 && hdr_freeall(B3header) == 1)
    {
        hdr_freeall(B2header) = 0;
        hdr_freeall(B2footer) = 0;
    }
    
    /// CASE 2 ///
    else if (hdr_freeall(B1footer) == 0 && hdr_freeall(B2header) == 1 &&hdr_freeall(B3header) == 1) // if footer of previous block free
    {
        hdr_freeall(B2header) = 0;
        hdr_freeall(B2footer) = 0;
        
        hdr_payload(B1header) += hdr_payload(B2header) + (2*sizehdr); // B1 gains B2 payload and header/footer sizes
        hdr_succesr(B1header) = B1header + hdr_payload(B1header) + (2*sizehdr); // Adjust B1+B2 successor in the header
        
        int B1B2footer = B1header + hdr_payload(B1header) + sizehdr; // position of B1+B2 footer
        
        hdr_payload(B1B2footer) = hdr_payload(B1header); // Set B1+B2 footer
        hdr_succesr(B1B2footer) = hdr_succesr(B1header); // Set B1+B2 footer
        hdr_previus(B1B2footer) = hdr_previus(B1header); // Set B1+B2 footer
        
        wipeBlock(B1footer); // Wipe old B1 footer
        wipeBlock(B2header); // Wipe old B2 header
    }
    
    /// CASE 3 ///
    else if (hdr_freeall(B1footer) == 1 && hdr_freeall(B2header) == 1 && hdr_freeall(B3header) == 0)
    {
        hdr_freeall(B2header) = 0;
        hdr_freeall(B2footer) = 0;
        
        hdr_payload(B2header) += hdr_payload(B3header) + (2*sizehdr); // B2 gains B3 payload and header/footer sizes
        hdr_succesr(B2header) = B2header + hdr_payload(B2header) + (2*sizehdr); // Adjust B2+B3 successor in the header
        
        int B2B3footer = B2header + hdr_payload(B2header) + sizehdr; // position of B2+B3 footer
        
        hdr_payload(B2B3footer) = hdr_payload(B2header); // Set B2+B3 footer
        hdr_succesr(B2B3footer) = hdr_succesr(B2header); // Set B2+B3 footer
        hdr_previus(B2B3footer) = hdr_previus(B2header); // Set B2+B3 footer
        
        wipeBlock(B2footer); // Wipe old B2 footer
        wipeBlock(B3header); // Wipe old B3 header
    }
    
    /// CASE 4 ///
    else if (hdr_freeall(B1footer) == 0 && hdr_freeall(B2header) == 1 && hdr_freeall(B3header) == 0)
    {
        hdr_freeall(B2header) = 0;
        hdr_freeall(B2footer) = 0;
        hdr_payload(B1header) += hdr_payload(B2header) + hdr_payload(B3header) + 4*sizehdr; // B1 gains B2+B3 payload AND header/footer sizes
        hdr_succesr(B1header) = B1header + hdr_payload(B1header) + 2*sizehdr; // Set B1 successor in header
        hdr_payload(B1header + hdr_payload(B1header) + sizehdr) = hdr_payload(B1header); // Set B1+B2+B3 footer
        hdr_previus(B1header + hdr_payload(B1header) + sizehdr) = hdr_previus(B1header); // Set B1+B2+B3 previous in footer
        
        wipeBlock(B1footer);
        wipeBlock(B2header);
        wipeBlock(B2footer);
        wipeBlock(B3header);
    }
    
    // Special situation after CASE 4 where B1 ends up in a negative position
    if (B1header < 0)
    {
        hdr_payload(0) = hdr_payload(B1header);
        hdr_succesr(0) = hdr_succesr(B1header);
        hdr_previus(0) = -1;
        hdr_freeall(0) = hdr_freeall(B1header);
    } // Set header
}

int lalloc( int amt )   // allocates a block of size int
{
    
    sizehdr = 16;
    int remainder = amt%16;
    if (remainder>0)
    {
        amt -= remainder;
        amt += 16;
    } // Round up amt to a multiple of 16
    
    int timesToRealloc = 0;
    while (amt+2*sizehdr > heapsize)
    {
        heapsize += HEAPSIZE;
        heap = (char *) realloc(heap,heapsize);
        timesToRealloc++;
    } // Add to the heap if necessary

    int footerOfLastFreeBlock = 0;
    if (timesToRealloc > 0)
    {
        for (int i=0; i<HEAPSIZE; i=i+sizehdr)
        {
            if (hdr_freeall(i) == 0)
            {
                footerOfLastFreeBlock = i;
            }
        }
        
        hdr_freeall(footerOfLastFreeBlock+sizehdr) = 0; // header of newly added block
        hdr_freeall(heapsize-sizehdr) = 0; // footer of newly added block
        
        hdr_payload(footerOfLastFreeBlock+sizehdr) = heapsize - footerOfLastFreeBlock-3*sizehdr; // payload of newly added block, set in header
        hdr_payload(heapsize-sizehdr) = heapsize - footerOfLastFreeBlock-3*sizehdr; // payload of newly added block, set in footer
        
        hdr_succesr(footerOfLastFreeBlock+sizehdr) = heapsize; // successor of newly added block, set in header
        hdr_previus(footerOfLastFreeBlock+sizehdr) = footerOfLastFreeBlock; // previous of newly added block, set in header

        hdr_freeall(hdr_previus(footerOfLastFreeBlock)+sizehdr) = 1; // set freeall of header of old free block to 1 so that lfree works
        
        int addressToFree = hdr_previus(footerOfLastFreeBlock)+2*sizehdr; // position of payload of last free old block
        int* pointer = &addressToFree;
        lfree(pointer);
    } // Coalesce the end of the first free block with the newly allocated block
    
    int blockposition = -1;
    for (int i=0; i<heapsize ; i=i+sizehdr)
    {
        if (hdr_freeall(i) == 0 && size <= hdr_payload(i))
        {
            blockposition = i;
            break;
        }
    } // find first fit block
    
    if (blockposition >= 0)
    {
        /// HEADER ///
        hdr_payload(blockposition) = amt;
        hdr_freeall(blockposition) = 1;
        hdr_succesr(blockposition) = blockposition+amt+(2*sizehdr);
        hdr_previus(blockposition) = blockposition-sizehdr;
        if (blockposition == 0)
        {
            hdr_previus(blockposition) = -1;
        }
        
        /// FOOTER ///
        hdr_payload(blockposition+amt+sizehdr) = amt;
        hdr_freeall(blockposition+amt+sizehdr) = 1;
        hdr_succesr(blockposition+amt+sizehdr) = blockposition+amt+(2*sizehdr);
        hdr_previus(blockposition+amt+sizehdr) = blockposition-sizehdr;
        if (blockposition == 0)
        {
            hdr_previus(blockposition+amt+sizehdr) = -1;
        }
        
        /// SPLIT BLOCK ///
        hdr_freeall(blockposition+amt+2*sizehdr) = 0;
        hdr_payload(blockposition+amt+2*sizehdr) = heapsize - (blockposition+amt+4*sizehdr);
        hdr_previus(blockposition+amt+2*sizehdr) = blockposition+amt+sizehdr;
        hdr_succesr(blockposition+amt+2*sizehdr) = heapsize;
        hdr_previus(heapsize-sizehdr) = blockposition+amt+sizehdr;
        hdr_payload(heapsize-sizehdr) = hdr_payload(blockposition+amt+2*sizehdr);
        hdr_succesr(heapsize-sizehdr) = heapsize;
    }
    return blockposition+sizehdr;
}


int main()
{
    int i ;
    int j = 0  ;
    int blks[100] ;
    
    init_heap() ;
    dump_heap( "init heap" ) ;
    
    for( i=0; i<20; i++ )
        blks[j++] = lalloc( rand()%500 ) ;
    dump_heap( "first allocs" ) ;
    
    lfree( &blks[10] ) ;
    lfree( &blks[11] ) ;
    dump_heap( "coalesce with upper" ) ;
    
    lfree( &blks[14] ) ;
    lfree( &blks[13] ) ;
    dump_heap( "coalesce with lower" ) ;

    lfree( &blks[5] ) ;
    lfree( &blks[7] ) ;
    lfree( &blks[6] ) ;
    dump_heap( "coalesce with both" ) ;

    blks[5] = lalloc( 80000 ) ;
    dump_heap( "blew the top off" ) ;
 
    for( i=0; i<20; i++ )
        if (blks[i] != 0 )
        {
            lfree( &blks[i] ) ;
        }
    dump_heap( "free everything " ) ;
    return 0 ;
}