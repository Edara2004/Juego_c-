#include <iostream>
#include <allegro.h>
#include <cmath>
#include <string>

using namespace std;

void rotacion (float &x, float &y, float coordenada_x, float coordenada_y, float da); //Rotacion de partes de la nave al destruirse
void pintar_nave(float coordenada_x, float coordenada_y, BITMAP *buffer); //Coordenadas de dibujado de la nave
void gravedad_nave (float &coordenada_x, float &coordenada_y, float &velocidad_x, float &velocidad_y ); //Funcion de la gravedad o caida libre de la nave
void aceleracion (float da, float &velocidad_x, float &velocidad_y); //Funcion para el movimiento de la nave
void motor(float da, float coordenada_x, float coordenada_y, BITMAP *buffer); // Dibujado del fuego de los motores
void medidor_combustible (bool gastar_combustible, float &combustible, BITMAP *buffer); //Barra de medicion del combustible de la nave
void contador(BITMAP *buffer);
void nivel (int numero_nivel, BITMAP *buffer ); //Diseño del nivel
void destruir_nave (float coordenada_x, float coordenada_y, BITMAP *buffer, int numero_nivel); // Animacion de destruccion de la nave
void avanzar_nivel (float &coordenada_x, float &coordenada_y, float &velocidad_x, float &velocidad_y, int &numero_nivel, float &combustible, BITMAP *buffer); //Funcion que permite avanzar de nivel una vez cumplida la condion del aterrizaje
void reiniciar_nivel (float &coordenada_x, float &coordenada_y, float &velocidad_x, float &velocidad_y, float &combustible); //Funcion que permite reiniciar el nivel
bool bordes (float &coordenada_x, float &coordenada_y, float &velocidad_x, float &velocidad_y, int &numero_nivel, float &combustible, BITMAP *buffer); //Definicion de hit box de los bordes del juego
bool aterrizaje (float coordenada_x, float coordenada_y, float velocidad_x, float velocidad_y, BITMAP *buffer, int numero_nivel); //Funcion boolena para el aterrizaje de la nave
bool HitBox_triangulo (float x1, float y1, float x2, float y2, float p1x, float p1y, float p2x, float p2y, string obstaculo); //Funcion booleana para definir las colisiones o hit box de los triangulos (nivel 2)
bool choque_nave(int numero_nivel, float coordenada_x, float coordenada_y);//Funcion la cual define las zonas de colision o hitbox de la nave en sus 3 partes
int pantalla_w = GFX_AUTODETECT_WINDOWED;

int juego();
int tutorial();
int contador_muertes = 0;
bool salida = false;

int main(){

    allegro_init();
    install_mouse();
    install_keyboard();

    set_color_depth(32);
    set_gfx_mode(pantalla_w, 1000, 800, 0, 0);

    // Bitmaps
    BITMAP *buffer_p = create_bitmap(1000,800);
    BITMAP *fondo_principal;
    BITMAP *fondo_inicio_2;
    BITMAP *fondo_tutorial_2;
    BITMAP *fondo_salir;
    BITMAP *fondo_salir_2;
    BITMAP *cursor;

    // Fondos de menu de inicio
    clear_to_color(buffer_p, 0x333333);
    fondo_principal = load_bitmap("inicio.bmp", NULL);
    fondo_inicio_2 = load_bitmap("inicio_2.bmp", NULL);
    fondo_tutorial_2 = load_bitmap("tutorial_2.bmp", NULL);
    fondo_salir_2 = load_bitmap("salir_2.bmp", NULL);
    cursor = load_bitmap("cursor.bmp", NULL);


    // Sonido

    if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL));
    set_volume(100,100);

    // Audio del juego de fondo

    SAMPLE *musica = load_wav("sonido_nave.wav");
    SAMPLE *beep = load_wav("beep.wav");

    // Variables
    int pausa = 0;


    // Botones de pantalla
    while (!salida){
        play_sample(musica, 100, 200, 2000, 2);

        if (mouse_x > 420 && mouse_x < 582 && mouse_y > 500 && mouse_y < 533){
            blit(fondo_inicio_2, buffer_p, 0, 0, 0, 0, 1000, 800);
            play_sample(beep, 200, 200, 1400, 1);
            if (mouse_b & 1){
                juego();
            }
        } else if(mouse_x > 393 && mouse_x < 618 && mouse_y > 564 && mouse_y < 593){
            blit(fondo_tutorial_2, buffer_p, 0, 0, 0, 0, 1000, 800);
            play_sample(beep, 200, 200, 1400, 1);
            if (mouse_b & 1){
                tutorial();
            }
        } else if(mouse_x > 857 && mouse_x < 975 && mouse_y > 730 && mouse_y < 765){
            blit(fondo_salir_2, buffer_p, 0, 0, 0, 0, 1000, 800);
            play_sample(beep, 200, 200, 1400, 1);
            if (mouse_b & 1){
                salida = true;
            }
        }else blit(fondo_principal, buffer_p, 0, 0, 0, 0, 1000, 800);

        masked_blit(cursor, buffer_p, 0, 0, mouse_x, mouse_y, 13,22);
        blit(buffer_p, screen, 0, 0, 0, 0, 1000, 800);
    }
    destroy_bitmap(buffer_p);
	destroy_bitmap(fondo_principal);
	destroy_bitmap(fondo_inicio_2);
	destroy_bitmap(fondo_salir);
	destroy_bitmap(fondo_salir_2);
	destroy_bitmap(fondo_tutorial_2);
	destroy_bitmap(cursor);

    return 0;
}
END_OF_MAIN();

