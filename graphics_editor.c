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
 
