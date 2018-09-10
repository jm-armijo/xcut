# xcut

`xcut` is my own version of cut, but extended to replace specific fields based
on a regular eXpression pattern.

```
Usage: xcut OPTION... [FILE]...
From each FILE, replaces text on all or selected parts of lines using PATTERN,
and print all or selected parts to standard output.

Example: xcut -f 1,2 -x 's/\d/<num>/' < file.txt

Options
  -d DELIM    Use DELIM instead of SPACE for field delimiter.
  -f FIELDS   Comma separated list of fiels to print (1-index base).
  -p FIELDS   Comma separated list of fiels to apply PATTERN to. (1-index base).
  -x PATTERN  sed like Regex to be applied on all or specified parts.
  -i          Apply PATTERN to inversed -p list.
  -s          Output lines sorted in the original order.
  -h          This help.


All options are optional, except in these cases:
    If option -p is used, option -x becomes mandatory.
    If option -i is used, option -x becomes mandatory.
```

## Example


```
$ cat example.txt
1 a1 aa11a1 1a =/=
2 a2 aa22a2 1a =	=
3 a3 aa33a3 1a =.=
4 a4 aa44a4 1a =&=
5 a5 aa55a5 1a =#=
6 a6 aa66a6 1a =<=
7 a7 aa77a7 1a =\=

$ xcut -d ' ' -f 1,2,3,5 -x "s/\d/b/" -p 1 -i example.txt
1 ab aabbab =/=
7 ab aabbab =\=
2 ab aabbab =	=
3 ab aabbab =.=
5 ab aabbab =#=
4 ab aabbab =&=
6 ab aabbab =<=
```


## Installation

Download the source code and run the `make` command to compile it. Then copy the
xcut binary to your ~/bin directory.

This programme uses POSIX to validate files, so it can be compiled in machines
where it is available. Besides that, the rest of the code has been writen using
the standard C++11.

## Class Diagram




```
    +----------------+             +----------------+
    |   ArgManager   |◆------------|    Arguments   |
    +----------------+             +----------------+
            |
            |
            |
           \/
    +----------------+                 +----------------+         +----------------+
    |     Master     |◆----------------|     Worker     |◇--------|    Arguments   |
    +----------------+                 +----------------+         +----------------+
            ◆                                   ◇
            |                                   |
            |                                   |
            |                                   |
    +----------------+     +----------------+   |   +----------------+      +----------------+
    |    DataQueue   |     |    DataReader  |---+---|  DataProcessor |◆-----|      Line      |
    +----------------+     +----------------+   |   +----------------+      +----------------+
                                                |
                                       +----------------+
                                       |    DataWriter  |
                                       +----------------+
```

## Sequence Diagram

```
  +----------------+       +----------------+       +----------------+       +----------------+       +----------------+       +----------------+       +----------------+
  |      main      |       |   ArgManager   |       |     Master     |       |   DataWriter   |       |    DataReader  |       |  DataProcessor |       |      Line      |
  +----------------+       +----------------+       +----------------+       +----------------+       +----------------+       +----------------+       +----------------+
          |                        |                        |                        |                        |                        |                        |
         +-+    processArgs        |                        |                        |                        |                        |                        |
         | |---------------------►+-+                       |                        |                        |                        |                        |
         | |<---------------------| |                       |                        |                        |                        |                        |
         | |                      | |                       |                        |                        |                        |                        |
         | |    getArgs           | |                       |                        |                        |                        |                        |
         | |---------------------►| |                       |                        |                        |                        |                        |
         | |◄---------------------+-+                       |                        |                        |                        |                        |
         | |                       |                        |                        |                        |                        |                        |
         | |    startWorkers       |                        |                        |                        |                        |                        |
         | |-----------------------|----------------------►+-+                       |                        |                        |                        |
         | |                       |                       | |   start               |                        |                        |                        |
         | |                       |                       | |-----------------------|---------------------->+-+                       |                        |
         | |                       |                       | |                       |                       | |                       |                        |
         | |                       |                       | |   start               |                       | |                       |                        |
         | |                       |                       | |-----------------------|-----------------------|-|--------------------->+-+                       |
         | |                       |                       | |                       |                       | |                      | | processLines          |
         | |                       |                       | |   start               |                       | |                      | |--------+              |
         | |                       |                       | |--------------------->+-+                      | |               _______|_|________|______________|_____
         | |                       |                       | |                      | |                      | |               |loop  | |        |              |    |
         | |                       |                       | |                      | |                      | |               |      |+-+<------+              |    |
         | |                       |                       | |                      | |                      | |               |      || |  process             |    |
         | |◄----------------------|-----------------------+-+                      | |                      | |               |      || |--------------------►+-+   |
         | |                       |                        |                       | |                      | |               |      || |                     | |   |
  _______|_|_______________________|________________________|___________            | |                      | |               |      || |◄--------------------+-+   |
  |loop  | |                       |                        |          |            | |                      +-+               |      |+-+                      |    |
  |      | |    workersDone        |                        |          |            | |                       |                |______|_|_______________________|____|
  |      | |-----------------------|----------------------►+-+         |            +-+                       |                       | |                       |
  |      | |                       |                       | |  done   |             |                        |                       +-+                       |
  |      | |                       |                       | |---------|-------------|----------------------►+-+                       |                        |
  |      | |                       |                       | |         |             |                       | |                       |                        |
  |      | |                       |                       | |◄--------|-------------|-----------------------+-+                       |                        |
  |      | |                       |                       | |         |             |                        |                        |                        |
  |      | |                       |                       | |  done   |             |                        |                        |                        |
  |      | |                       |                       | |---------|-------------|------------------------|----------------------►+-+                       |
  |      | |                       |                       | |         |             |                        |                       | |                       |
  |      | |                       |                       | |◄--------|--------------------------------------------------------------+-+                       |
  |      | |                       |                       | |         |             |                        |                        |                        |
  |      | |                       |                       | |         |             |                        |                        |                        |
  |      | |                       |                       | |  done   |             |                        |                        |                        |
  |      | |                       |                       | |---------|-----------►+-+                       |                        |                        |
  |      | |                       |                       | |         |            | |                       |                        |                        |
  |      | |                       |                       | |◄--------|------------+-+                       |                        |                        |
  |      | |                       |                       | |         |             |                        |                        |                        |
  |      | |◄----------------------|-----------------------+-+         |             |                        |                        |                        |
  |      | |                       |                        |          |             |                        |                        |                        |
  |______|_|_______________________|________________________|__________|             |                        |                        |                        |
         | |                       |                        |                        |                        |                        |                        |
         +-+                       |                        |                        |                        |                        |                        |
          |                        |                        |                        |                        |                        |                        |

```