int juego(){

    set_color_depth(32);
    BITMAP *buffer = create_bitmap(1000,800);
    float coordenada_x, coordenada_y, velocidad_x, velocidad_y;
    coordenada_x = 900, coordenada_y = 100;
    velocidad_y = -2;

    SAMPLE *despegar_sonido = load_wav("despegar_cohete.wav");

    float combustible = 100;
    int numero_nivel = 0;

    if(!key[KEY_ESC]){
            contador_muertes = 0;
        }

    while(!key[KEY_ESC] and !bordes(coordenada_x, coordenada_y, velocidad_x, velocidad_y, numero_nivel, combustible, buffer)){


        bool gastar_combustible = false;
        clear_to_color(buffer, 0x000000); // Color del buffer (fonde de la pantalla)
        nivel(numero_nivel, buffer);
        gravedad_nave(coordenada_x,coordenada_y,velocidad_x,velocidad_y);// Llamado de la funcion de la gravedad
        textprintf_right_ex(buffer, font, 900, 50, 0xFFFFFF, 0x000000, "Muertes: %d", contador_muertes);
        textprintf_right_ex(buffer, font, 900, 750, 0xFFFFFF, 0x000000, "Presiona [ESC] para salir de la partida");

        if (key[KEY_UP] and combustible > 0){
            aceleracion(0, velocidad_x, velocidad_y); // Impulso hacia arriba
            motor (0, coordenada_x, coordenada_y, buffer); // Pintado del motor
            gastar_combustible = true;
            play_sample(despegar_sonido, 200, 200, 1000, 1);
        }
        if (key[KEY_RIGHT] and combustible > 0){
            aceleracion(-90, velocidad_x, velocidad_y);  // Impulso hacia la derecha
            motor (-90, coordenada_x, coordenada_y, buffer);  // Pintado del motor
            gastar_combustible = true;
            play_sample(despegar_sonido, 200, 200, 1000, 1);
        }
        if (key[KEY_LEFT] and combustible > 0){
            aceleracion(90, velocidad_x, velocidad_y); // Impulso hacia la izquierda
            motor (90, coordenada_x, coordenada_y, buffer);  // Pintado del motor
            gastar_combustible = true;
            play_sample(despegar_sonido, 200, 200, 1000, 1);
        }

        // Pintado general
        medidor_combustible (gastar_combustible, combustible, buffer);
        pintar_nave(coordenada_x,coordenada_y,buffer); // Llamado de la funcion pintar_nave para su dibujado
        blit(buffer, screen, 0, 0, 0, 0, 1000,800); // Pantalla de buffer, Falla de pantalla
        avanzar_nivel(coordenada_x,coordenada_y,velocidad_x,velocidad_y, numero_nivel, combustible, buffer);
        rest (18);
    }
    return 0;
}

