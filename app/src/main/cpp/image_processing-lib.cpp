#include"image_utility.hpp"
#include<omp.h>
#include<cmath>
#include<stdexcept>

auto norm = [](auto...args) {
    return [](double x) {
        double a = x / 2;
        while (true) {
            double prev = a;
            if ((a = (a + x / a) / 2) == prev)break;
        }
        return a;
    }(((args * args) + ...));
};

using YUV = image_utility::YUV;
using RGB = image_utility::RGB;

extern "C" JNIEXPORT void JNICALL Java_com_example_salmon_ImageProcessor_differentialFilter(
        JNIEnv *env, jobject,
        jobject image, jobject surface_texture, jboolean is_landscape
)
{
    constexpr double magnification = norm(1, 1);
    try {
        image_utility::image_accessor_YUV_420_888 ia(env, image);
        image_utility::surface_texture_accessor_R8G8B8X8 sa(env, surface_texture);
        image_utility::coordinate_transformer ct(ia, sa, is_landscape);
#pragma omp parallel for default(none) shared(ia, sa, ct) collapse(2)
        for (int y = 0; y < ia.get_height(); ++y) {
            for (int x = 0; x < ia.get_width(); ++x) {
                auto c = ct(x, y);
                sa(c.x, c.y, RGB::R) =
                sa(c.x, c.y, RGB::G) =
                sa(c.x, c.y, RGB::B) = static_cast<uint8_t>(255 - std::hypot(
                        ia(x + 1, y, YUV::Y) - ia(x - 1, y, YUV::Y),
                        ia(x, y + 1, YUV::Y) - ia(x, y - 1, YUV::Y)
                ) / magnification);
            }
        }
    } catch (std::runtime_error &) {
        return;
    }
}

extern "C" JNIEXPORT void JNICALL Java_com_example_salmon_ImageProcessor_prewittFilter(
        JNIEnv *env, jobject,
        jobject image, jobject surface_texture, jboolean is_landscape
)
{
    constexpr double magnification = norm(3, 3);
    try {
        image_utility::image_accessor_YUV_420_888 ia(env, image);
        image_utility::surface_texture_accessor_R8G8B8X8 sa(env, surface_texture);
        image_utility::coordinate_transformer ct(ia, sa, is_landscape);
#pragma omp parallel for default(none) shared(ia, sa, ct) collapse(2)
        for (int y = 0; y < ia.get_height(); ++y) {
            for (int x = 0; x < ia.get_width(); ++x) {
                auto c = ct(x, y);
                sa(c.x, c.y, RGB::R) =
                sa(c.x, c.y, RGB::G) =
                sa(c.x, c.y, RGB::B) = static_cast<uint8_t>(255 - std::hypot(
                        ia(x + 1, y - 1, YUV::Y) + ia(x + 1, y, YUV::Y) + ia(x + 1, y + 1, YUV::Y)
                        - ia(x - 1, y - 1, YUV::Y) - ia(x - 1, y, YUV::Y) - ia(x - 1, y + 1, YUV::Y),
                        ia(x - 1, y + 1, YUV::Y) + ia(x, y + 1, YUV::Y) + ia(x + 1, y + 1, YUV::Y)
                        - ia(x - 1, y - 1, YUV::Y) - ia(x, y - 1, YUV::Y) - ia(x + 1, y - 1, YUV::Y)
                ) / magnification);
            }
        }
    } catch (std::runtime_error &) {
        return;
    }
}

extern "C" JNIEXPORT void JNICALL Java_com_example_salmon_ImageProcessor_sobelFilter(
        JNIEnv *env, jobject,
        jobject image, jobject surface_texture, jboolean is_landscape
)
{
    constexpr double magnification = norm(4, 4);
    try {
        image_utility::image_accessor_YUV_420_888 ia(env, image);
        image_utility::surface_texture_accessor_R8G8B8X8 sa(env, surface_texture);
        image_utility::coordinate_transformer ct(ia, sa, is_landscape);
#pragma omp parallel for default(none) shared(ia, sa, ct) collapse(2)
        for (int y = 0; y < ia.get_height(); ++y) {
            for (int x = 0; x < ia.get_width(); ++x) {
                auto c = ct(x, y);
                sa(c.x, c.y, RGB::R) =
                sa(c.x, c.y, RGB::G) =
                sa(c.x, c.y, RGB::B) = static_cast<uint8_t>(255 - std::hypot(
                        ia(x + 1, y - 1, YUV::Y) + ia(x + 1, y, YUV::Y) * 2 + ia(x + 1, y + 1, YUV::Y)
                        - ia(x - 1, y - 1, YUV::Y) - ia(x - 1, y, YUV::Y) * 2 - ia(x - 1, y + 1, YUV::Y),
                        ia(x - 1, y + 1, YUV::Y) + ia(x, y + 1, YUV::Y) * 2 + ia(x + 1, y + 1, YUV::Y)
                        - ia(x - 1, y - 1, YUV::Y) - ia(x, y - 1, YUV::Y) * 2 - ia(x + 1, y - 1, YUV::Y)
                ) / magnification);
            }
        }
    } catch (std::runtime_error &) {
        return;
    }
}

