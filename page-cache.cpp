


#include <infos/drivers/ata/ata-device.h>
#include <infos/drivers/ata/ata-controller.h>
#include <infos/kernel/kernel.h>
#include <infos/util/lock.h>
#include <infos/util/string.h>
#include <arch/x86/pio.h>


/*
Your code solution should be in page-cache.h and/or page-cache.cpp.

For our testing, these files should be placed under the 
infos/include/infos/drivers/ata/ and infos/drivers/ata/ 
directories respectively. The cache needs to have a fixed 
size of 64 blocks and for us to be able to test it with various 
cache sizes this should be a constant variable.
*/




/*
IMPORTANT Note: For the final submission, upon a block access
 your code should print the offset of the block and whether 
 it is a cache miss or cache hit (e.g., debug: cache: [CACHE HIT]
offset=686).
*/



# define CACHE_SIZE 64 // amount of blocks in the cache



/* TODO Using the offset (and block size) I can 
find the block pointer (buffer pointer), and then
I just copy the next 512 bytes from the buffer?
Actions
Hi, you should start storing number of 
blocksize bytes into the cache from each 
block pointer. And if you carefully read 
the code inside ata-device.cpp, you can 
find the treasure.

oh okay so buffer we can calculate using pointer
 arithmetic and block number can be calculated 
 by offset and block size right? 



*/






using namespace infos::kernel;
using namespace infos::drivers;
using namespace infos::drivers::ata;
using namespace infos::drivers::block;
using namespace infos::util;
using namespace infos::arch::x86;
using namespace infos::drivers::ata;

PageCache::PageCache()
{


}


PageCache::~PageCache()
{

}





void PageCache::init()
{
    // Initialize the cache
    // Initialize the cache
    for (int i = 0; i < CACHE_SIZE; i++)
    {
        ata.logf(LogLevel::DEBUG, "cache: Initializing cache");
        cache[i].valid = false;
        cache[i].dirty = false;
        cache[i].block_offset = 0;
        cache[i].buffer = new uint8_t[512];
    }
}

bool PageCache::checkfordata(uint32_t block_offset)
{
    // Check if the block is in the cache
    for (int i = 0; i < CACHE_SIZE; i++)
    {
        if (cache[i].valid && cache[i].block_offset == block_offset)
        {
            return true;
        }
    }
    return false;
}



void PageCache::read_blocks_from_cache(uint32_t block_offset, uint8_t* buffer)
{
    // Read the block from the cache
    for (int i = 0; i < CACHE_SIZE; i++)
    {
        if (cache[i].valid && cache[i].block_offset == block_offset)
        {
            memcpy(buffer, cache[i].buffer, 512);
            return;
        }
    }
}





void PageCache::write_blocks_to_cache(uint32_t block_offset, uint8_t* buffer)
{
    // Write the block to the cache
    for (int i = 0; i < CACHE_SIZE; i++)
    {
        if (!cache[i].valid)
        {
            cache[i].valid = true;
            cache[i].block_offset = block_offset;
            memcpy(cache[i].buffer, buffer, 512);
            return;
        }
    }
    // If the cache is full, replace the first block
    cache[0].block_offset = block_offset;
    memcpy(cache[0].buffer, buffer, 512);
}

