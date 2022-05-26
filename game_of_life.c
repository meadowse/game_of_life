#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define false 0
#define true 1

typedef int boolean;

typedef struct {
    boolean alive;
} Cell;

typedef struct {
    int w;
    int h;
    Cell *cells;
} Grid;

/* Новая сетка размеров ширины w и высоты h.
* Ячейки не инициализированы и не определены. */
Grid *Grid_new(int h, int w) {
    Grid *g = malloc(sizeof(Grid));
    if (!g) return NULL;
    g->w = w;
    g->h = h;
    g->cells = malloc(sizeof(Cell) * w * h);
    if (!g->cells) {
        free(g);
        return NULL;
    }
    return g;
}

/* Получите ячейку в (x, y) в сетке g. */
Cell *Grid_get_cell(Grid *g, int x, int y) {
    x %= g->w;
    y %= g->h;
    if (x < 0)
        x = g->w + x;
    if (y < 0)
        y = g->h + y;
    return &(g->cells[y * g->w + x]);
}

/* Уничтожить ячейку в (x, y) в сетке g. */
void kill(Grid *g, int x, int y) {
    Grid_get_cell(g, x, y)->alive = false;
}

/* Дайте жизнь ячейке в (x, y) в сетке g. */
void sustain(Grid *g, int x, int y) {
    Grid_get_cell(g, x, y)->alive = true;
}

/* Ячейка в (x, y) в сетке g жива? */

boolean alive(Grid *g, int x, int y) {
    return Grid_get_cell(g, x, y)->alive;
}

/* Уничтожить все ячейки в сетке g. */
void Grid_clear(Grid *g) {
    int x, y;
    for (x = 0; x < g->w; x++) {
        for (y = 0; y < g->h; y++)
            kill(g, x, y);
    }
}

/* Свободная сетка g целиком. */
void Grid_free(Grid *g) {
    free(g->cells);
    free(g);
}

/* Количество соседей для ячейки (x, y) в сетке g */
int neighbor_count(Grid *g, int x, int y) {
    int count = 0;
    if (alive(g, x-1, y-1)) count++;
    if (alive(g, x-1, y)) count++;
    if (alive(g, x-1, y+1)) count++;
    if (alive(g, x, y-1)) count++;
    if (alive(g, x, y+1)) count++;
    if (alive(g, x+1, y-1)) count++;
    if (alive(g, x+1, y)) count++;
    if (alive(g, x+1, y+1)) count++;
    return count;
}

/* Выживет ли ячейка (x, y) в сетке g (или вернется к жизни)? */
boolean survives(Grid *g, int x, int y) {
    int n = neighbor_count(g, x, y);
    if (alive(g, x, y)) {
        if (n < 2)
            return false; /* Колония погибнет */
        else if (n < 4)
            return true;
        else
            return false; /* Колония погибнет от перенаселения */
        } else {
            if (n == 3)
                return true; /* Будет крутиться в цикле */
        }
    return false;
}

/* Заполнить следующую сетку новыми состояниями ячеек предыдущей сетки */
void Grid_advance(Grid *g) {
    Grid *future = Grid_new(g->h, g->w);
    if (!future) {
        fprintf(stderr, "Allocation failure\n");
        return;
    }
    int x, y;
    for (y = 0; y < g->h; y++) {
        for (x = 0; x < g->w; x++) {
            if (survives(g, x, y))
                sustain(future, x, y);
            else
                kill(future, x, y);
        }
  }
    free(g->cells); /* очистит старые клетки */
    g->cells = future->cells; /* сохранит новые клетки */
    free(future); /* убедитесь, что не освободите будущие ячейки */
}

void Grid_print(Grid *g) { /* печатает сетку */
    int x, y;
    for (y = 0; y < g->h; y++) {
        for (x = 0; x < g->w; x++) {
            if (alive(g, x, y))
                printf("*");
            else
                printf(" ");
        }
        printf("\n");
  }

}
int main(void) {
    FILE *myfile;
    Grid *g;
    g = Grid_new(25, 80);
    Grid_clear(g);
    int f;
    printf("Задайте скорость развития симуляции жизни от 1 (очень быстро) до бессконечности (медленнo)\nP.S.: рекомендуем среднюю 200000\n");
        if (scanf("%d", &f) != 1)
            printf("Не верно введён параметр скорости");
    /*Планер или глайдер (пятиклеточная конфигурация "жизни") */
    /*myfile = fopen("first.txt.", "r");
    int x, y, b;
    for (y = 0; y < g->h; y++) {
        for (x = 0; x < g->w; x++) {
            b = fgetc(myfile);
            if (b == 1)
                Grid_get_cell(g, x, y)->alive = true;
            else
                Grid_get_cell(g, x, y)->alive = false;
        }
        b = fgetc(myfile);}
    fclose(myfile); */
    sustain(g, 5, 25);
    sustain(g, 6, 26);
    sustain(g, 4, 27);
    sustain(g, 5, 27);
    sustain(g, 6, 27);
    int i;
    for (i = 0; ; i++) {
        printf("Timer: %d\n", i);
        Grid_advance(g);
        Grid_print(g);
        usleep(f);
    }
  Grid_free(g);
    return 0;
}
