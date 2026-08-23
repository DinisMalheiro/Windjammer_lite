## Session 1 – 17/08/2026 

**Todays implementations:**
    1- Project setup
    2- Character controls and basic movement using a sprite sheet
    3- State machine for character animations
    4- Animated background and placement of prop art
    5- Created: main, background & player

**Dificulties:**
    1- Understanding the libraries and how to use their functions
    2- Debugging small syntax and code errors
    3- Organizing the animation system

**Todo next:** 
    1- Add map collisions
    2- Create disk properties for implementation

**What whas writen:** 
    1- Approximately 350 lines of code



## Session 2 – 20/08/2026 

**Todays implementations:**
    1- Map collisions / player movement boundaries
    2- Created the disk system with its own disk.c and disk.h
    3- Added player disk possession (hasDisk)
    4- Added disk throwing mechanics and throw direction restrictions
    5- Separated the game logic from main.c by creating game.c and game.h

**Dificulties:**
    1- Organizing the game logic between main, game, player, and disk
    2- Managing the player's disk state and making sure the player and disk stay synchronized
    3- Debugging C errors such as duplicated functions and missing struct fields/function declarations

**Todo next:** 
    1- Improve the disk throwing/flight system and implement disk collision
    2- Add the second player and player-specific controls

**What whas writen:** 
    1- Approximately 300 lines of code


## Session 3 – 21/08/2026 

**Todays implementations:**
    1- Implemented disk collision with the top and bottom map walls, causing the disk to ricochet
    2- Added left and right goal detection with three adjustable goal zones, including a larger middle zone
    3- Implemented goal behavior, including temporary right-side ricochet for debugging and left-side disk return
    4- Added disk catch detection with an adjustable player catch radius
    5- Added throw delay logic so the player cannot immediately catch the disk after throwing, while keeping player movement locked when holding the disk

**Dificulties:**
    1- Managing the different disk states between holding, throwing, bouncing, and catching
    2- Organizing goal collision zones and keeping the collision behavior easy to adjust for future gameplay changes

**Todo next:** 
    1- Create the CPU AI
    2- Implement basic CPU movement, throwing, and catching behavior

**What whas writen:** 
    1- Approximately 250 lines of code



## Session 4 – 23/08/2026 

**Todays implementations:**
    1- Added basic CPU opponent with restricted movement area
    2- Added CPU movement, disk catching, throwing, and throw delays
    3- Added CPU animation system with mirrored sprite rendering
    4- Added five throw directions for both player and CPU using different throw angles
    5- Added score variables and game countdown timer
    6- Added point logic so the player who loses a point receives the disk
    7- Removed the temporary enemy-side disk ricochet debug behavior

**Dificulties:**
    1- Cpu logic implementation

**Todo next:** 
    1- menus
    2- ui

**What whas writen:** 
    1- Approximately 400 lines of code

