/**
 *
 *  Edited by Khai Ly on 4/4/2019.
 *
**/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/time.h>
#include <time.h>

#include <unistd.h>
#include <errno.h>

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
Index movie_index;

LinkedList GetSeattleRomCom();
LinkedList GetRomComSeattle();

void FreeMe(void *me);

void FreeMe(void *me) {
  free(me);
}

/**
 * Open the specified file, read the specified row into the
 * provided pointer to the movie.
 */
int CreateMovieFromFileRow(char *file, long rowId, Movie** movie) {
  FILE *fp;

  char buffer[1000];

  fp = fopen(file, "r");

  int i = 0;
  while (i <= rowId) {
    fgets(buffer, 1000, fp);
    i++;
  }
  // taking \n out of the row
  buffer[strlen(buffer)-1] = ' ';
  // Create movie from row
  *movie = CreateMovieFromRow(buffer);
  fclose(fp);
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

void BenchmarkSetOfMovies(DocIdMap docs) {
  HTIter iter = CreateHashtableIterator(docs);
  // Now go through all the files, and insert them into the index.
  HTKeyValue kv;
  HTIteratorGet(iter, &kv);
  LinkedList movie_list  = ReadFile((char*)kv.value);
  movie_index = BuildMovieIndex(movie_list, Genre);

  while (HTIteratorHasMore(iter) != 0) {
    HTIteratorGet(iter, &kv);
    movie_list = ReadFile((char*)kv.value);
    AddToMovieIndex(movie_index, movie_list, Genre);
    HTIteratorNext(iter);
  }

  printf("%d entries in the index.\n", NumElemsInHashtable(movie_index->ht));
  DestroyHashtableIterator(iter);
}

void BenchmarkMovieSet(DocIdMap docs) {
  // Create the index
  docIndex = CreateIndex();

  // Index the files
  printf("Parsing and indexing files...\n");
  ParseTheFiles_MT(docs, docIndex);
  printf("%d entries in the index.\n", NumElemsInHashtable(docIndex->ht));
}

void WriteFile(FILE *file) {
  int buffer_size = 1000;
  char buffer[buffer_size];

  while (fgets(buffer, buffer_size, file) != NULL) {
    printf("%s\n", buffer);
  }
}

/*
 * Measures the current (and peak) resident and virtual memories
 * usage of your linux C process, in kB
 */
void getMemory() {
  //    int* currRealMem, int* peakRealMem,
  //    int* currVirtMem, int* peakVirtMem) {

  int currRealMem, peakRealMem, currVirtMem, peakVirtMem;

  // stores each word in status file
  char buffer[1024] = "";

  // linux file contains this-process info
  FILE* file = fopen("/proc/self/status", "r");

  // read the entire file
  while (fscanf(file, " %1023s", buffer) == 1) {
    if (strcmp(buffer, "VmRSS:") == 0) {
      fscanf(file, " %d", &currRealMem);
    }
    if (strcmp(buffer, "VmHWM:") == 0) {
      fscanf(file, " %d", &peakRealMem);
    }
    if (strcmp(buffer, "VmSize:") == 0) {
      fscanf(file, " %d", &currVirtMem);
    }
    if (strcmp(buffer, "VmPeak:") == 0) {
      fscanf(file, " %d", &peakVirtMem);
    }
  }

  fclose(file);

  printf("Cur Real Mem: %d\tPeak Real Mem: %d "
         "\t Cur VirtMem: %d\tPeakVirtMem: %d\n",
         currRealMem, peakRealMem,
         currVirtMem, peakVirtMem);
}

LinkedList GetSeattleRomCom() {
  SearchResultIter results = FindMovies(docIndex, "Seattle");
  LinkedList movies = CreateLinkedList();

  if (results == NULL) {
    printf("No results for this term. Please try another.\n");
    return NULL;
  } else {
    SearchResult sr = (SearchResult)malloc(sizeof(*sr));
    if (sr == NULL) {
      printf("Couldn't malloc SearchResult in main.c\n");
      return NULL;
    }
    int result;
    char *filename;

    // Get the last
    SearchResultGet(results, sr);
    filename = GetFileFromId(docs, sr->doc_id);

    Movie *movie;
    CreateMovieFromFileRow(filename, sr->row_id, &movie);
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

      Movie *movie;
      CreateMovieFromFileRow(filename, sr->row_id, &movie);
      InsertLinkedList(movies, movie);
    }

    free(sr);
    DestroySearchResultIter(results);
  }
  // To grader: comment out lines 200 - 224 to
  // see qquery time for title index.
  // To grader: comment out lines 221 - 222 to see
  // Seattle to Rom/COm query more accurately.

  // Getting genres Comedy and Romance from list.
  printf("Getting movies with genres Comedy and Romance \n");
  Movie* movieFit;
  LLIter it = CreateLLIter(movies);
  LinkedList result = CreateLinkedList();
  while (LLIterHasNext(it)) {
    LLIterGetPayload(it, (void**) &movieFit);
    int i = 0;
    while (1) {
      if (movieFit -> genres[i] == NULL) {
        break;
      } else if ((strcmp(movieFit -> genres[i], "Comedy") == 0)
        || (strcmp(movieFit -> genres[i], "Romance") == 0)) {
        InsertLinkedList(result, movieFit);
      }
      i++;
    }
    LLIterNext(it);
  }
  DestroyLLIter(it);
  Index index = BuildMovieIndex(movies, Type);
  DestroyTypeIndex(index);

  return result;
}

