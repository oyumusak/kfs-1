#ifndef FDF_H
# define FDF_H

// Ekran boyutları
# define WIDTH 1024
# define HEIGHT 768

// Klavye ve fare tuşları (opsiyonel)
# define ARROW_LEFT 123
# define ARROW_RIGHT 124
# define ARROW_DOWN 125
# define ARROW_UP 126
# define MINUS 27
# define PLUS 24
# define SPACE 49
# define KEY_R 15
# define MOUSE_CLICK_LEFT 1
# define MOUSE_CLICK_RIGHT 2
# define MOUSE_CLICK_MIDDLE 3
# define MOUSE_WHEEL_UP 4
# define MOUSE_WHEEL_DOWN 5
# define ESCAPE 53
# include <stdint.h>
// Nokta yapısı
typedef struct s_point
{
    int x;
    int y;
    int z;
    uint32_t color;  // color, uint32_t olarak değiştirildi
    int reverse;
} t_point;

// Harita yapısı
typedef struct s_map
{
    int height;
    int width;
    int ***array;  // 2D dizi olarak güncellendi
    int z_max;
    int z_min;
} t_map;


// Kamera yapısı
typedef struct s_camera
{
    int zoom;
    double x_angle;
    double y_angle;
    double z_angle;
    float z_height;
    int x_offset;
    int y_offset;
    int iso;
} t_camera;

// Fare yapısı (opsiyonel, kullanmayacaksanız kaldırabilirsiniz)
typedef struct s_mouse
{
    int button;
    int x;
    int y;
    int prev_x;
    int prev_y;
} t_mouse;

// Ana yapı (env)
typedef struct s_fdf
{
    char *data_addr;             // Framebuffer başlangıç adresi
    int framebuffer_pitch;       // Framebuffer pitch (satır başına bayt sayısı)
    int framebuffer_width;       // Ekran genişliği
    int framebuffer_height;      // Ekran yüksekliği
    int bpp;                     // Bit derinliği (bits per pixel)
    int steep;                   // Çizim eğimi (çizgi çizme işlemleri için)
    t_map *map;                  // Harita yapısı pointer
    t_camera *camera;            // Kamera yapısı pointer
} t_fdf;

// Fonksiyon prototipleri
void    ft_check_valid(char *filename, t_map *map);
void    ft_draw(t_map *map, t_fdf *env);
void    ft_put_pixel(t_fdf *env, int x, int y, uint32_t color);  // uint32_t olarak güncellendi
void    ft_draw_line(t_point s, t_point e, t_fdf *env);
t_point project(int x, int y, t_fdf *env);
int     ft_min(int a, int b);
uint32_t get_default_color(int z, t_map *map);  // uint32_t olarak güncellendi
void    ft_return_error(const char *err_msg, int system_function);
double  ft_reset_angles(double angle);

// Xiaolin Wu çizgi algoritması yardımcı fonksiyonları (opsiyonel, kullanılacaksa)
float   ft_abs(float n);
int     ft_ipart(float n);
float   ft_fpart(float n);
float   ft_rfpart(float n);

// Kontroller (opsiyonel, klavye/fare desteklemek istiyorsanız)
void    ft_hook_controls(t_fdf *env);
int     ft_key_press(int keycode, void *params);
int     ft_mouse_down(int button, int x, int y, void *params);
int     ft_mouse_up(int button, int x, int y, void *params);
int     ft_mouse_move(int x, int y, void *params);
int     ft_close_win(void *params);

#endif
