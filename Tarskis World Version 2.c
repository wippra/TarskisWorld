#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>

// Tarski's World

// Requires: w is non-null, sizeof_w == length of w
// Modifies: none
// Returns: true if no labels are repeated, else false
bool letter_check(uint32_t sizeof_w, uint32_t* w)
{
    // All the labels in use
    uint8_t labels = 0;
    uint8_t label; // The current labels on the object

    for(uint32_t i = 0; i < sizeof_w; i++)
    {
	label = w[i] & 63;
	if(labels & label) return false;
	labels = labels | label; 
    }
    return true;
}

bool location_check(uint32_t l_[])
{
	int c0c = (l_[0] & 4032) >> 6;
	int c1c = (l_[1] & 4032) >> 6;
	
	int c0x = (c0c & 56) >> 3;
	int c0y = c0c & 7;
	
	int c1x = (c1c & 56) >> 3;
	int c1y = c1c & 7;
	
	int c0large = (l_[0] & (1 << 15)) >> 15;
	int c1large = (l_[1] & (1 << 15)) >> 15;
	
	// if they are the exact same location, return false
	if(!(c0c ^ c1c))
		return false;
	
	// if either shape is large
	if(c0large | c1large)
	{
		// Their centers must at least have 1 square between them
		if(!((abs(c0x - c1x) > 1) || (abs(c0y - c1y) > 1)))
			return false;
	}
	return true;
}

void print_world(uint32_t w[], int sizeof_w)
{
	int ii = 0;
	for(ii = 0; ii < sizeof_w; ii++)
	{
		printf("%d ",w[ii]);
	}
	printf("\n");
}

int check_world(uint32_t w[], int sizeof_w)
{
	//print_world(w,sizeof_w);
	//int valid_objects_length = sizeof_w; // Unless I'm mistaken, valid_objects or sizeof_w is never changed in this function - Owen
	int indices[sizeof_w];
	int combo_length = 2;
	int y = 0;

	if(sizeof_w < 2) return 1;
	for(int a = 0; a < combo_length; a++)
		indices[a] = a;

	uint32_t temp_pair[combo_length]; //size k

	for(y = 0; y < combo_length; y++)
		temp_pair[y] = w[indices[y]];

	if(!letter_check(sizeof_w, w))
	    return 0;
	
	if(!(location_check(temp_pair)))
		return 0;
	
	while(true)
	{
		bool successful_loop = true;

		for(y = combo_length-1; y >= 0; y--)
		{
			if(indices[y] != y + sizeof_w - combo_length)
			{
				successful_loop = false;
				break;
			}
		}
		
		if(successful_loop)
			break;
		
		indices[y] += 1;
		
		for(int z = y+1; z < combo_length; z++)
			indices[z] = indices[z-1] + 1;
		
		uint32_t temp_pair_2[combo_length];
		
		for(y = 0; y < combo_length; y++)
			temp_pair_2[y] = w[indices[y]];

		if(!(location_check(temp_pair_2)))
			return 0;
	}

	return 1;
}

void test_cases()
{
	uint32_t idk[2];
	
	// Letter Tests
	
	// 0 & 0 should pass
	idk[0] = 0;
	idk[1] = 0;
	assert(letter_check(2, idk));
	printf("%d\n",letter_check(2, idk));
	
	// 1 & 1 should fail
	idk[0] = 1;
	idk[1] = 1;
	assert(!(letter_check(2, idk)));
	printf("%d\n",letter_check(2, idk));
	
	// 2 & 3 should fail
	idk[0] = 2;
	idk[1] = 3;
	assert(!(letter_check(2, idk)));
	printf("%d\n",letter_check(2, idk));
	
	// 4 & 5 should fail
	idk[0] = 4;
	idk[1] = 5;
	assert(!(letter_check(2, idk)));
	printf("%d\n",letter_check(2, idk));
	
	// 3 & 4 should pass
	idk[0] = 3;
	idk[1] = 4;
	assert(letter_check(2, idk));
	printf("%d\n",letter_check(2, idk));
	
	// 63 & 12 should fail
	idk[0] = 63;
	idk[1] = 12;
	assert(!(letter_check(2, idk)));
	printf("%d\n",letter_check(2, idk));
	
	// 56 & 7 should pass
	idk[0] = 56;
	idk[1] = 7;
	assert(letter_check(2, idk));
	printf("%d\n",letter_check(2, idk));
	
	// 42 & 21 should pass
	idk[0] = 42;
	idk[1] = 21;
	assert(letter_check(2, idk));
	printf("%d\n",letter_check(2, idk));

	printf("\n ================ \n\n");
	
	// Location Tests
	
	
	// 1) should fail
	idk[0] = 1 << 6;
	idk[1] = 1 << 6;
	assert(!(location_check(idk)));
	printf("%d\n",location_check(idk));
	
	// 2) should fail
	idk[0] = 8 << 6;
	idk[1] = 8 << 6;
	assert(!(location_check(idk)));
	printf("%d\n",location_check(idk));
	
	// 3) should pass
	idk[0] = 1 << 15;
	idk[1] = 33 << 10;
	assert(location_check(idk));
	printf("3 -- %d\n",location_check(idk));
	
	// 4) should fail
	idk[0] = 1 << 15;
	idk[1] = 65 << 9;
	assert(!(location_check(idk)));
	printf("4 -- %d\n",location_check(idk));
	
	// 5) should fail
	idk[0] = 1 << 15;
	idk[1] = ((1<<7)|1) << 9;
	assert(!(location_check(idk)));
	printf("%d\n",location_check(idk));
	
	// 6) should pass
	idk[0] = 1 << 15;
	idk[1] = ((1<<6)|1) << 10;
	assert(location_check(idk));
	printf("%d\n",location_check(idk));
	
	/*
	// 512 & 514 should pass
	idk[0] = 512;
	idk[1] = 514;
	assert(location_check(idk));
	printf("%d\n",location_check(idk));
	
	
	// 1024 & 528 should pass
	idk[0] = 1024;
	idk[1] = 528;
	assert(location_check(idk));
	printf("%d\n",location_check(idk));
	*/
	
	assert(0);
	//location_check(idk));
}

