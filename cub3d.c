/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jesus <jesus@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/18 10:25:56 by jtrancos          #+#    #+#             */
/*   Updated: 2020/11/28 19:42:39 by jesus            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mlx/mlx.h"
#include "mlx_linux/mlx.h"
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define mapwidth 24
#define mapheight 24
#define screenwidth 1280
#define screenheight 720
#define numsprite 3


typedef struct s_keys
{
	int		w;
	int		a;
	int		s;
	int		d;
	int		left;
	int		right;
	int		esc;
}				t_keys;

typedef struct s_player
{
	float	x;
	float	y;
	float	dir_x;
	float	dir_y;
	float	plane_x;
	float	plane_y;
	float	rotation;
	float	speed;
	t_keys	keys;

}				t_player;

/*typedef struct	s_map
{
	;
}				t_map;*/

/*typedef struct	s_rgb
{
	int		r;
	int		g;
	int		b;
}				t_rgb;*/

typedef struct	s_ray
{
	float	camera_x;
	float	dir_x;
	float	dir_y;
	int		map_x;
	int		map_y;
	float	side_dist_x;
	float	side_dist_y;
	float	delta_dist_x;
	float	delta_dist_y;
	float	perpwalldist;
	int		step_x;
	int		step_y;
	int		hit;
	int		side;
	int		line_height;
}				t_ray;

typedef struct	s_img
{
	void	*img;
	int		*addr;
	int		bpp;
	int		line_len;
	int		endian;
}				t_img;

typedef struct	s_texture
{
	int		width;
	int		height;
	char	*path;
	t_img	img;
}				t_texture;

typedef struct	s_textures
{
	t_texture	sprite;
	t_texture	north;
	t_texture	east;
	t_texture	south;
	t_texture	west;
}				t_textures;

typedef struct s_wall
{
	t_texture	texture;
	int			texture_x;
	int			x;
	float			draw_start;
	float			draw_end;
}				t_wall;

typedef struct s_sprite
{
	float			map_x;
	float			map_y;
	unsigned int	color;
	float			dist;
	float			calc_x;
	float			calc_y;
	float			inv_det;
	float			trans_x;
	float			trans_y;
	int				spritescreen_x;
	int				sprite_h;
	int				sprite_w;
	int				drawstart_x;
	int				drawstart_y;
	int				drawend_x;
	int				drawend_y;
	int				tex_x;
	int				tex_y;
	int				d;
}				t_sprite;

typedef struct	s_data
{
	void	*mlx;
	void	*win;
	unsigned int rgb;
	int 	blockx;
	int		blocky;
	int		blocklen;
	int		width;
	int		height;
	int		sprite_num;
	float	zbuffer[screenwidth];
	t_player	player;
	t_ray		ray;
	t_img		img;
	t_wall		wall;
	t_textures	textures;
	t_sprite	*sprite;
	//t_map		map;
	//t_rgb		rgb;
}				t_data;


void	sort_sprites(t_data *data)
{
	int j;
	int i;
	t_sprite tmp;

	i = 0;
	while (i < numsprite)
	{
		j = 0;
		while (j < numsprite)
		{
			//printf("sprite%d x: %f y: %f playerx: %f playery: %f dist: %f\n", i, spritemap[i].map_x, spritemap[i].map_y, data->player.x, data->player.y, data->sprite[i].dist);
			if (data->sprite[j].dist < data->sprite[i].dist)
			{
				tmp = data->sprite[j];
				data->sprite[j] = data->sprite[i];
				data->sprite[i] = tmp;
			}
			j++;
		}
		i++;
	}
	i = 0;
	while (i < numsprite)
	{
		printf("sprite%d x: %f y: %f playerx: %f playery: %f planex: %f planey: %f dist: %f\n", i, data->sprite[i].map_x, data->sprite[i].map_y, data->player.x, data->player.y, data->player.plane_x, data->player.plane_y, data->sprite[i].dist);
		i++;
	}
}

int		ft_close(t_data *data)
{
	mlx_destroy_window(data->mlx, data->win);
	exit(0);
	return(0);
}

int		ft_escape(int keycode, t_data *data)
{
	if (keycode == 53)
	{
		ft_close(data);
	}
	return(0);
}

/*
** comentario
** valido
**
*/

int worldmap [mapwidth][mapheight] = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


void	my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	data->img.addr[y * screenwidth + x] = color;
}

