/*
 * 2D ASCII Graphics Editor
 * Uses a 2D array of characters to store and display a picture.
 * Supports: circle, rectangle, line, triangle
 * You can add, delete, and modify objects.
 */
 
#include <stdio.h>
#include <math.h>
#include <string.h>
 
/* ──────────────────────────────────────────
   CONSTANTS
   ────────────────────────────────────────── */
 
#define ROWS 24
#define COLS 80
#define MAX_OBJECTS 50
#define EMPTY ' '
 
/* Shape type codes */
#define SHAPE_RECT     1
#define SHAPE_CIRCLE   2
#define SHAPE_LINE     3
#define SHAPE_TRIANGLE 4


/* ──────────────────────────────────────────
   DATA STRUCTURES
   ────────────────────────────────────────── */
 
/* One object in the picture */
struct Object {
    int  type;      /* SHAPE_RECT, SHAPE_CIRCLE, etc. */
    int  x, y;      /* top-left corner or center      */
    int  w, h;      /* width and height (rect/tri)    */
    int  radius;    /* radius (circle)                */
    int  x2, y2;   /* end point (line)               */
    char ch;        /* character used to draw: * or _ */
    int  active;    /* 1 = exists, 0 = deleted        */
};
 
/* Global picture grid */
char grid[ROWS][COLS];
 
/* Global list of objects */
struct Object objects[MAX_OBJECTS];
int object_count = 0;
 
/* ──────────────────────────────────────────
   GRID FUNCTIONS
   ────────────────────────────────────────── */
 
 
