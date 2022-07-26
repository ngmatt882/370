/*
 * EECS 370, University of Michigan
 * Project 4: LC-2K Cache Simulator
 * Instructions are found in the project spec.
 */

#include <stdio.h>
#include <math.h>

#define MAX_CACHE_SIZE 256
#define MAX_BLOCK_SIZE 256

extern int mem_access(int addr, int write_flag, int write_data);

enum actionType
{
    cacheToProcessor,
    processorToCache,
    memoryToCache,
    cacheToMemory,
    cacheToNowhere
};

typedef struct blockStruct
{
    int data[MAX_BLOCK_SIZE];
    int dirty;
    int valid;
    int lruLabel;
    int set;
    int tag;
} blockStruct;

typedef struct cacheStruct
{
    blockStruct blocks[MAX_CACHE_SIZE];
    int blockSize;
    int numSets;
    int blocksPerSet;
    int size;
} cacheStruct;

/* Global Cache variable */
cacheStruct cache;

void printAction(int, int, enum actionType);
void printCache();
void updateLRU(int set, int block);

/*
 * Set up the cache with given command line parameters. This is 
 * called once in main(). You must implement this function.
 */
void cache_init(int blockSize, int numSets, int blocksPerSet){
    cache.blockSize = blockSize;
    cache.numSets = numSets;
    cache.blocksPerSet = blocksPerSet;
    cache.size = blockSize*numSets*blocksPerSet;
    printf("Cache info: Block Size = %d, Number of Sets = %d, Blocks per Set = %d\n", blockSize, numSets, blocksPerSet);

    printf("Setting dirty and valid bits\n");
    for(int i = 0; i < numSets; ++i){
        int start = i*blocksPerSet;
        for(int j = 0; j < blocksPerSet; ++j){
            int next = start + j;
            cache.blocks[next].dirty = 0;
            cache.blocks[next].set = i;
            cache.blocks[next].valid = 0;
            cache.blocks[next].lruLabel = j;
        }
    }

    printf("Finished Initialization\n");
    return;
}

/*
 * Access the cache. This is the main part of the project,
 * and should call printAction as is appropriate.
 * It should only call mem_access when absolutely necessary.
 * addr is a 16-bit LC2K word address.
 * write_flag is 0 for reads (fetch/lw) and 1 for writes (sw).
 * write_data is a word, and is only valid if write_flag is 1.
 * The return of mem_access is undefined if write_flag is 1.
 * Thus the return of cache_access is undefined if write_flag is 1.
 */
int cache_access(int addr, int write_flag, int write_data) {
    // find tag bits, block bits and set bits
    int addr_copy = addr;
    int block_offset = addr & (cache.blockSize - 1);

    addr_copy = addr_copy >> (int)log2(cache.blockSize);

    int set_bit = addr_copy & (cache.numSets - 1);
    int tag_bits = addr_copy >> (int)log2(cache.numSets);

    // start of "block" address
    addr_copy = addr_copy << (int)log2(cache.blockSize);
    
    // find if in cache aka a hit
    for(int i = set_bit*cache.blocksPerSet; i < (set_bit+1)*cache.blocksPerSet; ++i){
        //error checking
        if(cache.blocks[i].set != set_bit){
            printf("Checking wrong block\n");
        }
        if(cache.blocks[i].tag == tag_bits && cache.blocks[i].valid == 1){
            printf("Hit!\n");
            updateLRU(set_bit, i - set_bit*cache.blocksPerSet);
            if(write_flag){
                printAction(addr, 1, processorToCache);
                cache.blocks[i].dirty = 1;
                cache.blocks[i].data[block_offset] = write_data;
                return cache.blocks[i].data[block_offset];
            }
            else{
                printAction(addr, 1, cacheToProcessor);
                return cache.blocks[i].data[block_offset];
            }
        }
    }

    printf("Miss!\n");
    // find if empty block in cache aka not valid
    for(int i = set_bit*cache.blocksPerSet; i < (set_bit+1)*cache.blocksPerSet; ++i){
        //error checking
        if(cache.blocks[i].set != set_bit){
            printf("Checking wrong block\n");
        }
        if(cache.blocks[i].valid == 0){
            printf("Empty!\n");
            printAction(addr_copy, cache.blockSize, memoryToCache);
            cache.blocks[i].valid = 1;
            cache.blocks[i].tag = tag_bits;
            updateLRU(set_bit, i - set_bit*cache.blocksPerSet);
            for(int j = 0; j < cache.blockSize; ++j){
                cache.blocks[i].data[j] = mem_access(addr_copy + j, 0, 0);
            }

            if(write_flag){
                printAction(addr, 1, processorToCache);
                cache.blocks[i].dirty = 1;
                cache.blocks[i].data[block_offset] = write_data;
                return cache.blocks[i].data[block_offset];
            }
            else{
                printAction(addr, 1, cacheToProcessor);
                return cache.blocks[i].data[block_offset];
            }
        }
    }
    printf("No empty blocks\n");

    // evict LRU, push to memory if dirty
    for(int i = set_bit*cache.blocksPerSet; i < (set_bit+1)*cache.blocksPerSet; ++i){
        //error checking
        if(cache.blocks[i].set != set_bit){
            printf("Checking wrong block");
        }
        if(cache.blocks[i].lruLabel == 0){
            //check if block is dirty
            int old_addr = cache.blocks[i].tag << (int)log2(cache.numSets);
            old_addr += set_bit;
            old_addr = old_addr << (int)log2(cache.blockSize); 
            if(cache.blocks[i].dirty == 1){
                printAction(old_addr, cache.blockSize, cacheToMemory);
                for(int j = 0; j < cache.blockSize; ++j){
                    mem_access(old_addr + j, 1, cache.blocks[i].data[j]);
                }
                cache.blocks[i].dirty = 0;
            }
            else{
                printAction(old_addr, cache.blockSize, cacheToNowhere);
            }

            cache.blocks[i].tag = tag_bits;
            printAction(addr_copy, cache.blockSize, memoryToCache);
            updateLRU(set_bit, i - set_bit*cache.blocksPerSet);
            for(int j = 0; j < cache.blockSize; ++j){
                cache.blocks[i].data[j] = mem_access(addr_copy + j, 0, 0);
            }

            if(write_flag){
                printAction(addr, 1, processorToCache);
                cache.blocks[i].dirty = 1;
                cache.blocks[i].data[block_offset] = write_data;
                return cache.blocks[i].data[block_offset];
            }
            else{
                printAction(addr, 1, cacheToProcessor);
                return cache.blocks[i].data[block_offset];
            }
        }
    }
    printf("Missed LRU\n");
    return mem_access(addr, write_flag, write_data);
}


