#include "inc/kern.h"
#include "inc/terminal.h"
#include "inc/fdf.h"
#include <stdint.h>

#define PI 3.14159265358979323846

// Multiboot bilgi yapısı
typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint16_t reserved;
} __attribute__((packed)) multiboot_info_t;

// Global yapı tanımları
t_fdf env;
t_map map;
t_camera camera;

// Global olarak map verisi
char *map_data = 
"0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n"
"0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n"
"0 0 8 8 0 0 8 8 0 0 0 8 8 8 8 8 0 0 0\n"
"0 0 8 8 0 0 8 8 0 0 0 0 0 0 0 8 8 0 0\n"
"0 0 8 8 0 0 8 8 0 0 0 0 0 0 0 8 8 0 0\n"
"0 0 8 8  8 8 8 8 0 0 0 0 8 8 8 8 0 0 0\n"
"0 0 0 8 8 8 8 8 0 0 0 8 8 0 0 0 0 0 0\n"
"0 0 0 0 0 0 8 8 0 0 0 8 8 0 0 0 0 0 0\n"
"0 0 0 0 0 0 8 8 0 0 0 8 8 8 8 8 8 0 0\n"
"0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n"
"0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0";

#define PI 3.14159265358979323846

// Açı sınırlandırma
double normalize_angle(double x) {
    while (x > PI) x -= 2 * PI;
    while (x < -PI) x += 2 * PI;
    return x;
}

// Taylor serisiyle sinüs hesaplaması
double sin(double x) {
    x = normalize_angle(x);
    double term = x;
    double sum = x;
    for (int i = 1; i < 15; i++) {
        term *= -x * x / (2 * i * (2 * i + 1));
        sum += term;
    }
    return sum;
}

// Taylor serisiyle kosinüs hesaplaması
double cos(double x) {
    x = normalize_angle(x);
    double term = 1.0;
    double sum = 1.0;
    for (int i = 1; i < 15; i++) {
        term *= -x * x / (2 * i * (2 * i - 1));
        sum += term;
    }
    return sum;
}

// `env` yapılarını başlatma
void ft_init(multiboot_info_t *mbi) {
    env.data_addr = (char *)((uintptr_t)mbi->framebuffer_addr);
    env.framebuffer_pitch = mbi->framebuffer_pitch;
    env.framebuffer_width = mbi->framebuffer_width;
    env.framebuffer_height = mbi->framebuffer_height;
    env.bpp = mbi->framebuffer_bpp;
    env.map = &map;
    env.camera = &camera;
}

void ft_camera_init() {
    camera.zoom = 30;
    camera.x_angle = -0.615;  
    camera.y_angle = -0.523;
    camera.z_angle = 0.615;
    camera.z_height = 0.3;
    camera.x_offset = 0;
    camera.y_offset = 0;
    camera.iso = 1;
}

// `map_data`'yı `map.array`'e yükleyen fonksiyon
void ft_load_map_data(void) {
    static int map_array[11][19];
    int row = 0, col = 0;

    map.array = (int ***)map_array;
    char *line = map_data;

    while (*line) {
        if (*line >= '0' && *line <= '9') {
            map_array[row][col] = *line - '0';
            col++;
            if (col >= map.width) {
                col = 0;
                row++;
            }
        }
        line++;
    }
}

// Harita ayarlarını başlatma
void ft_map_init() {
    map.height = 11;
    map.width = 19;
    map.z_max = 4;
    map.z_min = 0;
    ft_load_map_data();
}

// abs fonksiyonları
int abs(int n) { return (n < 0) ? -n : n; }
float ft_abs(float n) { return (n < 0) ? -n : n; }
int ft_ipart(float n) { return (int)n; }
float ft_fpart(float n) { return (n > 0.0f) ? n - ft_ipart(n) : n - (ft_ipart(n) + 1.0f); }
float ft_rfpart(float n) { return 1.0f - ft_fpart(n); }

uint32_t ft_get_color(int x, t_point s, t_point e, float factor) {
    float percent = ft_abs(x - s.x) / (float)ft_abs(e.x - s.x);
    uint32_t a = 0xFF;  // Tam opaklık
    uint32_t r = (uint32_t)(((s.color >> 16) & 0xFF) * (1 - percent) + ((e.color >> 16) & 0xFF) * percent);
    uint32_t g = (uint32_t)(((s.color >> 8) & 0xFF) * (1 - percent) + ((e.color >> 8) & 0xFF) * percent);
    uint32_t b = (uint32_t)((s.color & 0xFF) * (1 - percent) + (e.color & 0xFF) * percent);

    return (a << 24) | (r << 16) | (g << 8) | b;
}

