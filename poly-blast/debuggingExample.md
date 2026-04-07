# Debugging Example

## Context
Finished implementing new algorithm to select shapes.  
Went to test it.  

In the terminal, in `block-blast` folder.  
Execute usual compilation command:  
`clear; make MODE=clang-debug rebuild run-main`  

`clear`: clears the terminal  
`make`: invoke the `Makefile` of the project  
`MODE=clang-debug`: custom build mode that makes use of `clang` to compile + `strict-mode`'s debug flags and some Sanitizer flags to detect issues like memory leaks, undefined behaviors, ...  
`rebuild`: custom target that forces the compilation of the entire project  
`run-main`: custom target that runs the compiled executable  

## Example 1
Then after having placed the three shapes, the new algorithm triggers, undefined behavior happen and the Sanitizer triggers and in the termial appear:  
```bash
src/core/game.c:153:64: runtime error: member access within null pointer of type 'const Prefab_St'
SUMMARY: UndefinedBehaviorSanitizer: undefined-behavior src/core/game.c:153:64 
src/core/game.c:153:64: runtime error: load of null pointer of type 'const u8' (aka 'const unsigned char')
SUMMARY: UndefinedBehaviorSanitizer: undefined-behavior src/core/game.c:153:64 
AddressSanitizer:DEADLYSIGNAL
=================================================================
==45588==ERROR: AddressSanitizer: SEGV on unknown address 0x000000000000 (pc 0x55c58a548dd1 bp 0x7ffe6f4a4db0 sp 0x7ffe6f4a4680 T0)
==45588==The signal is caused by a READ memory access.
==45588==Hint: address points to the zero page.
    #0 0x55c58a548dd1 in adjustSizeWeights Multi-Mini-Games/block-blast/src/core/game.c:153:27
    #1 0x55c58a5546a8 in main Multi-Mini-Games/block-blast/src/main.c:40:13
    #2 0x7fd32fd256c0 in __libc_start_call_main /usr/src/debug/glibc/glibc/csu/../sysdeps/nptl/libc_start_call_main.h:59:16
    #3 0x7fd32fd257f8 in __libc_start_main /usr/src/debug/glibc/glibc/csu/../csu/libc-start.c:360:3
    #4 0x55c58a3df4d4 in _start (Multi-Mini-Games/block-blast/build/bin/main+0x694d4) (BuildId: 150acd5c66d89911e04bd32f24dbf36300c80ff8)

==45588==Register values:
rax = 0x0000000000000000  rbx = 0x00007ffe6f4a4680  rcx = 0x00007ffe6f4a4300  rdx = 0x0000000000000000  
rdi = 0x0000000000000000  rsi = 0x0000000000000000  rbp = 0x00007ffe6f4a4db0  rsp = 0x00007ffe6f4a4680  
 r8 = 0x00007ffe6f4a4368   r9 = 0x000055c58a6a3840  r10 = 0x000055c58a696000  r11 = 0x0000000000000206  
r12 = 0x00007ffe6f4a5028  r13 = 0x0000000000000001  r14 = 0x00007fd3300b9000  r15 = 0x000055c58a66ecb0  
AddressSanitizer can not provide additional info.
SUMMARY: AddressSanitizer: SEGV Multi-Mini-Games/block-blast/src/core/game.c:153:27 in adjustSizeWeights
==45588==ABORTING
```  

By now I get there's an issue. So I compile using a different command to invoke gdb:  
`clear; make MODE=strict-debug rebuild run-gdb`  

Note:
- The compilation mode changes because `clang` and `gdb` do go very well together. gdb allows to step through the execution of the program, but when the undefined behavior and ultimately the SIGSEGV trigger, debugging the rest of the program becomes impossible, `clang` output its summary and force exit the program.  
Meaning that `gdb` can't intercep the signal and stop execution for the programmer to debug the program, hence they are incompatible together.
- The running target also changed, went from `run-main` to `run-gdb`. Here it's also a custom target to simplify the execution workflow. Simply, rather than running the program as is, it runs it into `gdb`.

