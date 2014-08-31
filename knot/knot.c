#include <stdio.h>
#include <stdlib.h>

#define MAX_ROW 25
#define MAX_COL 80

// Since this is such a small project, I'm just going to use a global variable here.
// This contains the "grid" which is the ASCII picture of the knot. I'm going to call
// the knot "string" from now on.
char grid[MAX_ROW][MAX_COL];

#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4

int is_string(int row, int col)
{
	if (grid[row][col] == '|' || grid[row][col] == '-' || grid[row][col] == '+')
		return 1;
	
	return 0;
}

// Takes a look at two grid cells and determines if the current one is
// an "over" or "under" type of intersection. This assumes that the current
// cell is an intersection.
// 1 = over
// -1 = under
// 0 = unknown
int over_under(int last_row, int last_col, int curr_row, int curr_col)
{
	if (!is_string(last_row, last_col) || !is_string(curr_row, curr_col))
		return 0;
	
	// from '-' to '-' and from '|' to '|' are both over
	if (grid[last_row][last_col] == grid[curr_row][curr_col])
		return 1;
	// from '-' to '|' or from '|' to '-' is under
	if (grid[last_row][last_col] != grid[curr_row][curr_col])
		return -1;
}

// Determines if the given cell has regular string neighbors
// in the four cardinal directions. Only neighbors of type
// '-' or '|' are allowed.
// returns 1 if so, 0 otherwise
int is_crossover(int row, int col)
{
	// can't crossover on the edge
	if (row == 0 || row == MAX_ROW || col == 0 || col == MAX_COL)
	{
		return 0;
	}
	
	// only look at '|' or '-' cells
	if (grid[row][col] != '|' && grid[row][col] != '-')
		return 0;
	
	// the following four if statements make sure that the 
	// four adjacent cells are all string cells
	// but it ignores '+' corners. 
	if (grid[row - 1][col] != '|' && grid[row - 1][col] != '-')
		return 0;
	
	if (grid[row + 1][col] != '|' && grid[row + 1][col] != '-')
		return 0;
		
	if (grid[row][col - 1] != '|' && grid[row][col - 1] != '-')
		return 0;
	
	if (grid[row][col + 1] != '|' && grid[row][col + 1] != '-')
		return 0;
	
	return 1;
}

// printf helper to convert int direction to string
char* direction_to_string(int direction)
{
	switch(direction)
	{
		case NORTH: return "north";
		case EAST: return "east";
		case SOUTH: return "south";
		case WEST: return "west";
		default: return "";
	}
}

// debug helper to output the current state of the grid
void print_grid()
{
	int x,y;
	
	printf("grid is: \n");
	for (y=0; y<MAX_ROW; y++)
	{
		for(x=0; x<MAX_COL; x++)
		{
			if (grid[y][x] == '|' || grid[y][x] == '-' || grid[y][x] == '+' || grid[y][x] == '*' || grid[y][x] == '&')
			{
				printf("%c", grid[y][x]);
			}
			else
				printf(" ");
		}
		
		printf("\n");
	}
}

// scans the grid one row at a time to find the upper-left
// most corner, which is then considered the starting position.
void find_start(int *start_row, int *start_col)
{
	*start_row = -1;
	*start_col = -1;
	int row=0;
	int col=0;
	while (1)
	{
		switch(grid[row][col])
		{
			case ' ':
				break;
			case '+':
			case '|':
			case '-':
				*start_row = row;
				*start_col = col;
				break;
			default:
				break;
		}
		
		if (*start_row >= 0 && *start_col >= 0)
		{
			break;
		}
		
		col++;
		if (col >= MAX_COL)
		{
			col=0;
			row++;
			
			if (row >= MAX_ROW)
			{
				*start_row = -1;
				*start_col = -1;
				break;
			}
		}
	}
}

