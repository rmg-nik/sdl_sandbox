#include <memory>
#include <functional>
#include <string>
#include <cassert>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

using WindowPtr = std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>>;
using RenderPtr = std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer*)>>;
using TexturePtr = std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>>;
using SoundPtr = std::unique_ptr<Mix_Chunk, std::function<void(Mix_Chunk*)>>;

WindowPtr window = nullptr;
RenderPtr render = nullptr;
TexturePtr texture_awesome = nullptr;
SoundPtr sound_hit = nullptr;

constexpr int MAX_CHANNELS = 1024;

struct RenderData
{
	int design_w;
	int design_h;
	int offset_x;
	int offset_y;
	int output_w;
	int output_h;
	float scale_x;
	float scale_y;
};

SDL_Point get_window_size(bool fullscreen)
{
	SDL_Rect window_bounds;
	if (SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &window_bounds) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GetDisplayBounds error: %s", SDL_GetError());
		exit(-1);
	}
	SDL_Point ret{};
	ret.x = window_bounds.w;
	ret.y = window_bounds.h;
	if (!fullscreen)
	{
		ret.x = static_cast<int>(round(0.8 * ret.x));
		ret.y = static_cast<int>(round(0.8 * ret.y));
	}
	return ret;
}

RenderData get_render_data(const RenderPtr& renderer)
{
	constexpr int DESIGN_W = 1920;
	constexpr int DESIGN_H = 1080;
	RenderData ret;
	SDL_GetCurrentRenderOutputSize(renderer.get(), &ret.output_w, &ret.output_h);
	ret.design_w = DESIGN_W;
	ret.design_h = DESIGN_H;
	ret.scale_x = (float)ret.output_w / (float)ret.design_w;
	ret.scale_y = (float)ret.output_h / (float)ret.design_h;
	ret.scale_x = ret.scale_y = std::min(ret.scale_x, ret.scale_y);
	ret.offset_x = (int)(ret.output_w / ret.scale_x - ret.design_w) / 2;
	ret.offset_y = (int)(ret.output_h / ret.scale_y - ret.design_h) / 2;

	return ret;
}

void setup_render(const RenderPtr& renderer, const RenderData& data)
{
	SDL_Rect viewport = {
		int(data.offset_x * data.scale_x),
		int(data.offset_y * data.scale_y),
		int(data.design_w * data.scale_x),
		int(data.design_h * data.scale_y)
	};

	SDL_SetRenderScale(renderer.get(), 1.0f, 1.0f);
	SDL_SetRenderClipRect(renderer.get(), &viewport);
	SDL_SetRenderScale(renderer.get(), data.scale_x, data.scale_y);
}

void clear_screen(const RenderPtr& renderer, const SDL_Color& color)
{
	SDL_Rect cliprect;
	SDL_GetRenderClipRect(renderer.get(), &cliprect);
	SDL_GetRenderClipRect(renderer.get(), nullptr);
	SDL_SetRenderDrawColor(renderer.get(), color.r, color.g, color.b, color.a);
	SDL_RenderClear(renderer.get());
	SDL_SetRenderClipRect(renderer.get(), &cliprect);
}

TexturePtr load_texture(const RenderPtr& render, const std::string& path)
{
	auto sfc = IMG_Load(path.c_str());
	if (!sfc)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Could not load image '%s': %s", path.c_str(), SDL_GetError());
		return nullptr;
	}

	auto txt = SDL_CreateTextureFromSurface(render.get(), sfc);
	SDL_DestroySurface(sfc);

	if (!txt)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Could not create texture from image '%s': %s", path.c_str(), SDL_GetError());
		return nullptr;
	}

	TexturePtr ret = TexturePtr(txt, SDL_DestroyTexture);
	return std::move(ret);
}

SoundPtr load_sound(const std::string& path)
{
	auto sample = Mix_LoadWAV(path.c_str());
	if (!sample)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Could not load audio '%s': %s", path.c_str(), SDL_GetError());
		return nullptr;
	}

	SoundPtr ret = SoundPtr(sample, Mix_FreeChunk);
	return std::move(ret);
}

TexturePtr get_label_texture(const std::string& text, const std::string& font_path, int fontsize, const SDL_Color& color)
{
	TexturePtr ret;
	if (!text.empty())
	{
		if (auto ttf_font = TTF_OpenFont(font_path.c_str(), fontsize))
		{
			if (auto sfc = TTF_RenderText_Blended(ttf_font, text.c_str(), color))
			{
				if (auto txt = SDL_CreateTextureFromSurface(render.get(), sfc))
					ret = TexturePtr(txt, SDL_DestroyTexture);
				else
					SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Could not create texture from surface: %s", SDL_GetError());
				SDL_DestroySurface(sfc);
			}
			else
				SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Could not render text '%s': %s", text.c_str(), SDL_GetError());
			TTF_CloseFont(ttf_font);
		}
		else
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Could not load font '%s': %s", font_path.c_str(), SDL_GetError());
	}
	else
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "get_label_texture: text shouldn't be empty");

	return std::move(ret);
}