int tutorial(){

    set_color_depth(32);
    BITMAP *buffer_p = create_bitmap(1000,800);

    BITMAP *fondo_tutorial;
    BITMAP *cursor;
    BITMAP *contador;

    fondo_tutorial = load_bitmap("tutorial.bmp", NULL);
    cursor = load_bitmap("cursor.bmp", NULL);

    bool salida_tutorial = false;
    while (!salida_tutorial){

        if (mouse_x > 913 && mouse_x < 968 && mouse_y > 30 && mouse_y < 80){
            blit(fondo_tutorial, buffer_p, 0, 0, 0, 0, 1000, 800);
            if (mouse_b & 1){
                salida_tutorial = true; // cambiar por funcion;
            }
        }else blit(fondo_tutorial, buffer_p, 0, 0, 0, 0, 1000, 800);
        masked_blit(cursor, buffer_p, 0, 0, mouse_x, mouse_y, 13,22);
        blit(buffer_p, screen, 0, 0, 0, 0, 1000, 800);
    }
    destroy_bitmap(buffer_p);
	destroy_bitmap(fondo_tutorial);
	destroy_bitmap(cursor);

    return 0;
}


int fondo_final(){

    set_color_depth(32);
    BITMAP *buffer_p = create_bitmap(1000,800);

    BITMAP *fondo_final;
    BITMAP *cursor;
    BITMAP *contador;

    fondo_final = load_bitmap("fondo_final.bmp", NULL);
    cursor = load_bitmap("cursor.bmp", NULL);

    while (!salida, !key[KEY_ESC]){

        blit(fondo_final, buffer_p, 0, 0, 0, 0, 1000, 800);
        masked_blit(cursor, buffer_p, 0, 0, mouse_x, mouse_y, 13,22);
        blit(buffer_p, screen, 0, 0, 0, 0, 1000, 800);
    }
    destroy_bitmap(buffer_p);
	destroy_bitmap(fondo_final);
	destroy_bitmap(cursor);

    return 0;
}

void reiniciar_nivel (float &coordenada_x, float &coordenada_y, float &velocidad_x, float &velocidad_y, float &combustible){

    contador_muertes++;
    coordenada_x = 900; coordenada_y = 100;
    velocidad_x = 0; velocidad_y = -2;
    combustible = 100;
}

void avanzar_nivel (float &coordenada_x, float &coordenada_y, float &velocidad_x, float &velocidad_y, int &numero_nivel, float &combustible, BITMAP *buffer){

    if (aterrizaje(coordenada_x, coordenada_y, velocidad_x, velocidad_y, buffer, numero_nivel) == true) {
        coordenada_x = 900; coordenada_y = 100;
        velocidad_y = -2; velocidad_x = 0;
        if (numero_nivel != 3) numero_nivel++;
            while (!key[KEY_ENTER]){
                textout_centre_ex(screen, font, "Presiona <ENTER> para pasar al siguiente nivel", 500, 400, 0xFBFF00, 0x000000);
                rest(20);
            }
        combustible = 100;
    }
}

