//
//  main.cpp
//  AiSD_Project_2
//
//  Created by Kacper Kędzierski on 31/03/2020.
//  Copyright © 2020 Kacper Kędzierski. All rights reserved.
//

#include <iostream>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#define INVALID_NUMBER false
#define SUCCESS true

// Debug stream
#ifdef _DEBUG
  #define _DEBUG_PRINTF(...) fprintf(stderr, __VA_ARGS__ );
#else
  #define _DEBUG_PRINTF(...)
#endif

using namespace std;

//  ===
//  Data structures
//  ===

typedef struct {
    int_fast32_t notches_count;
    int_fast32_t* notches;
    int_fast32_t* permutation;
    int_fast32_t* permutation_reverse;
} rotor_t;

typedef struct {
    int_fast32_t* permutation;
} reflector_t;

typedef struct {
    int_fast32_t alphabet_size;
    int_fast32_t rotors_count;
    rotor_t** rotors;
    int_fast32_t* rotors_positions;
    reflector_t* reflector;
    bool STOS_Flags[3]; // STOS Flags: check for first and second rotor turnover
} encryptor_t;


//  ===
//  Functions
//  ===

int_fast32_t* permutation_scan( int_fast32_t size ) {
    int_fast32_t* return_permutation = (int_fast32_t*)malloc( size * sizeof(int_fast32_t));
    
    for( int_fast32_t i = 0; i < size; i++ ) scanf(" %" SCNdFAST32, &return_permutation[i]);
    return return_permutation;
}

int_fast32_t* permutation_revert( int_fast32_t alphabet_size, int_fast32_t permutation[] ) {
    int_fast32_t* permutation_reverted = (int_fast32_t*)malloc( alphabet_size * sizeof(int_fast32_t) );
    
    for( int_fast32_t i = 0; i < alphabet_size; i++ ) permutation_reverted[permutation[i]-1] = i+1;

    return permutation_reverted;
}

void permutation_apply( int_fast32_t* permutation, int_fast32_t* number ) {
    *number = permutation[*number - 1];
}

rotor_t* rotor_scan( int_fast32_t size ) {
    rotor_t* return_rotor = (rotor_t*)malloc(sizeof(rotor_t));
    
    // Scan permuation - call permutation_scan function
    return_rotor->permutation = permutation_scan(size);
    
    // Scan notches
    scanf("%" SCNdFAST32, &return_rotor->notches_count);
    if( return_rotor->notches_count > 0 )
        return_rotor->notches = (int_fast32_t*)malloc( return_rotor->notches_count * sizeof(int_fast32_t));
    
    for( int_fast32_t i = 0; i < return_rotor->notches_count; i++ ) {
        scanf(" %" SCNdFAST32, &return_rotor->notches[i]);
        // Substract 2 - instead of 1. STOS Workaround
        return_rotor->notches[i] -= 2;
        return_rotor->notches[i] = (return_rotor->notches[i] + size) % size;
    }
    
    // Revert permutation for backward movement
    return_rotor->permutation_reverse = permutation_revert( size, return_rotor->permutation );
    return return_rotor;
}

reflector_t* reflector_scan( int_fast32_t size ) {
    reflector_t* return_reflector = (reflector_t*)malloc(sizeof(reflector_t));
    
    return_reflector->permutation = permutation_scan(size);
    return return_reflector;
}

bool rotor_check_notches( rotor_t* rotor, int_fast32_t position ) {
    
    for( int_fast32_t i = 0; i < rotor->notches_count; i++ ) {
        if( rotor->notches[i] == position ) return true;
    }
    return false;
}

void advance_rotors( encryptor_t* enigma ) {
    
    int_fast32_t moveable_rotors = enigma->rotors_count >= 3 ? 3 : enigma->rotors_count;
    
    switch(moveable_rotors) {
        case 3:
            if(enigma->STOS_Flags[1] == true && rotor_check_notches(enigma->rotors[1], enigma->rotors_positions[1])) {
                _DEBUG_PRINTF("Rotor 2 at notch position; advance rotor 3\n");
                enigma->rotors_positions[2] += 1;
                enigma->rotors_positions[2] %= enigma->alphabet_size;
                enigma->STOS_Flags[2] = true; // double-stepping
            }
            // 'break' excluded; double-stepping
        case 2:
            if((enigma->STOS_Flags[0] == true && rotor_check_notches(enigma->rotors[0], enigma->rotors_positions[0])) || enigma->STOS_Flags[2] == true ) {
                _DEBUG_PRINTF("Rotor 1 at notch position; advance rotor 2\n");
                enigma->rotors_positions[1] += 1;
                enigma->rotors_positions[1] %= enigma->alphabet_size;
                enigma->STOS_Flags[1] = true; // Brute force rotor 2 flag update
                enigma->STOS_Flags[2] = false; // Brute force rotor 3 flag update
            }
            break;
    }
    
    // Rotor 1 is advanced every encryption
    enigma->rotors_positions[0] += 1;
    enigma->rotors_positions[0] %= enigma->alphabet_size;
    enigma->STOS_Flags[0] = true; // Brute force rotor 1 flag update
}