Running the command, and entering gdb.  
Input `r` and press `enter`, to run the program.  
-> it immediately run into a SIGSEGV:
```bash
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/usr/lib/libthread_db.so.1".
[New Thread 0x7fffe7fff6c0 (LWP 46756)]
[New Thread 0x7fffe77fe6c0 (LWP 46757)]
[New Thread 0x7fffe6f7d6c0 (LWP 46758)]
[New Thread 0x7fffe677c6c0 (LWP 46759)]

Thread 1 "main" received signal SIGSEGV, Segmentation fault.
0x0000555555581eec in initPrefabManager (manager=0x7fffffffdc90) at src/setups/game.c:82
82              da_append(&manager->bags[size_idx], i);
(gdb) 
```

To have more information about where that line is executed in the stack frame:  
Input `bt` (or `backtrace`) and press `enter`:  
```bash
Thread 1 "main" received signal SIGSEGV, Segmentation fault.
0x0000555555581eec in initPrefabManager (manager=0x7fffffffdc90) at src/setups/game.c:82
82              da_append(&manager->bags[size_idx], i);
(gdb) bt
#0  0x0000555555581eec in initPrefabManager (manager=0x7fffffffdc90) at src/setups/game.c:82
#1  0x000055555558217e in initGame (game=0x7fffffffdba0) at src/setups/game.c:105
#2  0x0000555555582c31 in main () at src/main.c:27
(gdb) 
```

Great, it crashes right at initialization...  
```bash
(gdb) p manager
$1 = (PrefabManager_St * const) 0x7fffffffdc90
```

Okay, the pointer to the `manager` isn't the issue. Note: I could have got that from the call of the function `initPrefabManager (manager=0x7fffffffdc90)`  

Let's see it's content, and since it's a pointer to `PrefabManager_St` then it requires to be deferenced:
```bash
(gdb) p *manager
$2 = {bags = {{items = 0x34000000340, count = 6047313953665, capacity = 10445360466304}, {items = 0x98000000980, 
      count = 10445360466304, capacity = 10445360466304}, {items = 0x98000000980, count = 10445360466304, capacity = 240}, {
      items = 0xc5, count = 0, capacity = 0}, {items = 0x0, count = 140737353992049, capacity = 0}, {items = 0x0, 
      count = 4194304, capacity = 12884901915}, {items = 0x4, count = 74489, capacity = 1}, {items = 0x2, count = 64, 
      capacity = 140737488346656}, {items = 0x7fffffffde60, count = 140737353995800, capacity = 17415141758693539842}}, slots = {
    {prefab = 0x14, center = {x = 8.96831017e-44, y = 0}, placed = false, colorIndex = BLOCK_COLOR_RED, dragging = 255, 
      id = 255 '\377'}, {prefab = 0x140000, center = {x = 1.12103877e-44, y = 0}, placed = 64, colorIndex = BLOCK_COLOR_RED, 
      dragging = 12, id = 0 '\000'}, {prefab = 0xc000, center = {x = 3.58732407e-43, y = 0}, placed = 64, 
      colorIndex = BLOCK_COLOR_RED, dragging = false, id = 0 '\000'}}, sizeWeights = {baseWeights = {0.0500000007, 0.200000003, 
      0.25, 0.25, 0.100000001, 0.0500000007, 0, 0, 0.100000001}, runTimeWeights = {0.0500000007, 0.200000003, 0.25, 0.25, 
      0.100000001, 0.0500000007, 0, 0, 0.100000001}}}
```

That's a lot, but we now that it's not corrupted.  
But actually, from the line where it broke, the information that we search is about `manager->bags`.  
From the codebase, it's an array of `u32` (simply put, it's an `unsigned int`).  
Since it's an array, maybe the index is wrong:  
```bash
(gdb) p size_idx
$3 = 0 '\000'
```

Nope, so let's took a look closer at `manager->bags`:  
```bash
(gdb) p manager->bags
$4 = {{items = 0x34000000340, count = 6047313953665, capacity = 10445360466304}, {items = 0x98000000980, count = 10445360466304, 
    capacity = 10445360466304}, {items = 0x98000000980, count = 10445360466304, capacity = 240}, {items = 0xc5, count = 0, 
    capacity = 0}, {items = 0x0, count = 140737353992049, capacity = 0}, {items = 0x0, count = 4194304, capacity = 12884901915}, 
  {items = 0x4, count = 74489, capacity = 1}, {items = 0x2, count = 64, capacity = 140737488346656}, {items = 0x7fffffffde60, 
    count = 140737353995800, capacity = 17415141758693539842}}
```

