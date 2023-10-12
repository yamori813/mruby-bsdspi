/*
** mrb_bsdspi.c - BsdSpi class
**
** Copyright (c) Hiroki Mori 2017
**
** See Copyright Notice in LICENSE
*/

#include "mruby.h"
#include "mruby/data.h"
#include "mruby/array.h"
#include "mrb_bsdspi.h"

#include <err.h>
#include <errno.h>
#include <sysexits.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <sys/spigenio.h>

#define DONE mrb_gc_arena_restore(mrb, 0);

typedef struct {
  int fd;
} mrb_bsdspi_data;

static const struct mrb_data_type mrb_bsdspi_data_type = {
  "mrb_bsdspi_data", mrb_free,
};

static mrb_value mrb_bsdspi_init(mrb_state *mrb, mrb_value self)
{
  mrb_bsdspi_data *data;
  char device[32];
  mrb_int num;

  data = (mrb_bsdspi_data *)DATA_PTR(self);
  if (data) {
    mrb_free(mrb, data);
  }
  DATA_TYPE(self) = &mrb_bsdspi_data_type;
  DATA_PTR(self) = NULL;

  mrb_get_args(mrb, "i", &num);
  data = (mrb_bsdspi_data *)mrb_malloc(mrb, sizeof(mrb_bsdspi_data));
  snprintf(device, sizeof(device), "/dev/spigen%u.0", num);
  data->fd = open(device, O_RDWR);
  DATA_PTR(self) = data;

  return self;
}

static mrb_value mrb_bsdspi_getclk(mrb_state *mrb, mrb_value self)
{
  mrb_bsdspi_data *data = DATA_PTR(self);
  int clk;

  ioctl(data->fd, SPIGENIOC_GET_CLOCK_SPEED, &clk);

  return mrb_fixnum_value(clk);
}

static mrb_value mrb_bsdspi_setclk(mrb_state *mrb, mrb_value self)
{
  mrb_bsdspi_data *data = DATA_PTR(self);
  mrb_int val;
  int clk;

  mrb_get_args(mrb, "i", &val);
  clk = val;
  ioctl(data->fd, SPIGENIOC_SET_CLOCK_SPEED, &clk);

  return mrb_fixnum_value(clk);
}

#define MAXBUF 1024

static mrb_value mrb_bsdspi_transfer(mrb_state *mrb, mrb_value self)
{
  mrb_bsdspi_data *data = DATA_PTR(self);
  struct spigen_transfer cmd;
  int error;
  unsigned char txbuf[MAXBUF];
  unsigned char rxbuf[MAXBUF];
  int len;
  mrb_value arr;
  mrb_value res;
  mrb_int rxsize;
  int i;

  mrb_get_args(mrb, "Ai", &arr, &rxsize);
  len = RARRAY_LEN( arr );
  if (len > MAXBUF || rxsize > MAXBUF) {
    return mrb_fixnum_value(0);
  }
  for (i = 0; i < len; ++i)
    txbuf[i] = mrb_fixnum( mrb_ary_ref( mrb, arr, i ) );

  cmd.st_command.iov_base = txbuf;
  cmd.st_command.iov_len = len;
  cmd.st_data.iov_base = rxbuf;
  cmd.st_data.iov_len = rxsize;

  error = ioctl(data->fd, SPIGENIOC_TRANSFER, &cmd);

  res = mrb_ary_new(mrb);
  for (i = 0; i < rxsize; ++i)
    mrb_ary_push(mrb, res, mrb_fixnum_value(rxbuf[i]));

  return res;
}

void mrb_mruby_bsdspi_gem_init(mrb_state *mrb)
{
  struct RClass *bsdspi;
  bsdspi = mrb_define_class(mrb, "BsdSpi", mrb->object_class);
  mrb_define_method(mrb, bsdspi, "initialize", mrb_bsdspi_init, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, bsdspi, "transfer", mrb_bsdspi_transfer, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, bsdspi, "getclk", mrb_bsdspi_getclk, MRB_ARGS_NONE());
  mrb_define_method(mrb, bsdspi, "setclk", mrb_bsdspi_setclk, MRB_ARGS_REQ(1));
  DONE;
}

void mrb_mruby_bsdspi_gem_final(mrb_state *mrb)
{
}

