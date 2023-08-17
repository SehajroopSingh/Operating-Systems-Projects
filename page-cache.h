#pragma once
#include <infos/drivers/ata/ata-device.h>
#include <infos/drivers/ata/ata-controller.h>
#include <infos/kernel/kernel.h>
#include <infos/util/lock.h>
#include <infos/util/string.h>
#include <arch/x86/pio.h>
#include <infos/util/list.h>
#include <infos/util/map.h>


using namespace infos::kernel;
using namespace infos::drivers;
using namespace infos::drivers::ata;
using namespace infos::drivers::block;
using namespace infos::util;
using namespace infos::arch::x86;


/*
namespace infos
{
    namespace util 
    {
        template<typename T>
        
        class ExtendedList : public List<T> 
        {
        public:


        
            void move_to_front(typename List<T>::Node *node) 
            {
                if (!node || node == this->_head) {
                    return;
                }

                this->remove(node); // Assuming you have a 'remove' function that removes a node from the list
                node->Next = this->_head;
                this->_head->Prev = node;
                this->_head = node;
                node->Prev = NULL;
            }

            void remove_at(int index) 
            {
                if (index < 0 || index > this->count()) 
                {
                    return; // Invalid index
                }

                typename List<T>::Node **slot = &this->_elems;
                int ctr = 0;

                while (*slot && ctr != index) 
                {
                slot = &(*slot)->Next;
                ctr++;
                }

                if (*slot) {
                typename List<T>::Node *candidate = *slot;
                *slot = candidate->Next;

                delete candidate;
                this->_count--;
                }
            }

            void move_to_back(size_t index) 
            {
				if (index >= this->count()) {
					return; // Invalid index
				}

				if (index == this->count() - 1) {
					return; // Already at the back, no need to move
				}

				typename List<T>::Node *prev = nullptr;
				typename List<T>::Node *current = this->head();
				size_t counter = 0;

				while (current) {
					if (counter == index) {
						break;
					}
					prev = current;
					current = current->Next;
					counter++;
				}

				// Remove the node from its current position
				if (prev) {
					prev->Next = current->Next;
				} else {
					this->Head = current->Next;
				}

				// Move the node to the back
				typename List<T>::Node *last = this->last();
				last->Next = current;
				current->Next = nullptr;
            }
        };
    }
}

*/
namespace infos
{
	namespace kernel
	{

	}
	
	namespace drivers
	{
		namespace ata
		{

        extern kernel::ComponentLog cache_log;

        class Cache;
        

        struct CacheBlock
        {
           public: 
            uint8_t* originalbufferpointer;
            uint32_t block_offset = 0;
            uint8_t* data;
            uint8_t* buffer;
            size_t count;
            uint8_t* constantbufferpointer;
            int access_counter_ = 0;


        };


        class PageCache
        {
            public:
            # define CACHE_SIZE 64 // amount of blocks in the cache

            # define BLOCK_SIZE 512
                CacheBlock block;
                //block.buffer = &block.data;
                //List<CacheBlock>cache_; //the cache itself
                List<uint32_t>cacheoffsetlist; //the cache itself
                List<CacheBlock>cache_;

                Map<uint32_t, void*>cache_map_; //map to quickly look up cache blocks by offset
                uint32_t access_counter_ =0 ; // Counter to track access times for LRU replacement algorithm


            PageCache();
            ~PageCache();

            void* LRU_buffer(void* buffer, uint32_t block_offset, size_t count);
            bool init();
            bool check_for_data_in_cache(void* buffer,uint32_t block_offset,  size_t count);
            void* buffer_cache_pointer(void* buffer, uint32_t block_offset, size_t count);
            void write_data_to_cache(void* buffer, uint32_t block_offset, size_t count);
            void read_data_from_block_in_cache( void* buffer, uint32_t block_offset, size_t count);
            void print_cache();
            void buffer_data_checker(void* buffer);
            void check_cache_and_write_to_buffer(void* buffer, uint32_t block_offset, size_t count);
            void read_data_from_buffer(void* buffer, uint32_t block_offset, size_t count);


            void write_to_buffer(void* buffer, void* cachebuffer);
            void* check_for_data_in_cache2(void* buffer,uint32_t block_offset,  size_t count);

            void* check_for_data_in_cache_map(void* buffer, uint32_t block_offset,size_t count);
            void* return_pointer_MAP(void* buffer, size_t offset, size_t count);



            void* check_list_and_return(void* buffer, uint32_t block_offset,size_t count);
            void print_cacheoffsetlist();




            int hitcounter = 1;
            int misscounter = 1;
            int totalcounter = hitcounter + misscounter;
            int hitrate = hitcounter/totalcounter;
            int missrate = 1-hitrate;
        };



      
        }

    }

}



