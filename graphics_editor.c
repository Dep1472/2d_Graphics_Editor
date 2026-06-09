/*
 * 2D ASCII Graphics Editor
 * Uses a 2D array of characters to store and display a picture.
 * Supports: circle, rectangle, line, triangle
 * You can add, delete, and modify objects.
 */
 
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
 
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
 
/* Fill every cell of the grid with a space */
void clear_grid(void)
{
    int r, c;
    for (r = 0; r < ROWS; r++)
        for (c = 0; c < COLS; c++)
            grid[r][c] = EMPTY;
}
 
/* Print every row of the grid to the screen */
void display_picture(void)
{
    int r, c;
    /* Top border */
    printf("+");
    for (c = 0; c < COLS; c++) printf("-");
    printf("+\n");
 
    for (r = 0; r < ROWS; r++) {
        printf("|");
        for (c = 0; c < COLS; c++)
            printf("%c", grid[r][c]);
        printf("|\n");
    }
 
    /* Bottom border */
    printf("+");
    for (c = 0; c < COLS; c++) printf("-");
    printf("+\n");
}
 
/* Safely place one character in the grid (bounds check) */
void set_cell(int x, int y, char ch)
{
    if (x >= 0 && x < COLS && y >= 0 && y < ROWS)
        grid[y][x] = ch;
}
 
/* ──────────────────────────────────────────
   DRAW FUNCTIONS
   ────────────────────────────────────────── */
 
/*
 * draw_rectangle
 * x, y : top-left corner
 * w    : width
 * h    : height
 * ch   : character to use
 */
void draw_rectangle(int x, int y, int w, int h, char ch)
{
    int i, j;
 
    /* Top and bottom edges */
    for (i = 0; i < w; i++) {
        set_cell(x + i, y,         ch);
        set_cell(x + i, y + h - 1, ch);
    }
 
    /* Left and right edges */
    for (j = 0; j < h; j++) {
        set_cell(x,         y + j, ch);
        set_cell(x + w - 1, y + j, ch);
    }
}
 
/*
 * draw_circle
 * cx, cy : centre of the circle
 * r      : radius
 * ch     : character to use
 *
 * Uses the midpoint circle algorithm, then corrects for
 * monospace fonts (characters are taller than wide) by
 * checking horizontal distance at half scale.
 */
void draw_circle(int cx, int cy, int r, char ch)
{
    int x, y;
    double dx, dy, dist;
 
    for (y = -r; y <= r; y++) {
        for (x = -r * 2; x <= r * 2; x++) {
            dx   = x / 2.0;   /* half because chars are ~2x taller */
            dy   = (double)y;
            dist = sqrt(dx * dx + dy * dy);
            if (fabs(dist - r) < 0.6)
                set_cell(cx + x, cy + y, ch);
        }
    }
}
 
/*
 * draw_line
 * (x1,y1) : start point
 * (x2,y2) : end point
 * ch       : character to use
 *
 * Uses Bresenham's line algorithm.
 */
