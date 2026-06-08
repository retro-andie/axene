#include <X11/Xlib.h>

int main(argc, argv)
int	argc;
char	**argv;
{
  Display	*display;
  Colormap	cmap;
  XColor	*colors;
  Visual	*visual;
  unsigned long	*pixels;
  unsigned short value = 0;
  int		nb_color, screen, count, i;
  
  if (argc == 2)
    value = atoi(argv[1]);
  
  display = XOpenDisplay(0);
  
  screen = DefaultScreen(display);
  cmap = DefaultColormap(display, screen);
  nb_color = DisplayCells(display, screen);
  visual = DefaultVisual(display, screen);
  
  if (visual->class == PseudoColor)
  {
    pixels = (unsigned long *)malloc(sizeof(unsigned long) * nb_color);   
    
    count = nb_color;
    while(count && !XAllocColorCells(display, cmap, False,
				     NULL, 0, pixels, count))
      count--;
      
    printf("Color allocated: %d  Color free: %d\n", nb_color-count, count);
    
    if (count)
    {
      colors = (XColor *)malloc(sizeof(XColor) * count);
      
      for(i = 0; i < count; i++)
      {
	colors[i].pixel = pixels[i];
	colors[i].red = colors[i].green = colors[i].blue = value; 
	colors[i].flags = DoRed | DoGreen | DoBlue;
      }
      
      XStoreColors(display, cmap, colors, count);
      
      free(colors);
    }
    
    XFreeColors(display, cmap, pixels, count, 0);
    free(pixels);
  }
  
  XCloseDisplay(display);
}
