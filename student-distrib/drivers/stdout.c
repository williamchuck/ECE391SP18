#include "stdout.h"

int32_t stdout_open(const uint8_t* filename)
{
  return 0;
}

int32_t stdout_close(int32_t fd)
{
  return 0;
}

int32_t stdout_read(int32_t fd, void* buf, int32_t nbytes)
{
  return -1;
}

int32_t stdout_write(int32_t fd, const void* buf, int32_t nbytes)
{
  int i;
  unsigned char* buf_ptr;
  buf_ptr = (unsigned char*)buf;

  if (buf_ptr == NULL)
  {
    return -1;
  }

  for (i = 0; i < nbytes; i++)
  {
    if (buf_ptr[i] == TERM_EOF)
    {
      return i;
    }
    putc(buf_ptr[i]);
  }
  return nbytes;
}

void cursor_reset()
{
  int x;
  int y;
  uint16_t pos;

  x = 0;
  y = 0;

  pos = y * VGA_WIDTH + x;
  set_xy(x, y);

  outb(0x0F, 0x3D4);
  outb((uint8_t)(pos & TERM_EOF), 0x3D5);
  outb(0x0E, 0x3D4);
  outb((uint8_t)((pos >> 8) & TERM_EOF), 0x3D5);
}

void term_init()
{
  int i;
  cursor_reset();
  clear();
  for (i = 0; i < BUF_SIZE; i++)
  {
    term_buf[i] = 0xFF;
  }
  term_buf_index = 0;
}

void term_del()
{
  int x;
  int y;
  uint16_t pos;

  x = get_x();
  y = get_y();
  pos = y * VGA_WIDTH + x;

  if (pos == 0)
  {
    return;
  }
  else
  {
    pos--;
    x = pos % VGA_WIDTH;
    y = pos / VGA_WIDTH;
  }
  set_xy(x, y);
  putc(0x00);
  set_xy(x, y);

  if (term_buf_index != 0)
  {
    if (term_buf[term_buf_index - 1] != TERM_EOF)
    {
      term_buf[term_buf_index - 1] = TERM_EOF;
      term_buf_index--;
      return;
    }
  }

  return;
}

void cursor_update()
{
  int x;
  int y;
  uint16_t pos;

  x = get_x();
  y = get_y();
  pos = y * VGA_WIDTH + x;

  outb(0x0F, 0x3D4);
  outb((uint8_t)(pos & TERM_EOF), 0x3D5);
  outb(0x0E, 0x3D4);
  outb((uint8_t)((pos >> 8) & TERM_EOF), 0x3D5);
}