// Define la hitbox de las partes de la nave
bool choque_nave(int numero_nivel, float coordenada_x, float coordenada_y){

    // Hitbox pata izquierda
    float pata_izquierda_1x = coordenada_x-20, pata_izquierda_1y = coordenada_y;
    float pata_izquierda_2x = coordenada_x-10, pata_izquierda_2y = coordenada_y+20;

    // Hitbox pata derecha
    float pata_derecha_1x = coordenada_x+10, pata_derecha_1y = coordenada_y;
    float pata_derecha_2x = coordenada_x+20, pata_derecha_2y = coordenada_y+20;

    // Hitbox cabina
    float cabina_1x = coordenada_x-10, cabina_1y = coordenada_y-15;
    float cabina_2x = coordenada_x+10, cabina_2y = coordenada_y;

         if (numero_nivel == 0 ){
         if (HitBox_triangulo(305, 300, 600, 800, pata_izquierda_1x, pata_izquierda_1y, pata_izquierda_2x, pata_izquierda_2y, "abajo") or HitBox_triangulo(305, 300, 600, 800, pata_derecha_1x, pata_derecha_1y, pata_derecha_2x, pata_derecha_2y, "abajo") or HitBox_triangulo(305, 300, 600, 800, cabina_1x, cabina_1y, cabina_2x, cabina_2y, "abajo")) return true;
         else if (HitBox_triangulo(400, 0, 600, 650, pata_izquierda_1x, pata_izquierda_1y, pata_izquierda_2x, pata_izquierda_2y, "arriba") or HitBox_triangulo(400, 0, 600, 650, pata_derecha_1x, pata_derecha_1y, pata_derecha_2x, pata_derecha_2y, "arriba") or HitBox_triangulo(400, 0, 600, 650, cabina_1x, cabina_1y, cabina_2x, cabina_2y, "arriba")) return true;//lado derecho triangulo superior

    }

    else if (numero_nivel == 1 ){
        if (HitBox_triangulo(110, 100, 300, 800, pata_izquierda_1x, pata_izquierda_1y, pata_izquierda_2x, pata_izquierda_2y, "abajo") or HitBox_triangulo(110, 100, 300, 800, pata_derecha_1x, pata_derecha_1y, pata_derecha_2x, pata_derecha_2y, "abajo") or HitBox_triangulo(110, 100, 300, 800, cabina_1x, cabina_1y, cabina_2x, cabina_2y, "abajo")) return true;
        else if (HitBox_triangulo(620, 355, 900, 800, pata_izquierda_1x, pata_izquierda_1y, pata_izquierda_2x, pata_izquierda_2y, "abajo") or HitBox_triangulo(620, 355, 900, 800, pata_derecha_1x, pata_derecha_1y, pata_derecha_2x, pata_derecha_2y, "abajo") or HitBox_triangulo(620, 355, 900, 800, cabina_1x, cabina_1y, cabina_2x, cabina_2y, "abajo")) return true;
        else if (HitBox_triangulo(800, 400, 1250, 800, pata_izquierda_1x, pata_izquierda_1y, pata_izquierda_2x, pata_izquierda_2y, "abajo") or HitBox_triangulo(800, 400, 1250, 800, pata_derecha_1x, pata_derecha_1y, pata_derecha_2x, pata_derecha_2y, "abajo") or HitBox_triangulo(800, 400, 1250, 800, cabina_1x, cabina_1y, cabina_2x, cabina_2y, "abajo")) return true;
        else if (HitBox_triangulo(400, 330, 510, 0, pata_izquierda_1x, pata_izquierda_1y, pata_izquierda_2x, pata_izquierda_2y, "arriba") or HitBox_triangulo(400, 330, 510, 0, pata_derecha_1x, pata_derecha_1y, pata_derecha_2x, pata_derecha_2y, "arriba") or HitBox_triangulo(400, 330, 510, 280, cabina_1x, cabina_1y, cabina_2x, cabina_2y, "arriba")) return true;//lado derecho triangulo superior
        else if (HitBox_triangulo(380, 440, 500, 1000, pata_izquierda_1x, pata_izquierda_1y, pata_izquierda_2x, pata_izquierda_2y, "abajo") or HitBox_triangulo(380, 440, 500, 1000, pata_derecha_1x, pata_derecha_1y, pata_derecha_2x, pata_derecha_2y, "abajo") or HitBox_triangulo(380, 440, 500, 1000, cabina_1x, cabina_1y, cabina_2x, cabina_2y, "abajo")) return true;//lado derecho triangulo superior
    }

    else if (numero_nivel == 2 ){
       if (HitBox_triangulo(110, 150, 300, 800, pata_izquierda_1x, pata_izquierda_1y, pata_izquierda_2x, pata_izquierda_2y, "abajo") or
           HitBox_triangulo(110, 150, 300, 800, pata_derecha_1x, pata_derecha_1y, pata_derecha_2x, pata_derecha_2y, "abajo") or HitBox_triangulo(110, 150, 300, 800, cabina_1x, cabina_1y, cabina_2x, cabina_2y, "abajo")) return true;
       else if (HitBox_triangulo(305, 300, 510, 800, pata_izquierda_1x, pata_izquierda_1y, pata_izquierda_2x, pata_izquierda_2y, "abajo") or HitBox_triangulo(305, 300, 510, 800, pata_derecha_1x, pata_derecha_1y, pata_derecha_2x, pata_derecha_2y, "abajo") or HitBox_triangulo(305, 300, 510, 800, cabina_1x, cabina_1y, cabina_2x, cabina_2y, "abajo")) return true;
       else if (HitBox_triangulo(400, 0, 600, 650, pata_izquierda_1x, pata_izquierda_1y, pata_izquierda_2x, pata_izquierda_2y, "arriba") or HitBox_triangulo(400, 0, 600, 650, pata_derecha_1x, pata_derecha_1y, pata_derecha_2x, pata_derecha_2y, "arriba") or HitBox_triangulo(400, 0, 600, 650, cabina_1x, cabina_1y, cabina_2x, cabina_2y, "arriba")) return true;
       else if (HitBox_triangulo(100, 0, 250, 250, pata_izquierda_1x, pata_izquierda_1y, pata_izquierda_2x, pata_izquierda_2y, "arriba") or HitBox_triangulo(100, 0, 250, 250, pata_derecha_1x, pata_derecha_1y, pata_derecha_2x, pata_derecha_2y, "arriba") or HitBox_triangulo(100, 0, 250, 250, cabina_1x, cabina_1y, cabina_2x, cabina_2y, "arriba")) return true;


    }
    return false;

}

