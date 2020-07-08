#include <iostream>
#include <vector>
#include <SDL.h>

#include "map.h"
#include "utils.h"
#include "player.h"
#include "sprite.h"
#include "framebuffer.h"
#include "textures.h"
#include "raycaster.h"

int main(int argc, char* args[]){
    FrameBuffer fb{1024, 512, std::vector<uint32_t>(1024*512, pack_color(255, 255, 255))};
	Player player{3.456, 2.345, 1.523, M_PI/3., 0, 0};
	Map map;
	Texture tex_walls("walltext.png");
	Texture tex_monst("monsters.png");

	if(!tex_walls.count || !tex_monst.count){
		std::cerr << "Error: Failed to load textures." << std::endl;
		return -1;
	}

	 std::vector<Sprite> sprites{ {3.523, 3.812, 2, 0}, {1.834, 8.765, 0, 0}, {5.323, 5.365, 1, 0}, {4.123, 10.765, 1, 0}, {13, 12.5, 3, 0} };

    // Init SDL
    SDL_Window    *window   = nullptr;
    SDL_Renderer  *renderer = nullptr;

    if(SDL_Init(SDL_INIT_VIDEO)){
        std::cerr << "Error: Could not initializate SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    if(SDL_CreateWindowAndRenderer(fb.w, fb.h, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS, &window, &renderer)){
        std::cerr << "Error: Could not create window and renderer: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Texture *framebuffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, fb.w, fb.h);
    SDL_Event event;
    
    while(1){
        if(SDL_PollEvent(&event)){
            if(SDL_QUIT == event.type || (SDL_KEYDOWN == event.type && SDLK_ESCAPE == event.key.keysym.sym)) break;
            if(SDL_KEYUP == event.type){
                if('a' == event.key.keysym.sym || 'd' == event.key.keysym.sym) player.turn = 0;
                if('w' == event.key.keysym.sym || 's' == event.key.keysym.sym) player.walk = 0;
            }

            if(SDL_KEYDOWN == event.type){
                if('a' == event.key.keysym.sym) player.turn = -1;
                if('d' == event.key.keysym.sym) player.turn =  1;
                if('w' == event.key.keysym.sym) player.walk =  1;
                if('s' == event.key.keysym.sym) player.walk = -1;
            }
        }

        player.a += float(player.turn)*.025;
        float nx = player.x + player.walk * cos(player.a) * .035;
        float ny = player.y + player.walk * sin(player.a) * .035;

        if(int(nx) >= 0 && int(nx) < int(map.w) && int(ny) >= 0 && int(ny) < int(map.h) && map.is_empty(nx, ny)){
            player.x = nx;
            player.y = ny;
        }

	    render(fb, map, player, sprites, tex_walls, tex_monst);
        SDL_UpdateTexture(framebuffer_texture, NULL, reinterpret_cast<void *>(fb.img.data()), fb.w * 4);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, framebuffer_texture, NULL, NULL);
        SDL_RenderPresent(renderer);

    }

    SDL_DestroyTexture(framebuffer_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;

	// --------------------------------------------------------------------

	// for (size_t frame = 0; frame < 360; frame++) {
	// 	std::stringstream ss;
	// 	ss << std::setfill('0') << std::setw(5) << frame << ".ppm";
	// 	player.a += 2 * M_PI / 360;

	// 	render(fb, map, player, tex_walls);
	// 	drop_ppm_image(ss.str(), fb.img, fb.w, fb.h);

	// }


	// --------------------------------------------------------------------

	// Draw the 4th texture on the screen
	// const size_t texid = 4;
	// for(size_t i = 0; i < walltext_size; i++){
	// 	for(size_t j = 0; j < walltext_size; j++){
	// 		framebuffer[i + j * win_w] = walltext[i + texid * walltext_size + j * walltext_size * walltext_cnt];
	// 	}
	// }

	// --------------------------------------------------------------------

	// Draw the Player on the map

	// draw_rectangle(framebuffer, win_w, win_h, player_x * rect_w, player_y * rect_h, 5, 5, pack_color(255, 255, 255));

	// --------------------------------------------------------------------

	// Draw the Line of View
	// This only shows one line, but we need all the lines that the palyer is looking

	// for (float t = 0; t < 20; t += .05){
	// 	float cx = player_x + t * cos(player_a);
	// 	float cy = player_y + t * sin(player_a);
	// 	if (map[int(cx) + int(cy) * map_w] != ' ') break;

	// 	size_t pix_x = cx * rect_w;
	// 	size_t pix_y = cy * rect_h;
	// 	framebuffer[pix_x + pix_y * win_w] = pack_color(255,255,255);
	// }

	// --------------------------------------------------------------------

	// Draw all the lines. A Cone of Sight	

	// for(size_t i = 0; i < win_w; i++){
	// 	float angle = player_a - fov/2 + fov * i / float(win_w);
	// 	for(float t = 0; t < 20; t += .05){
	// 		float cx = player_x + t * cos(angle);
	// 		float cy = player_y + t * sin(angle);
	// 		if(map[int(cx) + int(cy) * map_w] != ' ') break;
	// 		size_t pix_x = cx * rect_w;
	// 		size_t pix_y = cy * rect_h;
	// 		framebuffer[pix_x + pix_y * win_w] = pack_color(255, 255, 255);
	// 	}
	// }

	// --------------------------------------------------------------------

}