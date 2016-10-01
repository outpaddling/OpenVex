#ifndef ABS
#define ABS(x)  ((x) >= 0 ? (x) : -(x))
#endif


#define SWAP(a,b) \
{ \
    int     temp = *a; \
\
    *a = *b; \
    *b = temp; \
}

status_t    arcade_drive(signed char joy_x, signed char joy_y,
		signed char power_max,
		signed char *left_power_ptr, signed char *right_power_ptr);