// Given an input cell and the direction the cell travelled from to arrive
// at the current cell, the next string cell is determined.
// Continues on from the previous direction, unless the next cell in that direction
// is not a string. In that case, the two perpendicular directions are tried.
// Attempts to avoid the case where two corners ('+') are adjacent.
//
// row, col, and from_direction are all set to the next cell along the string.
// If something unexpected happens, row and col are set to -1.
void travel(int* row, int* col, int *from_direction)
{
	switch (*from_direction)
	{
		case NORTH:
			if (is_string(*row + 1, *col) && !(grid[*row][*col] == '+' && grid[*row + 1][*col] == '+')
				&& !(grid[*row][*col] == '+' && grid[*row + 1][*col] == '-'))
			{
				*row = *row + 1;
				*from_direction = NORTH;
				return;
			}
			if (is_string(*row, *col - 1) && !(grid[*row][*col] == '+' && grid[*row][*col - 1] == '+')
				&& !(grid[*row][*col] == '+' && grid[*row][*col - 1] == '|'))
			{
				*col = *col - 1;
				*from_direction = EAST;
				return;
			}
			if (is_string(*row, *col + 1) && !(grid[*row][*col] == '+' && grid[*row][*col + 1] == '+')
				&& !(grid[*row][*col] == '+' && grid[*row][*col + 1] == '|'))
			{
				*col = *col + 1;
				*from_direction = WEST;
				return;
			}
		break;
		case EAST:
			if (is_string(*row, *col - 1) && !(grid[*row][*col] == '+' && grid[*row][*col - 1] == '+')
				&& !(grid[*row][*col] == '+' && grid[*row][*col - 1] == '|'))
			{
				*col = *col - 1;
				*from_direction = EAST;
				return;
			}
			if (is_string(*row + 1, *col) && !(grid[*row][*col] == '+' && grid[*row + 1][*col] == '+')
				&& !(grid[*row][*col] == '+' && grid[*row + 1][*col] == '-'))
			{
				*row = *row + 1;
				*from_direction = NORTH;
				return;
			}
			if (is_string(*row - 1, *col) && !(grid[*row][*col] == '+' && grid[*row - 1][*col] == '+')
				&& !(grid[*row][*col] == '+' && grid[*row - 1][*col] == '-'))
			{
				*row = *row - 1;
				*from_direction = SOUTH;
				return;
			}
		break;
		case SOUTH:
			if (is_string(*row - 1, *col) && !(grid[*row][*col] == '+' && grid[*row - 1][*col] == '+')
				&& !(grid[*row][*col] == '+' && grid[*row - 1][*col] == '-'))
			{
				*row = *row - 1;
				*from_direction = SOUTH;
				return;
			}
			if (is_string(*row, *col - 1) && !(grid[*row][*col] == '+' && grid[*row][*col - 1] == '+')
				&& !(grid[*row][*col] == '+' && grid[*row][*col - 1] == '|'))
			{
				*col = *col - 1;
				*from_direction = EAST;
				return;
			}
			if (is_string(*row, *col + 1) && !(grid[*row][*col] == '+' && grid[*row][*col + 1] == '+')
				&& !(grid[*row][*col] == '+' && grid[*row][*col + 1] == '|'))
			{
				*col = *col + 1;
				*from_direction = WEST;
				return;
			}
		break;
		case WEST:
			if (is_string(*row, *col + 1) && !(grid[*row][*col] == '+' && grid[*row][*col + 1] == '+')
				&& !(grid[*row][*col] == '+' && grid[*row][*col + 1] == '|'))
			{
				*col = *col + 1;
				*from_direction = WEST;
				return;
			}
			if (is_string(*row + 1, *col) && !(grid[*row][*col] == '+' && grid[*row + 1][*col] == '+')
				&& !(grid[*row][*col] == '+' && grid[*row + 1][*col] == '-'))
			{
				*row = *row + 1;
				*from_direction = NORTH;
				return;
			}
			if (is_string(*row - 1, *col) && !(grid[*row][*col] == '+' && grid[*row - 1][*col] == '+')
				&& !(grid[*row][*col] == '+' && grid[*row - 1][*col] == '-'))
			{
				*row = *row - 1;
				*from_direction = SOUTH;
				return;
			}
		break;
	}
	
	*row = -1;
	*col = -1;
}

// Travels around the string from the given starting position.
// over_count and under_count will contain the total number of crossovers of the respective
// types after done travelling.
void travel_loop(int start_row, int start_col, int start_from_direction, int *over_count, int *under_count)
{
	int last_x;
	int last_y;
	int x = start_col;
	int y = start_row;
	int from_direction = start_from_direction;
	
	int ou;
	*over_count = 0;
	*under_count = 0;
	
	do
	{
		last_x = x;
		last_y = y;
		travel(&y, &x, &from_direction);
		
		if (!(x == start_col && y == start_row) && is_crossover(y, x))
		{
			ou = over_under(last_y, last_x, y, x);
			if (ou == -1)
			{
				*under_count = *under_count + 1;
			}
			else if (ou == 1)
			{
				*over_count = *over_count + 1;
			}
		}
	} while (!(x == start_col && y == start_row));
}

