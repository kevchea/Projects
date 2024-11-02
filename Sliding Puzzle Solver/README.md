This program solves an N x N sliding puzzle, where the goal is to arrange numbered tiles in a specific order by sliding them into an empty space. It uses the Breadth-First Search (BFS) algorithm to systematically explore possible moves and find the shortest sequence of moves that transforms the initial puzzle configuration into the solved state.

How It Works:
Reads the Puzzle Configuration:
The program takes an input file containing the size of the puzzle and the initial arrangement of the tiles.
Initializes Data Structures:
It creates a hash table to track visited states and a queue to manage states to be explored, ensuring efficient searching and avoiding revisiting the same puzzle configuration.
Performs BFS to Explore Moves:
It generates all possible moves (up, down, left, right) from the current board state and enqueues new states to explore further.
If a state matches the goal configuration, the program backtracks to record the moves that led to the solution.
Outputs the Solution:
If the puzzle is solvable, the program outputs the sequence of moves to solve the puzzle.
If no solution is found, it outputs a message indicating that the puzzle cannot be solved.
Memory Management: The program ensures all dynamically allocated memory is freed to prevent memory leaks.
