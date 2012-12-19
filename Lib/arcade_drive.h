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

status_t    arcade_drive(char joy_x, char joy_y, char power_max,
		    char *left_power_ptr, char *right_power_ptr);

