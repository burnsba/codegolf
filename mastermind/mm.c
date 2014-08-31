#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// helper macros
#define min(x,y) ( (x<y) ? x : y )
#define max(x,y) ( (x>y) ? x : y )

#define MAX_SLOTS 2
#define MAX_COLORS 3

// max combinations is MAX_COLORS to the power of MAX_SLOTS
#define MAX_COMBINATIONS 9

// track the maximum number of sub-steps each step can have
// NOT USED
//uint8_t max_children = 0;

// global array holding all possible combinations of rows
// to test each strategy
uint8_t challenges[MAX_COMBINATIONS][MAX_SLOTS];

// Simple row. Contains an array where each component of the array holds a color
typedef struct mm_row
{
	uint8_t x[MAX_SLOTS];
} mm_row;

// Guess result. Contains the number of black and white pegs for each guess.
typedef struct mm_guess
{
	// correct color and correct position
	size_t black;
	
	// correct color but incorrect position
	size_t white;
} mm_guess;

// A strategy is made of multiple steps. Each step has a condition
// for whether or not it should be used. Each step contains a pointer
// to the next step in the chain and any children steps.
//
// The idea is to lay it out something like this:
//
// strategy
// |
// step ---> next step ---> next step
// |
// child ---> next step ---> next step
//
// For example, strategy for MM(2,3) should look like
//
// {AB:{10:{AC:{10:AA,01:CB,00:BB}},02:BA,01:{BC:{01:CA}},00:CC}}
//
// strategy
// |
// AB
// |
// (10)AC --------> (02)BA -------> (01)BC ---> (00)CC
// |                                   |
// (10)AA ---> (20)CB ---> (20)BB   (01)CA
typedef struct mm_step
{
	// current step number
	// NOT USED
	//size_t depth;
	
	// whether or not this step has been run
	// NOT USED
	//uint8_t has_been_evaluated;
	
	// guess for this step
	mm_row step_guess;
	
	// result for this guess
	mm_guess guess_result;
	
	// pointer to next step in the strategy
	struct mm_step *next_step;
	
	// pointer to step to be executed if this fails to resolve the challenge
	struct mm_step *child_step;
	
	// preconditions for this step to be execute.
	// must match the number of white and black pegs exactly.
	// NOT USED
	//size_t precondition_white;
	//size_t precondition_black;
} mm_step;

// master strategy class
typedef struct mm_strategy
{
	mm_step *first_step;
	
} mm_strategy;

// Helper function to display black and white pegs.
void print_mm_guess(mm_guess guess)
{
	printf("black: %d, white %d\n", guess.black, guess.white);
}

// Helper function to display the contents of a row.
void print_mm_row(mm_row row, char* name)
{
	size_t i;
	
	if (name != 0)
	{
		printf("%s ", name);
	}
	
	printf("row: ");
	
	for (i=0; i<MAX_SLOTS - 1; i++)
	{
		printf("%d,", row.x[i]);
	}
	
	printf("%d\n", row.x[MAX_SLOTS - 1]);
}

// Helper function to show all challenges
void print_all_challenges()
{
	size_t slot;
	size_t index;
	
	printf("challenges: \n");
	
	for (index=0; index<MAX_COMBINATIONS; index++)
	{
		printf("row %d: ", index);
	
		for (slot=0; slot<MAX_SLOTS - 1; slot++)
		{
			printf("%d,", challenges[index][slot]);
		}
	
		printf("%d\n", challenges[index][MAX_SLOTS - 1]);
	}
}

// Used to increment a guess to the next logical guess. This
// is treated as a number, more or less, where the least "bit" (greatest index)
// is incremented by one, and if the value overflows, the increment is trickled
// down to other values in the array.
void increment_row_guess(mm_row* row)
{
	size_t i = MAX_SLOTS - 1;
	
	while(1)
	{
		row->x[i]++;

		if (row->x[i] == MAX_COLORS)
		{
			row->x[i] = 0;
			i--;	
		}
		else
		{
			break;
		}
		
		if (i + 1 == 0)
		{
			break;
		}
	}
}

// Builds all challenges and loads them into the global object
void load_challenges()
{
	size_t slot;
	uint8_t color = 0;
	size_t index;
	
	size_t color_inc = 1;
	size_t color_inc_count = 0;
	
	slot = MAX_SLOTS - 1;
	
	// size_t is unsigned ...
	while(slot + 1 > 0)
	{
		// go down by slot, filling the last column and incrementing
		// the color each time. On the next slot, the color will
		// only be incremented everytime it "overflows". For example,
		// the first column might be 0,1,2,0,1,2, then the next column
		// would only increment every third time: 0,0,0,1,1,1, etc.
		for (index=0; index<MAX_COMBINATIONS; index++)
		{
			challenges[index][slot] = color;
			
			color_inc_count++;
			
			if (color_inc_count == color_inc)
			{
				color_inc_count = 0;
				color++;
		
				if (color == MAX_COLORS)
				{
					color = 0;
				}
			}
		}
		
		color_inc *= MAX_COLORS;
		
		color_inc_count = 0;
		color = 0;
		
		slot--;
	}
}

