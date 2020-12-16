#include "mlx_linux/mlx.h"
#include <stdio.h>

typedef struct s_vars
{
	void *mlx;
	void *win;
}				t_vars;

int key_hook(int keycode, t_vars *vars)
{
	return(keycode);
}

int main(void)
{
	t_vars vars;

	vars.mlx = mlx_init();
	vars.win = mlx_new_window(vars.mlx, 640, 480, "hola");
	mlx_hook(vars.win, 02, 1L<<0, key_hook, &vars);
	mlx_loop(vars.mlx);
}

