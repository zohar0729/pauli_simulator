#ifndef DEF_PAULI_SIMULATOR_H
#define DEF_PAULI_SIMULATOR_H

#include "conditions.h"

typedef struct pauli_error {
    int bit, phase;
    int reg;
}pauli_error_t;

void update_pauli_state(
    pauli_error_t state[max_size], 
    const char circuit[][max_size + 1], 
    int depth
);

#endif