void play_sound(const SoundPtr& sound)
{
	static int current_channel = 0;
	Mix_VolumeChunk(sound.get(), MIX_MAX_VOLUME);
	if (Mix_PlayChannel(current_channel, sound.get(), 0) < 0)
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Could not play audio: %s", SDL_GetError());
	if (++current_channel == MAX_CHANNELS)
		current_channel = 0;
}

SDL_FRect translate_destination(SDL_FRect dst, const RenderData& render_data)
{
	dst.x += render_data.offset_x;
	dst.y += render_data.offset_y;
	return dst;
}

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	Mix_Init(MIX_INIT_OGG);
	TTF_Init();

	if (Mix_OpenAudio(0, NULL) < 0)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Could not open audio: %s", SDL_GetError());
	}
	else
	{
		Mix_AllocateChannels(MAX_CHANNELS);
	}

	bool fullscreen = false;
#ifdef ANDROID
	fullscreen = true;
#endif
	auto window_size = get_window_size(fullscreen);

	window = WindowPtr(SDL_CreateWindow(
		"Sandbox", 
		window_size.x,
		window_size.y,
		SDL_WINDOW_OPENGL | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0)
	), SDL_DestroyWindow);
	assert(window);
	
	SDL_PropertiesID props = SDL_CreateProperties();
  SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, window.get());
  SDL_SetStringProperty(props, SDL_PROP_RENDERER_CREATE_NAME_STRING, nullptr);
  SDL_SetNumberProperty(props, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER, 1);

	render = RenderPtr(SDL_CreateRendererWithProperties(props), SDL_DestroyRenderer);
	assert(render);

	texture_awesome = load_texture(render, "assets/awesomeface.png");
	assert(texture_awesome);

	sound_hit = load_sound("assets/sound_effect.ogg");
	assert(sound_hit);
	
	bool working = true;
	SDL_FRect dst = { 0.f, 0.f, 100, 100 };
	SDL_FPoint direction = { (rand() % 100) / 100.f, (rand() % 100) / 100.f };
	float speed = 500.f;
	float accum = 0.f;
	constexpr float fixed_time_step = 0.01f;
	auto last_tick = SDL_GetTicks();
	auto render_data = get_render_data(render);
	setup_render(render, render_data);
	int hits_count = 0;
	while (working)
	{
		SDL_Event evt;
		while (SDL_PollEvent(&evt))
		{
			switch (evt.type)
			{
			case SDL_EVENT_QUIT: 
				working = false;
				break;
			}
		}
		auto curr_tick = SDL_GetTicks();
		auto dt = curr_tick - last_tick;
		last_tick = curr_tick;
		accum += (dt / 1000.f);
		while (accum >= fixed_time_step)
		{
			accum -= fixed_time_step;
			dst.x += direction.x * speed * fixed_time_step;
			if (dst.x + dst.w > render_data.design_w || dst.x < 0)
			{
				++hits_count;
				play_sound(sound_hit);
				direction.x *= -1;
				speed += 1.f;
				dst.x = SDL_clamp(dst.x, 0, render_data.design_w - dst.w);
			}

			dst.y += direction.y * speed * fixed_time_step;
			if (dst.y + dst.h > render_data.design_h || dst.y < 0)
			{
				++hits_count;
				play_sound(sound_hit);
				direction.y *= -1;
				speed += 1.f;
				dst.y = SDL_clamp(dst.y, 0, render_data.design_h - dst.h);
			}
		}

		clear_screen(render, { 0, 0, 0, 255 });
		SDL_SetRenderDrawColor(render.get(), 25, 25, 25, 255);
		SDL_FRect bg{ 0.f, 0.f, (float)render_data.design_w, (float)render_data.design_h };
		bg = translate_destination(bg, render_data);
		SDL_RenderFillRect(render.get(), &bg);
		SDL_FRect tdst = translate_destination(dst, render_data);
		SDL_RenderTexture(render.get(), texture_awesome.get(), nullptr, &tdst);
		SDL_RenderPresent(render.get());
	}

	sound_hit.reset();
	texture_awesome.reset();
	render.reset();
	window.reset();

	Mix_CloseAudio();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();

	return 0;
}
