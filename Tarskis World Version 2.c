#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include "bn.h"

#define NUM_VALID_OBJECTS 24576
#define MAX_OBJECTS_IN_WORLD 12

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

bool location_check_v2(uint32_t sizeof_w, uint32_t* w)
{
	// World representation: Each column is two bits (reversed), each row is each element in the array
	// Please note the the right side is x == 0, leftside is x == 7
	// If the point has 0, nothing occupies it
	// If the point has 1, a center occupies it
	// If the point has 2, a edge of a large object occupies it
	uint16_t world[8] = {0,0,0,0,0,0,0,0};
	
	for(uint32_t i = 0; i < sizeof_w; i++)
	{
		uint8_t c = (w[i] >> 6) & 63;
		
		uint8_t cy = c & 7;
		uint8_t cx = (c >> 2) & 14; // We want this to be multiplied by 2 - really the shift left

		// Y position above and below the center
		uint8_t by = cy + 1;
		uint8_t ty = cy - 1;
		
		uint8_t large = (w[i] >> 15) & 1;

		// Placement of object
		if((world[cy] >> cx) & 3) // If center is occupied by 1 or 2
			return false;
		
		world[cy] = world[cy] | (1 << cx); 
		if(large) 
		{ // Bound search
			uint16_t e_bit =  (2 << cx);
			if(cx != 14) 
			{ // Check "left" side
				if((world[cy] >> cx) & 4) return false; // If space if occupied by a center
				else world[cy] = world[cy] | (e_bit << 2);
			}
			if(cx) 
			{ // Check "right" side - x >= 2
				if((world[cy] >> (cx - 2)) & 1) return false; // If space if occupied by a center
				else world[cy] = world[cy] | (e_bit >> 2);
			}

			if(cy != 7) 
			{ // Check bottom
				if((world[by] >> cx) & 1) return false;
				else world[by] = world[by] | e_bit;
			}
			if(cy) 
			{ // Check top (y != 0)
				if((world[ty] >> cx) & 1) return false;
				else world[ty] = world[ty] | e_bit;
			}

			// Edge cases
			if(cx != 14 && cy) 
			{ // Check "left" and up side
				if((world[ty] >> cx) & 4) return false; // If space if occupied by a center
				else world[ty] = world[ty] | (e_bit << 2);
			}

			if(cx && cy) 
			{ // Check "right" and up side - x >= 2
				if((world[ty] >> (cx - 2)) & 1) return false; // If space if occupied by a center
				else world[ty] = world[ty] | (e_bit >> 2);
			}

			if(cx != 14 && cy != 7) 
			{ // Check "left" and bottom side
				if((world[by] >> cx) & 4) return false; // If space if occupied by a center
				else world[by] = world[by] | (e_bit << 2);
			}

			if(cx && cy != 7) 
			{ // Check "right" and bottom side - x >= 2
				if((world[by] >> (cx - 2)) & 1) return false; // If space if occupied by a center
				else world[by] = world[by] | (e_bit >> 2);
			}	    
		}
	}
	
	return true;
}

// Requires: l contains 2 valid objects (18-bit numbers)
// Returns: true if the locations of the objects do not conflict
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

// Requires: Requires: sizeof_w == length of w
// Modifies: nothing
// Effects: Prints all elements of w[] with spaces, and a new line at the end
// Note: Only for debugging; never actually called
void print_world(uint32_t w[], int sizeof_w)
{
	int ii = 0;
	for(ii = 0; ii < sizeof_w; ii++)
	printf("%d ",w[ii]);
	printf("\n");
}

// Requires: *a : a reference to a non-null big num
// Modifies: nothing
// Effects: Prints a 'bignum' (IN HEX)
void print_bignum(struct bn* a)
{
	char buf[4096];
	bignum_to_string(a, buf, sizeof(buf));
	printf("bignum (hex) = %s\n", buf);
}

