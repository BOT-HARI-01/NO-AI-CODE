# commit -1
    In the initial commit we have created
    1. Inverted Index
    2. Mapping words and position for multiple text files
    The tokenized words are read but the positions of the words with special characters in btw are not handled properly they were left without any handling.

# commit -2
    Now we are reading each and every character of file and considering special characters as delimiters and sperating the words.
    for e.g -> 'ad-hoc' is stored as 'ad'- pos 1 & 'hoc'- pos 4