int		get_texture(t_data *data, t_texture *texture, char *path)
{
	int fd;

	if ((fd = open(path, O_RDONLY)) == -1)
	{
		printf("hola\n");
		close(fd);
		return (0);
	}
	texture->img.img = mlx_xpm_file_to_image(data->mlx, path, &texture->width, &texture->height);
	if (!texture->img.img)
	{
		printf("There is something wrong with your texture\n");
		return (0);
	}
	if (!(texture->img.addr = (int *)mlx_get_data_addr(texture->img.img, &texture->img.bpp, &texture->img.line_len, &texture->img.endian)))
		return (0);
//	printf("tw: %d, th: %d, img: %p, bpp: %d, linelen: %d, endian: %d\n", texture->width, texture->height, texture->img.img, texture->img.bpp, texture->img.line_len, texture->img.endian);
	//printf("%x\n", ((unsigned int)data->wall.texture.img.addr[0]));
	return (1);
}
void	put_texture(t_data *data, t_wall wall, t_ray ray, int x)
{
	float	step;
	float	tex_pos;
	int		tex_y;
	int		y;
	unsigned int color;
	
	step = 1.0 * wall.texture.height / ray.line_height;
	//printf("th: %d, lh: %d, ds: %f, step: %f, tex_pos: %f, tex_y: %d, y: %d\n", wall.texture.height, ray.line_height, wall.draw_start, step, tex_pos, tex_y, y);
	tex_pos = (wall.draw_start - screenheight / 2 + ray.line_height / 2) * step;
	//printf("th: %d, lh: %d, ds: %f, step: %f, tex_pos: %f, tex_y: %d, y: %d\n", wall.texture.height, ray.line_height, wall.draw_start, step, tex_pos, tex_y, y);
	y = wall.draw_start;
	while (y < wall.draw_end)
	{
		//printf("th: %d, lh: %d, ds: %f, step: %f, tex_pos: %f, tex_y: %d, y: %d\n", wall.texture.height, ray.line_height, wall.draw_start, step, tex_pos, tex_y, y);
		tex_y = (int)tex_pos;
		tex_pos += step;
		color = ((unsigned int *)wall.texture.img.addr)[wall.texture.width * tex_y + wall.texture_x];
		//if (data->ray.side == 1 && worldmap[data->ray.map_x][data->ray.map_y] != 1 )
		//	color /= 2;
		my_mlx_pixel_put(data, x, y, color);
		y++;
	}
}
void	draw_texture(t_data *data, t_wall wall, t_ray ray, int x)
{
	float	wall_x;
	int		tex_x;

	if (ray.side == 0)
		wall_x = data->player.y + ray.perpwalldist * ray.dir_y;
	else
		wall_x = data->player.x + ray.perpwalldist * ray.dir_x;
	wall_x -= floor(wall_x);
	tex_x = (int)(wall_x * (float)wall.texture.width);
	if (ray.side == 0 && ray.dir_x > 0)
		tex_x = wall.texture.width	- tex_x - 1;
	if (ray.side == 1 && ray.dir_y < 0)
		tex_x = wall.texture.width - tex_x - 1;
	wall.texture_x = tex_x;
	put_texture(data, wall, ray, x);
}

int press_key(int keycode, t_data * data)
{
	if (keycode == 119)
		data->player.keys.w = 1;
	else if (keycode == 97)
		data->player.keys.a = 1;
	else if (keycode == 115)
		data->player.keys.s = 1;
	else if (keycode == 100)
		data->player.keys.d = 1;
	else if (keycode == 65361)
		data->player.keys.left = 1;
	else if (keycode == 65363)
		data->player.keys.right = 1;
	return (1);
}

int release_key(int keycode, t_data *data)
{
	if (keycode == 65307)
		ft_close(data);
	else if (keycode == 119)
		data->player.keys.w = 0;
	else if (keycode == 97)
		data->player.keys.a = 0;
	else if (keycode == 115)
		data->player.keys.s = 0;
	else if (keycode == 100)
		data->player.keys.d = 0;
	else if (keycode == 65361)
		data->player.keys.left = 0;
	else if (keycode == 65363)
		data->player.keys.right = 0;
	return (1);
	
}