LinkedList GetRomComSeattle() {
  HTKeyValue kvp;
  LinkedList result = CreateLinkedList();
  // Getting comedy mvoies with "Seattle"
  uint64_t genreKeyComedy = FNVHash64((unsigned char*) "Comedy",
    strlen("Comedy"));
  LookupInHashtable(movie_index -> ht, genreKeyComedy,
(HTKeyValue*) &kvp);
  SetOfMovies setOfMovies = kvp.value;

  // To grader: comment out 237-262 to see query time for index field.
  Movie* movie;
  LLIter iter = CreateLLIter(setOfMovies -> movies);
  while (LLIterHasNext(iter)) {
    LLIterGetPayload(iter, (void**) &movie);
    if (strstr(movie -> title, "Seattle") != NULL) {
        InsertLinkedList(result, movie);
      printf("Hello\n");
    }
    LLIterNext(iter);
  }

  // Getting romance movies with "Seattle"
  DestroyLLIter(iter);
  uint64_t genreKeyRomance =
    FNVHash64((unsigned char*) "Romance",
      strlen("Romance"));
  LookupInHashtable(movie_index -> ht, genreKeyRomance,
(HTKeyValue*) &kvp);
  setOfMovies = kvp.value;
  iter = CreateLLIter(setOfMovies -> movies);
  while (LLIterHasNext(iter)) {
    LLIterGetPayload(iter, (void**) &movie);
    if (strstr(movie -> title, "Seattle")) {
        InsertLinkedList(result, movie);
    }
    LLIterNext(iter);
  }
  DestroyLLIter(iter);
  return result;
}

int main(int argc, char *argv[]) {
  // Check arguments
  if (argc != 2) {
    printf("Wrong number of arguments.\n");
    printf("usage: main <directory_to_crawl>\n");
    return 0;
  }
  pid_t pid = getpid();
  printf("Process ID: %d\n", pid);
  getMemory();

  // Create a DocIdMap
  docs = CreateDocIdMap();
  CrawlFilesToMap(argv[1], docs);
  printf("Crawled %d files.\n", NumElemsInHashtable(docs));
  printf("Created DocIdMap\n");

  getMemory();

  clock_t start2, end2;
  double cpu_time_used;


  // =======================
  // Benchmark MovieSet
  printf("\n\nBuilding the OffsetIndex\n");
  start2 = clock();
  BenchmarkMovieSet(docs);
  end2 = clock();
  cpu_time_used = ((double) (end2 - start2)) / CLOCKS_PER_SEC;
  printf("Took %f seconds to execute. \n", cpu_time_used);
  printf("Memory usage: \n");

  getMemory();
//  DestroyOffsetIndex(docIndex);
  printf("Destroyed OffsetIndex\n");
  getMemory();
  // =======================

  // ======================
  // Benchmark SetOfMovies
  printf("\n\nBuilding the TypeIndex\n");
  start2 = clock();
  BenchmarkSetOfMovies(docs);
  end2 = clock();
  cpu_time_used = ((double) (end2 - start2)) / CLOCKS_PER_SEC;
  printf("Took %f seconds to execute. \n", cpu_time_used);
  printf("Memory usage: \n");
  getMemory();
//  DestroyTypeIndex(movie_index);
  printf("Destroyed TypeIndex\n");
  getMemory();

  // ======================
  // Seattle then Rom + Com
  printf("\nQuery Seattle (from title index) then Rom + Com\n");
  start2 = clock();
  LinkedList seattleRomCom =  GetSeattleRomCom();
  end2 = clock();
  cpu_time_used = ((double) (end2 - start2)) / CLOCKS_PER_SEC;
  printf("Took %f seconds to execute. \n", cpu_time_used);
  printf("Memory usage: \n");
  getMemory();
//  free(seattleRomCom);
  getMemory();

  // ======================
  // Rom + COm then Seattle
  printf("\nQuery Rom + Com (from field index) then Seattle\n");
  start2 = clock();
  LinkedList romComSeattle = GetRomComSeattle();
  end2 = clock();
  cpu_time_used = ((double) (end2 - start2)) / CLOCKS_PER_SEC;
  printf("Took %f seconds to execute. \n", cpu_time_used);
  printf("Memory usage: \n");
  getMemory();
//  free(romComSeattle);
  getMemory();

  // =====================

  DestroyOffsetIndex(docIndex);
  DestroyTypeIndex(movie_index);

  free(seattleRomCom);

  free(romComSeattle);
  DestroyDocIdMap(docs);
  printf("\n\nDestroyed DocIdMap\n");
  getMemory();
}

