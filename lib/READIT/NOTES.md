In this folder there is Testing files and output images 

This folder is a temporary folder will be removed but it for some clarification 
First of all, i made code for only uint8_t but it didnt work cuz addition will be over 255 
for horizontal pass then for vertical pass so the only way to solve it is to enlarge the vector into u16 
it worked well then i tried to make code for u16 and u32 without enlarging but it didnt worked then i tried to 
enlarge u16 into u32 when adding and also u32 into u64 but it gives same output. 
I tried to debug and find this problem but the code worked on vectors up to 20x20 correctly for every datatype but on real image just only
u8 gives correct output , Just still trying to solve this problem 
Then implemented it with templates and work correctly but i modified some template issues in ImageRead and ImageWrite to work properly

Added Shell function for downloading toolchain,spike and pk for getting RV64 in easy way to be simulated 

```
source install_riscv.sh
```

