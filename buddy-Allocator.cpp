/*
 * The Buddy Page Allocator
 * SKELETON IMPLEMENTATION TO BE FILLED IN FOR TASK 2
 */

#include <infos/mm/page-allocator.h>
#include <infos/mm/mm.h>
#include <infos/kernel/kernel.h>
#include <infos/kernel/log.h>
#include <infos/util/math.h>
#include <infos/util/printf.h>

using namespace infos::kernel;
using namespace infos::mm;
using namespace infos::util;

#define MAX_ORDER	18
// #define DEBUGPRINT


/**
 * A buddy page allocation algorithm.
 */
class BuddyPageAllocator : public PageAllocatorAlgorithm
{
private:

 	/** XX
 	 * Returns the number of pages that comprise a 'block', in a given order.
 	 * @param order The order to base the calculation off of.
 	 * @return Returns the number of pages in a block, in the order.
 	 */
 	static inline constexpr uint64_t pages_per_block(int order)
 	{
 		/* The number of pages per block in a given order is simply 1, shifted left by the order number.
 		 * For example, in order-2, there are (1 << 2) == 4 pages in each block.
 		 */
 		return (1 << order);
 	}


 	/** XX
 	 * Returns TRUE if the supplied page descriptor is correctly aligned for the 
 	 * given order.  Returns FALSE otherwise.
 	 * @param pgd The page descriptor to test alignment for.
 	 * @param order The order to use for calculations.
 	 */
 	static inline bool is_correct_alignment_for_order(const PageDescriptor *pgd, int order)
 	{
 		// Calculate the page-frame-number for the page descriptor, and return TRUE if
 		// it divides evenly into the number pages in a block of the given order.
 		return (sys.mm().pgalloc().pgd_to_pfn(pgd) % pages_per_block(order)) == 0;
 	}


	/** XX
	 * Given a page descriptor, and an order, 
	 * returns the buddy PGD.  The buddy could either be
	 * to the left or the right of PGD, in the given order.
	 * @param pgd The page descriptor to find the buddy for.
	 * @param order The order in which the page descriptor lives.
	 * @return Returns the buddy of the given page descriptor,
	 *  in the given order.
	 */
	PageDescriptor *buddy_of(PageDescriptor *pgd, int order)
	{

 		if (order >= MAX_ORDER) {
 			return NULL;
 		}

 		// (2) Check to make sure that PGD is correctly aligned in the order
 		if (!is_correct_alignment_for_order(pgd, order)) {
 			return NULL;
 		}

 		// (3) Calculate the page-frame-number of the buddy of this page.
 		// * If the PFN is aligned to the next order, then the buddy is the next block in THIS order.
 		// * If it's not aligned, then the buddy must be the previous block in THIS order.
 		uint64_t buddy_pfn = is_correct_alignment_for_order(pgd, order + 1) ?
 			sys.mm().pgalloc().pgd_to_pfn(pgd) + pages_per_block(order) : 
 			sys.mm().pgalloc().pgd_to_pfn(pgd) - pages_per_block(order);

 		// (4) Return the page descriptor associated with the buddy page-frame-number.
 		return sys.mm().pgalloc().pfn_to_pgd(buddy_pfn);
/* 
The page descriptor representing the current block will always be 
the one from the leftmost of the current block,but the buddy of 
the current block could be to the right or to the left of the 
current block.
*/
	}


 	/** XX
 	 * Inserts a block into the free list of the given order.  The block is inserted in ascending order.
 	 * @param pgd The page descriptor of the block to insert.
 	 * @param order The order in which to insert the block.
 	 * @return Returns the slot (i.e. a pointer to the pointer that points to the block) that the block
 	 * was inserted into.
 	 */
 	PageDescriptor **insert_block(PageDescriptor *pgd, int order)
 	{
 		// Starting from the _free_area array, find the slot in which the page descriptor
 		// should be inserted.
 		PageDescriptor **slot = &_free_areas[order];

 		// Iterate whilst there is a slot, and whilst the page descriptor pointer is numerically
 		// greater than what the slot is pointing to.
 		while (*slot && pgd > *slot) {
 			slot = &(*slot)->next_free;
 		}

 		// Insert the page descriptor into the linked list.
 		pgd->next_free = *slot;
 		*slot = pgd;

 		// Return the insert point (i.e. slot) 
 		return slot;
 	}