Well the issue, seems clear, since it's at initialization, it meant one thing, it's being access while being full of garbage values.  

Searching through the codebase, going up the stack frame, I find the culprit: `GameState_St game;`  
I define the game without zero-initializing it.  

## Example 2

With the first issue fix, it's now time to quit `gdb`:
`q` (or `quit`). And confirm.  
Recompile the project using the same command:  
`clear; make MODE=strict-debug rebuild run-main`  

Enter `gdb`, input `r` and press `enter`;  
The program program runs normally, no issue at first.  
Coming back to where we where. Placing the first generated shape, triggers the new generation algorithm.  
And indeed, it SIGSEGV:  
```bash
[New Thread 0x7fffe7fff6c0 (LWP 48013)]
[New Thread 0x7fffe77fe6c0 (LWP 48014)]
[New Thread 0x7fffe6f7d6c0 (LWP 48015)]
[New Thread 0x7fffe677c6c0 (LWP 48016)]

Thread 1 "main" received signal SIGSEGV, Segmentation fault.
adjustSizeWeights (game=0x7fffffffdba0, scoreDelta=0) at src/core/game.c:153
153             placementScore += game->prefabManager.slots[i].prefab->blockCount * SCORE_PER_UNIT_PLACED;
(gdb)
```

Already, something is weird:  
In the callsite, the function received `0` for the `scoreDelta`, but the game just only launched and after having placed the three shapes, the game UI indicates a score of 70 points.  
We can verify that:  
```bash
(gdb) p game.scoring.score
$1 = 70
```

We do get 70. so it's very weird.  

Next step: setting a `breakpoint`:  
1. Get the stack trace:  
```bash
(gdb) bt
#0  adjustSizeWeights (game=0x7fffffffdba0, scoreDelta=0) at src/core/game.c:153
#1  0x0000555555582d71 in main () at src/main.c:40
```

2. Now we're know where `adjustSizeWeights` has been called, let's set a breakpoint there:  
```bash
(gdb) b main.c:40
Breakpoint 1 at 0x555555582bc6: file src/main.c, line 40.
```

3. Now let's re-run the program:  
`r`, then confirm:  
```bash
[New Thread 0x7fffe7fff6c0 (LWP 49202)]
[New Thread 0x7fffe77fe6c0 (LWP 49203)]
[New Thread 0x7fffe6f7d6c0 (LWP 49204)]
[New Thread 0x7fffe677c6c0 (LWP 49205)]

Thread 1 "main" hit Breakpoint 2, main () at src/main.c:40
40                  adjustSizeWeights(&game, game.scoring.score - prevScore);
```

4. Now that were just before the call, we can query the value of `prevScore`:  
```bash
(gdb) p prevScore
$3 = 0
```

4.1. Curious? What about `game.scoring.score`:  
```bash
(gdb) p game.scoring.score
$4 = 130
```

Note: The game window has stop rendering, so the shape that is being placed is frozen and the score text rendering hasn't had time to update, hence we can still see `110` rather than `130`.  

5. Okay, so everything seems fine, so let's go deeper:  
```
(gdb) s
adjustSizeWeights (game=0x7fffffffdba0, scoreDelta=130) at src/core/game.c:108
108     void adjustSizeWeights(GameState_St* const game, const f32 scoreDelta) {
```

Using `s` (or `step`) to step into the function.  
Note: using `n` (or `next`) would have executed the function without entering like `step` do.  

We can clearly see that in the function call it received `scoreDelta=130`.  
Let's continue execution until it crashes again:  
```bash
(gdb) c
Continuing.

Thread 1 "main" hit Breakpoint 2, main () at src/main.c:40
40                  adjustSizeWeights(&game, game.scoring.score - prevScore);
```

Curious again ? A frame has passed and it triggered the function again. But why ?  
Checking the algorithm and comparing it to the previous used and still used function, it revealed a few flaws in the algorithm...