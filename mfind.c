/*  Name: Blaize K. Rodrigues
 *  Subject: CS370 Project#4
 *  Date: December 12th, 2018
 *  Decription: Implementation of an mfind program that uses strnstr to find the
 *  location of a string in a file using mmap();
 *
 *  Copyright (c) 2018, Jisoo Yang
 *
 *  This is written as part of CS 370 project
 *  
 *  Writen by Jisoo Yang <jisoo.yang@unlv.edu>  
 */

#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <fcntl.h>	// for open()
#include <sys/stat.h>	// for fstat()
#include <sys/mman.h>	// for mmap()


/* below copyright notice for BSD strnstr.c implemenentation */
/*-
 * Copyright (c) 2001 Mike Barcroft <mike@FreeBSD.org>
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* 
 * strnstr()
 * Find the first occurrence of find in s, where the search is limited to the
 * first slen characters of s.
 */
char* strnstr(const char* s, const char* find, size_t slen)
{
    char c, sc;
    size_t len;

    if ((c = *find++) != '\0') {
	len = strlen(find);
	do {
	    do {
		if (slen-- < 1 || (sc = *s++) == '\0')
		    return (NULL);
	    } while (sc != c);
	    if (len > slen)
		return (NULL);
	} while (strncmp(s, find, len) != 0);
	s--;
    }
    return ((char *)s);
}

/*
 * Your task is to implement a program (mfind) which locates a string
 * in a file using mmap(). Its command line syntax is as follows:
 *
 *  mfind <filename> <string>
 *
 * For example, if user executes '$ mfind /var/log/dmesg SELinux', 
 * 'SELinux' is searched from /var/log/dmesg file and if found, it prints out
 * "string SELinux found at offset 8671" or if not found, it prints out
 * "string SELinux not found in file"
 * 
 * You have to implement this program using mmap() - you need to search
 * the string using memory file map instead of read() system call.
 *
 * By using mmap(), you don't have to allocate memory buffer to store
 * the file content if you use read() - mmap() just creates region of 
 * virtual address from which you can directly access the content of the file.
 *
 * The actual string search part should be done using supplied strnstr()
 * function. (strnstr() is the same as the standard strstr() except that it
 * checks the length of the source. check man page of strstr for details)
 *
 * The mmap() call requires many parameters. Part of your task is to figure
 * out the correct set of parameters by studying man pages, internet search,
 * looking up POSIX mmap() man pages written by OPEN group, and experimentation.
 * 
 * mmap() needs you to specify the size of the memory map via second parameter.
 * You may set this value equal to the size of the file. fstat() is the call
 * you should use to query information of a file. Again, it is your task to 
 * figure out how to use fstat() library call.
 * 
 * In C, argv[] contains the command line arguments as null-terminated strings.
 * argc contains the number of arguments including the command itself. 
 * For example, if one executes 
 *
 *  $ ./mfind ./exactly_4k.txt SEL
 *
 * then 
 * argv[0] is a pointer to string "./mfind",
 * argv[1] is a pointer to string "./exactly_4k.txt", 
 * argv[2] is a pointer to string "SEL" and
 * argv[3] is NULL.
 */


int main(int argc, char** argv)
{
    int fd;
    struct stat st;
    size_t file_size;
    char *filemap_addr, *found;
    const char *str_to_find;
    
    if (argc != 3) {
	fprintf(stderr, "command syntax: mfind <filename> <string>\n");
	exit(EXIT_FAILURE);
    }
    str_to_find = argv[2];

    fd = open(argv[1], O_RDONLY, 0); //set the file descriptor
    if(fd < 0){
        printf("Error: Invalid File Descriptor\n");//error check 
    }
    fstat(fd, &st); // get the statistics of the file
    file_size = st.st_size; 
    
    filemap_addr = mmap(0, file_size, PROT_READ, MAP_PRIVATE, fd, 0);//use of mmap
    if(filemap_addr == MAP_FAILED){
        printf("Error: MMAP\n");
    }
    found =  strnstr(filemap_addr,str_to_find, file_size);
    int offset = (found - filemap_addr); // the location where the string was found
    printf("string %s found at offset %d\n", str_to_find, offset);
    munmap(filemap_addr, file_size); 
    close(fd);//close the file descriptor

    exit(EXIT_SUCCESS);

    return 0;
}