bool HitBox_triangulo (float x1, float y1, float x2, float y2,
                        float p1x, float p1y, float p2x, float p2y, string obstaculo){

    float m= (y2-y1)/(x2-x1);
    float b = y1-m*x1;

    if (obstaculo == "abajo"){
        if (m>0){
            if(x1 <= p1x and p1x <= x2){
                if(p2y >= m*p1x + b) return true;
            }
            else {
                if (p1x <= x1 and x1 <= p2x)
                    if(y1 <= p2y) return true;
            }
        }
        if (m<0){
            if(x1 <= p2x and p2x <= x2){
                if(p2y <= m*p2x + b) return true;
            }
            else {
                if (p1x <= x2 and x2 <= p2x)
                    if(y2 <= p2y) return true;
            }
        }
    }

    if (obstaculo == "arriba"){
        if (m>0){
            if(x1 <= p2x and p2x <= x2){
                if(p1y <= m*p2x + b) return true;
            }
            else {
                if (p1x <= x2 and x2 <= p2x)
                    if(p1y <= y2) return true;
            }
            return false;
        }
        else {
            if (x1 <= p1x and p1x <= x2){
                if(p1y <= m*p1x + b)return true;
            }
        else {
            if (p1x <= x1 and x1 <= p2x)
                if(p1y <= y1) return true;
        }
            return false;
        }
    }
    return false;
}

bool aterrizaje (float coordenada_x, float coordenada_y, float velocidad_x, float velocidad_y, BITMAP *buffer, int numero_nivel){

    if (coordenada_y+20 >= 747 and coordenada_x-20 >= 10 and coordenada_x+20 <= 100 ){
        if (velocidad_y <= 3.5){
            return true;
        }
    }
    return false;
}

bool bordes (float &coordenada_x, float &coordenada_y, float &velocidad_x, float &velocidad_y, int &numero_nivel, float &combustible, BITMAP *buffer){

    if (coordenada_x+20 >=1000 or coordenada_x-20 <= 0 or coordenada_y-15 <= 0 or coordenada_y+20 >= 800){
        destruir_nave(coordenada_x,coordenada_y,buffer,numero_nivel);
        reiniciar_nivel(coordenada_x, coordenada_y, velocidad_x, velocidad_y, combustible);
        //return true;
    }
    else if (choque_nave(numero_nivel, coordenada_x, coordenada_y) == true){
        destruir_nave(coordenada_x, coordenada_y, buffer, numero_nivel);
        reiniciar_nivel(coordenada_x, coordenada_y, velocidad_x, velocidad_y, combustible);
        //return true;
    }

    else if (coordenada_y+20 >= 747 and coordenada_x-20 >= 10 and coordenada_x+20 <= 100 and velocidad_y > 3.5 ){
        destruir_nave(coordenada_x, coordenada_y, buffer, numero_nivel);
        reiniciar_nivel(coordenada_x, coordenada_y, velocidad_x, velocidad_y, combustible);
    }

    else if (coordenada_x-20 <= 100 and 100 <= coordenada_x+20 and coordenada_y+20 >= 750){
        destruir_nave(coordenada_x, coordenada_y, buffer, numero_nivel);
        reiniciar_nivel(coordenada_x, coordenada_y, velocidad_x, velocidad_y, combustible);
    }
    return false;
}


