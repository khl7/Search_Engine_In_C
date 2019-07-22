## Created by Khai Ly on 4/4/2019

**Note to grader:**

* The memory information in the provided code is not totally correct (Adrienne confirms this). We can see some difference between memory used in building OffsetIndex and TypeIndex (if we move the provided free() around. Other than that, please don't focus too much on the memory information.

* For the data folder, I had to delete about 1/2 the files to prevent virtual box from running out of memory. 

**Time to index:**

* My hypothesis is it would take much longer for title index. The result proves my theory to be true as the time for title index is about 3x longer than field index. 

* This makes sense because there are many more title to index than field types so whenever we try to index a movie by title, there are more nodes that we need to process/ go through (particularly in the lookup function that is called whenever we add a new movie to the index). This would significantly increase the indexing time. 

* Another factor that increase the title indexing time is that there are more loops as we need to go through the titles and parse them by words. Whereas for field indexing, we are just directly using the field in the Movie struct.

* Another factor that increases the title indexing time is the time the hash table has to resize due to overload (notice the 22197 entries vs 51/ 10 entries, the title index probably run resize function several times).

**Space to index:**

* My hypothesis is field index will take much less space than title index. The results of the test proves this hypothesis true (Note to grader: we can only see the memory difference if we comment out the free/ destroy function as freeing is just saying we can reuse that part of the memory. If we free, there is no way to see the memory difference using the current method). Index title takes up 3x space comparing to field type or field genre and 2x to field year.
 
* The first reason for my hypothesis is that indexing by title has many more 'nodes' of information comparing to indexing by fieldi. An example is there are 22197 entries in title index and only 10 in field index (and 9 of we index by genre). The more number of entries will increase and load factor and will significatly increase the size of the title index hash table. 

* The second reason for my hypothesis that title index takes up more space than field index is that we are making many seperate hash table in title index instead of making linkedlist in field index. Hashtable takes up much more space than linkedlist (albeit the values are difference in this case). 

* The third and final reason for my hypothesis is in title index, we are working on words of a movie (not the movie) so there are more 'nodes' (similar to first reason) and every word contains all the movie occurances. Albeit the movie occurances are in the form of doc_ID and linkedlist of rows (which is less than linkedlist of movie structs in field index) but the repeated occurances significantly increase its memory usage.

**Time to query:**

* My hypothesis for query time is that title index will take much longer to query than field index. The reason being there are more collisions on title index so the look up (depending on how big the collision/ the length of that chain) could result in significant longer time.

* (To grader: comment out line 200 - 224 in GetSeattleRomCom()  and 237 - 262 in GetRomComSeattle() to see my experiments). The result of my experiment proves my hypothesis correct. Query by title (in this case "Seattle") takes about 200x longer than query my field (in this case genre). 


**Time to query Seattle and Rom/Com both ways:**

* (To grader: comment out line 221-22 in GetSeattleRomCom to see the result more accurately. These two lines were there for valgrind purposes). If we query for both Seattle and Rom/Com, given the provided code, we can query for Rom/COm first then find Seattle and then query Seattle first and find Rom/Com.  

* My hypothesis for time of these two ways of queries is that the timing will be very similar. Because of we first query by Seattle, we need to put it in some form to search for Rom/Com and the most straightforward way (and the way implemented in this experiment) is to put it in a linked list and search it (the time complexity will be dominated by forming the original linked listi and forming a new one); whereas for getting Seattle from RomCom indexing, we need to search through a linkedlist of movies also but this linkedlist of movies is much longer because there are only 9 genres/ keys in field index. The search through in both ways will return similar time complexity.

* The result proves my hypothesis true as the query time are almost identical between the two.

**Multi-threaded parser**

* After applying multithreaded, the memory usage increases aby about 20% for both types of indexing. This makes sense because pthread takes up memory to create and join and there are mutex factors as well. The time differences increases by double. This actually makes sense the bulk of the work is in AddMovieTitleToIndex which we lock, and another large portion of the work is iterating through the DocID which we also lock. The time we are saving is only from parsing the file (fgets()) which is made up by the overhead cost of using multithread in the first place.  

* Adrienne explains that the time does not improve after multi threading because we are only on 1 processor. We should see multi threading improving time more significantly if we are on a multiple processors with each processor running some of the threads. Actually when we changed the number of processors in Virtual Machine, the time still does not improve. There is an unfinished discussion on Piazza regarding this issue.

**Queries curious about**

* There are no queries I am curious about given the current implementation. 

**Tests (Need to change implementation)**

* A test that I would like to run and would need to change the implementation is to see the difference between title index and the ID field index (There is an ID field in enum). This is easy to change by adding one more condition when we add to field index. I would like to know if there is a signification memory usage between title index and ID field index. My hypothesis is maybe not. Yes, we will still have more entries for title index but as the title index does not contains heavy movie struct, the difference in memory usage between the two index might be insignificant. 

**Miscellaneous**
* There are 7 movies with "Seattle" in their name in data small (just run ./main data_small/). There are about Action movies (just change the type to ge
