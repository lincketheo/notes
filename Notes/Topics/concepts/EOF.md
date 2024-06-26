# EOF
The standard library returns -1 "when the end of the file is reached". How it knows how to return -1 is up to the OS / standard library. 

Deep in the source code, when you execute a `read` the OS knows that the read is "done" based on various implementation specific reasons (CTRL-D/CTRL-K/signal/file size etc) - and it returns `EOF` when that read is "done"

## Things to remember:
### ASCII 26
Ascii 26 is "[Substitute](https://en.wikipedia.org/wiki/Substitute_character)" which is used as EOF in:
- CP/M
- DOS
So many tools that encounter that byte will call EOF.

#### Takeaways:
- To aid in portability, if 26 might be in the file, read in binary mode ("rb")
- If you want your files to open portably in text mode, don’t embed Control-Z characters in them.
- If you must embed Control-Z characters, and you want the files to be read portably, open the files in binary mode.

### feof/eof
- You must execute a read first before calling these.
This is invalid:
```c
#include <stdio.h>

int main() {
    FILE * f = fopen( "afile.txt", "r" );
    char line[100];
    int n = 0;
    while( ! feof( f ) ) {
        fgets( line, 100, f );
        printf( "%d %s", ++n, line );
    }
    fclose( f );
}
```

This is valid:
```c
#include <stdio.h>

int main() {
    FILE * f = fopen( "afile.txt", "r" );
    char line[100];
    int n = 0;
    while( fgets( line, 100, f ) != NULL ) {
        printf( "%d %s", ++n, line );
    }
    fclose( f );
}
```

When would I use eof?
Check if a read produced 0 bytes for a different reason than EOF:
```c
#include <stdio.h>

int main() {
    FILE * f = fopen( "afile.txt", "r" );
    char line[100];
    int n = 0;
    while( fgets( line, 100, f ) != NULL ) {
        printf( "%d %s", ++n, line );
    }
    if(!feof(f)){
        //ERROR
    }
    fclose( f );
}
```