The tool is designed to compress and decompress text data efficiently. It achieves this by encoding frequently used characters with shorter binary codes and less common characters with longer binary codes.

How It Works:

Encoding: The program first reads an input text file and calculates the frequency of each character. It then builds a Huffman Tree based on these frequencies, assigning binary codes to characters in such a way that characters with higher frequencies get shorter codes. The original text is then converted into a binary string using these codes, resulting in a compressed version of the text.
Code Table: The character-to-code mapping (Huffman codes) is stored in a separate file, which is essential for decoding.
Decoding: To reconstruct the original text, the program uses the stored Huffman codes to rebuild the Huffman Tree. It then decodes the compressed binary string back into the original text.
