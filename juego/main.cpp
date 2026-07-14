#include <iostream>
#include "raylib.h"
#include <vector>
#include <map>

using namespace std;

string page = "info";
Texture2D mapa_info;

// ============================================================
//  SISTEMA DE ESCALADO RESPONSIVE
// ============================================================
// Resolucion "de diseno": la resolucion con la que se construyeron
// originalmente los rectangulos de colision, el tamano del jugador
// y la escala del mapa (5x el PNG original).
const float REF_WIDTH  = 1600.0f;
const float REF_HEIGHT = 900.0f;

// "Unidad de medida" de referencia: 1% del promedio (ancho+alto).
// A esta resolucion, factorEscala() = 1.0, y el juego se comporta
// EXACTAMENTE igual que el codigo original.
const float REF_UNIT = (REF_WIDTH + REF_HEIGHT) / 200.0f; // 12.5

// Tamanos "base" (unidades de diseno), se multiplican por el factor
// de escala cada frame.
const float PLAYER_SIZE_BASE = 30.0f;
const float MOVE_SPEED_BASE  = 10.0f;
const float MAP_SCALE_BASE   = 5.0f; // el mapa ya es 5x el PNG original

// Unidad de medida actual: 1% del promedio ancho+alto de la ventana,
// igual que un "vw/vh" de HTML pero combinando ambos ejes.
float unidadActual() {
    return (GetScreenWidth() + GetScreenHeight()) / 200.0f;
}

// Factor de escala relativo a la resolucion de diseno.
// Es UN SOLO numero (no un x/y independiente), por eso el mapa
// crece o se achica de forma uniforme y nunca se deforma.
float factorEscala() {
    return unidadActual() / REF_UNIT;
}

// Revisa si "player" (en espacio de PANTALLA, ya escalado) choca
// contra alguno de los rectangulos de colision del mapa.
// col[] esta en "unidades de diseno" (sin escalar); mapX/mapY son el
// offset del mapa tambien en unidades de diseno. Se escalan aqui mismo.
bool colisionEnPosicion(Rectangle* col, int n, float mapX, float mapY, float scale, Rectangle player) {
    for (int i = 0; i < n; i++) {
        Rectangle r = {
            (col[i].x + mapX) * scale,
            (col[i].y + mapY) * scale,
            col[i].width  * scale,
            col[i].height * scale
        };
        if (CheckCollisionRecs(player, r)) return true;
    }
    return false;
}

// Punto de aparicion, en coordenadas del MUNDO (unidades de diseno),
// que siempre debe quedar centrado en pantalla sin importar el
// ancho/alto/aspect-ratio de la ventana. Equivale al x=-1890,y=-1650
// originales, pero expresado como un punto fijo del mapa en vez de
// un offset pensado unicamente para 1600x900.
const float WORLD_SPAWN_X = 2690.0f;
const float WORLD_SPAWN_Y = 2100.0f;

bool info() {
    // x,y = offset del mapa, en unidades de diseno (SIN escalar).
    // Se calculan para que WORLD_SPAWN quede exactamente en el centro
    // de la pantalla actual, sea cual sea su ancho/alto/aspect-ratio.
    float scaleInicial = factorEscala();
    float x = (GetScreenWidth()  / 2.0f) / scaleInicial - WORLD_SPAWN_X;
    float y = (GetScreenHeight() / 2.0f) / scaleInicial - WORLD_SPAWN_Y;

    // Rectangulos de colision, en unidades de diseno (identicos a los originales).
    Rectangle col[11];
    col[0]  = {0, 0, 175, 2660};
    col[1]  = {0, 2135, 3380, 525};
    col[2]  = {0, 0, 3380, 515};
    col[3]  = {3205, 0, 175, 2660};
    col[4]  = {350, 555, 335, 400};
    col[5]  = {810, 380, 265, 1575};
    col[6]  = {1235, 380, 265, 1575};
    col[7]  = {1640, 380, 265, 1575};
    col[8]  = {2040, 380, 265, 1575};
    col[9]  = {2435, 380, 265, 1575};
    col[10] = {2820, 380, 265, 1575};

    while (!WindowShouldClose())
    {
        // --- Recalcular escala y jugador cada frame (por si la ventana cambio) ---
        float scale = factorEscala();
        float playerSize = PLAYER_SIZE_BASE * scale;

        // El jugador SIEMPRE se dibuja centrado en la pantalla actual,
        // sin importar el tamano de la ventana.
        Rectangle player = {
            GetScreenWidth()  / 2.0f - playerSize / 2.0f,
            GetScreenHeight() / 2.0f - playerSize / 2.0f,
            playerSize,
            playerSize
        };

        if (IsKeyPressed(KEY_R))
        {
            while (IsKeyDown(KEY_R))
            {
                BeginDrawing();
                EndDrawing();
            }
            return true;
        }

        if (IsKeyDown(KEY_W)) {
            if (!colisionEnPosicion(col, 11, x, y + MOVE_SPEED_BASE, scale, player)) y += MOVE_SPEED_BASE;
        }
        if (IsKeyDown(KEY_A)) {
            if (!colisionEnPosicion(col, 11, x + MOVE_SPEED_BASE, y, scale, player)) x += MOVE_SPEED_BASE;
        }
        if (IsKeyDown(KEY_S)) {
            if (!colisionEnPosicion(col, 11, x, y - MOVE_SPEED_BASE, scale, player)) y -= MOVE_SPEED_BASE;
        }
        if (IsKeyDown(KEY_D)) {
            if (!colisionEnPosicion(col, 11, x - MOVE_SPEED_BASE, y, scale, player)) x -= MOVE_SPEED_BASE;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextureEx(mapa_info, {x * scale, y * scale}, 0, MAP_SCALE_BASE * scale, WHITE);
        //for (int i=0; i<11; i++) DrawRectangle((col[i].x+x)*scale, (col[i].y+y)*scale, col[i].width*scale, col[i].height*scale, Color(200, 50, 75, 180));
        DrawRectangle(player.x, player.y, player.width, player.height, BLUE);
        EndDrawing();
    }
    return false;
}

int main()
{
    float screenWidth = 1600;
    float screenHeight = 900;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Juego");
    SetWindowMinSize(640, 360); // evita que la escala se vuelva absurdamente pequena
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    bool a = 1;
    mapa_info = LoadTexture("assets/img/mapas/Info-C.png");

    while (!WindowShouldClose() && a)
    {
        a = info();
    }
    CloseWindow();
    return 0;
}