// Free memory belonging to a step and each of its children.
void free_step(mm_step* step)
{
	if (step == 0)
	{
		return;
	}
	
	mm_step* child = step->child_step;
	mm_step* next = step->next_step;
	
	mm_step* ptr;
	
	while(child != 0)
	{
		ptr = child->child_step;
		
		free_step(child);
		free(child);
		
		child = ptr;
	}
	
	while(next != 0)
	{
		ptr = next->next_step;
		
		free_step(next);
		free(next);
		
		next = ptr;
	}
}

// Free memory belonging to a strategy.
void free_strategy(mm_strategy* strat)
{
	free_step(strat->first_step);
	
	free(strat);
}

// Initialize a row filling it with 0.
void init_mm_row(mm_row *row)
{
	size_t slot;
	
	for (slot=0; slot<MAX_SLOTS; slot++)
	{
		row->x[slot] = 0;
	}
}

// Initialize a guess filling it with 0.
void init_mm_guess(mm_guess *guess)
{
	guess->white = 0;
	guess->black = 0;
}

// Initialize a step filling it with 0.
void init_mm_step(mm_step *step)
{
	//step->depth = 0;
	//step->has_been_evaluated = 0;
	
	init_mm_row(&(step->step_guess));
	init_mm_guess(&(step->guess_result));
	
	step->next_step = 0;
	step->child_step = 0;
}

// Allocate memory for a strategy and return pointer.
mm_strategy * new_strategy()
{
	return (mm_strategy*)malloc(sizeof(mm_strategy));
}


// Allocate memory for a step and return pointer.
mm_step* new_step()
{
	mm_step *step = (mm_step*)malloc(sizeof(mm_step));
	
	init_mm_step(step);
	
	return step;
}

// evaluates a guess against the answer and sets the number of white and black
// pegs in the result
void guess(mm_row guess, mm_row secret_answer, mm_guess *result)
{
	size_t slot;
	size_t color;
	
	init_mm_guess(result);
	
	for (slot=0; slot<MAX_SLOTS; slot++)
	{
		if (guess.x[slot] == secret_answer.x[slot])
		{
			// correct color and correct position will be zero
			result->black = result->black + 1;	
		}
	}
	
	if (result->black == MAX_SLOTS)
	{
		return;
	}
	
	// otherwise, figure out if there are correct colors in incorrect positions
	
	uint8_t colors_count_guess[MAX_COLORS];
	uint8_t colors_count_answer[MAX_COLORS];
	
	// The idea here is to count the total number of colors that show up in
	// both the guess and the answer. Then the min value for each color is
	// added to the total sum. For example (five colors, five slots):
	// guess: 00012
	// answer: 01234
	// colors_count_guess is 3,1,1,0,0
	// colors_count_answer is 1,1,1,1,1
	// min of both is 1,1,1,0,0
	// Therefore, the number returned would be three white pegs.
	//
	// Note that correct answers (black pegs) are skipped in the white peg count.
	for (color=0; color<MAX_COLORS; color++)
	{
		colors_count_guess[color] = 0;
		colors_count_answer[color] = 0;
		
		for (slot=0; slot<MAX_SLOTS; slot++)
		{
			if (guess.x[slot] == color && guess.x[slot] != secret_answer.x[slot])
			{
				colors_count_guess[color] = colors_count_guess[color] + 1;
			}
			
			if (secret_answer.x[slot] == color && guess.x[slot] != secret_answer.x[slot])
			{
				colors_count_answer[color] = colors_count_answer[color] + 1;
			}
		}
		
		result->white += min(colors_count_guess[color], colors_count_answer[color]);
	}
}

