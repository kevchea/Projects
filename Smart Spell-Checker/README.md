This program reads a dictionary file and an input text file, checks for misspelled words, and provides suggestions to correct them using an open-addressed hash table. The hash table is used to store dictionary words and quickly check if a word from the input text is spelled correctly.

Setup:

The dictionary file is read to count the total number of words and initialize a hash table. Each word is then inserted into the hash table.
Spell Checking:

The input text is read word by word, and each word is checked against the hash table.
If a word is not found in the dictionary (misspelled), it is flagged as a typo, and the program suggests possible corrections:
Inverted Letters: Checks if swapping adjacent letters forms a valid word.
Missing Letters: Checks if adding a letter at the beginning or end forms a valid word.
Extra Letters: Checks if removing a letter from the beginning or end forms a valid word.
If specified, the program can add the misspelled word to the dictionary.
Output:

If a word is misspelled, it is printed along with suggestions. If no typos are found, "No typo!" is displayed.
Memory Management:

All allocated memory for the hash table and nodes is freed before the program exits to prevent memory leaks.
