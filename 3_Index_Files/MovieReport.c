/*
 *
 *  Edited by Khai Ly on 3/31/2019.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "MovieIndex.h"
#include "MovieReport.h"
#include "Movie.h"
#include "MovieSet.h"
#include "htll/LinkedList.h"
#include "htll/Hashtable.h"


// Prints a report of an index, assuming the key is a
// field/string and the value is a SetOfMovies.
void PrintReport(Index index) {
  OutputReport(index, stdout);
}

void OutputListOfMovies(LinkedList movie_list, char *desc, FILE *file) {
  fprintf(file, "%s: %s\n", "indexType", desc);
  fprintf(file, "%d items\n", NumElementsInLinkedList(movie_list));
  LLIter iter = CreateLLIter(movie_list);
  if (iter == NULL) {
    fprintf(file, "iter null for some reason.. \n");
    return;
  }
  Movie *movie;

  LLIterGetPayload(iter, (void**)&movie);
  if (movie->title != NULL) fprintf(file, "\t%s\n", movie->title);
  else fprintf(file, "title is null\n");

  while (LLIterHasNext(iter) != 0) {
    LLIterNext(iter);
    LLIterGetPayload(iter, (void**)&movie);
    if (movie->title != NULL) fprintf(file, "\t%s\n", movie->title);
    else fprintf(file, "title is null\n");
  }

  DestroyLLIter(iter);
}

// Assumes the value of the index is a SetOfMovies
void OutputReport(Index index, FILE* output) {
  // TODO: Implement this function.
  // After you've implemented it, consider modifying PrintReport()
  // to utilize this function.
  HTIter iter = CreateHashtableIterator(index -> ht);
  if (iter == NULL) {
    fprintf(output, "iter null for some reason.. \n");
    return;
  }
  HTKeyValue temp;

  HTIteratorGet(iter, &temp);
  OutputListOfMovies(((SetOfMovies) temp.value) -> movies,
    ((SetOfMovies) temp.value) -> desc, output);
  while(HTIteratorHasMore(iter) != 0) {
    HTIteratorNext(iter);
    HTIteratorGet(iter, &temp);
    OutputListOfMovies(((SetOfMovies) temp.value) -> movies,
    ((SetOfMovies) temp.value) -> desc, output);

  }

  DestroyHashtableIterator(iter);
}

void SaveReport(Index index, char* filename) {
  // TODO: Implement this. You might utilize OutputReport.
}

