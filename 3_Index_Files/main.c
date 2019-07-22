/*
 *
 *  Edited by Khai Ly on 3/31/2019.
 *
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "htll/Hashtable.h"
#include "htll/LinkedList.h"

#include "MovieSet.h"
#include "DocIdMap.h"
#include "FileParser.h"
#include "FileCrawler.h"
#include "MovieIndex.h"
#include "Movie.h"
#include "QueryProcessor.h"
#include "MovieReport.h"


DocIdMap docs;
Index docIndex;


/**
 * Open the specified file, read the specified row into the
 * provided pointer to the movie.
 */
int CreateMovieFromFileRow(char *file, long rowId, Movie** movie) {
  // TODO: Read a specified row from a specified file into Movie.

  FILE* fPtr;
  if ((fPtr = fopen(file, "r")) == NULL) {
    printf("File could not be opened\n");
    return -1;
  }
  char row[1000];
  long i = 0;
  char* rowInfo;

  while (fgets(row, 1000, fPtr) != NULL) {
    if (i == rowId) {
      rowInfo= row;
      break;
    }
    i++;
  }
  *movie = CreateMovieFromRow(rowInfo);
  fclose(fPtr);
  return 0;
}

void doPrep(char *dir) {
  printf("Crawling directory tree starting at: %s\n", dir);
  // Create a DocIdMap
  docs = CreateDocIdMap();
  CrawlFilesToMap(dir, docs);

  printf("Crawled %d files.\n", NumElemsInHashtable(docs));

  // Create the index
  docIndex = CreateIndex();

  // Index the files
  printf("Parsing and indexing files...\n");
  ParseTheFiles(docs, docIndex);
  printf("%d entries in the index.\n", NumElemsInHashtable(docIndex->ht));
}

void runQuery(char *term) {
  // TODO: This function pretty much runs the queries.
  // Figure out how to get a set of Movies and create
  // a nice report from them.
  SearchResultIter results = FindMovies(docIndex, term);
  LinkedList movies = CreateLinkedList();

  if (results == NULL) {
    printf("No results for this term. Please try another.\n");
    return;
  } else {
    SearchResult sr = (SearchResult)malloc(sizeof(*sr));
    if (sr == NULL) {
      printf("Couldn't malloc SearchResult in main.c\n");
      DestroyLinkedList(movies, NullFree);
      return;
    }
    int result;
    char *filename;

    // Get the first
    SearchResultGet(results, sr);
    filename = GetFileFromId(docs, sr->doc_id);

    // TODO: What to do with the filename?
    Movie* movie;
    CreateMovieFromFileRow(filename, sr -> row_id, &movie);
    InsertLinkedList(movies, movie);

    // Check if there are more
    while (SearchResultIterHasMore(results) != 0) {
      result =  SearchResultNext(results);
      if (result < 0) {
        printf("error retrieving result\n");
        break;
      }
      SearchResultGet(results, sr);
      char *filename = GetFileFromId(docs, sr->doc_id);
      // TODO: What to do with the filename?


      CreateMovieFromFileRow(filename, sr -> row_id, &movie);
      InsertLinkedList(movies, movie);
    }

    free(sr);
    DestroySearchResultIter(results);
  }
  // TODO: Now that you have all the search results, print them out nicely.

  Index index = BuildMovieIndex(movies, Type);
  PrintReport(index);
  DestroyTypeIndex(index);
}

void runQueries() {
  char input[1000];
  while (1) {
    printf("\nEnter a term to search for, or q to quit: ");
    scanf("%s", input);

    if (strlen(input) == 1 &&
        (input[0] == 'q')) {
          printf("Thanks for playing! \n");
          return;
      }

    printf("\n");
    runQuery(input);
  }
}

int main(int argc, char *argv[]) {
  // Check arguments
  if (argc != 2) {
    printf("Wrong number of arguments.\n");
    printf("usage: main <directory_to_crawl>\n");
    return 0;
  }

  doPrep(argv[1]);
  runQueries();

  DestroyOffsetIndex(docIndex);

  DestroyDocIdMap(docs);
}