/*
 * print end of run statistics like in the spec. This is not required,
 * but is very helpful in debugging.
 * This should be called once a halt is reached.
 * DO NOT delete this function, or else it won't compile.
 */
void printStats(){
    return;
}

/*
 * Log the specifics of each cache action.
 *
 * address is the starting word address of the range of data being transferred.
 * size is the size of the range of data being transferred.
 * type specifies the source and destination of the data being transferred.
 *  -    cacheToProcessor: reading data from the cache to the processor
 *  -    processorToCache: writing data from the processor to the cache
 *  -    memoryToCache: reading data from the memory to the cache
 *  -    cacheToMemory: evicting cache data and writing it to the memory
 *  -    cacheToNowhere: evicting cache data and throwing it away
 */
void printAction(int address, int size, enum actionType type)
{
    printf("$$$ transferring word [%d-%d] ", address, address + size - 1);

    if (type == cacheToProcessor) {
        printf("from the cache to the processor\n");
    }
    else if (type == processorToCache) {
        printf("from the processor to the cache\n");
    }
    else if (type == memoryToCache) {
        printf("from the memory to the cache\n");
    }
    else if (type == cacheToMemory) {
        printf("from the cache to the memory\n");
    }
    else if (type == cacheToNowhere) {
        printf("from the cache to nowhere\n");
    }
}

/*
 * Prints the cache based on the configurations of the struct
 * This is for debugging only and is not graded, so you may
 * modify it, but that is not recommended.
 */
void printCache()
{
    printf("\ncache:\n");
    for (int set = 0; set < cache.numSets; ++set) {
        printf("\tset %i:\n", set);
        for (int block = 0; block < cache.blocksPerSet; ++block) {
            printf("\t\t[ %i ]: {", block);
            for (int index = 0; index < cache.blockSize; ++index) {
                printf(" %i", cache.blocks[set * cache.blocksPerSet + block].data[index]);
            }
            printf(" }\n");
        }
    }
    printf("end cache\n");
}
/*
int log2(int input){
    return (int)(log(input)/log(2));
}*/

void updateLRU(int set, int block){
    int less = cache.blocks[cache.blocksPerSet*set + block].lruLabel;
    for(int i = cache.blocksPerSet*set; i < cache.blocksPerSet*(set+1); ++i){
        if(cache.blocks[i].lruLabel > less){
            --cache.blocks[i].lruLabel;
        }
    }
    //Set to most recently used
    cache.blocks[cache.blocksPerSet*set + block].lruLabel = cache.blocksPerSet - 1;
}