void draw_line(int x1, int y1, int x2, int y2, char ch)
{
    int dx, dy, sx, sy, err, e2;
 
    dx =  abs(x2 - x1);
    dy = -abs(y2 - y1);
    sx = (x1 < x2) ? 1 : -1;
    sy = (y1 < y2) ? 1 : -1;
    err = dx + dy;
 
    while (1) {
        set_cell(x1, y1, ch);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}
 
/*
 * draw_triangle
 * x, y : top-left of bounding box
 * w    : base width
 * h    : height
 * ch   : character to use
 *
 * Draws a triangle with the apex at the top centre
 * and the base at the bottom.
 */
void draw_triangle(int x, int y, int w, int h, char ch)
{
    int row, half, apex, left, right, i;
 
    apex = w / 2;   /* x-offset of the tip from x */
 
    for (row = 0; row < h; row++) {
        half  = (int)round((double)row / (h - 1) * apex);
        left  = x + apex - half;
        right = x + apex + half;
 
        if (row == 0) {
            /* Apex: single point */
            set_cell(x + apex, y, ch);
        } else if (row == h - 1) {
            /* Base: full bottom edge */
            for (i = left; i <= right; i++)
                set_cell(i, y + row, ch);
        } else {
            /* Sides only */
            set_cell(left,  y + row, ch);
            set_cell(right, y + row, ch);
        }
    }
}
 
/* ──────────────────────────────────────────
   REBUILD THE GRID FROM THE OBJECT LIST
   ────────────────────────────────────────── */
 
/*
 * Clears the grid and redraws every active object.
 * Call this after any add / delete / modify operation.
 */
void rebuild_grid(void)
{
    int i;
    clear_grid();
 
    for (i = 0; i < object_count; i++) {
        if (!objects[i].active) continue;
 
        switch (objects[i].type) {
            case SHAPE_RECT:
                draw_rectangle(objects[i].x, objects[i].y,
                               objects[i].w, objects[i].h,
                               objects[i].ch);
                break;
            case SHAPE_CIRCLE:
                draw_circle(objects[i].x, objects[i].y,
                            objects[i].radius, objects[i].ch);
                break;
            case SHAPE_LINE:
                draw_line(objects[i].x,  objects[i].y,
                          objects[i].x2, objects[i].y2,
                          objects[i].ch);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle(objects[i].x, objects[i].y,
                              objects[i].w, objects[i].h,
                              objects[i].ch);
                break;
        }
    }
}
 
/* ──────────────────────────────────────────
   OBJECT MANAGEMENT FUNCTIONS
   ────────────────────────────────────────── */
 
/*
 * add_object
 * Adds a pre-filled Object struct to the list.
 * Returns the index of the new object, or -1 if full.
 */
int add_object(struct Object obj)
{
    if (object_count >= MAX_OBJECTS) {
        printf("Error: maximum number of objects reached.\n");
        return -1;
    }
    obj.active = 1;
    objects[object_count] = obj;
    object_count++;
    rebuild_grid();
    return object_count - 1;
}
 
/*
 * delete_object
 * Marks the object at index i as deleted.
 */
void delete_object(int i)
{
    if (i < 0 || i >= object_count) {
        printf("Error: invalid object index %d.\n", i);
        return;
    }
    if (!objects[i].active) {
        printf("Object %d is already deleted.\n", i);
        return;
    }
    objects[i].active = 0;
    printf("Object %d deleted.\n", i);
    rebuild_grid();
}
 
/*
 * modify_object
 * Replaces the object at index i with a new Object struct.
 */
void modify_object(int i, struct Object new_obj)
{
    if (i < 0 || i >= object_count) {
        printf("Error: invalid object index %d.\n", i);
        return;
    }
    new_obj.active = 1;
    objects[i] = new_obj;
    printf("Object %d modified.\n", i);
    rebuild_grid();
}
 
/* ──────────────────────────────────────────
   LIST OBJECTS
   ────────────────────────────────────────── */
 
/* Print a summary of all active objects */
void list_objects(void)
{
    int i, found;
    char *names[] = {"", "Rectangle", "Circle", "Line", "Triangle"};
 
    found = 0;
    printf("\n--- Object List ---\n");
    for (i = 0; i < object_count; i++) {
        if (!objects[i].active) continue;
        found = 1;
        printf("[%d] %s  char='%c'",
               i, names[objects[i].type], objects[i].ch);
 
        if (objects[i].type == SHAPE_RECT)
            printf("  x=%d y=%d w=%d h=%d",
                   objects[i].x, objects[i].y,
                   objects[i].w, objects[i].h);
        else if (objects[i].type == SHAPE_CIRCLE)
            printf("  cx=%d cy=%d r=%d",
                   objects[i].x, objects[i].y, objects[i].radius);
        else if (objects[i].type == SHAPE_LINE)
            printf("  (%d,%d)->(%d,%d)",
                   objects[i].x,  objects[i].y,
                   objects[i].x2, objects[i].y2);
        else if (objects[i].type == SHAPE_TRIANGLE)
            printf("  x=%d y=%d w=%d h=%d",
                   objects[i].x, objects[i].y,
                   objects[i].w, objects[i].h);
        printf("\n");
    }
    if (!found) printf("  (no objects)\n");
    printf("-------------------\n\n");
}
 
/* ──────────────────────────────────────────
   MENU FUNCTIONS
   ────────────────────────────────────────── */
 
/* Print the main menu */
void print_menu(void)
{
    printf("\n=== 2D ASCII Graphics Editor ===\n");
    printf("1. Add Rectangle\n");
    printf("2. Add Circle\n");
    printf("3. Add Line\n");
    printf("4. Add Triangle\n");
    printf("5. Delete Object\n");
    printf("6. Modify Object\n");
    printf("7. List Objects\n");
    printf("8. Display Picture\n");
    printf("9. Quit\n");
    printf("Choice: ");
}
 
/* Read an integer from the user with a prompt */
int read_int(char *prompt)
{
    int val;
    printf("%s", prompt);
    scanf("%d", &val);
    return val;
}
 
/* Read a character from the user with a prompt */
char read_char(char *prompt)
{
    char ch;
    printf("%s", prompt);
    scanf(" %c", &ch);   /* space before %c skips whitespace */
    return ch;
}

/* ──────────────────────────────────────────
   MAIN
   ────────────────────────────────────────── */
 
int main(void)
{
    int choice, idx;
    struct Object obj;
 
    clear_grid();
 
    while (1) {
        print_menu();
        scanf("%d", &choice);
 
        switch (choice) {
 
            /* ---- Add Rectangle ---- */
            case 1:
                obj.type   = SHAPE_RECT;
                obj.x      = read_int("  Top-left x: ");
                obj.y      = read_int("  Top-left y: ");
                obj.w      = read_int("  Width: ");
                obj.h      = read_int("  Height: ");
                obj.ch     = read_char("  Character (* or _): ");
                idx        = add_object(obj);
                if (obj.w <= 0 || obj.h <= 0)
                {
                  printf("Invalid dimensions.\n");
                  break;
                }
                if (idx >= 0)
                    printf("Rectangle added as object %d.\n", idx);
                break;
 
            /* ---- Add Circle ---- */
            case 2:
                obj.type   = SHAPE_CIRCLE;
                obj.x      = read_int("  Centre x: ");
                obj.y      = read_int("  Centre y: ");
                obj.radius = read_int("  Radius: ");
                obj.ch     = read_char("  Character (* or _): ");
                idx        = add_object(obj);
                if (idx >= 0)
                    printf("Circle added as object %d.\n", idx);
                break;
 
            /* ---- Add Line ---- */
            case 3:
                obj.type = SHAPE_LINE;
                obj.x    = read_int("  Start x: ");
                obj.y    = read_int("  Start y: ");
                obj.x2   = read_int("  End x: ");
                obj.y2   = read_int("  End y: ");
                obj.ch   = read_char("  Character (* or _): ");
                idx      = add_object(obj);
                if (idx >= 0)
                    printf("Line added as object %d.\n", idx);
                break;
 
            /* ---- Add Triangle ---- */
            case 4:
                obj.type = SHAPE_TRIANGLE;
                obj.x    = read_int("  Top-left x: ");
                obj.y    = read_int("  Top-left y: ");
                obj.w    = read_int("  Base width: ");
                obj.h    = read_int("  Height: ");
                obj.ch   = read_char("  Character (* or _): ");
                idx      = add_object(obj);
                if (idx >= 0)
                    printf("Triangle added as object %d.\n", idx);
                break;
 
            /* ---- Delete Object ---- */
            case 5:
                list_objects();
                idx = read_int("  Object index to delete: ");
                delete_object(idx);
                break;
 
            /* ---- Modify Object ---- */
            case 6:
                list_objects();
                idx = read_int("  Object index to modify: ");
                if (idx < 0 || idx >= object_count || !objects[idx].active) {
                    printf("Invalid index.\n");
                    break;
                }
                printf("  Current type: %d (1=Rect 2=Circle 3=Line 4=Triangle)\n",
                       objects[idx].type);
                obj.type = objects[idx].type;  /* keep same type */
 
                if (obj.type == SHAPE_RECT) {
                    obj.x  = read_int("  New top-left x: ");
                    obj.y  = read_int("  New top-left y: ");
                    obj.w  = read_int("  New width: ");
                    obj.h  = read_int("  New height: ");
                    obj.ch = read_char("  New character: ");
                } else if (obj.type == SHAPE_CIRCLE) {
                    obj.x      = read_int("  New centre x: ");
                    obj.y      = read_int("  New centre y: ");
                    obj.radius = read_int("  New radius: ");
                    obj.ch     = read_char("  New character: ");
                } else if (obj.type == SHAPE_LINE) {
                    obj.x  = read_int("  New start x: ");
                    obj.y  = read_int("  New start y: ");
                    obj.x2 = read_int("  New end x: ");
                    obj.y2 = read_int("  New end y: ");
                    obj.ch = read_char("  New character: ");
                } else if (obj.type == SHAPE_TRIANGLE) {
                    obj.x  = read_int("  New top-left x: ");
                    obj.y  = read_int("  New top-left y: ");
                    obj.w  = read_int("  New base width: ");
                    obj.h  = read_int("  New height: ");
                    obj.ch = read_char("  New character: ");
                }
                modify_object(idx, obj);
                break;
 
            /* ---- List Objects ---- */
            case 7:
                list_objects();
                break;
 
            /* ---- Display Picture ---- */
            case 8:
                display_picture();
                break;
 
            /* ---- Quit ---- */
            case 9:
                printf("Goodbye!\n");
                return 0;
 
            default:
                printf("Invalid choice. Please enter 1-9.\n");
        }
    }
 
    return 0;
}
 

 
 
