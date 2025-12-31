# commit -1
    In the initial commit we have created
    1. Inverted Index
    2. Mapping words and position for multiple text files
    The tokenized words are read but the positions of the words with special characters in btw are not handled properly they were left without any handling.

# commit -2
    Now we are reading each and every character of file and considering special characters as delimiters and sperating the words.
    for e.g -> 'ad-hoc' is stored as 'ad'- pos 1 & 'hoc'- pos 4

# commit -3
    Today i have am done with intersection of query values. Here we take the query and trying to find out all the words in a multiple docs only we return the doc where we have all the words present in.
    But from the current code it is not the case that is happening, and it is returning the doc id even then a single word is present in only a single doc. so we need to fix that in next code.