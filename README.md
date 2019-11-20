# DiscreteMath_HW2
#### 21800147 Kim YouYoung


# DNF converter
DNF converter is a program which converts the propositional formula to disjunctive normal form. 

#### Build & Execute(+input file)
      $ gcc dnf_converter.c -o dnf
      $ ./dnf < input
#### Input File
      (or a1 (not (or (not (ot a2 a3)) a4)))
#### Output 
      1 
      2 -4
      3 -4
      0
      -1 2 3 -4
      
# Makefile
    $ make
    gcc -c -o dnf_converter.o dnf_converter.c
    gcc -o dnf dnf_converter.o
    $ ./dnf < input
    
* If you want to delete the object file and the executable file

      $ make clean
    
