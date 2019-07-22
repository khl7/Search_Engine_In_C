/*
 *  Created by Adrienne Slaughter
 *  CS 5007 Spring 2019
 *  Northeastern University, Seattle
 *
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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>

#include "FileCrawler.h"
#include "DocIdMap.h"
#include "LinkedList.h"

/**
  void CrawlFilesToMap(const char *dir, DocIdMap map) {
  struct stat s;

  struct dirent **namelist;
  int n;
  n = scandir(dir, &namelist, 0, alphasort);
// TODO: use namelist to find all the files and put them in map.
// NOTE: There may be nested folders.
// Be sure to lookup how scandir works. Don't forget about memory use.
if (n < 0) {
perror("Scandir error");
return;
}
for (int i = 0; i < n; i++) {

if (strcmp(namelist[i] -> d_name, ".") != 0
&& strcmp(namelist[i] -> d_name, "..") != 0) {
char temp[strlen(namelist[i] -> d_name
+ strlen(dir) + 2)];
//      strcpy(temp, dir);
//      strcat(temp, namelist[i] -> d_name);
//      strcat(temp, "/");
//    char* temp = malloc(((
//          strlen(namelist[i] -> d_name)
//        + strlen(dir)) + 2 ) * sizeof(char));
sprintf(temp, "%s/%s", dir, namelist[i] -> d_name);

if (stat(temp, &s) == 0 && S_ISDIR(s.st_mode)) {
CrawlFilesToMap(temp, map);
} else {
char* newTemp = strdup(temp);
PutFileInMap(temp, map);
}
}
}
}
 **/

void CrawlFilesToMap(const char *dir, DocIdMap map) {
  struct stat s;

  struct dirent **namelist;
  int n;
  n = scandir(dir, &namelist, 0, alphasort);
  // TODO: use namelist to find all the files and put them in map.
  // NOTE: There may be nested folders.
  // Be sure to lookup how scandir works. Don't forget about memory use.
  if (n < 0) {
    perror("Scandir error");
    return;
  }
  for (int i = 0; i < n; i++) {
    if (strcmp(namelist[i] -> d_name, ".") != 0
        && strcmp(namelist[i] -> d_name, "..") != 0) {

      char* temp = malloc(((
              strlen(namelist[i] -> d_name)
              + strlen(dir)) + 2 ) * sizeof(char));
      sprintf(temp, "%s/%s", dir, namelist[i] -> d_name);
      stat(temp, &s);
      if (S_ISDIR(s.st_mode)) {
        CrawlFilesToMap(temp, map);
        free(temp);
      } else {
        PutFileInMap(temp, map);
      }

    }
  }
  for (int i = 0; i < n; i++) {
    free(namelist[i]);
  }
  free(namelist);
}

