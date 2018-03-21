#include "stdin.h"
#include "stdout.h"

int32_t stdin_open(const uint8_t* filename)
{
  return 0;
}

int32_t stdin_close(int32_t fd)
{
  return 0;
}

int32_t stdin_read(int32_t fd, void* buf, int32_t nbytes)
{
  unsigned char* buf_ptr;
  int i, j;
  buf_ptr = (unsigned char*)buf;

  if (buf_ptr == NULL)
  {
    return -1;
  }

  for (i = 0; i < nbytes; i++)
  {
    if (term_buf_index == BUF_SIZE)
    {
      for (j = 0; j < BUF_SIZE; j++)
      {
        term_buf[j] = TERM_EOF;
      }
      term_buf_index = 0;
    }

    if (buf_ptr[i] == TERM_EOF)
    {
      return (uint32_t)i;
    }

    term_buf[term_buf_index] = buf_ptr[i];
    term_buf_index++;

    if (buf_ptr[i] == ASCII_NL)
    {
      return (uint32_t)(i + 1);
    }
  }

  return nbytes;
}

int32_t stdin_write(int32_t fd, const void* buf, int32_t nbytes)
{
  return -1;
}