 	/** XX
 	 * Removes a block from the free list of the given order.  The block MUST be present in the free-list, otherwise
 	 * the system will panic.
 	 * @param pgd The page descriptor of the block to remove.
 	 * @param order The order in which to remove the block from.
 	 */
 	void remove_block(PageDescriptor *pgd, int order)
 	{
 		// Starting from the _free_area array, iterate until the block has been located in the linked-list.
 		PageDescriptor **slot = &_free_areas[order];
 		while (*slot && pgd != *slot) {
 			slot = &(*slot)->next_free;
 		}


 		// Make sure the block actually exists.  Panic the system if it does not.
 		//assert(*slot == pgd);

 		// Remove the block from the free list.
 		*slot = pgd->next_free;
 		pgd->next_free = NULL;
 	}

	/** XX
	 * Given a pointer to a block of free memory 
	 * in the order "source_order", this function will
	 * split the block in half, and insert it into the order below.
	 * @param block_pointer A pointer to a pointer containing 
	 * the beginning of a block of free memory.
	 * @param source_order The order in which the block of 
	 * free memory exists.  Naturally, the split will 
	 * insert the two new blocks into the order below.
	 * @return Returns the left-hand-side of the new block.
	 */
	PageDescriptor *split_block(PageDescriptor **block_pointer, int source_order)
	{



		// Make sure there is an incoming pointer.
 		assert(*block_pointer);

 		// Make sure the block_pointer is correctly aligned.
 		assert(is_correct_alignment_for_order(*block_pointer, source_order));

 		
 	
		// 1. Make sure 'source_order' is within range
		assert(source_order > 0);

		// 2. Make sure 'block_pointer' is not NULL
		assert(block_pointer != NULL);


		// 3. Mark the order below 
		int target_order = source_order - 1;

		// 4. Get the two black locations
		PageDescriptor *left_block = *block_pointer; //should these be pointers or regular?
		PageDescriptor *right_block = left_block + pages_per_block(target_order); //should these be pointers or regular?

		// 5.5 Make sure the two blocks are correctly ordered
		assert(left_block<right_block);

		// 6. Remove the block from the free list for the order
		remove_block(*block_pointer, source_order);

		// 7. Make sure the two blocks are correctly aligned

		// 8. Insert the two blocks into the order below
		insert_block(left_block, target_order);
		insert_block(right_block, target_order);


		// 9. Debug

		mm_log.messagef(LogLevel::DEBUG,"SPLIT_BLOCK: left_block: %p, right_block: %p", left_block, right_block);
		// 8. Return the left block
		mm_log.messagef(LogLevel::DEBUG,"is page free %p, %p", is_page_free(left_block, target_order), is_page_free(right_block, target_order));
		return left_block;
	}

	/** XX
	 * Reorders two page descriptors to be in order
	 * @param left A pointer to a page descriptor (what is believed to be the LHS)
	 * @param right A pointer to another page descriptor (what is believed to be the RHS)
	 * @action Do left,right = right,left if left > right
	 */
	void order_pages(PageDescriptor* &left, PageDescriptor* &right) {
		if (left > right) {
			auto temp = left;
			left = right;
			right = temp;
		}
	}


	/** XX
	 * Takes a block in the given source order, and
	 *  merges it (and its buddy) into the next order.
	 * @param block_pointer A pointer to a pointer 
	 * containing a block in the pair to merge.
	 * @param source_order The order in which the pair of blocks live.
	 * @return Returns the new slot that points to the merged block.
	 */
	PageDescriptor **merge_block(PageDescriptor **block_pointer, int source_order)
	{
 		assert(*block_pointer);

 		// Make sure the area_pointer is correctly aligned.
 		assert(is_correct_alignment_for_order(*block_pointer, source_order));

		//ensure that source order is less than max order
		assert(source_order < MAX_ORDER);

		// Get the target order
		int target_order = source_order + 1;

		// Get the buddy block
		PageDescriptor *left_block = *block_pointer;
		PageDescriptor *right_block = buddy_of(*block_pointer, source_order);

		// Make sure the buddy block is not NULL
		assert(right_block != NULL);

		// Make sure the buddy block is in the correct order
		//assert(buddy_of()->order == source_order);

		//order the blocks to make sure that right is greater than left
		order_pages(left_block, right_block);

		// Remove both blocks from the source order
		remove_block(right_block, source_order);
		remove_block(left_block, source_order);

		// Insert the new block into the target order
		return insert_block(left_block, target_order);
	}

public:
	/**
	 * Constructs a new instance of the Buddy Page Allocator.
	 */
	BuddyPageAllocator() {
		// Iterate over each free area, and clear it.
		for (unsigned int i = 0; i < ARRAY_SIZE(_free_areas); i++) {
			_free_areas[i] = NULL;
		}
	}