void	move_front_back(t_data *data)
{
	if (data->player.keys.w == 1) //forward
	{
		if(worldmap[(int)(data->player.x + data->player.dir_x * data->player.rotation)][(int)(data->player.y)] == 0)
			data->player.x += data->player.dir_x * data->player.rotation;
		if(worldmap[(int)(data->player.x)][(int)(data->player.y + data->player.dir_y * data->player.rotation)] == 0)
			data->player.y += data->player.dir_y * data->player.rotation;
	}
	if (data->player.keys.s == 1) //back
	{
		if(worldmap[(int)(data->player.x - data->player.dir_x * data->player.rotation)][(int)(data->player.y)] == 0) 
			data->player.x -= data->player.dir_x * data->player.rotation;
		if(worldmap[(int)(data->player.x)][(int)(data->player.y - data->player.dir_y * data->player.rotation)] == 0)
			data->player.y -= data->player.dir_y * data->player.rotation;
	}
}

void move_strafe(t_data *data)
{
	if (data->player.keys.d == 1) //right
	{
		if(worldmap[(int)(data->player.x + data->player.dir_y * data->player.rotation)][(int)(data->player.y)] == 0) 
			data->player.x += data->player.dir_y * data->player.rotation;
		if(worldmap[(int)(data->player.x)][(int)(data->player.y - data->player.dir_x * data->player.rotation)] == 0)
			data->player.y -= data->player.dir_x * data->player.rotation;
	}
	if (data->player.keys.a == 1) //left
	{
		if(worldmap[(int)(data->player.x - data->player.dir_y * data->player.rotation)][(int)(data->player.y)] == 0) 
			data->player.x -= data->player.dir_y * data->player.rotation;
		if(worldmap[(int)(data->player.x)][(int)(data->player.y + data->player.dir_x * data->player.rotation)] == 0)
			data->player.y += data->player.dir_x * data->player.rotation;
	}
}

void	rotation_right(t_data *data)
{
	float old_dir_x;
	float old_plane_x; 
	
	old_dir_x = data->player.dir_x;
	old_plane_x = data->player.plane_x;
	data->player.dir_x = data->player.dir_x * cos(-data->player.rotation) - data->player.dir_y * sin(-data->player.rotation);
	data->player.dir_y = old_dir_x * sin(-data->player.rotation) + data->player.dir_y * cos(-data->player.rotation);
	data->player.plane_x = data->player.plane_x * cos(-data->player.rotation) - data->player.plane_y * sin(-data->player.rotation);
	data->player.plane_y = old_plane_x * sin(-data->player.rotation) + data->player.plane_y * cos(-data->player.rotation);
}

void rotation_left(t_data *data)
{
	float old_dir_x;
	float old_plane_x;

	old_dir_x = data->player.dir_x;
	old_plane_x = data->player.plane_x;
	data->player.dir_x = data->player.dir_x * cos(data->player.rotation) - data->player.dir_y * sin(data->player.rotation);
	data->player.dir_y = old_dir_x * sin(data->player.rotation) + data->player.dir_y * cos(data->player.rotation);
	data->player.plane_x = data->player.plane_x * cos(data->player.rotation) - data->player.plane_y * sin(data->player.rotation);
	data->player.plane_y = old_plane_x * sin(data->player.rotation) + data->player.plane_y * cos(data->player.rotation);
}

void move_player(t_data *data)
{
	data->player.speed = 0.3;
	data->player.rotation = 0.025;

	move_strafe(data);
	move_front_back(data);
	if (data->player.keys.right == 1) //right rot
		rotation_right(data);
	if (data->player.keys.left == 1) //left rot
		rotation_left(data);
}

