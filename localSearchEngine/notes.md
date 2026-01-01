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

# commit -4
    Now I have fixed the search & query processing, where we only go for intersection only when we have all the words of the query present in the index so that we can find the correct document which has the relation to the query.
    yesterday we have done soft matching search where we see for the highest repeated words in multiple docs if not in a single one, we have done with intersection search.
    