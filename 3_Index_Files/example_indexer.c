/*
 *  Created by Adrienne Slaughter
 *  CS 5007 Spring 2019
 *  Northeastern University, Seattle
 *
 *  Edited by Khai Ly on 3/31/2019.
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
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#include "htll/LinkedList.h"
#include "MovieIndex.h"
#include "Movie.h"
#include "MovieReport.h"
#include "FileParser.h"


int main(int argc, char* argv[]) {

  // TODO: Parse command-line args to index based on the correct field (see getopt)

  LinkedList movie_list  = ReadFile(argv[2]);
  enum IndexField field;
  char* type;
//  char* haystack[100] = argv[1];
  if (strcmp(argv[1], "-t") == 0) {
    field = Type;
    type = "type: t";
  } else if (strcmp(argv[1], "-y") == 0) {
    field = Year;
    type = "year: y";
  } else if ((strcmp(argv[1], "-g") == 0)
    || (strcmp(argv[1], "-") == 0)) {
    field = Genre;
    type = "genre: g";
  } else {
    printf("More than one flags is provided\n\n");
    return -1;
  }

  printf("\nsorting by %s\n\n", type);
  Index index = BuildMovieIndex(movie_list, field);

  // TODO: Output report to file, rather than terminal (see MovieReport.h)
  PrintReport(index);

  DestroyTypeIndex(index);

  return 0;
}