int raycasting(t_data *data)
{
	int x = 0;
	int color;
	int tex_y;
	int i = 0;
	
	move_player(data);
	while (x < screenwidth)
	{
		data->ray.camera_x = 2 * x / (float)screenwidth - 1;
		data->ray.dir_x = data->player.dir_x + data->player.plane_x * data->ray.camera_x;
		data->ray.dir_y = data->player.dir_y + data->player.plane_y * data->ray.camera_x;
		data->ray.map_x = (int)data->player.x;
		data->ray.map_y = (int)data->player.y;
		data->ray.hit = 0;
		if (data->ray.dir_y == 0)
			data->ray.delta_dist_x = 0;
		else
			data->ray.delta_dist_x = data->ray.dir_x == 0 ? 1 : fabs(1 / data->ray.dir_x);
		if (data->ray.dir_x == 0)
			data->ray.delta_dist_y = 0;
		else
			data->ray.delta_dist_y = data->ray.dir_y == 0 ? 1 : fabs(1 / data->ray.dir_y);


		if (data->ray.dir_x < 0)
		{
			data->ray.step_x = -1;
			data->ray.side_dist_x = (data->player.x - data->ray.map_x) * data->ray.delta_dist_x;
		}
		else
		{
			data->ray.step_x = 1;
			data->ray.side_dist_x = (data->ray.map_x + 1.0 - data->player.x) * data->ray.delta_dist_x;
		}
		if (data->ray.dir_y < 0)
		{
			data->ray.step_y = -1;
			data->ray.side_dist_y = (data->player.y - data->ray.map_y) * data->ray.delta_dist_y;
		}
		else
		{
			data->ray.step_y = 1;
			data->ray.side_dist_y = (data->ray.map_y + 1.0 - data->player.y) * data->ray.delta_dist_y;
		}
		while (data->ray.hit == 0)
		{
			if (data->ray.side_dist_x < data->ray.side_dist_y)
			{
				data->ray.side_dist_x += data->ray.delta_dist_x;
				data->ray.map_x += data->ray.step_x;
				data->ray.side = 0;
			}
			else
			{
				data->ray.side_dist_y += data->ray.delta_dist_y;
				data->ray.map_y += data->ray.step_y;
				data->ray.side = 1;
			}
			if (worldmap[data->ray.map_x][data->ray.map_y] > 0)
				data->ray.hit = 1;
		}
		//calculate sides
		if (data->ray.side == 0)
			data->ray.perpwalldist = (data->ray.map_x - data->player.x + (1 - data->ray.step_x) / 2) / data->ray.dir_x;
		else
			data->ray.perpwalldist = (data->ray.map_y - data->player.y + (1 - data->ray.step_y) / 2) / data->ray.dir_y;
		if (data->ray.side == 0)
			data->wall.texture = (data->ray.step_x == -1) ? data->textures.east : data->textures.west;
		else
			data->wall.texture = (data->ray.step_y == -1) ? data->textures.south : data->textures.north;
		data->ray.line_height = (int)(screenheight / data->ray.perpwalldist);
		data->wall.draw_start = -data->ray.line_height / 2 + screenheight / 2;
		if (data->wall.draw_start < 0)
			data->wall.draw_start = 0;
		data->wall.draw_end = data->ray.line_height / 2 + screenheight / 2;
		if (data->wall.draw_end >= screenheight)
			data->wall.draw_end = screenheight - 1;

		//suelo y cielo y textura
		int y = 0;
		while (y < data->wall.draw_start)
		{
			my_mlx_pixel_put(data, x, y, 0x44ceeb);
			y++;
		}
		draw_texture(data, data->wall, data->ray, x);
		y = data->wall.draw_end;
		while (y < screenheight)
		{
			my_mlx_pixel_put(data, x, y, 0x595959);
			y++;
		}
		data->zbuffer[x] = data->ray.perpwalldist;
		x++; 
	}
	// SPRITESS
	if (!(data->sprite = malloc(sizeof(t_sprite) * numsprite)))
		return (0);
	data->sprite[0].map_x = 8.5;
	data->sprite[0].map_y = 7.5;
	data->sprite[1].map_x = 10.5;
	data->sprite[1].map_y = 7.5;
	data->sprite[2].map_x = 6.5;
	data->sprite[2].map_y = 7.5;
	while (i < numsprite)
	{
		data->sprite[i].dist = ((data->player.x - data->sprite[i].map_x) * (data->player.x - data->sprite[i].map_x) + (data->player.y - data->sprite[i].map_y) * (data->player.y - data->sprite[i].map_y));
		//printf("sprite%d x: %f y: %f playerx: %f playery: %f dist: %f\n", i, spritemap[i].map_x, spritemap[i].map_y, data->player.x, data->player.y, data->sprite[i].dist);
		i++;
	}
	sort_sprites(data);
	for (int i = 0; i < numsprite; i++)
	{
		data->sprite[i].calc_x = data->sprite[i].map_x - data->player.x;
		data->sprite[i].calc_y = data->sprite[i].map_y - data->player.y;
		data->sprite[i].inv_det = 1.0 / (data->player.plane_x * data->player.dir_y - data->player.dir_x * data->player.plane_y);
		data->sprite[i].trans_x = data->sprite[i].inv_det * (data->player.dir_y * data->sprite[i].calc_x - data->player.dir_x * data->sprite[i].calc_y);
		data->sprite[i].trans_y = data->sprite[i].inv_det * (-data->player.plane_y * data->sprite[i].calc_x + data->player.plane_x * data->sprite[i].calc_y);
		data->sprite[i].spritescreen_x = (int)((screenwidth / 2) * (1 + data->sprite[i].trans_x / data->sprite[i].trans_y));
		data->sprite[i].sprite_h = abs((int)(screenheight / data->sprite[i].trans_y));
		data->sprite[i].drawstart_y = -data->sprite[i].sprite_h / 2 + screenheight / 2;
		if (data->sprite[i].drawstart_y < 0)
			data->sprite[i].drawstart_y = 0;
		data->sprite[i].drawend_y = data->sprite[i].sprite_h / 2 + screenheight / 2;
		if (data->sprite[i].drawend_y >= screenheight)
			data->sprite[i].drawend_y = screenheight - 1;
		data->sprite[i].sprite_w = abs((int)(screenheight / (data->sprite[i].trans_y)));
		data->sprite[i].drawstart_x = -data->sprite[i].sprite_w / 2 + data->sprite[i].spritescreen_x;
		if (data->sprite[i].drawstart_x < 0)
			data->sprite[i].drawstart_x = 0;
		data->sprite[i].drawend_x = data->sprite[i].sprite_w / 2 + data->sprite[i].spritescreen_x;
		if (data->sprite[i].drawend_x >= screenwidth)
			data->sprite[i].drawend_x = screenwidth - 1;	
		for (int stripe = data->sprite[i].drawstart_x; stripe < data->sprite[i].drawend_x; stripe++)
		{
			data->sprite[i].tex_x = (int)(256 * (stripe - (-data->sprite[i].sprite_w / 2 + data->sprite[i].spritescreen_x)) * data->textures.sprite.width / data->sprite[i].sprite_w) / 256;
			if (data->sprite[i].trans_y > 0 && stripe > 0 && stripe < screenwidth && data->sprite[i].trans_y < data->zbuffer[stripe])
			{
				for (int y = data->sprite[i].drawstart_y; y < data->sprite[i].drawend_y; y++)
				{
					data->sprite[i].d = (y) * 256 - screenheight * 128 + data->sprite[i].sprite_h * 128;
					data->sprite[i].tex_y = ((data->sprite[i].d * data->textures.sprite.height) / data->sprite[i].sprite_h) / 256;
					data->sprite[i].color = ((unsigned int*)data->textures.sprite.img.addr)[data->textures.sprite.width * data->sprite[i].tex_y + data->sprite[i].tex_x];
					if ((data->sprite[i].color & 0x00FFFFFF) != 0)
						my_mlx_pixel_put(data, stripe, y, data->sprite[i].color);
				}
			}
		}
	}
	mlx_put_image_to_window(data->mlx, data->win, data->img.img, 0, 0);
	return (0);
}

