#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"

// Définitions des dimensions de la grille de confinement
#define MAP_ROWS 12 
#define MAP_COLS 20 

// Limite critique de dégradation avant fusion du cœur
#define MAX_SURCHARGE 5 

/**
 * @brief Types d'énergies élémentaires (Cellules du réacteur).
 * CELL_EMPTY (0) représente une zone instable/vide.
 */
typedef enum {
    CELL_EMPTY = 0,
    CELL_R, // Thermique 
    CELL_B, // Cryogénique
    CELL_G, // Plasma
    CELL_Y, // Photonique
    CELL_V, // Gravitationnelle
    CELL_COUNT
} CellType;

/**
 * @brief États de la machine à états principale (Flux Raylib).
 */
typedef enum {
    STATE_MENU,      // Écran d'identification de l'opérateur
    STATE_GAMEPLAY,  // Phase active, attente des commandes joueur
    STATE_CASCADE,   // Animation/Résolution automatique des réactions en chaîne (entrées bloquées)
    STATE_VICTORY,   // Palier de stabilisation atteint (Niveau réussi)
    STATE_GAMEOVER,  // Rupture temporaire du confinement (Échec du niveau, surcharge +1)
    STATE_CLEAR      // Instabilité critique définitive (Game Over global, Surcharge = 5)
} GameState;

typedef struct{
    float scrollingBack;
    float scrollingMid;
    float scrollingFore;
} ScrollingBackground;

/**
 * @brief Objectifs quantiques et limites du niveau en cours de traitement.
 */
typedef struct {
    int req_energy[CELL_COUNT]; // Quantités d'énergies requises pour stabiliser le niveau
    int cur_energy[CELL_COUNT]; // Quantités d'énergies actuellement absorbées
    int max_moves;              // Limite maximale de couplages (N)
    int cur_moves;              // Nombre de couplages effectués par l'opérateur
    float time_limit;           // Temps imparti avant pic critique (T en secondes)
    float time_elapsed;         // Compteur de temps écoulé
} LevelConfig;

/**
 * @brief Contexte central de l'application (Unique source de vérité).
 * Transite par pointeur à travers l'ensemble des modules logiques et graphiques.
 */
typedef struct {
    char operator_name_1[50];
    char operator_name_2[50];
    char operator_name_3[50];
    int surcharge_1;
    int surcharge_2;
    int surcharge_3;
    int current_level_idx_1;
    int current_level_idx_2;
    int current_level_idx_3;
} GameSave;

typedef struct {
    CellType matrix[MAP_ROWS][MAP_COLS]; // Matrice du cœur du réacteur
    int marked[MAP_ROWS][MAP_COLS];      // Grille de détection algorithmique (1 si aligné, 0 sinon)
    
    int cursor_x;                        // Coordonnée X du curseur de navigation
    int cursor_y;                        // Coordonnée Y du curseur de navigation
    
    int selected_x;                      // Coordonnée X de la première cellule verrouillée (-1 si aucune)
    int selected_y;                      // Coordonnée Y de la première cellule verrouillée (-1 si aucune)
    
    int surcharge;                       // Compteur de dégradation persistant (0 à 5)
    int current_level_idx;               // Index du niveau opérationnel actif (1, 2, 3...)
    char operator_name[50];              // Identifiant/Nom de l'opérateur (Sauvegarde)
    
    LevelConfig level;                   // Configuration et scores du niveau courant
    GameState state;                     // État actuel du système
    GameSave save;
} GameContext;

#endif // TYPES_H