void destruir_nave (float coordenada_x, float coordenada_y, BITMAP *buffer, int numero_nivel){

    // Coordenadas del pintado de las piezas de la nve al destruirse
    float x[14] = {coordenada_x-10, coordenada_x+10, coordenada_x,    coordenada_x,    coordenada_x+15, coordenada_x-15, coordenada_x+5, coordenada_x-10, coordenada_x+10, coordenada_x-5, coordenada_x-10,  coordenada_x-20,  coordenada_x-5,  coordenada_x-10};
    float y[14] = {coordenada_y,    coordenada_y,    coordenada_y-15, coordenada_y+15, coordenada_y-15, coordenada_y+15, coordenada_y+5, coordenada_y-10, coordenada_y-10, coordenada_y+10,coordenada_y, coordenada_y +40, coordenada_y+25, coordenada_y+50};

    // Coordenadas de movimiento de la animacion de las piezas de la nave al destruirse
    float dx[7] = {7,  7,  0,  -7, -7, 0,  5};
    float dy[7] = {0, -7, -7,  -7,   0,  7,  -5};

    // Sonido de explosion

    SAMPLE *explosion = load_wav("explosion_nave.wav");
    play_sample(explosion, 100, 200, 1000, 0);

    // Limpiar pantalla al tocar los bordes para mostrar el do
    clear(screen);
    do {
        clear(buffer);
        nivel(numero_nivel, buffer);
        contador_muertes = contador_muertes++;
        int j=0;

        for (int i = 0; i <=12 ; i +=2){
            // Recorrido del pintado de las piezas de la nave
            line(buffer, x[i], y[i], x[i+1], y[i+1], 0xFFFFFF);
            // Recorrido de las coordenadas de movimiento de la animacion de las piezas de la nave al destruirse
            rotacion(x[i+1], y[i+1], x[i], y[i], 15);

            x[i] += dx[j]; // Recorrido del pintado de la linea
            y[i] += dy[j];
            x[i+1] += dx[j]; // Recorrido del eje de la linea (pieza de la nave)
            y[i+1] += dy[j];
            j++;
        }
        textout_centre_ex(buffer, font, "Presiona <ENTER> para reiniciar el nivel", 500, 400, 0xFBFF00, 0x000000);
        textout_centre_ex(buffer, font, "Presiona <ESC> para salir del juego", 500, 420, 0xFBFF00, 0x000000);
        blit(buffer, screen, 0,0,0,0,1000,800);
        rest(20);

    }while(!key[KEY_ESC] and !key[KEY_ENTER]);
}

// Diseño de nivel
void nivel (int numero_nivel, BITMAP *buffer ){

    // Nivel inicial
    if(numero_nivel == 0){
        triangle (buffer, 400, 0, 600, 650, 600, 0, 0x999999);
        triangle (buffer, 305, 300, 600, 800, 310, 800, 0x999999);

        rectfill (buffer, 10,800,100, 750, 0x395161);// Plataforma de aterrizaje
    }

    // Nivel 1
    if(numero_nivel == 1){
        rectfill (buffer, 900,800,700, 555, 0x999999);// Cuadrado inferior interior izquierdo triangulo1 - triangulo 2
        triangle (buffer, 620, 800, 900, 800, 620, 350, 0x999999);//triangulo 2
        triangle (buffer, 110, 100, 300, 800, 110, 800, 0x999999);//triangulo 3
        rectfill (buffer, 400, 330, 500, 0, 0x999999); // Cuadrado superior
        rectfill (buffer, 444, 325, 500, 330, 0x000000); // Cuadrado superior
        rectfill (buffer, 444, 330, 500, 310, 0x000000); // Cuadrado superior
        triangle (buffer, 380, 440, 500, 1000, 400, 800, 0x999999); // Cuadrado inferior
        triangle (buffer, 900, 800, 1250, 800, 800, 400, 0x999999);// triangulo 1
        triangle (buffer, 500, 600, 1250, 800, 800, 400, 0x999999);// triangulo 4

        rectfill (buffer, 10,800,100, 750, 0x395161); // Plataforma de aterrizaje
    }
    //Nivel 2
    if(numero_nivel == 2){
        triangle (buffer, 110, 150, 300, 800, 110, 800, 0x999999); //Triangulo inferior izquierdo
        triangle (buffer, 305, 300, 510, 800, 310, 800, 0x999999);//Triangulo inferior derecho
        triangle (buffer, 400, 0, 600, 650, 600, 0, 0x999999);//Triangulo superior derecho
        triangle (buffer, 100, 0, 250, 250, 250, 0, 0x999999);//Triangulo superior izquierdo

        rectfill (buffer, 10,800,100, 750, 0x395161); // Plataforma de aterrizaje
    }
    if(numero_nivel == 3){
        fondo_final();
    }
}

