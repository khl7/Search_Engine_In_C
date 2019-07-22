/*
 *  Created by Adrienne Slaughter
 *  CS 5007 Spring 2019
 *  Northeastern University, Seattle
 *
 *  Edited by Khai Ly on 4/4/2019
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  See <http://www.gnu.org/licenses/>.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include "MovieIndex.h"
#include "FileParser.h"
#include "Movie.h"
#include "DocIdMap.h"
#include "MovieSet.h"
//  Only for NullFree; TODO(adrienne): NullFree should live somewhere else.

#define BUFFER_SIZE 1000

//=======================
// To minimize the number of files we have, I'm
// putting the private function prototypes for
// the fileparser here.

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t n = PTHREAD_MUTEX_INITIALIZER;

HTIter it;
int flag = 0;

void IndexTheFile(char *file, uint64_t docId, Index index);
void* IndexTheFile_MT(void* index);
void* IndexTheFile_MT(void* index) {
  FILE *cfPtr;
  HTKeyValue kv;
  pthread_mutex_lock(&m);
  while (HTIteratorGet(it, &kv) == 0
      && flag == 0) {
    if (HTIteratorHasMore(it) != 0) {
      HTIteratorNext(it);
    } else {
      flag = 1;
    }
    pthread_mutex_unlock(&m);
    if ((cfPtr = fopen(kv.value, "r")) == NULL) {
      printf("File could not be opened\n");
      return NULL;
    } else {
      char buffer[BUFFER_SIZE];
      int row = 0;
      while (fgets(buffer, BUFFER_SIZE, cfPtr) != NULL) {
        Movie *movie = CreateMovieFromRow(buffer);
        if (movie == NULL) {
          continue;
        }
        pthread_mutex_lock(&n);
        int result = AddMovieTitleToIndex(index, movie, kv.key, row);
        pthread_mutex_unlock(&n);
        if (result < 0) {
          fprintf(stderr, "Didn't add MovieToIndex.\n");
        }
        row++;
        DestroyMovie(movie);  // Done with this now
      }
      fclose(cfPtr);
    }
    pthread_mutex_lock(&m);
    if (flag == 1) {
      pthread_mutex_unlock(&m);
    }
  }

  pthread_mutex_unlock(&m);
  return NULL;
}

int ParseTheFiles_MT(DocIdMap docs, Index index) {
  pthread_t tid1, tid2, tid3, tid4, tid5;
  it = CreateHashtableIterator(docs);

  pthread_create(&tid1, NULL, IndexTheFile_MT, (void*) index);
  pthread_create(&tid2, NULL, IndexTheFile_MT, (void*) index);
  pthread_create(&tid3, NULL, IndexTheFile_MT, (void*) index);
  pthread_create(&tid4, NULL, IndexTheFile_MT, (void*) index);
  pthread_create(&tid5, NULL, IndexTheFile_MT, (void*) index);

  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
  pthread_join(tid3, NULL);
  pthread_join(tid4, NULL);
  pthread_join(tid5, NULL);


  DestroyHashtableIterator(it);

  return 0;
}

/**
 * \fn Parses the files that are in a provided DocIdMap.
 * Builds an OffsetIndex
 */
int ParseTheFiles(DocIdMap docs, Index index) {
  HTIter iter = CreateHashtableIterator(docs);
  HTKeyValue kv;

  HTIteratorGet(iter, &kv);
  IndexTheFile(kv.value, kv.key, index);
  // Just do the while looop here
  while (HTIteratorHasMore(iter) != 0) {
    HTIteratorGet(iter, &kv);
    IndexTheFile(kv.value, kv.key, index);
    HTIteratorNext(iter);
  }

  HTIteratorGet(iter, &kv);
  IndexTheFile(kv.value, kv.key, index);

  DestroyHashtableIterator(iter);

  return 0;
}

// Builds an OffsetIndex
void IndexTheFile(char *file, uint64_t doc_id, Index index) {
  FILE *cfPtr;

  if ((cfPtr = fopen(file, "r")) == NULL) {
    printf("File could not be opened\n");
    return;
  } else {
    char buffer[BUFFER_SIZE];
    int row = 0;

    while (fgets(buffer, BUFFER_SIZE, cfPtr) != NULL) {
      Movie *movie = CreateMovieFromRow(buffer);
      if (movie == NULL) {
        continue;
      }
      int result = AddMovieTitleToIndex(index, movie, doc_id, row);
      if (result < 0) {
        fprintf(stderr, "Didn't add MovieToIndex.\n");
      }
      row++;
      DestroyMovie(movie);  // Done with this now
    }
    fclose(cfPtr);
  }
}

// Takes a linkedlist of movies, and builds a hashtable based on the given field
// Builds a TypeIndex
Index BuildMovieIndex(LinkedList movies, enum IndexField field_to_index) {
  Index movie_index = CreateIndex();
  movie_index->movies = movies;

  LLIter iter = CreateLLIter(movies);
  Movie* cur_movie;
  LLIterGetPayload(iter, (void**)&cur_movie);

  AddMovieToIndex(movie_index, cur_movie, field_to_index);

  while (LLIterHasNext(iter)) {
    LLIterNext(iter);
    LLIterGetPayload(iter, (void**)&cur_movie);
    AddMovieToIndex(movie_index, cur_movie, field_to_index);
  }

  DestroyLLIter(iter);

  return movie_index;
}


// Takes a linkedlist of movies, and builds a hashtable based on the given field
// Adds a movie to a TypeIndex
Index AddToMovieIndex(Index movie_index,
    LinkedList movies,
    enum IndexField field_to_index) {
  LLIter iter = CreateLLIter(movies);
  Movie* cur_movie;
  LLIterGetPayload(iter, (void**)&cur_movie);

  AddMovieToIndex(movie_index, cur_movie, field_to_index);
  InsertLinkedList(movie_index->movies, cur_movie);

  while (LLIterHasNext(iter)) {
    LLIterNext(iter);
    LLIterGetPayload(iter, (void**)&cur_movie);
    AddMovieToIndex(movie_index, cur_movie, field_to_index);
    InsertLinkedList(movie_index->movies, cur_movie);
  }

  DestroyLLIter(iter);

  //  NullFree because the payloads now live in the HT
  DestroyLinkedList(movies, NullFree);

  return movie_index;
}

// Returns a LinkedList of Movie structs from the specified file
LinkedList ReadFile(const char* filename) {
  FILE *cfPtr;

  LinkedList movie_list = CreateLinkedList();

  if ((cfPtr = fopen(filename, "r")) == NULL) {
    printf("File could not be opened\n");
    DestroyLinkedList(movie_list, NULL);
    return NULL;
  } else {
    char row[BUFFER_SIZE];

    int i = 0;

    while (!feof(cfPtr)) {
      fgets(row, BUFFER_SIZE, cfPtr);
      i++;
      // Got the line; create a movie from it
      MoviePtr movie = CreateMovieFromRow(row);
      if (movie != NULL) {
        InsertLinkedList(movie_list, movie);
      }
    }
    fclose(cfPtr);
  }
  return movie_list;
}
