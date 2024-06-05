
#ifndef DIGITS_H_
#define DIGITS_H_ 1

#define DIGIT_COUNT 11 /* '0-9' and ':' */
#define DIGIT_WIDTH (digits.width / DIGIT_COUNT)
#define DIGIT_HEIGHT digits.height

typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned int bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
    unsigned char pixel_data[1918 * 305 * 4 + 1];
} GimpImage;

extern GimpImage digits;

#endif /* DIGITS_H_ */