	/** XX
	 * Allocates 2^order number of contiguous pages
	 * @param order The power of two, of the number of contiguous pages to allocate.
	 * @return Returns a pointer to the first page descriptor for the newly allocated page range, or NULL if
	 * allocation failed.
	 */
	PageDescriptor *allocate_pages(int order) override
	{

		mm_log.messagef(LogLevel::DEBUG, "ALLOC_PAGES: order: %d", order);
		
		dump_state();
		// Ensure order is valid
		assert(order >= 0);
		assert(order <= MAX_ORDER);

		//Here we find the highest order which has a block and is non empty to start with, making sure we
		//don't cross maximum order 
		int x = order;
		while( _free_areas[x] == NULL && x <= MAX_ORDER) {
			x++;
		}
	mm_log.messagef(LogLevel::DEBUG, "ALLOC_PAGES: x: %d", x);
		PageDescriptor *block_pointer = _free_areas[x];
		
		//Till we don't reach our required order containing the block of 2^order pages
		//Since we're allocating anything, don't need to check for buddies 
		for(int j = x; j > order; j--) {
			block_pointer = split_block(&block_pointer, j);
		}
		//Remove the block of contiguous pages as it has been allocated
		remove_block(block_pointer, order);
		mm_log.messagef(LogLevel::DEBUG, "ALLOC_PAGES: pfn: %p", sys.mm().pgalloc().pgd_to_pfn(block_pointer));
		return block_pointer;	 	  		

	}
	
	PageDescriptor** is_page_free(PageDescriptor* pgd, int order)
	{
		auto slot = &_free_areas[order];
		while (*slot !=nullptr) {
			if (*slot ==pgd){
				return slot;
			}
		
			slot = &(*slot)->next_free;
		}

		return nullptr;
	}

	/**
	 * Get the smallest page descriptor (Student defined.)
	 * @param pgd_a A pointer to the page descriptor for a page
	 * @param pgd_b A pointer to another page descriptor (usually its buddy)
	 * @return Returns the smallest page descriptor
	 */
	PageDescriptor* get_smallest_pgd(PageDescriptor* pgd_a, PageDescriptor* pgd_b) {
		return (pgd_a > pgd_b) ? pgd_b : pgd_a;
	}

	/**
	 * Check whether a block contains a page. (Student defined.)
	 * @param block A pointer to an array of page descriptors
	 * @param order The power of two of the number of contiguous pages in the block
	 * @param pgd The page to check if is inside the block
	 * @return Returns TRUE if the page is inside the block, FALSE otherwise
	 */
	bool does_block_contain_page(PageDescriptor* block, int order, PageDescriptor* pgd) {
		auto size = pages_per_block(order);
		PageDescriptor* last_page = block + size;
		return (pgd >= block) && (pgd < last_page);
	}

	/**
	 * The result of a coalesce call. This is useful so you know what the new order is.
	 * @param pgd A pointer to the first page descriptor for the page.
	 * @param order The order of that page descriptor
	 */
	struct CoalesceResult
	{
		PageDescriptor* pgd;
		int order;
	};

	/**
	 * Merges pages as much as possible. (Student defined.)
	 * @param pgd A pointer to an array of page descriptors.
	 * @param order The power of two of number of contiguous pages.
	 * @return Returns the result of the coalesce request (see CoalesceResult for more details)
	 */
	CoalesceResult coalesce(PageDescriptor* pgd, int order) {
		// Make sure that the incoming page descriptor is correctly aligned
		// for the order on which it is being freed, for example, it is
		// illegal to free page 1 in order-1.
		assert(is_correct_alignment_for_order(pgd, order));

		// Ensure order is valid
		assert(order >= 0);
		assert(order <= MAX_ORDER);

		// If we are on largest order, we can't coalesce further, so short-circuit.
		if (order == MAX_ORDER) {
			return CoalesceResult{pgd, order};
		}

		auto buddy = buddy_of(pgd, order);
		while (is_page_free(buddy, order) != nullptr) {
			// Since the buddy is free, merge ourselves and the buddy. Always returns the LHS.
			pgd = *merge_block(&pgd, order);

			// Now pgd refers to the free pgd in an order above, so bump the order
			order++;

			// Update our buddy reference to be of the higher order
			buddy = buddy_of(pgd, order);

			// If we have hit the max order, we shouldn't continue
			if (order == MAX_ORDER) {
				break;
			}
		}

		return CoalesceResult{pgd, order};
	}

