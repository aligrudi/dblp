#!/bin/sh
# Query the DBLP database
curl "http://dblp.org/search/publ/api?q=`echo $1 | sed 's/ /+/g'`&format=json" >dblp.out
# Convert search results to refer records
./dblp <dblp.out