bool encrypt( encryptor_t* enigma, int_fast32_t number ) {
    if( number == 0 ) return INVALID_NUMBER;
    
    // Advance(move) rotors
    advance_rotors(enigma);
    
    // Forward movement
    for( int_fast32_t i = 0; i < enigma->rotors_count; i++ ) {
        number -= 1;
        number = (number + enigma->rotors_positions[i]) % enigma->alphabet_size;
        number += 1;
        _DEBUG_PRINTF("Encrypting: rotor[%d]; passing number %d; position %d, FORWARD\n",i, number, enigma->rotors_positions[i]);
        permutation_apply(enigma->rotors[i]->permutation, &number);
        _DEBUG_PRINTF("Applied permutation; number %d\n",number);
        number -= 1;
        number = ((number - enigma->rotors_positions[i]) + enigma->alphabet_size) % enigma->alphabet_size;
        number += 1;
    }
    _DEBUG_PRINTF("Encrypting: reflector; passing number %d\n", number);
    permutation_apply(enigma->reflector->permutation, &number);
    _DEBUG_PRINTF("Applied permutation; number %d\n",number);
    
    // Backward movement
    for( int_fast32_t i = enigma->rotors_count - 1; i >= 0; i-- ) {
        number -= 1;
        number = (number + enigma->rotors_positions[i]) % enigma->alphabet_size;
        number += 1;
        _DEBUG_PRINTF("Encrypting: rotor[%d]; passing number %d; position %d, BACKWARD\n",i, number, enigma->rotors_positions[i]);
        permutation_apply(enigma->rotors[i]->permutation_reverse, &number);
        _DEBUG_PRINTF("Applied permutation; number %d\n",number);
        number -= 1;
        number = ((number - enigma->rotors_positions[i]) + enigma->alphabet_size) % enigma->alphabet_size;
        number += 1;
    }
    
    printf("%" SCNdFAST32 " ", number);
    _DEBUG_PRINTF("\n");
    
    return SUCCESS;
}

void encryptor_free( encryptor_t* encryptor ) {
    free(encryptor->rotors);
    free(encryptor->rotors_positions);
    
    free(encryptor);
}

void free_rotors( rotor_t** rotors, int_fast32_t rotors_count ) {
    for( int_fast32_t i = 0; i < rotors_count; i++ ) {
        if( rotors[i]->notches_count > 0 ) free(rotors[i]->notches);
        free(rotors[i]->permutation);
        free(rotors[i]->permutation_reverse);
        free(rotors[i]);
    }
    free(rotors);
}

void free_reflectors( reflector_t** reflectors, int_fast32_t reflector_count ) {
    for( int_fast32_t i = 0; i < reflector_count; i++ ) {
        free(reflectors[i]->permutation);
        free(reflectors[i]);
    }
    free(reflectors);
}


//  ===
//  Main
//  ===

int main() {
    
    _DEBUG_PRINTF("Debug stream initialized\n");
    
    int_fast32_t alphabet_size;
    int_fast32_t rotors_count;
    int_fast32_t reflectors_count;
    int_fast32_t tasks_count;
    int_fast32_t rotor_index;
    int_fast32_t rotor_position;
    int_fast32_t reflector_index;
    int_fast32_t number;
    
    scanf("%" SCNdFAST32, &alphabet_size);
    
    // Initialize rotors definitions
    scanf("%" SCNdFAST32, &rotors_count);
    rotor_t** rotors = (rotor_t**)malloc( rotors_count * sizeof(rotor_t) );
    for( int_fast32_t i = 0; i < rotors_count; i++ ) {
        rotors[i] = rotor_scan(alphabet_size);
    }
    
    // Initialize reflectors definitions
    scanf("%" SCNdFAST32, &reflectors_count);
    reflector_t** reflectors = (reflector_t**)malloc( reflectors_count * sizeof(reflector_t) );
    for( int_fast32_t i = 0; i < reflectors_count; i++ ) {
        reflectors[i] = reflector_scan(alphabet_size);
    }
    
    scanf("%" SCNdFAST32, &tasks_count);
    for( int_fast32_t i = 0; i < tasks_count; i++ ) {
        
        // Initialize encryptor
        encryptor_t* enigma = (encryptor_t*)malloc(sizeof(encryptor_t));
        
        enigma->alphabet_size = alphabet_size;
        
        scanf("%" SCNdFAST32, &enigma->rotors_count);
        enigma->rotors = (rotor_t**)malloc( enigma->rotors_count * sizeof(rotor_t) );
        enigma->rotors_positions = (int_fast32_t*)malloc( enigma->rotors_count * sizeof(int_fast32_t) );
        
        // Fix flags for STOS
        enigma->STOS_Flags[0] = false;
        enigma->STOS_Flags[1] = false;
        enigma->STOS_Flags[2] = false;
        
        // Scan encryptor rotors
        for( int_fast32_t j = 0; j < enigma->rotors_count; j++ ) {
            scanf("%" SCNdFAST32, &rotor_index);
            scanf("%" SCNdFAST32, &rotor_position);
            
            enigma->rotors[j] = rotors[rotor_index];
            enigma->rotors_positions[j] = --rotor_position;
        }
        
        // Scan encryptor reflector
        scanf("%" SCNdFAST32, &reflector_index);
        enigma->reflector = reflectors[reflector_index];
        
        do {
            scanf("%" SCNdFAST32, &number);
        } while( encrypt(enigma,number) );
        
        printf("\n");
        
        // Free encryptor
        encryptor_free(enigma);
        
    }
    
    // Free dynamically allocated data
    free_rotors(rotors, rotors_count);
    free_reflectors(reflectors, reflectors_count);
    
    return 0;
    
}
