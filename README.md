# Commons
These are mostly header only libraries I'm making for myself. They will have their own test modules and they are managed through [cman](https://github.com/alexcouch/cman).

## commons/includes/commons.h
This right now just has some typedefs to make my life easier I guess

## string/includes/string.h
Some string functions such as a way to represent string as a string that keeps track of its own length. Some concatenation functions too. It's recommended that you use the string representation instead of str. strcat is slower than string2string. If you try to make everything into a string object via create_string, you will thank me later. Let me know if you get different results.

For me, cman tells me that using string2string cat function is about 20-25 ms faster than strcat.