// blargh. Everything happens here.
void evaluate_strategy(mm_strategy * strat)
{
	mm_row answer;
	size_t slot, combination;
	size_t i;
	
	size_t resolved_steps = 0;
	size_t unresolved_steps = 0;
	
	uint8_t do_stuff = 1;
	uint8_t new_step_count = 0;
	size_t loop_counter = 0;
	
	mm_step* current_step = strat->first_step;
	mm_step* step_ptr;
	mm_step* last_step_ptr = 0;
	
	// there's actually a break statement at the end of the while loop depending
	// on how many news steps should be added.
	while(1)
	{
		printf("\n\nStarting loop %d\n", loop_counter);
		loop_counter++;
		
		resolved_steps = 0;
		unresolved_steps = 0;
		
		// test out the strategy on every combination
		for(combination=0; combination<MAX_COMBINATIONS; combination++)
		{
			// set the answer per the current combination
			for (slot=0; slot<MAX_SLOTS; slot++)
			{
				answer.x[slot] = challenges[combination][slot];
			}
			
			printf("\nEvaluating combination %d\n", combination);
			print_mm_row(answer, "answer");
			
			guess(current_step->step_guess, answer, &(current_step->guess_result));
			print_mm_row(current_step->step_guess, "guess");
			
			// Check if the very first guess was correct.
			if (current_step->guess_result.black != MAX_SLOTS)
			{
				// nope. not correct.
				printf("Strategy was not successful. Checking for children.\n");
				
				step_ptr = current_step->child_step;
				
				// is there a child?
				if (step_ptr != 0)
				{
					// yes, there is a child.
					printf("Found child guess.\n");
				
					guess(step_ptr->step_guess, answer, &(step_ptr->guess_result));
					print_mm_row(step_ptr->step_guess, "guess");

					// will this child resolve the current challange?
					if (step_ptr->guess_result.black == MAX_SLOTS)
					{
						// yes, this child is all we need.
						print_mm_guess(step_ptr->guess_result);
					
						printf("Strategy resolved correctly with children.\n");
						resolved_steps++;
					}
					else
					{
						// nope, first child was not correct.
						size_t children = 0;
						uint8_t success = 0;
						
						// keep trying the next child, and the next, and the next...
						while(step_ptr = step_ptr->next_step)
						{
							children++;
							
							guess(step_ptr->step_guess, answer, &(step_ptr->guess_result));
							
							print_mm_row(step_ptr->step_guess, "guess");
							
							if (step_ptr->guess_result.black == MAX_SLOTS)
							{
								success = 1;
								break;
							}
						}
						
						if (success)
						{
							printf("guess row: ---\nanswer row: ---\nblack: -, white: -\n");
					
							printf("Strategy resolved correctly at child %d.\n", children);
							resolved_steps++;
						}
						else
						{
							printf("guess row: ---\nanswer row: ---\nblack: -, white: -\n");
					
							printf("Done evaluating. Strategy did not resolve; %d child(ren) found.\n", children);
							unresolved_steps++;
						}
					}
				}
				else
				{
					// no children found. Nothing else to do.
					print_mm_guess(current_step->guess_result);
					
					printf("Done evaluating. Strategy did not resolve. No children found.\n");

					unresolved_steps++;
				}
			
			}
			// else, first guess was correct. yay
			else
			{
				print_mm_guess(current_step->guess_result);
					
				printf("Strategy resolved correctly without children.\n");
				resolved_steps++;
			}
		}
	
		printf("resolved: %d, unresolved: %d\n", resolved_steps, unresolved_steps);
		
		// strategy isn't perfect; lets add a child step.
		if (resolved_steps != MAX_COMBINATIONS)
		{
			printf("Appending child step.\n");
			
			new_step_count++;
			
			last_step_ptr = 0;
			
			// check if there are any child steps 
			if (current_step->child_step != 0)
			{
				// loop through the child until the last step is found
				step_ptr = current_step->child_step;
			
				while(step_ptr != 0 && step_ptr->next_step != 0)
				{
					step_ptr = step_ptr->next_step;
				}
				
				step_ptr->next_step = new_step();
				last_step_ptr = step_ptr;
				step_ptr = step_ptr->next_step;
			}
			// So this is the first time around. Set the child_step
			else
			{
				step_ptr = current_step;
				step_ptr->child_step = new_step();
				last_step_ptr = step_ptr;
				step_ptr = step_ptr->child_step;
			}
			
			// Copy over the guess from the previous item (parent or previous step)
			for(i=0; i<MAX_SLOTS; i++)
			{
				step_ptr->step_guess.x[i] = last_step_ptr->step_guess.x[i];
			}
			
			// Change the guess to something new.
			increment_row_guess(&(step_ptr->step_guess));
		}
	
		if (new_step_count > 5)
		{
			break;
		}
	}
}

// Simply fills out the first initial guess of the strategy.
void build_strategy(mm_strategy *strat)
{
	size_t i, color=0;
	
	// initialize first guess to be half of the first color,
	// and the other half will be the next color.
	// AB; ABB; AABB; AABBB; AAABBB; etc
	for (i=0; i< MAX_SLOTS/2; i++)
	{
		strat->first_step->step_guess.x[i] = color;
	}
	
	for (color++; i<MAX_SLOTS; i++)
	{
		strat->first_step->step_guess.x[i] = color;
	}
}

int main()
{
	load_challenges();
	print_all_challenges();
	
	mm_strategy *strat = new_strategy();
	
	strat->first_step = new_step();
	
	build_strategy(strat);
	
	evaluate_strategy(strat);
	
	free_strategy(strat);
	
	return 0;
}