void ft_swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void ft_put_pixel(t_fdf *env, int x, int y, uint32_t color) {
    if (x >= 0 && x < env->framebuffer_width && y >= 0 && y < env->framebuffer_height) {
        uint32_t *fb = (uint32_t *)(uintptr_t)(env->data_addr);
        uint32_t pixel_offset = y * (env->framebuffer_pitch / 4) + x;
        fb[pixel_offset] = color;  // Renk doğrudan atanır
    }
}

static void ft_draw_line_loop(t_point s, t_point e, float gradient, t_fdf *env) {
    float inter_y = s.y;
    int x = s.x;

    while (x <= e.x) {
        if (env->steep) {
            ft_put_pixel(env, ft_ipart(inter_y), x, ft_get_color(x, s, e, ft_rfpart(inter_y)));
            ft_put_pixel(env, ft_ipart(inter_y) + 1, x, ft_get_color(x, s, e, ft_fpart(inter_y)));
        } else {
            ft_put_pixel(env, x, ft_ipart(inter_y), ft_get_color(x, s, e, ft_rfpart(inter_y)));
            ft_put_pixel(env, x, ft_ipart(inter_y) + 1, ft_get_color(x, s, e, ft_fpart(inter_y)));
        }
        inter_y += gradient;
        x++;
    }
}

void ft_draw_line(t_point start, t_point end, t_fdf *env) {
    float dx = end.x - start.x;
    float dy = end.y - start.y;
    float gradient;
    env->steep = ft_abs(dy) > ft_abs(dx);

    if (env->steep) {
        ft_swap(&start.x, &start.y);
        ft_swap(&end.x, &end.y);
    }
    if (start.x > end.x) {
        ft_swap(&start.x, &end.x);
        ft_swap(&start.y, &end.y);
    }

    dx = end.x - start.x;
    dy = end.y - start.y;
    gradient = (dx == 0) ? 1 : dy / dx;

    ft_draw_line_loop(start, end, gradient, env);
}

uint32_t get_default_color(int z, t_map *map) {
    double percent = ((double)(z - map->z_min) / (map->z_max - map->z_min));

    if (percent < 0.2) return 0xFF0000FF;     // Mavi
    else if (percent < 0.4) return 0xFF00FF00; // Yeşil
    else if (percent < 0.6) return 0xFFFFFF00; // Sarı
    else if (percent < 0.8) return 0xFFFFA500; // Turuncu
    else return 0xFFFF0000;                    // Kırmızı
}
void ft_rotate_x(int *y, int *z, double x_angle) {
    int prev_y = *y;
    *y = prev_y * cos(x_angle) + *z * sin(x_angle);
    *z = prev_y * -sin(x_angle) + *z * cos(x_angle);
}

void ft_rotate_y(int *x, int *z, double y_angle) {
    int prev_x = *x;
    *x = prev_x * cos(y_angle) + *z * sin(y_angle);
    *z = prev_x * -sin(y_angle) + *z * cos(y_angle);
}

void ft_rotate_z(int *x, int *y, double z_angle) {
    int prev_x = *x;
    int prev_y = *y;
    *x = prev_x * cos(z_angle) - prev_y * sin(z_angle);
    *y = prev_x * sin(z_angle) + prev_y * cos(z_angle);
}

t_point project(int x, int y, t_fdf *env) {
    t_point point;
    point.z = env->map->array[y][x][0];
    point.color = get_default_color(point.z, env->map);
    point.x = x * env->camera->zoom;
    point.y = y * env->camera->zoom;
    point.z *= env->camera->zoom / env->camera->z_height;

    point.x -= (env->map->width * env->camera->zoom) / 2;
    point.y -= (env->map->height * env->camera->zoom) / 2;

    ft_rotate_x(&point.y, &point.z, env->camera->x_angle);
    ft_rotate_y(&point.x, &point.z, env->camera->y_angle);
    ft_rotate_z(&point.x, &point.y, env->camera->z_angle);

    point.x += WIDTH / 2 + env->camera->x_offset;
    point.y += (HEIGHT + env->map->height / 2 * env->camera->zoom) / 2 + env->camera->y_offset;

    return point;
}

void ft_draw(t_map *map, t_fdf *env) {
    for (int y = 0; y < env->framebuffer_height; y++) {
        for (int x = 0; x < env->framebuffer_width; x++) {
            ft_put_pixel(env, x, y, 0xFF000000);  // Tam opak siyah
        }
    }

    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            if (x < map->width - 1)
                ft_draw_line(project(x, y, env), project(x + 1, y, env), env);
            if (y < map->height - 1)
                ft_draw_line(project(x, y, env), project(x, y + 1, env), env);
        }
    }
}


int kmain(multiboot_info_t *mbi) {
    init_idt();
    kb_init();
    enable_interrupts();

    ft_init(mbi);
    ft_map_init();
    ft_camera_init();
    ft_draw(env.map, &env);

    return 0;
}
