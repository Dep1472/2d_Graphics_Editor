# 2D ASCII Graphics Editor

## Overview

This project is a simple 2D graphics editor written in C. It allows users to create and manage basic geometric shapes using ASCII characters (`*` and `_`) on a text-based canvas.

The editor stores all shapes in memory and redraws the entire picture whenever an object is added, modified, or deleted.

## Features

* Draw Rectangles
* Draw Circles
* Draw Lines
* Draw Triangles
* Add Objects
* Delete Objects
* Modify Objects
* Display the Picture
* Store the picture using a 2D character array

---

## Program Structure

The program is divided into several sections:

### 1. Constants

The program uses constants to define the canvas size and shape types.

```c
#define ROWS 24
#define COLS 80
#define MAX_OBJECTS 50
```

These values determine the size of the drawing area and the maximum number of objects that can be stored.

---

### 2. Object Structure

Each shape is stored using a structure.

```c
struct Object
{
    int type;
    int x, y;
    int w, h;
    int radius;
    int x2, y2;
    char ch;
    int active;
};
```

The structure stores all information required to recreate a shape.

For example:

* Rectangle → position, width, height
* Circle → center coordinates and radius
* Line → start and end points
* Triangle → position, width and height

---

### 3. Grid Storage

The picture is stored using a 2D character array.

```c
char grid[ROWS][COLS];
```

Each cell contains a character.

Example:

```text
********
*      *
********
```

The grid represents the current picture displayed to the user.

---

### 4. Drawing Functions

The program contains separate functions for each shape.

#### draw_rectangle()

Draws the borders of a rectangle using the selected character.

#### draw_circle()

Draws a circle using distance calculations from the center point.

#### draw_line()

Draws a line between two points using Bresenham's Line Algorithm.

#### draw_triangle()

Draws a triangle using a top vertex and expanding side edges.

---

### 5. Grid Management

#### clear_grid()

Resets every cell in the canvas to a blank space.

#### set_cell()

Places a character safely into the grid while checking boundaries.

#### rebuild_grid()

Redraws every active object onto the grid.

This function is called whenever an object is added, modified, or deleted.

---

### 6. Object Management

#### add_object()

Adds a new shape to the object list.

#### delete_object()

Marks an object as inactive.

#### modify_object()

Replaces an existing object with updated values.

#### list_objects()

Displays all active objects and their properties.

---

### 7. User Interface

The program uses a menu-driven interface.

Options available:

1. Add Rectangle
2. Add Circle
3. Add Line
4. Add Triangle
5. Delete Object
6. Modify Object
7. List Objects
8. Display Picture
9. Quit

---

## Program Workflow

1. User selects an operation from the menu.
2. Shape information is stored in the object list.
3. The grid is cleared.
4. All active objects are redrawn.
5. The updated picture is displayed.

This approach ensures that deleted and modified objects are handled correctly.

---

## Compilation

Compile using GCC:

```bash
gcc main.c -o graphics_editor -lm
```

The `-lm` flag links the math library required for circle calculations.

---

## Running the Program

```bash
./graphics_editor
```

---

## Concepts Used

This project demonstrates:

* Functions
* Arrays
* 2D Arrays
* Structures
* Loops
* Conditional Statements
* User Input and Output
* Basic Computer Graphics Algorithms
* Object Management

---

## Conclusion

This project implements a basic ASCII graphics editor that allows users to create, modify, delete, and display geometric shapes using a character-based canvas. The design separates object storage from picture rendering, making the program easier to manage and extend.
