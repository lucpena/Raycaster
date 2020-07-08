#ifndef PLAYER_H
#define PLAYER_H

struct Player {
    // Position
    float x, y;

    // View Direction
    float a;

    // Field of View
    float fov;

    // Handle moviment
    int turn, walk;
};

#endif