void valid_objects_tests(uint32_t v[])
{
	int k;
	k = 0;
	
	bool checks[10];
	
	for(k = 0; k < 36864; k++)
	{
		assert(v[k] >= (9 << 12));
		
		switch(v[k])
		{
			case(((1 << 5) | 1) << 12):
				printf("Valid_1 Correct \n");
				break;
			case(151551):
				printf("Valid_2 Correct \n");
				break;
			case(74898):
				printf("Valid_3 Correct \n");
				break;
			case(147519):
				printf("Valid_4 Correct \n");
				break;
			case(3 << 16):
				assert(false);
		}
		
	}
}

int main()
{
	//test_cases();

	int s, m, l, t, c, d, i, j;
	int size;
	
	// Generation of valid objects
	uint32_t valid_objects[36864];

	//i = 0;
	j = 0;

	for(i = 0; i <= 262143; i++)
	{
	    
	    // Small, medium, or large   
	    s = (i >> 17) & 1;
	    m = (i >> 16) & 1;
	    l = (i >> 15) & 1;

	    // I don't know what t,c, and d are - Owen
	    t = (i >> 14) & 1;
	    c = (i >> 13) & 1;
	    d = (i >> 12) & 1;
		
		if(!((s ^ m) ^ l) ^ (s & m & l))
			continue;
		if(!((t ^ c) ^ d) ^ (t & c & d)) 
			continue;
		
		valid_objects[j] = i;
		j++;
	}
	valid_objects_tests(valid_objects);

	printf("Last index for valid_objects: %d \n", j);
	printf("*\n");
	printf("*\n");
	printf("*\n");

	uint32_t final_count = 0;
	uint32_t overflows = 0;

	int combo_length = 0;
	for(combo_length = 0; combo_length < 3; combo_length++)
	{
		int valid_objects_length = 36864;
		int indices[valid_objects_length];
		//int combo_length = 3;
		//int final_count = 0;
		int y = 0;
	
		for(int a = 0; a < combo_length; a++)
			indices[a] = a;
	
		uint32_t temp_world[combo_length]; //size k
	
		for(y = 0; y < combo_length; y++)
			temp_world[y] = valid_objects[indices[y]];
		
		bool overflow_check = false;
		final_count += check_world(temp_world, combo_length);
		if(final_count == 1)
			overflow_check = true;
		
		if(overflow_check &&  final_count == 0)
		{
			overflow_check = false;
			overflows += 1;
		}
		
		while(true)
		{
			bool successful_loop = true;
	
			for(y = combo_length-1; y >= 0; y--)
			{
				if(indices[y] != y + valid_objects_length - combo_length)
				{
					successful_loop = false;
					break;
				}
			}
			
			if(successful_loop)
				break;
			
			indices[y] += 1;
			
			for(int z = y+1; z < combo_length; z++)
				indices[z] = indices[z-1] + 1;
			
			uint32_t temp_world_2[combo_length];
			
			for(y = 0; y < combo_length; y++)
				temp_world_2[y] = valid_objects[indices[y]];
	
			bool overflow_check_2 = false;
			final_count += check_world(temp_world_2, combo_length);
			if(final_count == 1)
			overflow_check_2 = true;
		
			if(overflow_check_2 &&  final_count == 0)
			{
				overflow_check_2 = false;
				overflows += 1;
			}
		}
		printf("%d -- %d -- %"PRIu32"\n",combo_length,final_count,final_count);
		printf("Overflows: %"PRIu32"\n",overflows);
	}
	return 0;
}