int main (int argc, char **argv)
{
	t_data data;
	data.width = 640;
	data.player.x = 15;
	data.player.y = 6;
	data.player.dir_x = -1;
	data.player.dir_y = 0;
	data.player.plane_x = 0;
	data.player.plane_y= 0.66;
	data.player.keys.w = 0;
	data.player.keys.a = 0;
	data.player.keys.s = 0;
	data.player.keys.d = 0;
	data.player.keys.left = 0;
	data.player.keys.right = 0;

	data.mlx = mlx_init();
	data.win = mlx_new_window(data.mlx, screenwidth, screenheight, "raycaster");
	data.img.img = mlx_new_image(data.mlx, screenwidth, screenheight);
	data.img.addr = (int *)mlx_get_data_addr(data.img.img, &data.img.bpp, &data.img.line_len, &data.img.endian);
	get_texture(&data, &data.textures.east, "/home/jesus/42/cub3D/textures/eagle.xpm");
	get_texture(&data, &data.textures.south, "/home/jesus/42/cub3D/textures/mossy.xpm");
	get_texture(&data, &data.textures.west, "/home/jesus/42/cub3D/textures/greystone.xpm");
	get_texture(&data, &data.textures.north, "/home/jesus/42/cub3D/textures/bluestone.xpm");
	get_texture(&data, &data.textures.sprite, "/home/jesus/42/cub3D/textures/barrel.xpm");
	//printf("x: %f y: %f\n", data.player.x, data.player.y);
	mlx_loop_hook(data.mlx, raycasting, &data);
	mlx_hook(data.win, 02, 1L<<0, press_key, &data);
	mlx_hook(data.win, 03, 1L<<1, release_key, &data);
	mlx_hook(data.win, 17, 0L, ft_close, &data);
	mlx_loop(data.mlx);
}