    /** XX
	 * Frees 2^order contiguous pages.
	 * @param pgd A pointer to an array of page descriptors to be freed.
	 * @param order The power of two number of contiguous pages to free.
	 */
    void free_pages(PageDescriptor *pgd, int order) override
		{
		// Make sure that the incoming page descriptor is correctly aligned
		// for the order on which it is being freed, for example, it is
		// illegal to free page 1 in order-1.
		assert(is_correct_alignment_for_order(pgd, order));
		
		// Ensure order is valid
		assert(order >= 0);
		assert(order <= MAX_ORDER);

		// Free these pages straight away.
		insert_block(pgd, order);

		// Now coalesce
		coalesce(pgd, order);
	}

    /** 
     * Marks a range of pages as available for allocation.
     * @param start A pointer to the first page descriptors to be made available.
     * @param count The number of page descriptors to make available.
     */
virtual void insert_page_range(PageDescriptor *start, uint64_t count) override
  
    {

    		mm_log.messagef(LogLevel::DEBUG, "New call to Inserting pages:start pgd %p, count: %lu)", start, count);
		auto order = MAX_ORDER;
		auto newcount = count;
    			dump_state();
		while (order >=0)
		{ 
			lable1:
			// If the count is zero, we are done
				if (newcount == 0)
				{		dump_state();
					return;
				}
			int x = (1 << order);
			mm_log.messagef(LogLevel::DEBUG, "pages in order %lu  are %lu", order, x);
			// Check if the range is aligned for this order
			if (is_correct_alignment_for_order(start, order))
			{	
				// Check if the range is large enough for this order
				if (newcount >= (1 << order))
				{
				mm_log.messagef(LogLevel:: DEBUG, " newCount (%lu) is greater than pages in order %lu" , newcount, order);
					// Insert the range into the free list for this order
					insert_block(start, order);
	mm_log.messagef(LogLevel::DEBUG, "Inserting block into order: %lu with start page %p" , order, start);
					dump_state();
					// Move the start pointer to the next block
					start += (1 << order);

					// Reduce the count by the number of pages in the block
					newcount -= (1 << order);
			mm_log.messagef(LogLevel::DEBUG, "NewCount after decrement %lu" , newcount);
					order = MAX_ORDER;

					goto lable1;
				}

			

			}
			// Move to the next order
			order--;
		}

		}
  /*  {
		mm_log.messagef(LogLevel::DEBUG, "Inserting pages:start pgd %p, count: %lu)", start, newcount);
		int newcount = count;
		auto order = MAX_ORDER;
		while (order >=0 && newcount!=0)
		{ 

			
			

			int x = (1 << order);
			mm_log.messagef(LogLevel::DEBUG, "pages in order %lu  are %lu", order, x);
			
		// Check if the range is large enough for this order
			if (newcount >= x)
			{
		mm_log.messagef(LogLevel:: DEBUG, " Count (%lu) is greater than pages in order %lu" , newcount, order);
			// Check if the range is aligned for this order
			if (is_correct_alignment_for_order(start, order))

				{
			// Insert the range into the free list for this order
					insert_block(start, order);
	mm_log.messagef(LogLevel::DEBUG, "Inserting block into order: %lu with start page %p" , order, start);

			// Move the start pointer to the next block
					start += (1 << order);
					


			// Reduce the count by the number of pages in the block
					newcount = newcount - x;
mm_log.messagef(LogLevel::DEBUG, "NewCount after decrement %lu" , newcount);
					order = MAX_ORDER;
					// If the count is zero, we are done
			if (newcount == 0){
				return;
			}

				}
				
			}
			else
			// If the range is not large enough for this order, move to the next order
			{
					order--;
			}
	
		}	
		
		//label2:
		
		//mm_log.messagef(LogLevel::DEBUG, "Finished inserting pages");
		//break;
		}*/
/*		
insert_page_range() is called by the memory management 
core to mark a number of contiguous pages as available for 
allocation. This may be done during initialization, or when 
the system no longer needs pages that were previously marked 
as unavailable. This just denotes that you can put these pages 
in their respective order as free. 

● Called by the kernel to "mark" a specific page as available for allocation
● Accepts the starting page descriptor and the count of the following pages which
must be marked as available for allocation
● Hint: You should not assume that all pages are available at the start in the free
lists area.
*/