extern "C" JNIEXPORT void JNICALL Java_com_example_salmon_ImageProcessor_laplacianFilter(
        JNIEnv *env, jobject,
        jobject image, jobject surface_texture, jboolean is_landscape
)
{
    constexpr double magnification = norm(4);
    try {
        image_utility::image_accessor_YUV_420_888 ia(env, image);
        image_utility::surface_texture_accessor_R8G8B8X8 sa(env, surface_texture);
        image_utility::coordinate_transformer ct(ia, sa, is_landscape);
#pragma omp parallel for default(none) shared(ia, sa, ct) collapse(2)
        for (int y = 0; y < ia.get_height(); ++y) {
            for (int x = 0; x < ia.get_width(); ++x) {
                auto c = ct(x, y);
                sa(c.x, c.y, RGB::R) =
                sa(c.x, c.y, RGB::G) =
                sa(c.x, c.y, RGB::B) = static_cast<uint8_t>(255 - std::abs(
                        ia(x - 1, y, YUV::Y) + ia(x + 1, y, YUV::Y) + ia(x, y - 1, YUV::Y) + ia(x, y + 1, YUV::Y) - ia(x, y, YUV::Y) * 4
                ) / magnification);
            }
        }
    } catch (std::runtime_error &) {
        return;
    }
}

extern "C" JNIEXPORT void JNICALL Java_com_example_salmon_ImageProcessor_negativeFilm(
        JNIEnv *env, jobject,
        jobject image, jobject surface_texture, jboolean is_landscape
)
{
    try {
        image_utility::image_accessor_YUV_420_888 ia(env, image);
        image_utility::surface_texture_accessor_R8G8B8X8 sa(env, surface_texture);
        image_utility::coordinate_transformer ct(ia, sa, is_landscape);
#pragma omp parallel for default(none) shared(ia, sa, ct) collapse(2)
        for (int y = 0; y < ia.get_height(); ++y) {
            for (int x = 0; x < ia.get_width(); ++x) {
                auto rgb = image_utility::YUV_to_RGB::convert(
                        ia(x, y, YUV::Y),
                        ia(x, y, YUV::U) - 128,
                        ia(x, y, YUV::V) - 128
                );
                auto c = ct(x, y);
                sa(c.x, c.y, RGB::R) = static_cast<uint8_t>(255 - rgb.R);
                sa(c.x, c.y, RGB::G) = static_cast<uint8_t>(255 - rgb.G);
                sa(c.x, c.y, RGB::B) = static_cast<uint8_t>(255 - rgb.B);
            }
        }
    } catch (std::runtime_error &) {
        return;
    }
}

extern "C" JNIEXPORT void JNICALL Java_com_example_salmon_ImageProcessor_pseudoColor(
        JNIEnv *env, jobject,
        jobject image, jobject surface_texture, jboolean is_landscape
)
{
    try {
        image_utility::image_accessor_YUV_420_888 ia(env, image);
        image_utility::surface_texture_accessor_R8G8B8X8 sa(env, surface_texture);
        image_utility::coordinate_transformer ct(ia, sa, is_landscape);
#pragma omp parallel for default(none) shared(ia, sa, ct) collapse(2)
        for (int y = 0; y < ia.get_height(); ++y) {
            for (int x = 0; x < ia.get_width(); ++x) {
                int luminance = ia(x, y, YUV::Y);
                auto c = ct(x, y);
                sa(c.x, c.y, RGB::R) = static_cast<uint8_t>(luminance < 128 ? 0 : luminance >= 192 ? 255 : (luminance - 128) * 4);
                sa(c.x, c.y, RGB::G) = static_cast<uint8_t>(luminance < 64 ? luminance * 4 : luminance >= 192 ? 255 - (luminance - 192) * 4 : 255);
                sa(c.x, c.y, RGB::B) = static_cast<uint8_t>(luminance < 64 ? 255 : luminance >= 128 ? 0 : 255 - (luminance - 64) * 4);
            }
        }
    } catch (std::runtime_error &) {
        return;
    }
}

extern "C" JNIEXPORT void JNICALL Java_com_example_salmon_ImageProcessor_noFilter(
        JNIEnv *env, jobject,
        jobject image, jobject surface_texture, jboolean is_landscape
)
{
    try {
        image_utility::image_accessor_YUV_420_888 ia(env, image);
        image_utility::surface_texture_accessor_R8G8B8X8 sa(env, surface_texture);
        image_utility::coordinate_transformer ct(ia, sa, is_landscape);
#pragma omp parallel for default(none) shared(ia, sa, ct) collapse(2)
        for (int y = 0; y < ia.get_height(); ++y) {
            for (int x = 0; x < ia.get_width(); ++x) {
                auto rgb = image_utility::YUV_to_RGB::convert(
                        ia(x, y, YUV::Y),
                        ia(x, y, YUV::U) - 128,
                        ia(x, y, YUV::V) - 128
                );
                auto c = ct(x, y);
                sa(c.x, c.y, RGB::R) = static_cast<uint8_t>(rgb.R);
                sa(c.x, c.y, RGB::G) = static_cast<uint8_t>(rgb.G);
                sa(c.x, c.y, RGB::B) = static_cast<uint8_t>(rgb.B);
            }
        }
    } catch (std::runtime_error &) {
        return;
    }
}