int check_world(uint32_t w[], int sizeof_w)
{	
	// If a given world does not have 2 or more objects, it is automatically
	//  valid, since there is nothing that can be invalid
	if(sizeof_w < 2) return 1;

	if(!letter_check(sizeof_w, w))
		return 0;
	
	if(!(location_check_v2(sizeof_w, w)))
		return 0;
	
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
	assert(!(location_check_v2(2, idk)));
	printf("%d\n",location_check_v2(2, idk));
	
	// 2) should fail
	idk[0] = 8 << 6;
	idk[1] = 8 << 6;
	assert(!(location_check_v2(2, idk)));
	printf("%d\n",location_check_v2(2, idk));
	
	// 3) should pass
	idk[0] = 1 << 15;
	idk[1] = 33 << 10;
	assert(location_check_v2(2, idk));
	printf("3 -- %d\n",location_check_v2(2, idk));
	
	// 4) should fail
	idk[0] = 1 << 15;
	idk[1] = 65 << 9;
	assert(!(location_check_v2(2, idk)));
	printf("4 -- %d\n",location_check_v2(2, idk));
	
	// 5) should fail
	idk[0] = 1 << 15;
	idk[1] = ((1<<7)|1) << 9;
	assert(!(location_check_v2(2, idk)));
	printf("%d\n",location_check_v2(2, idk));
	
	// 6) should pass
	idk[0] = 1 << 15;
	idk[1] = ((1<<6)|1) << 10;
	assert(location_check_v2(2, idk));
	printf("%d\n",location_check_v2(2, idk));
	
	/*
	// 512 & 514 should pass
	idk[0] = 512;
	idk[1] = 514;
	assert(location_check_v2(2, idk));
	printf("%d\n",location_check_v2(2, idk));
	
	
	// 1024 & 528 should pass
	idk[0] = 1024;
	idk[1] = 528;
	assert(location_check_v2(2, idk));
	printf("%d\n",location_check_v2(2, idk));
	*/
	
	assert(0);
	//location_check_v2(2, idk));
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
	uint32_t valid_objects[NUM_VALID_OBJECTS];

	//i = 0;
	j = 0;

	// Goes through all potential objects (all 18 digit numbers)
	for(i = 0; i <= 262143; i++)
	{
		// Isolates the relevant bit to each possible size or shape
		//  Sizes: {Small, Medium, Large}
		//  Shapes: {Tetrahedron, Cube, Dodecahedron}
		s = (i >> 17) & 1;
		m = (i >> 16) & 1;
		l = (i >> 15) & 1;
		t = (i >> 14) & 1;
		c = (i >> 13) & 1;
		d = (i >> 12) & 1;

		if(l) continue;
		
		// Ensures only 1 size and shape bit is on for any given valid object
		if(!((s ^ m) ^ l) ^ (s & m & l))
			continue;
		if(!((t ^ c) ^ d) ^ (t & c & d)) 
			continue;
		
		// Any object that has met these requirements is valid, and can be added to the array
		valid_objects[j] = i;
		j++;
	}
	
	//valid_objects_tests(valid_objects);

	printf("Last index for valid_objects: %d \n", j);
	printf("*\n");
	printf("*\n");
	printf("*\n");

	struct bn final_count;
	bignum_from_int(&final_count, 0);

	for(int objects_in_world = 0; objects_in_world <= MAX_OBJECTS_IN_WORLD; objects_in_world++)
	{
		int indices[NUM_VALID_OBJECTS];
		int y = 0;
		
		for(int a = 0; a < objects_in_world; a++)
			indices[a] = a;
		
		uint32_t temp_world[objects_in_world]; //size k
		
		for(y = 0; y < objects_in_world; y++)
			temp_world[y] = valid_objects[indices[y]];
		
		bool overflow_check = false;
		
		if(check_world(temp_world, objects_in_world))
			bignum_inc(&final_count);
		
		while(true)
		{
			bool successful_loop = true;
			
			for(y = objects_in_world-1; y >= 0; y--)
			{
				if(indices[y] != y + NUM_VALID_OBJECTS - objects_in_world)
				{
					successful_loop = false;
					break;
				}
			}
			
			if(successful_loop)
				break;
			
			indices[y] += 1;
			
			for(int z = y+1; z < objects_in_world; z++)
				indices[z] = indices[z-1] + 1;
			
			uint32_t temp_world_2[objects_in_world];
			
			for(y = 0; y < objects_in_world; y++)
				temp_world_2[y] = valid_objects[indices[y]];
			
			if(check_world(temp_world_2, objects_in_world))
				bignum_inc(&final_count);
		}
		printf("Objects in world: %d \n",objects_in_world);
		print_bignum(&final_count);
	}
	return 0;
}
