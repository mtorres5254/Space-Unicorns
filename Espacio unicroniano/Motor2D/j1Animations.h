#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#define MAX_FRAMES 50000

#include "SDL/include/SDL_timer.h"
#include "SDL/include/SDL_rect.h"


class Animation
{
public:
	SDL_Rect frames[MAX_FRAMES];
	float speed = 1.0f;
	bool loop = true;

private:
	float endframe = 0;
	int last_frame = 0;
	int loops = 0;
	float current_frame;

public:
		void PushBack(const SDL_Rect& rect) {
			frames[last_frame++] = rect;
		}

		SDL_Rect& GetCurrentFrame()
		{
			current_frame += speed * App->dt;
			if (current_frame >= last_frame)
			{
				current_frame = ((loop) ? 0.0f : last_frame - 1);
				loops++;
			}

			return frames[(int)current_frame];
		}
		void Reset()
		{
			current_frame = 0;
		}
		int GetEndframe() {
			return endframe;
		}
		bool Finished() {
			return loops > 0;
		}
		//add reset(?)
};
#endif