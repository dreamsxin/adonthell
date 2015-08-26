/*
   Copyright (C) 2003 Alexandre Courbot <alexandrecourbot@linuxgames.com>
   Copyright (C) 2011 Kai Sterker <kai.sterkerr@gmail.com>
   Part of the Adonthell Project http://adonthell.linuxgames.com

   Adonthell is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Adonthell is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Adonthell; if not, write to the Free Software 
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include <iostream>
#include <algorithm>

#include "surface_sdl.h"
#include "screen_sdl.h"

namespace gfx
{
    SDL_Rect surface_sdl::srcrect; 
    SDL_Rect surface_sdl::dstrect; 

    surface_sdl::surface_sdl() : surface_ext () 
    { 
        Surface = NULL;
        Info = new pixel_info();
        mask_changed = false; 
    }

    surface_sdl::~surface_sdl() 
    {
        delete Info;
        if (Surface) SDL_DestroyTexture (Surface);
    }

    void surface_sdl::set_mask (bool m)
    {
        // since SDL textures do not support masking, we need to
        // convert the masked image into the appropriate RGBA
        // representation. This behavior is different than the
        // SDL 1.2 backend, but with the current usage of set_mask,
        // we can get away by doing this here.

        if (!Surface)
        {
            LOG(FATAL) << "surface::set_mask called before creating surface!";
        }

        if (m && m != is_masked ())
        {
            is_masked_ = true;
       
            SDL_Surface *s1 = to_sw_surface();
            SDL_Surface *s2 = SDL_CreateRGBSurfaceFrom(NULL, length(), height(),
                            32, 0, R_MASK, G_MASK, B_MASK, A_MASK);

            Info->Format = s2->format->format;

            SDL_Texture *tmp = SDL_CreateTexture (display->get_renderer(), Info->Format, SDL_TEXTUREACCESS_STREAMING, length(), height());
            if (!tmp) LOG(ERROR) << "*** surface_sdl::set_mask: " << SDL_GetError();

            SDL_LockTexture(tmp, NULL, &s2->pixels, &s2->pitch);

            SDL_SetSurfaceAlphaMod (s1, SDL_ALPHA_OPAQUE);
            SDL_SetSurfaceBlendMode (s1, SDL_BLENDMODE_NONE);
            SDL_BlitSurface (s1, NULL, s2, NULL);

            SDL_UnlockTexture(tmp);

            SDL_FreeSurface(s1);
            SDL_FreeSurface(s2);
            SDL_DestroyTexture(Surface);

            Surface = tmp;
            alpha_channel_ = true;
        }
    }

    void surface_sdl::set_alpha (const u_int8 & t, const bool & alpha_channel)
    {
        if ((t == 255) && (alpha_ != 255) && Surface)
        {
            SDL_SetTextureAlphaMod(Surface, t);
            SDL_SetTextureBlendMode(Surface, SDL_BLENDMODE_NONE);
        }
        
        else if (!alpha_channel && alpha_channel_ && Surface)
        {
            SDL_SetTextureBlendMode(Surface, SDL_BLENDMODE_NONE);
        }

        alpha_ = t;
        alpha_channel_ = alpha_channel || is_masked_;
    }

    SDL_Surface *surface_sdl::to_sw_surface(SDL_Rect *rect) const
    {
        int bpp;
        u_int32 rmask, gmask, bmask, amask;

        lock(rect);

        SDL_PixelFormatEnumToMasks(Info->Format, &bpp, &rmask, &gmask, &bmask, &amask);
        SDL_Surface *s = SDL_CreateRGBSurfaceFrom(Info->Pixels, length(), height(),
                bpp, Info->Pitch, rmask, gmask, bmask, amask);

        if (is_masked_)
        {
            u_int32 trans_col = alpha_channel_? SDL_MapRGBA(s->format, 0xFF, 0x00, 0xFF, 0xFF) : SDL_MapRGB(s->format, 0xFF, 0x00, 0xFF);
            SDL_SetColorKey(s, 1, trans_col);
        }

        if (alpha_channel_ || alpha_ != 255)
        {
            if (!alpha_channel_ || is_masked_) SDL_SetSurfaceAlphaMod (s, alpha_);
            SDL_SetSurfaceBlendMode (s, SDL_BLENDMODE_BLEND);
        }

        return s;
    }

    void surface_sdl::draw (s_int16 x, s_int16 y, s_int16 sx, s_int16 sy, u_int16 sl,
                            u_int16 sh, const drawing_area * da_opt,
                            surface * target) const
    { 
        setup_rects (x, y, sx, sy, sl, sh, da_opt); 

        if (!dstrect.w || !dstrect.h)
            return;

        if (!target || target == display)
        {
            // blit to screen surface (--> hardware accelerated)
            if (alpha_channel_ || alpha_ != 255)
            {
                if (!alpha_channel_ || is_masked_) SDL_SetTextureAlphaMod(Surface, alpha_);
                SDL_SetTextureBlendMode(Surface, SDL_BLENDMODE_BLEND);
            }

            if (base::Scale > 1)
            {
                dstrect.x *= base::Scale;
                dstrect.y *= base::Scale;
                dstrect.w *= base::Scale;
                dstrect.h *= base::Scale;
            }

            SDL_RenderCopy (display->get_renderer(), Surface, &srcrect, &dstrect);
        }
        else
        {
            // blit from one surface to another (--> needs to be in software)
            SDL_Surface *source_surf = to_sw_surface ();
            SDL_Surface *target_surf = ((surface_sdl*) target)->to_sw_surface ();

            SDL_BlitSurface (source_surf, &srcrect, target_surf, &dstrect);

            target->unlock();

            SDL_FreeSurface(source_surf);
            SDL_FreeSurface(target_surf);
        }
    }

    void surface_sdl::fillrect (s_int16 x, s_int16 y, u_int16 l, u_int16 h, u_int32 col, 
                                drawing_area * da_opt)
    {
        if (da_opt) 
        {
            drawing_area da = da_opt->setup_rects (); 
            dstrect.x = da.x();
            dstrect.y = da.y();
            dstrect.w = da.length();
            dstrect.h = da.height();
        }
        else
        { 
            dstrect.x = x;
            dstrect.y = y;
            dstrect.w = l;
            dstrect.h = h;
        }

        if (this == display)
        {
            u_int8 r, g, b, a;
            unmap_color(col, r, g, b, a);

            if (base::Scale > 1)
            {
                dstrect.x *= base::Scale;
                dstrect.y *= base::Scale;
                dstrect.w *= base::Scale;
                dstrect.h *= base::Scale;
            }

            SDL_SetRenderDrawBlendMode(display->get_renderer(), SDL_BLENDMODE_NONE);
            SDL_SetRenderDrawColor(display->get_renderer(), r, g, b, a);
            SDL_RenderFillRect(display->get_renderer(), &dstrect);
        }
        else
        {
            lock(&dstrect);

            u_int32 *src = new u_int32[dstrect.w * dstrect.h];
            std::fill_n(src, dstrect.w * dstrect.h, col);

            SDL_ConvertPixels (dstrect.w, dstrect.h,
                SDL_PIXELFORMAT_ARGB8888, (const void*) src, dstrect.w*4,
                Info->Format, Info->Pixels, Info->Pitch);

            delete[] src;
            unlock();
        }
    }

    // convert RGBA color to surface format
    u_int32 surface_sdl::map_color (const u_int8 & r, const u_int8 & g, const u_int8 & b, const u_int8 & a) const
    {
        return (a << 24) | (r << 16) | (g << 8) | b;
    }

    // convert surface color format into RGBA
    void surface_sdl::unmap_color(u_int32 col, u_int8 & r, u_int8 & g, u_int8 & b, u_int8 & a) const
    {
        a = (col & 0xFF000000) >> 24;
        r = (col & 0x00FF0000) >> 16;
        g = (col & 0x0000FF00) >> 8;
        b = (col & 0x000000FF);
    }

    void surface_sdl::lock (SDL_Rect *rect) const
    {
        if (!length () || !height ()) return;

        if (this != display)
        {
            SDL_LockTexture (Surface, rect, &Info->Pixels, &Info->Pitch);
            Info->BytesPerPixel = SDL_BYTESPERPIXEL(Info->Format);
        }
    }

    void surface_sdl::unlock () const
    {
        if (!length () || !height ()) return;   

        if (Info->Pixels)
        {
            SDL_UnlockTexture (Surface);
            Info->Pixels = NULL;
        }
    }

    void surface_sdl::put_pix (u_int16 x, u_int16 y, u_int32 col) 
    {
        u_int8 r, g, b, a;

        if (this == display)
        {
            unmap_color(col, r, g, b, a);
            
            if (base::Scale > 1)
            {
                fillrect(x, y, 1, 1, col);
                return;
            }

            SDL_SetRenderDrawBlendMode(display->get_renderer(), SDL_BLENDMODE_NONE);
            SDL_SetRenderDrawColor(display->get_renderer(), r, g, b, a);
            SDL_RenderDrawPoint(display->get_renderer(), x, y);
            return;
        }

        if (!Info->Pixels) return;

        u_int8 *offset = ((Uint8 *) Info->Pixels) + y * Info->Pitch + x * Info->BytesPerPixel;

        if (!alpha_channel_)
        {
            unmap_color(col, r, g, b, a);
            
            switch (Info->Format)
            {
                case SDL_PIXELFORMAT_RGB24:
                    *(offset) = r;
                    *(++offset) = g;
                    *(++offset) = b;
                    break;
                case SDL_PIXELFORMAT_BGR24:
                    *(offset) = b;
                    *(++offset) = g;
                    *(++offset) = r;
                    break;
            }
            return;
        }

#ifdef __BIG_ENDIAN__
        unmap_color(col, b, a, g, r);
#else
        unmap_color(col, r, g, b, a);
#endif
                
        switch (Info->Format)
        {
            case SDL_PIXELFORMAT_BGR888:
                *(++offset) = r;
                *(++offset) = g;
                *(++offset) = b;
                break;
            case SDL_PIXELFORMAT_RGBA8888:
                *(offset) = a;
                *(++offset) = r;
                *(++offset) = g;
                *(++offset) = b;
                break;
            case SDL_PIXELFORMAT_ARGB8888:
                *(offset) = r;
                *(++offset) = g;
                *(++offset) = b;
                *(++offset) = a;
                break;
            case SDL_PIXELFORMAT_RGB888:
                *(++offset) = b;
                *(++offset) = g;
                *(++offset) = r;
                break;
            case SDL_PIXELFORMAT_ABGR8888:
                *(offset) = b;
                *(++offset) = g;
                *(++offset) = r;
                *(++offset) = a;
                break;
            case SDL_PIXELFORMAT_BGRA8888:
                *(offset) = a;
                *(++offset) = b;
                *(++offset) = g;
                *(++offset) = r;
                break;
            default:
                LOG(FATAL) << "*** sdl::put_pix: Unsupported format " << SDL_GetPixelFormatName(Info->Format);
                break;
        }
    }

    u_int32 surface_sdl::get_pix (u_int16 x, u_int16 y) const
    {
        if (!Info->Pixels) return 0;

        u_int8 r, g, b, a = SDL_ALPHA_OPAQUE;
        u_int8 *offset = ((Uint8 *) Info->Pixels) + y * Info->Pitch + x * Info->BytesPerPixel;

        if (!alpha_channel_)
        {
            switch (Info->Format)
            {
                case SDL_PIXELFORMAT_RGB24:
                    r = *(offset);
                    g = *(++offset);
                    b = *(++offset);
                    break;
                case SDL_PIXELFORMAT_BGR24:
                    b = *(offset);
                    g = *(++offset);
                    r = *(++offset);
                    break;
            }
            
            return map_color (r, g, b, a);
        }
        
        switch (Info->Format)
        {
            case SDL_PIXELFORMAT_BGR888:
                r = *(++offset);
                g = *(++offset);
                b = *(++offset);
                break;                
            case SDL_PIXELFORMAT_BGRA8888:
                a = *(offset);
                r = *(++offset);
                g = *(++offset);
                b = *(++offset);
                break;
            case SDL_PIXELFORMAT_ABGR8888:
                r = *(offset);
                g = *(++offset);
                b = *(++offset);
                a = *(++offset);
                break;
            case SDL_PIXELFORMAT_RGB888:
                b = *(++offset);
                g = *(++offset);
                r = *(++offset);
                break;
            case SDL_PIXELFORMAT_ARGB8888:
                b = *(offset);
                g = *(++offset);
                r = *(++offset);
                a = *(++offset);
                break;
            case SDL_PIXELFORMAT_RGBA8888:
                a = *(offset);
                b = *(++offset);
                g = *(++offset);
                r = *(++offset);
                break;
            default:
                LOG(FATAL) << "sdl::get_pix: Unsupported format " << SDL_GetPixelFormatName(Info->Format);
                break;
        }

#ifdef __BIG_ENDIAN__
        return map_color (g, r, a, b);
#else
        return map_color (r, g, b, a);
#endif
    }

    void surface_sdl::scale_up(surface *target, const u_int32 & factor) const
    {
        // scaling of the final result is handled in surface_sdl::draw
        if (!target || target == display) return;

    	if (length() * factor > target->length() ||
    		height() * factor > target->height())
    		return;

    	lock(NULL);
        SDL_Surface *target_surf = ((surface_sdl*) target)->to_sw_surface ();

        u_int8 *target_data = (u_int8*) target_surf->pixels;
        s_int32 target_line_length = target_surf->format->BytesPerPixel * target->length();

        for (s_int32 src_y = 0; src_y < height(); ++src_y)
        {
            s_int32 target_x = 0;
            s_int32 target_x_end = 0;

            // we scale one line horizontally
            for (s_int32 src_x = 0; src_x < length(); ++src_x)
            {
                u_int32 px = get_pix (src_x, src_y);
                for (target_x_end += factor; target_x < target_x_end; ++target_x)
                {
                    target->put_pix (target_x, src_y * factor, px);
                }
            }

            // the next lines will be the same, so we just copy them
            for (u_int32 i = 1; i < factor; i++)
            {
                u_int8 *target_next_line = target_data + target_surf->pitch;
                memcpy (target_next_line, target_data, target_line_length);
                target_data = target_next_line;
            }

            // goto next line
            target_data += target_surf->pitch;
        }

        target->unlock();
        SDL_FreeSurface(target_surf);
    }

    void surface_sdl::scale_down(surface *target, const u_int32 & factor) const
    {
        // downscaling directly to screen is not supported
        if (!target || target == display) return;

        if (length() / factor > target->length() ||
            height() / factor > target->height())
            return;

        lock(NULL);
        SDL_Surface *target_surf = ((surface_sdl*) target)->to_sw_surface ();

        s_int32 target_y = 0;
        for (s_int32 src_y = factor/2; src_y < height(); src_y += factor)
        {
            s_int32 target_x = 0;
            for (s_int32 src_x = factor/2; src_x < length(); src_x += factor)
            {
                u_int32 px = get_pix (src_x, src_y);
                target->put_pix (target_x, target_y, px);
                target_x++;
            }
            target_y++;
        }

        target->unlock();
        SDL_FreeSurface(target_surf);
    }

    surface & surface_sdl::operator = (const surface& src)
    {
        const surface_sdl & src_sdl = (const surface_sdl &) src;

        (drawable&) (*this) = (drawable&) src;
        alpha_channel_ = src.has_alpha_channel();
        is_masked_ = src.is_masked();
        alpha_ = src.alpha();

        if (Surface) SDL_DestroyTexture(Surface);
        if (!src_sdl.Surface)
        {
            Surface = NULL;
        }
        else
        {
            int l, h, pitch;
            void *src_pixels, *dst_pixels;

            SDL_QueryTexture(src_sdl.Surface, &Info->Format, NULL, &l, &h);
            Surface = SDL_CreateTexture (display->get_renderer(), Info->Format, SDL_TEXTUREACCESS_STREAMING, length(), height());

            SDL_LockTexture(src_sdl.Surface, NULL, &src_pixels, &pitch);
            SDL_LockTexture(Surface, NULL, &dst_pixels, &pitch);

            while (h-- > 0)
            {
                SDL_memcpy (dst_pixels, src_pixels, pitch);
                src_pixels = (u_int8*) src_pixels + pitch;
                dst_pixels = (u_int8*) dst_pixels + pitch;
            }

            SDL_UnlockTexture(Surface);
        }

        return *this; 
    }

    void surface_sdl::resize (u_int16 l, u_int16 h)
    {
        if (l == length () && h == height ()) return;

        if (Surface) SDL_DestroyTexture(Surface);

        set_length (l);
        set_height (h); 

        // is screen surface initialized?
        if (display->get_renderer())
        {
#ifdef __BIG_ENDIAN__
            Info->Format = has_alpha_channel() ? SDL_PIXELFORMAT_ARGB8888 : SDL_PIXELFORMAT_RGB24;
#else
            Info->Format = has_alpha_channel() ? SDL_PIXELFORMAT_ABGR8888 : SDL_PIXELFORMAT_BGR24;
#endif
            Surface = SDL_CreateTexture (display->get_renderer(), Info->Format, SDL_TEXTUREACCESS_STREAMING, l, h);
            if (!Surface) LOG(ERROR) << "*** surface::resize: " << SDL_GetError();
        }
        else
        {
            Surface = NULL;
            LOG(FATAL) << "*** surface:resize: screen surface not initialized!";
        }
    }

    void surface_sdl::clear () 
    {
        if (Surface)
        {
            SDL_DestroyTexture(Surface);
            Surface = NULL;
            set_length (0);
            set_height (0); 
            set_alpha (255);
            is_masked_ = false;
        }
    }

    void surface_sdl::set_data(void * data, u_int16 l, u_int16 h, u_int8 bytes_per_pixel, u_int32 red_mask, 
                               u_int32 green_mask, u_int32 blue_mask, u_int32 alpha_mask)
    {
        if (Surface) SDL_DestroyTexture(Surface);

        set_length(l);
        set_height(h);

        alpha_channel_ = alpha_mask != 0;

        Info->Format = SDL_MasksToPixelFormatEnum (bytes_per_pixel * 8, red_mask, green_mask, blue_mask, alpha_mask);
        Surface = SDL_CreateTexture (display->get_renderer(), Info->Format, SDL_TEXTUREACCESS_STREAMING, l, h);
        if (!Surface) LOG(ERROR) << "*** surface_sdl::set_data: " << SDL_GetError();

        lock(NULL);

        int pitch = bytes_per_pixel * l;
        void *src = data;

        while (h-- > 0)
        {
            SDL_memcpy (Info->Pixels, src, pitch);
            src = (u_int8*) src + pitch;
            Info->Pixels = (u_int8*) Info->Pixels + Info->Pitch;
        }

        unlock();

        free (data);
    }

    void * surface_sdl::get_data (u_int8 bytes_per_pixel,
                                  u_int32 red_mask, u_int32 green_mask,
                                  u_int32 blue_mask, u_int32 alpha_mask) const
    {
        lock(NULL);

        u_int32 dst_format = SDL_MasksToPixelFormatEnum(bytes_per_pixel*8, red_mask, green_mask, blue_mask, alpha_mask);
        void *dst_pixels = SDL_calloc (bytes_per_pixel, length() * height());
        int dst_pitch = length() * bytes_per_pixel;

        if (dst_format != Info->Format)
        {
            SDL_ConvertPixels(length(), height(), Info->Format, Info->Pixels, Info->Pitch, dst_format, dst_pixels, dst_pitch);
        }
        else
        {
            u_int8 *dest = (u_int8*) dst_pixels;
            int h = height();
            while (h-- > 0)
            {
                SDL_memcpy (dest, (u_int8*) Info->Pixels, Info->Pitch);
                Info->Pixels = (u_int8*) Info->Pixels + Info->Pitch;
                dest = dest + dst_pitch;
            }
        }

        return dst_pixels;
    }

    void surface_sdl::setup_rects (s_int16 x, s_int16 y, s_int16 sx, s_int16 sy,
                                   u_int16 sl, u_int16 sh, const drawing_area * draw_to) const
    {
        if (draw_to)
        { 
            drawing_area im_zone (x, y, sl, sh);
            im_zone.assign_drawing_area (draw_to);

            drawing_area da_int = im_zone.setup_rects ();
            dstrect.x = da_int.x();
            dstrect.y = da_int.y();
            dstrect.w = da_int.length();
            dstrect.h = da_int.height();

            srcrect = dstrect;
            srcrect.x = x < dstrect.x ? sx + dstrect.x - x : sx;
            srcrect.y = y < dstrect.y ? sy + dstrect.y - y : sy;
        }
        else
        {
            srcrect.x = sx;
            srcrect.y = sy;
            srcrect.w = sl;
            srcrect.h = sh;

            dstrect = srcrect;
            dstrect.x = x;
            dstrect.y = y;
        } 
    }
}