    /** XX
     * Marks a range of pages as unavailable for allocation.
     * @param start A pointer to the first page descriptors to be made unavailable.
     * @param count The number of page descriptors to make unavailable.
     */
virtual void remove_page_range(PageDescriptor *start, uint64_t count) override
    {
		mm_log.messagef(LogLevel::DEBUG, "RESERVE_PAGE(pgd: %p)", start);
        dump_state();
        auto order = MAX_ORDER;
        PageDescriptor* current_block = nullptr;
        
        auto *pgd = start;
        
        auto *last_page = pgd + count;

		mm_log.messagef(LogLevel::DEBUG, "RESERVE_PAGE(pgd: %p, count: %lu)", pgd, count);
		mm_log.messagef(LogLevel::DEBUG, "RESERVE_PAGE(last pgd: %p, count: %lu)", last_page, count);


		PageDescriptor *pageblock;
		bool flag = false;

		for( int i = 0; i < count ; i++)
		{
			//Iterate through all orders
		for (int x = 0; flag == false && x < MAX_ORDER; ) {
			PageDescriptor *block_pointer = _free_areas[x];

			//Till we haven't reached a NULL free memory or last block for order
			while(block_pointer != NULL) {

			//If the block "contains" the page
				if (block_pointer <= pgd && pgd < (block_pointer + pages_per_block(x))) {
					pageblock = block_pointer;

					//Keep splitting until we get down to page
					for (int i = x; i>0;) {
						pageblock = split_block(&pageblock, i);
						i--;

						PageDescriptor *pagebuddy = buddy_of(pageblock, i);
						//Making sure page not gone into buddy
						if(pagebuddy <= pgd && pgd < (pagebuddy + pages_per_block(i))) {
							pageblock = pagebuddy;		
						}
					}
					//If we've come into this loop, we've found the page in a block, so break. 
					flag = true;
					break;
				}
				else {
					//Go to the next block
					block_pointer = block_pointer->next_free;
				}
			}
			x++;
		}
		//Found the page in a block
		if (flag == true) {
			remove_block(pgd, 0);
		}
		else {
			assert(false);
		}

			pgd++;
		}
mm_log.messagef(LogLevel::DEBUG, "RESERVED PAGES(from start:%p, to last page %p, %p, count: %lu)", start, last_page, pgd, count);
		
	}


	/**
	 * Initialises the allocation algorithm.
	 * @return Returns TRUE if the algorithm was successfully initialised, FALSE otherwise.
	 */
bool init(PageDescriptor *page_descriptors, uint64_t nr_page_descriptors) override
	{
	
	
		mm_log.messagef(LogLevel::DEBUG, "Buddy Allocator Initialising pd=%p, nr=0x%lx", page_descriptors, nr_page_descriptors);
		
	
		
				for (unsigned int i = 0; i < ARRAY_SIZE(_free_areas); i++) {
			_free_areas[i] = NULL;
		return true;
	}


}
	/**
	 * Returns the friendly name of the allocation algorithm, for debugging and selection purposes.
	 */
	const char* name() const override { return "buddy"; }


	/**
	 * Dumps out the current state of the buddy system
	 */
	void dump_state() const override
	{
		// Print out a header, so we can find the output in the logs.
		mm_log.messagef(LogLevel::DEBUG, "BUDDY STATE:");

		// Iterate over each free area.
		for (unsigned int i = 0; i < ARRAY_SIZE(_free_areas); i++) {
			char buffer[256];
			snprintf(buffer, sizeof(buffer), "[%d] ", i);

			// Iterate over each block in the free area.
			PageDescriptor *pg = _free_areas[i];
			while (pg) {
				// Append the PFN of the free block to the output buffer.
				snprintf(buffer, sizeof(buffer), "%s%lx ", buffer, sys.mm().pgalloc().pgd_to_pfn(pg));
				pg = pg->next_free;
			}

			mm_log.messagef(LogLevel::DEBUG, "%s", buffer);
		}
	}


private:
	PageDescriptor *_free_areas[MAX_ORDER+1];
};

/* --- DO NOT CHANGE ANYTHING BELOW THIS LINE --- */

/*
 * Allocation algorithm registration framework
 */
RegisterPageAllocator(BuddyPageAllocator);