// Similar to the travel_loop function, but this function travels in a loop erasing the string
// as it goes. Assumes the starting position is an intersection.
void erase_loop(int start_row, int start_col, int start_from_direction)
{
	int last_x;
	int last_y;
	int x;
	int y;
	int from_direction = start_from_direction;
	
	// create a temporary grid to mark which cells need to be deleted.
	// cell values of zero will be ignored, otherwise the contents of the grid
	// will be changed to the contents of the erase_map. This is done because it's
	// difficult to correctly travel along the string that has parts of it erased.
	char erase_map[MAX_ROW][MAX_COL];
	
	for (y=0; y<MAX_ROW; y++)
	{
		for(x=0; x<MAX_COL; x++)
		{
			erase_map[y][x] = 0;
		}
	}
	
	// no idea why, but if I remove this line I get a segmentation fault
	fflush(stdout);
	
	x = start_col;
	y = start_row;
	
	// travel along the string marking how to rewrite the grid
	do
	{
		if (!(x == start_col && y == start_row) && is_crossover(y, x))
		{
			if (grid[last_y][last_x] == '|')
				erase_map[y][x] = '-';
			else if (grid[last_y][last_x] == '-')
				erase_map[y][x] = '|';
			else
				erase_map[y][x] = '&';
		}
		else
		{
			erase_map[y][x] = ' ';
		}

		last_x = x;
		last_y = y;
		travel(&y, &x, &from_direction);
		
		if (x == -1 || y == -1)
		{
			printf("Arrived at bad position.\n");
			exit(0);
		}
		
	} while (!(x == start_col && y == start_row));
	
	// since the starting position is assumed to be an intersection, 
	// it's probably a corner now.
	erase_map[y][x] = '+';
	
	// done figuring out what to do. Make it so number one.
	for (y=0; y<MAX_ROW; y++)
	{
		for(x=0; x<MAX_COL; x++)
		{
			if (erase_map[y][x] != 0)
			{
				grid[y][x] = erase_map[y][x];
			}
		}
	}
}

int main(int argc, char** argv)
{
	char c;
	
	int start_row = -1;
	int start_col = -1;
	
	int x;
	int y;
	int last_x;
	int last_y;
	int from_direction;
	
	int ou;
	int over_count;
	int under_count;
	
	int loops_erased_count = 0;
	int erased_loop_last_time = 0;

	if (argc < 2)
	{
		printf("need grid file as command line argument.\n");
		exit(0);
	}

	FILE *ifp;
	
	ifp = fopen(argv[1], "r");
	
	if (!ifp)
	{
		printf("Invalid file.\n");
		exit(0);
	}
	
	// load file into grid array
	x=y=0;
	do {
	
		c = (char)(fgetc(ifp) & 0xff);
		
		if (c == '\n')
		{
			y++;
			x = 0;
			continue;
		}

		grid[y][x] = c;
		x++;
		
		if (y >= MAX_ROW || x >= MAX_COL)
		{
			printf("Input too large.\n");
			fclose(ifp);
			exit(0);
		}

	} while (c != EOF);

	fclose(ifp);
	
	// make sure it loaded correctly
	print_grid();
	
	// Travels around the string until an intersection is found.
	// For every intersection, travel and see if it can be eliminated.
	// If there are no crossovers, eliminate the loop and start over.
	do
	{
		erased_loop_last_time = 0;
		over_count = 0;
		under_count = 0;
	
		find_start(&start_row, &start_col);
	
		if (start_row == -1 || start_col == -1)
		{
			printf("Could not find a starting position\n");
			exit(0);
		}
	
		printf("Found start position at %d %d\n", start_col, start_row);
		y = start_row;
		x = start_col;
		from_direction = WEST;
	
		printf("x: %d, y: %d\n", x, y);
	
		do
		{
			last_x = x;
			last_y = y;
			travel(&y, &x, &from_direction);
		
			if (x == -1 || y == -1)
			{
				printf("Arrived at bad position.\n");
				exit(0);
			}
		
			printf("travelling to x: %d, y: %d from %s\n", x, y, direction_to_string(from_direction));
		
			if (is_crossover(y, x))
			{
				printf("found crossover at x: %d, y: %d\n", x, y);
				ou = over_under(last_y, last_x, y, x);
				if (ou == -1)
					printf("crossover is under\n");
				else if (ou == 1)
					printf("crossover is over\n");
				else
					printf("unknown crossover\n");
				
				printf("Sub traversing starting at crossover\n");
			
				travel_loop(y, x, from_direction, &over_count, &under_count);
				printf("Sub loop has %d overs, %d unders\n", over_count, under_count);
			
				if (over_count == 0 && under_count == 0 ||
					over_count == 0 && under_count > 0 ||
					over_count > 0 && under_count == 0)
				{
					printf("Erasing loop.\n");
					erase_loop(y, x, from_direction);
					print_grid();
					loops_erased_count++;
					erased_loop_last_time = 1;
					break;
				}
			}
		} while (!(x == start_col && y == start_row));
	
	} while (erased_loop_last_time == 1);
	
	printf("Done processing. Found %d loops to erase.\n", loops_erased_count);
	
	if (over_count == 0 && under_count == 0)
	{
		printf("Could not find a crossover. Must NOT be a knot.\n");
	}
	else
		printf("Looks like a sophisticated KNOT.\n");
	
	return 0;
}