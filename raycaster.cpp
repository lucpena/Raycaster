#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdint>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "map.h"
#include "utils.h"
#include "player.h"
#include "sprite.h"
#include "framebuffer.h"
#include "textures.h"

int wall_x_texcoord(const float hitx, const float hity, Texture &tex_walls){
	// hitx and hity contain (signed) fractional parts of cx and cy
	// They vary between -0.5 and +0.5, and one of them is supposed to be very close to 0
	float x = hitx - floor(hitx+.5);
	float y = hity - floor(hity+.5);

	int tex = x * tex_walls.size;

	// Here we need to determine whether we hit a vertical or a horizontal wall 
	if(std::abs(y) > std::abs(x)) {
		tex = y * tex_walls.size;
	}

	// Do not forget x_texcoord can be negative, let's fix that
	if(tex < 0) {
		tex += tex_walls.size;
	}

	assert(tex >= 0 && tex < (int)tex_walls.size);

	return tex;
}

void map_show_sprite(Sprite &sprite, FrameBuffer &fb, Map &map){
	// Size of one map cell on the screen
	const size_t rect_w = fb.w/(map.w * 2);

	const size_t rect_h = fb.h/map.h;
	fb.draw_rectangle(sprite.x * rect_w - 3, sprite.y * rect_h - 3, 6, 6, pack_color(255, 0, 0));
}

void draw_sprite(Sprite &sprite, std::vector<float> &depth_buffer, FrameBuffer &fb, Player &player, Texture &tex_sprites){
	// Absolute direction from the player to the sprite (in radians)
	float sprite_dir = atan2(sprite.y - player.y, sprite.x - player.x);

	// Remove unnesessary periods from the relative direction
	while (sprite_dir - player.a >  M_PI) sprite_dir -= 2 * M_PI;
	while (sprite_dir - player.a < -M_PI) sprite_dir += 2 * M_PI;

	// Screen sprite Size
	size_t sprite_screen_size = std::min(1000, static_cast<int>(fb.h / sprite.player_dist));

	// Do not forget the 3D view takes only a half of the framebuffer
	int h_offset = (sprite_dir - player.a)*(fb.w/2) + (fb.w/2)/2 - tex_sprites.size/2;
	int v_offset = fb.h / 2 - sprite_screen_size / 2;

	for (size_t i = 0; i < sprite_screen_size; i++) {
		if (h_offset + int(i) < 0 || h_offset + i >= fb.w / 2) continue;

		// This sprite column is occluded
		if(depth_buffer[h_offset + i] < sprite.player_dist) continue;

		for(size_t j = 0; j < sprite_screen_size; j++){
			if(v_offset + int(j) < 0 || v_offset + j >= fb.h) continue;
			uint32_t color = tex_sprites.get(i * tex_sprites.size / sprite_screen_size, j * tex_sprites.size / sprite_screen_size, sprite.tex_id);
			uint8_t r, g, b, a;
			unpack_color(color, r, g, b, a);

			if (a > 128) {
				fb.set_pixel(fb.w / 2 + h_offset + i, v_offset + j, color);
			}

		}
	}
}

void render(FrameBuffer &fb, Map &map, Player &player, std::vector<Sprite> &sprites, Texture &tex_walls, Texture &tex_monst){
	// Clear Screen
	fb.clear(pack_color(255,255,255));

	const size_t rect_w = fb.w / (map.w*2);
	const size_t rect_h = fb.h / map.h;

	for(size_t j=0; j < map.h; j++){
		for(size_t i = 0; i < map.w; i++){
			if(map.is_empty(i,j)) continue;

			size_t rect_x = i * rect_w;
			size_t rect_y = j * rect_h;
			size_t texid = map.get(i, j);
			assert(texid < tex_walls.count);

			// The color is taken from the upper left pixel of the texture #texid
			fb.draw_rectangle(rect_x, rect_y, rect_w, rect_h, tex_walls.get(0, 0, texid));
		}
	}

	// 1e3 == 1000, but it's a double
	std::vector<float> depth_buffer(fb.w / 2, 1e3);

	// Draw the visibility cone AND the pseud0-3D view
	for (size_t i = 0; i < fb.w / 2; i++) {
		float angle = player.a - player.fov / 2 + player.fov * i / float(fb.w / 2);

		// Ray Marching loop
		for(float t = 0; t < 20; t +=.01){
			float x = player.x + t * cos(angle);
			float y = player.y + t * sin(angle);

			// Draws the visibility cone
			fb.set_pixel(x * rect_w, y * rect_h, pack_color(160, 160, 160));

			if (map.is_empty(x, y)) continue;

			// If the ray casted from the player touches a wall, we draw the vertical
			// column to create the illusion of 3D
			size_t texid = map.get(x, y);
			assert(texid < tex_walls.count);
			float dist = 0.2 + t * cos(angle - player.a);
			depth_buffer[i] = dist;
			size_t column_height = fb.h/dist;

			int x_texcoord = wall_x_texcoord(x, y, tex_walls);
			std::vector<uint32_t> column = tex_walls.get_scaled_column(texid, x_texcoord, column_height);

			// We are drawing at the right half of the screen, thus +fb.w/2
			int pix_x = i + fb.w / 2;

			for(size_t j = 0; j < column_height; j++){
				int pix_y = j + fb.h / 2 - column_height / 2;

				if(pix_y >= 0 && pix_y < (int)fb.h){
					fb.set_pixel(pix_x, pix_y, column[j]);
				}
			}

			break;

		} // Ray Maching loop
	} // FoV loop

	// Update the distances from the player to each sprite
	for(size_t i = 0; i < sprites.size(); i++){
		sprites[i].player_dist = std::sqrt(pow(player.x - sprites[i].x, 2) + pow(player.y - sprites[i].y, 2));
	}

	// Sort it from farthest to closest
	std::sort(sprites.begin(), sprites.end());

	// Draw the sprites
	for(size_t i = 0; i < sprites.size(); i++){
		map_show_sprite(sprites[i], fb, map);
		draw_sprite(sprites[i], depth_buffer, fb, player, tex_monst);
	}
}

/*
int main(){
	drop_ppm_image("./out.ppm", fb.img, fb.w, fb.h);

	return 0;
}
*/