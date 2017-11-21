#include "random.h"

short int rand_unset = 1;

int random_max(int up_limit) {
    if (rand_unset) {
        rand_unset = 0;
        srand(time(NULL));
    }
    return rand() % up_limit;
}