// Medidor de combustible
void medidor_combustible (bool gastar_combustible, float &combustible, BITMAP *buffer){

    textout_centre_ex(buffer, font, "Combustible", 100, 30, 0x999999, 0x000000);
    rectfill (buffer, 50, 50, 50+combustible, 55, 0xf6000b); // Largo x1, tamaño y1, largo x2, tamaño y2, color del texto
    if (gastar_combustible == true) combustible -= 0.2;
}


// Dibujar motor de la nave
void motor(float da, float coordenada_x, float coordenada_y, BITMAP *buffer){

    float c1,c2 ;
    c1 = coordenada_x,  c2 = coordenada_y;
    if(da !=0) c2+=9;
    float fuego_motor[14] = {c1-5,c2+5 , c1-10,c2+20 , c1-5,c2+20 , c1,c2+35 , c1+5,c2+20 , c1+10,c2+20 , c1+5,c2+5};

    for(int i = 0; i <=12; i +=2){
        rotacion(fuego_motor[i], fuego_motor[i+1],coordenada_x, coordenada_y, da);
    }
    for(int i = 0; i <=10; i +=2){
        line(buffer,fuego_motor[i],fuego_motor[i+1], fuego_motor[i+2],fuego_motor[i+3], 0xdf302a);
    }
}

// Aceleracion de la nave
void aceleracion (float da, float &velocidad_x, float &velocidad_y){

    float ax = 0;
    float gravedad = -0.2;
    rotacion(ax, gravedad, 0, 0, da);
    velocidad_x += ax;
    velocidad_y += gravedad;
}

// Coordenadas de dibujado de la nave
void pintar_nave(float coordenada_x, float coordenada_y, BITMAP *buffer) {

    line(buffer, coordenada_x-20,coordenada_y+20, coordenada_x-20,coordenada_y+10, 0xeaeae6);
    line(buffer, coordenada_x-20,coordenada_y+20, coordenada_x-20,coordenada_y+10, 0xeaeae6);
    line(buffer, coordenada_x-20,coordenada_y+10, coordenada_x-10,coordenada_y, 0xeaeae6);
    line(buffer, coordenada_x-10,coordenada_y, coordenada_x-10,coordenada_y-10, 0xeaeae6);
    line(buffer, coordenada_x-10,coordenada_y-10, coordenada_x,coordenada_y-15, 0xeaeae6);
    line(buffer, coordenada_x,coordenada_y-15, coordenada_x+10,coordenada_y-10, 0xeaeae6);
    line(buffer, coordenada_x+10,coordenada_y-10, coordenada_x+10,coordenada_y, 0xeaeae6);
    line(buffer, coordenada_x+10,coordenada_y, coordenada_x+20,coordenada_y+10, 0xeaeae6);
    line(buffer, coordenada_x+20,coordenada_y+10, coordenada_x+20,coordenada_y+20, 0xeaeae6);
    // Linea inferior de la nave
    line(buffer, coordenada_x-10,coordenada_y, coordenada_x+10,coordenada_y, 0xeaeae6);
}

// Funcion caida libre o funcion de gravedad de la nave
void gravedad_nave (float &coordenada_x, float &coordenada_y, float &velocidad_x, float &velocidad_y ) {

    float ax,gravedad;
    ax= 0.0;
    gravedad= 0.1; // Valor de la gravedad

    velocidad_x += ax;
    velocidad_y += gravedad; // Valor de la velocidad y en funcion a la gravedad constante

    coordenada_x += velocidad_x;
    coordenada_y += velocidad_y; // Dibujado de la nave durante la caida en la gravedad
}

void rotacion (float &x, float &y, float coordenada_x, float coordenada_y, float da){

    float dx = x - coordenada_x;
    float dy = y - coordenada_y;
    float r = sqrt(dx*dx + dy*dy);
    float a = atan2(dy,dx);
    float da_rad = da/180 * M_PI;
    a -= da_rad;

    x = coordenada_x + r*cos(a);
    y = coordenada_y + r*sin(a);
}
