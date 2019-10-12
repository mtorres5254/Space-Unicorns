#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#define MAX_FRAMES 30000

#include "SDL/include/SDL_timer.h"
#include "SDL/include/SDL_rect.h"


class Animation
{
public:
	SDL_Rect frames[MAX_FRAMES];
	float speed = 1.0f;
public:
	float endframe = 0;
	int last_frame = 0;
	float current_frame;

		void PushBack(const SDL_Rect& rect) {
			frames[last_frame++] = rect;
	}

		SDL_Rect& GetCurrentFrame()
		{
			current_frame += speed;
			
			if (current_frame >= last_frame) {
				current_frame = 0;
				endframe = 1;
			}
			else endframe = 0;

		}

		int GetEndframe() {
			return endframe;
		}
		void Finished() {
			endframe = 1;
		}
		//add reset(?)
};
#endif